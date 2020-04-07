//==========================================================================
//
//      devs/pcmcia/dharma/dharma_pcmcia.c
//
//      PCMCIA support (Card Services)
//
//==========================================================================
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    toddm
// Contributors: toddm
// Date:         2001-09-14
// Purpose:      PCMCIA support
// Description: 
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/io_pcmcia.h>

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_arch.h>           // Register state info
#include <cyg/hal/hal_intr.h>           // HAL interrupt macros
#include <cyg/hal/drv_api.h>

#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>   // Configuration header
#include <cyg/kernel/kapi.h>
#endif
#include <cyg/hal/hal_if.h>

#include <cyg/io/pcmcia.h>
#include <cyg/infra/diag.h>

#include <cyg/hal/hal_edb7xxx.h>  // Board definitions

/* GPIO Port B */
#define SLA0 (1<<4)
#define PCMCIA_IO_SELECT (1<<3)
#define PCMCIA_8_16_SELECT (1<<2)
#define PCMCIA_REG (1<<1)
#define PCMCIA_RESET (1<<0)

/* GPIO Port D */
#define SCL (1<<5)
#define SDA (1<<4)
#define PCMCIA_RDY (1<<3)
#define PCMCIA_CD (1<<1)

typedef enum 
{
    VL_0,
    VL_33,
    VL_5,
    VL_12
} VoltageLevel_T;

static void ClockPowerSupplyBit(int Bit);
static bool ConfigurePowerSupply(VoltageLevel_T Voltage);

#ifdef CYGPKG_KERNEL
static cyg_interrupt cf_detect_interrupt;
static cyg_handle_t  cf_detect_interrupt_handle;
static cyg_interrupt cf_irq_interrupt;
static cyg_handle_t  cf_irq_interrupt_handle;

// This ISR is called when a PC board is inserted
static int
cf_detect_isr(cyg_vector_t vector, cyg_addrword_t data, HAL_SavedRegisters *regs)
{
    /* TODO Enable interrupt sharing */
    cyg_drv_interrupt_mask(CYGNUM_HAL_INTERRUPT_EINT2);
    return (CYG_ISR_HANDLED|CYG_ISR_CALL_DSR);  // Run the DSR
}

// This DSR handles the board insertion
static void
cf_detect_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    struct cf_slot *slot = (struct cf_slot *)data;
    cyg_uint8 new_state = *(volatile cyg_uint8 *)PDDR;
    if ((new_state & PCMCIA_CD) == PCMCIA_CD) {
        slot->state = CF_SLOT_STATE_Removed;  // Implies powered up, etc.
    } else {
        slot->state = CF_SLOT_STATE_Inserted;
    }
    cyg_drv_interrupt_acknowledge(CYGNUM_HAL_INTERRUPT_EINT2);
    cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_EINT2);
}

// This ISR is called when the card interrupt occurs
static int
cf_irq_isr(cyg_vector_t vector, cyg_addrword_t data, HAL_SavedRegisters *regs)
{
    /* TODO Support memory mapped interrupts and interrupt sharing */
    cyg_drv_interrupt_mask(CYGNUM_HAL_INTERRUPT_UMSINT);
    return (CYG_ISR_HANDLED|CYG_ISR_CALL_DSR);  // Run the DSR
}

// This DSR handles the card interrupt
static void
cf_irq_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    struct cf_slot *slot = (struct cf_slot *)data;

    // Clear interrupt [edge indication]
    cyg_drv_interrupt_acknowledge(CYGNUM_HAL_INTERRUPT_UMSINT);
    // Process interrupt
    (slot->irq_handler.handler)(vector, count, slot->irq_handler.param);
    // Allow interrupts to happen again
    //cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_UMSINT);
}
#endif

static void
do_delay(int ticks)
{
	volatile int i;

	// cyg_thread_delay does not work since cf_hwr_init is called before
	// scheduler is started.
#if 0//def CYGPKG_KERNEL
    cyg_thread_delay(ticks);
#else
	// Looks like there are overflow issues with this calculation
	//CYGACC_CALL_IF_DELAY_US(10000*ticks);
	for (i = 0; i < 10000; ++i) CYGACC_CALL_IF_DELAY_US(ticks);
#endif
}

//
// Fill in the details of a PCMCIA slot and initialize the device
//
void
cf_hwr_init(struct cf_slot *slot)
{
    static int int_init = 0;
    cyg_uint8 new_state;

    /* Set up GPIO inputs and outputs */
    //*(volatile cyg_uint8 *)PBDDR |= (SLA0 | PCMCIA_IO_SELECT | PCMCIA_8_16_SELECT | PCMCIA_REG | PCMCIA_RESET);
    //*(volatile cyg_uint8 *)PDDDR &= ~(SCL | SDA);
    //*(volatile cyg_uint8 *)PDDDR |= (PCMCIA_RDY | PCMCIA_CD);

    new_state = *(volatile cyg_uint8 *)PDDR;

    if (!int_init) {
        int_init = 1;

	/* Configure bus to 16bit databus, 8 wait states, EXPRDY.
	   Card is found at nCS3 | A25 */
	*(volatile cyg_uint32 *)MEMCFG1 &= ~0xff000000;
	*(volatile cyg_uint32 *)MEMCFG1 |= 0x81000000 | ((0<<2)<<24);
	*(volatile cyg_uint32 *)SYSCON1 |= SYSCON1_EXCKEN; /* This doesn't seem to do anything, but I will play it safe */
	
#ifdef CYGPKG_KERNEL
        // Set up interrupts
        cyg_drv_interrupt_create(CYGNUM_HAL_INTERRUPT_EINT2,
                                 99,                     // Priority - what goes here?
                                 (cyg_addrword_t) slot,  //  Data item passed to interrupt handler
                                 (cyg_ISR_t *)cf_detect_isr,
                                 (cyg_DSR_t *)cf_detect_dsr,
                                 &cf_detect_interrupt_handle,
                                 &cf_detect_interrupt);
        cyg_drv_interrupt_attach(cf_detect_interrupt_handle);
	cyg_drv_interrupt_acknowledge(CYGNUM_HAL_INTERRUPT_EINT2);
        //cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_EINT2);

        cyg_drv_interrupt_create(CYGNUM_HAL_INTERRUPT_UMSINT,
                                 99,                     // Priority - what goes here?
                                 (cyg_addrword_t) slot,  //  Data item passed to interrupt handler
                                 (cyg_ISR_t *)cf_irq_isr,
                                 (cyg_DSR_t *)cf_irq_dsr,
                                 &cf_irq_interrupt_handle,
                                 &cf_irq_interrupt);
        cyg_drv_interrupt_attach(cf_irq_interrupt_handle);
        //cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_UMSINT);
        cyg_drv_interrupt_acknowledge(CYGNUM_HAL_INTERRUPT_UMSINT);
#endif
    }
    slot->attr = (unsigned char *)0x32000000;
    slot->attr_length = 0x00400000;
    slot->io = (unsigned char *)0x32000000;
    slot->io_length = 0x00400000;
    slot->mem = (unsigned char *)0x32000000;
    slot->mem_length = 0x00400000;
    slot->int_num = CYGNUM_HAL_INTERRUPT_UMSINT;

    /* Set up GPIO before anything else so that we know what state the
       outputs are in before trying to power on the card */
    //SetMMGPO(MMGPO_PCMCIA_IO_SELECT, (MMGPO_PCMCIA_8_16_SELECT | MMGPO_PCMCIA_REG));
    *(volatile cyg_uint8 *)PBDR |= PCMCIA_IO_SELECT;
    *(volatile cyg_uint8 *)PBDR &= ~(PCMCIA_8_16_SELECT | PCMCIA_REG);

    if ((new_state & PCMCIA_CD) == PCMCIA_CD) {
        slot->state = CF_SLOT_STATE_Empty;
    } else {
	slot->state = CF_SLOT_STATE_Inserted;
    }
}

void
cf_hwr_poll(struct cf_slot *slot)
{
    cyg_uint8 new_state = *(volatile cyg_uint8 *)PDDR;
    if ((new_state & PCMCIA_CD) == PCMCIA_CD) {
        slot->state = CF_SLOT_STATE_Empty;
    } else {
        slot->state = CF_SLOT_STATE_Inserted;
    }
}

//
// Transition the card/slot to a new state
// note: currently only supports transitions to Ready, Empty
//
bool
cf_hwr_change_state(struct cf_slot *slot, int new_state)
{    
    int i;

    if (new_state == CF_SLOT_STATE_Ready) {
        if (slot->state == CF_SLOT_STATE_Inserted) {
	    do_delay(10); // At least 50ms
	    ConfigurePowerSupply(VL_5);
            do_delay(60);  // At least 300 ms
            slot->state = CF_SLOT_STATE_Powered;
	    *(volatile cyg_uint8 *)PBDR |= PCMCIA_RESET;
            do_delay(2);  // At least 10 us
            slot->state = CF_SLOT_STATE_Reset;
	    *(volatile cyg_uint8 *)PBDR &= ~PCMCIA_RESET;
            do_delay(10); // At least 20 ms
            // Wait until the card is ready to talk (up to 10s)
	    for (i = 0; i < 10; ++i) {
			if (*(volatile cyg_uint8 *)PDDR & PCMCIA_RDY) {
		    diag_printf("Ready!\n");
		    slot->state = CF_SLOT_STATE_Ready;
		    break;
		}
		diag_printf("#\n");
		do_delay(100);
		}
		// Unmask the interrupt now that it is safe to do so - OOPS not safe to do so here.
	    //cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_UMSINT);
        }
    }
}

//
// Acknowledge interrupt (used in non-kernel environments)
//
void
cf_hwr_clear_interrupt(struct cf_slot *slot)
{
    // Clear interrupt [edge indication]
    cyg_drv_interrupt_acknowledge(CYGNUM_HAL_INTERRUPT_UMSINT);
}

static void
ClockPowerSupplyBit(int Bit)
{
    /* Data is clocked in on positive leading edge of clock,
       so set data line... */
    if (Bit) {
	*(volatile cyg_uint8 *)PDDR |= SDA;
    }
    else {
	*(volatile cyg_uint8 *)PDDR &= ~SDA;
    }

    /* ...and clock it in */
    *(volatile cyg_uint8 *)PDDR |= SCL;
    do_delay(1);
    *(volatile cyg_uint8 *)PDDR &= ~SCL;
}
    
static bool
ConfigurePowerSupply(VoltageLevel_T Voltage) 
{
    int ConfigurationData;
    int BitMask;

    /* Configure the GPIO first
      PD4 - SDA
      PD5 - SCL
      PB2 - SLA0 */
    *(volatile cyg_uint8 *)PDDDR &= ~(SDA | SCL);
    *(volatile cyg_uint8 *)PBDDR |= SLA0;
    
    /* Configuration will be for 5V, this gives the following
       bits, MSB (D8) first: 1 0110 1010 */
    if (Voltage != VL_5) {
	diag_printf("XXX Not supported XXX\n");
	return false;
    }
    ConfigurationData = 0x16a;

    /* The interface cannot be clocked faster than 2.5MHz, which
       is a 0.4uS pulse width, so we will be fine */

    /* The datasheet shows a clock before the first data bit is
       clocked in */
    *(volatile cyg_uint8 *)PDDR |= SCL;
    do_delay(1);
    *(volatile cyg_uint8 *)PDDR &= ~SCL;

    /* Now clock the data in */
    for (BitMask = 0x100; BitMask; BitMask >>= 1) {
	ClockPowerSupplyBit(ConfigurationData & BitMask);
    }

    /* Now latch the data in */
    *(volatile cyg_uint8 *)PBDR |= SLA0;
    do_delay(1);
    *(volatile cyg_uint8 *)PBDR &= ~SLA0;

    return true;
}

