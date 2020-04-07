/* THIS SOFTWARE IS PROVIDED "AS IS" WITH NO WARRANTIES EXPRESSED OR IMPLIED,
   INCLUDING WITHOUT LIMITATION WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
   PARTICULAR PURPOSE OR NON-INFRINGEMENT. LICENSEE BEARS ALL RISK OF 
   NON-PERFORMANCE, LOSS OF DATA, AND OTHER PROBLEMS AND LICENSOR WILL NOT BE
   LIABLE UNDER ANY CONTRACT, NEGLIGENCE, STRICT LIABILITY OR OTHER THEORY
   FOR ANY DAMAGES INCLUDING, WITHOUT LIMITATION, DIRECT, INCIDENTAL OR 
   CONSEQUENTIAL DAMAGES OR COST OF PROCUREMENT OF SUBSTITUTE GOODS, SERVICES
   OR TECHNOLOGY */

#include <pkgconf/io_atapi_arm_dadio.h>

#include <time.h>		/* clock_t,clock */
#include "edk_sw.h"

#ifndef _EDK_HW_H_
#define _EDK_HW_H_

#if defined(DDOHWR_IO_ATAPI_INTERFACE_CLIK)
/* ATAPI task file registers */
/* dc: ported to arm board
 * note that the layout on our clik system is foobar since it is wired through
 * what used to be a CF interface. we had a problem with addressing odd registers off
 * the dev, so a0 -> a5
 */
#define IO_ATA_DATA_REG              0x50000010
#define IO_ATA_FEATURES_REG          0x50000030
#define IO_ATA_INTERRUPT_REASON_REG  0x50000012
#define IO_ATA_BYTE_CNT_LOW_REG      0x50000014
#define IO_ATA_BYTE_CNT_HI_REG       0x50000034
#define IO_ATA_DRIVE_SELECT_REG      0x50000016
#define IO_ATA_STATUS_REG            0x50000036
#define IO_ATA_CMD_REG               IO_ATA_STATUS_REG
#define IO_ATA_DEVICE_CTL_REG        0x5000000e

#define USE_IRQ 1
//#define USE_IRQ 0
#elif defined(DDOHWR_IO_ATAPI_INTERFACE_CF_V2)
/* ATA task file registers */
#define IO_ATA_DATA_REG         0x50000800            /* rw */
#define IO_ATA_ERR_REG          0x50000801            /* r */
#define IO_ATA_DUP_ERR_REG      0x5000080d            /* r */
#define IO_ATA_FEATURES_REG     IO_ATA_ERR_REG        /* w */
#define IO_ATA_DUP_FEATURES_REG IO_ATA_DUP_ERR_REG    /* w */
#define IO_ATA_SEC_CNT_REG      0x50000802            /* rw */
#define IO_ATA_SEC_NUM_REG      0x50000803            /* rw */
#define IO_ATA_CYL_LOW_REG      0x50000804            /* rw */
#define IO_ATA_CYL_HI_REG       0x50000805            /* rw */
#define IO_ATA_DEVICE_HEAD_REG  0x50000806            /* rw */
#define IO_ATA_STATUS_REG       0x50000807            /* r */
#define IO_ATA_CMD_REG          IO_ATA_STATUS_REG     /* w */
#define IO_ATA_ALT_STATUS_REG   0x5000080e            /* r */
#define IO_ATA_DEVICE_CTL_REG   IO_ATA_ALT_STATUS_REG /* w */

/* ATAPI task file registers */
#define IO_ATA_INTERRUPT_REASON_REG IO_ATA_SEC_CNT_REG /* r */
#define IO_ATA_BYTE_CNT_LOW_REG     IO_ATA_CYL_LOW_REG /* rw */
#define IO_ATA_BYTE_CNT_HI_REG      IO_ATA_CYL_HI_REG  /* rw */
#define IO_ATA_DRIVE_SELECT_REG     IO_ATA_DEVICE_HEAD_REG /* rw */

#define IO_ATA_16BIT_ACCESS	/* this should only be defined when the registers are contiguous
				 * and the memory is configured as 16 bit access.  if the registers
				 * are not contiguous and the memory is 16 bit access, then simply
				 * set the addresses of the registers to the appropriate values above. */

#define USE_IRQ 0
#else
#err Invalid ATA interface selected
#endif /* DDOHWR_IO_ATAPI_CLIK */

#if 1
#define ReadReg8(reg,var)             \
        var = *((volatile char *)reg)

#define WriteReg8(reg,val)            \
        *((volatile char *)reg) = (val&0xff)

#define ReadReg16(reg,var)            \
        var = *((volatile short *)reg)

#define WriteReg16(reg,val)           \
        *((volatile short *)reg) = (val&0xffff)

#define ReadDataBulk(address,count)   \
      {                               \
        int i;                        \
        for(i=0;i<count;i++)        \
          ((volatile short *)address)[i]=*((volatile short *)IO_ATA_DATA_REG);  \
      }
#define WriteDataBulk(address, count) \
      {                               \
	  int i;                      \
	  for(i=0;i<count;i++)        \
            *((volatile short *)IO_ATA_DATA_REG) = ((volatile short *)address)[i];   \
      }
#else
#define ReadReg8(reg,var)			\
{						\
    volatile int i;				\
    for (i = 0; i < 60; ++i);			\
    var = *((volatile char *)reg);		\
}

#define WriteReg8(reg,val)			\
{						\
    volatile int i;				\
    for (i = 0; i < 60; ++i);			\
    *(volatile char *)reg = val & 0xff;		\
}

#define ReadDataBulk(address,count)			\
{							\
    int i;						\
    volatile int j;					\
    for (i = 0; i < count; ++i) {			\
	for (j = 0; j < 60; ++j);			\
	((short *)address)[i] = *((volatile short *)IO_ATA_DATA_REG);	\
    }							\
}

#define WriteDataBulk(address,count)					\
{									\
    int i;								\
    volatile int j;							\
    for (i = 0; i < count; ++i) {					\
	for (j = 0; j < 60; ++j);					\
	*(volatile short *)IO_ATA_DATA_REG = ((short *)address)[i];	\
    }									\
}

#endif

#ifndef IO_ATA_16BIT_ACCESS
//#define ReadError(variable) ReadReg8(IO_ATA_ERR_REG,variable);
#define ReadInterruptReason(variable) ReadReg8(IO_ATA_INTERRUPT_REASON_REG,variable)
//#define ReadSectorCount(variable) ReadReg8(IO_ATA_SEC_CNT_REG,variable)
//#define ReadSectorNumber(variable) ReadReg8(IO_ATA_SEC_NUM_REG,variable)
#define ReadByteCountLow(variable) ReadReg8(IO_ATA_BYTE_CNT_LOW_REG,variable)
#define ReadByteCountHigh(variable) ReadReg8(IO_ATA_BYTE_CNT_HI_REG,variable)
//#define ReadCylinderLow(variable) ReadReg8(IO_ATA_CYL_LOW_REG,variable)
//#define ReadCylinderHigh(variable) ReadReg8(IO_ATA_CYL_HI_REG,variable)
//#define ReadDeviceHead(variable) ReadReg8(IO_ATA_DEVICE_HEAD_REG,variable)
#define ReadStatus(variable) ReadReg8(IO_ATA_STATUS_REG,variable)
#else
#define ReadStatus(variable) ReadReg16(IO_ATA_DEVICE_HEAD_REG,variable) >> 8
#define ReadSectorCount(variable) ReadReg16(IO_ATA_SEC_CNT_REG,variable) & 0xff
#define ReadInterruptReason(variable) ReadSectorCount(variable)
#define ReadAlternateStatus(variable) ReadReg16(IO_ATA_ALT_STATUS_REG,variable) & 0xff
#define ReadError(variable) ReadReg16((IO_ATA_DUP_ERR_REG-1),variable) >> 8 /* note the -1 to get 16bit address */
#define ReadSectorNumber(variable) ReadReg16(IO_ATA_SEC_CNT_REG,variable) >> 8
#define ReadCylinderLow(variable) ReadReg16(IO_ATA_CYL_LOW_REG,variable) & 0xff
#define ReadCylinderHigh(variable) ReadReg16(IO_ATA_CYL_LOW_REG,variable) >> 8
#define ReadByteCountLow(variable) ReadCylinderLow(variable)
#define ReadByteCountHigh(variable) ReadCylinderHigh(variable)
#define ReadDeviceHead(variable) ReadReg16(IO_ATA_DEVICE_HEAD_REG,variable) & 0xff
#define ReadDriveAddress(variable) ReadReg16(IO_ATA_ALT_STATUS_REG,variable) >> 8
#endif /* !IO_ATA_16BIT_ACCESS */

#ifdef IO_ATA_16BIT_ACCESS
#define WriteFeatures(value) WriteReg16((IO_ATA_DUP_FEATURES_REG-1),(value << 8)) /* note the -1 to get 16 address */
#else
#define WriteFeatures(value) WriteReg8(IO_ATA_FEATURES_REG,value)
#endif /* IO_ATA_16BIT_ACCESS */
//#define WriteSectorCount(variable) WriteReg8(IO_ATA_SEC_CNT_REG,variable)
//#define WriteSectorNumber(variable) WriteReg8(IO_ATA_SEC_NUM_REG,variable)
#define WriteByteCountLow(value) WriteReg8(IO_ATA_BYTE_CNT_LOW_REG,value)
#define WriteByteCountHigh(value) WriteReg8(IO_ATA_BYTE_CNT_HI_REG,value)
//#define WriteCylinderLow(value) WriteReg8(IO_ATA_CYL_LOW_REG,value)
//#define WriteCylinderHigh(value) WriteReg8(IO_ATA_CYL_HI_REG,value)
#define WriteDriveSelect(value) WriteReg8(IO_ATA_DRIVE_SELECT_REG,value)
//#define WriteDeviceHead(value) WriteReg8(IO_ATA_DEVICE_HEAD_REG,value)
#define WriteCommand(value) WriteReg8(IO_ATA_CMD_REG,value)
#define WriteControl(value) WriteReg8(IO_ATA_DEVICE_CTL_REG,value)

#define WriteSector16(cnt,num) WriteReg16(IO_ATA_SEC_CNT_REG, ((cnt) | ((num) << 8)))
#define WriteCylinder16(low,high) WriteReg16(IO_ATA_CYL_LOW_REG,((low) | ((high) << 8)))
#define WriteDeviceCmd16(device,cmd) WriteReg16(IO_ATA_DEVICE_HEAD_REG,((device) | ((cmd) << 8)))

#if USE_IRQ
void InitializeInterrupt(void);
void MaskInterrupt(void);
void UnmaskInterrupt(void);
#endif /* USE_IRQ */

typedef clock_t TIMER;
TIMER GetOSTicks(void);
//#define TIMEOUT_5SECONDS CLOCKS_PER_SEC * 5
//#define TIMEOUT_5SECONDS (5*100)
#define TIMEOUT_5SECONDS (10*100)/* Iomega requested 10s to compensate for drive problems */

#endif /* _EDK_HW_H_ */
