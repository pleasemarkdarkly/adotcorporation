/* THIS SOFTWARE IS PROVIDED "AS IS" WITH NO WARRANTIES EXPRESSED OR IMPLIED,
   INCLUDING WITHOUT LIMITATION WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
   PARTICULAR PURPOSE OR NON-INFRINGEMENT. LICENSEE BEARS ALL RISK OF 
   NON-PERFORMANCE, LOSS OF DATA, AND OTHER PROBLEMS AND LICENSOR WILL NOT BE
   LIABLE UNDER ANY CONTRACT, NEGLIGENCE, STRICT LIABILITY OR OTHER THEORY
   FOR ANY DAMAGES INCLUDING, WITHOUT LIMITATION, DIRECT, INCIDENTAL OR 
   CONSEQUENTIAL DAMAGES OR COST OF PROCUREMENT OF SUBSTITUTE GOODS, SERVICES
   OR TECHNOLOGY */

#include <time.h>		/* clock */
#include <dadio/io/edk.h>
#include <dadio/io/edk_hw.h>
//#include "edk.h"
//#include "edk_hw.h"
//#include "edk_tool.h"

#ifdef _ECOS_
#include <cyg/kernel/kapi.h>
#include <cyg/hal/hal_edb7xxx.h>
#include <cyg/hal/drv_api.h>
#endif

/* Hardware Dependant Code that is used no matter the switch setting */
TIMER GetOSTicks(void)
{
#ifdef _ECOS_
    //    diag_printf("%x\n", cyg_current_time());
    return cyg_current_time();
#else
    return(clock());
#endif
}

/* Hardware dependant code used if not using macros (MACROS = 0) */
#if !MACROS
               
void ReadDataBulk(PBUFFER address, WORD count)
{

    __asm {                                    
	push  di                   
	    push  ds                   
	    pop   es                   
	    mov	di,address           
	    mov	cx,count             
	    mov   dx,0x170             
	    rep   insw                 
	    pop   di                   
	    }
}		
    
void WriteDataBulk(PBUFFER address, WORD count)
{
    __asm                                
	{                                    
	    push  si                   
		mov	si,address           
		mov	cx,count             
		mov   dx,0x170             
		rep   outsw                
		pop	si					 
		}
}
void WriteFeatures(BYTE value)
{
    __asm                        
	{                            
	    mov dx,0x171       
		mov al,value       
		out dx,al          
		}
}
void ReadInterruptReason__(BYTE *variable)
{
    BYTE temp;

    __asm                        
	{                            
	    mov dx, 0x172
		in al,dx           
		mov temp,al    
		}
    *variable = temp;
}
void ReadByteCountLow__(BYTE *variable)
{
    BYTE temp;

    __asm                        
	{                            
	    mov dx, 0x174
		in al,dx           
		mov temp,al    
		}
    *variable = temp;
}
void WriteByteCountLow(BYTE value)
{
    __asm                        
	{                            
	    mov dx,0x174
		mov al,value       
		out dx,al          
		}
}

void ReadByteCountHigh__(BYTE *variable)
{
    BYTE temp;

    __asm                        
	{                            
	    mov dx, 0x175
		in al,dx           
		mov temp,al    
		}
    *variable = temp;
}
void WriteByteCountHigh(BYTE value)
{
    __asm                        
	{                            
	    mov dx,0x175
		mov al,value       
		out dx,al          
		}
}
void WriteDriveSelect(BYTE value)
{
    __asm                        
	{                            
	    mov dx,0x176
		mov al,value       
		out dx,al          
		}
}
void ReadStatus__(BYTE *variable)
{
    BYTE temp;

    __asm                        
	{                            
	    mov dx, 0x177
		in al,dx           
		mov temp,al    
		}
    *variable = temp;
}
void WriteCommand(BYTE value)
{
    __asm                        
	{                            
	    mov dx,0x177
		mov al,value       
		out dx,al          
		}
}
void WriteControl(BYTE value)
{
    __asm                        
	{                            
	    mov dx,0x376
		mov al,value       
		out dx,al          
		}
}
#endif  /* !MACROS */
   

/* Hardware dependant code used only if USE_IRQ = 1 */
#if USE_IRQ

#ifdef _ECOS_
static cyg_uint32 clik_interrupt_isr(cyg_vector_t vector, cyg_addrword_t data);
static void clik_interrupt_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);
static cyg_interrupt clik_interrupt;
static cyg_handle_t  clik_interrupt_handle;
static bool interrupt_initialized = false;
cyg_sem_t clik_sem;
#else
void __interrupt __far  InterruptHandler(void);
#endif

extern BYTE Interrupt_Occurred;
extern BYTE status;

void InitializeInterrupt(void)
{
#ifdef _ECOS_
    if( !interrupt_initialized ) {
	interrupt_initialized = true;
	//Interrupt_Occurred = 0;	
	cyg_semaphore_init( &clik_sem, 0 );
	cyg_drv_interrupt_create(CYGNUM_HAL_INTERRUPT_EINT3,
				 10,
				 0,
				 (cyg_ISR_t*) clik_interrupt_isr,
				 (cyg_DSR_t*) clik_interrupt_dsr,
				 &clik_interrupt_handle,
				 &clik_interrupt);
	cyg_drv_interrupt_attach(clik_interrupt_handle);
	/* the following two lines are here because ResetAtapi is called
	 * before InitAtapi, and ResetAtapi unmasks the interrupt and InitAtapi
	 * initializes it, so the interrupt would never get unmasked without
	 * these. */
	cyg_drv_interrupt_acknowledge(CYGNUM_HAL_INTERRUPT_EINT3);
	cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_EINT3);
    }
  
#else
    DWORD far *OldInterrupt;
    
    OldInterrupt = (void far *)476;
    *OldInterrupt = (DWORD) *InterruptHandler;
#endif
}

void MaskInterrupt(void)
{
#ifdef _ECOS_
    cyg_drv_interrupt_mask(CYGNUM_HAL_INTERRUPT_EINT3);
#else
    __asm
	{
	    mov	dx,0A1h      ; Slave PIC Mask Register
	   in  al,dx        ; read it
	  or  al,080h      ; mask the interrupt
	 out dx,al        ; write it
													}
#endif
}

void UnmaskInterrupt(void)
{
#ifdef _ECOS_
    if( interrupt_initialized )
	cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_EINT3);
#else
    __asm
	{
	    mov dx,0A1h      ; Slave PIC Mask Register
	   in  al,dx        ; read it
	  and al,0EFh      ; clear it
	 out dx,al        ; write it
													}
#endif
}
#ifdef _ECOS_
static cyg_uint32
clik_interrupt_isr(cyg_vector_t vector, cyg_addrword_t data) 
{
    cyg_drv_interrupt_mask(CYGNUM_HAL_INTERRUPT_EINT3);
    return (CYG_ISR_HANDLED|CYG_ISR_CALL_DSR);  // Run the DSR
}

static void
clik_interrupt_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    // avoid spurious/duplicate interrupts
    if( interrupt_initialized ) {
	//Interrupt_Occurred++;
	cyg_semaphore_post( &clik_sem ); /* TODO make this a binary semaphore?? */
	ReadStatus(status);
	cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_EINT3);
    }
}

#else
void __interrupt __far InterruptHandler(void)
{
    Interrupt_Occurred = 1;
    ReadStatus(status);

    /* signal end of interrupt to PIC */
    __asm
	{
	mov	al,20h		// signal End of Interrupt
	out	20h,al		// Master PIC
	out	0A0h,al		// Slave PIC
	}	
}
#endif
#endif /* USE_IRQ */






