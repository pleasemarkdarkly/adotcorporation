#ifndef _ATA_H
#define _ATA_H

#include <cyg/kernel/kapi.h>
#include <cyg/hal/hal_edb7xxx.h>
#include <dadio/io/ata.h>

#define USE_IRQ
#define ATA_IRQ CYGNUM_HAL_INTERRUPT_EINT3

#define USE_MACROS 0
#define MAX_RETRIES 5

#define ATA_MEMCFG_MASK 0x0000ff00

/* Values to play with */
#define MEMCFG_BUS_WIDTH(n)   (n<<0)
#if 0
#define MEMCFG_BUS_WIDTH_16   (0<<0)
#define MEMCFG_BUS_WIDTH_32   (1<<0)
#define MEMCFG_BUS_WIDTH_8    (3<<0)
#endif
#define MEMCFG_BUS_WIDTH_32   (0<<0)
#define MEMCFG_BUS_WIDTH_16   (1<<0)
#define MEMCFG_BUS_WIDTH_8    (2<<0)
#define MEMCFG_WAIT_STATES(n) (n<<2)
#define MEMCFG_SQAEN          (1<<6)
#define MEMCFG_CLKENB         (1<<7)

#define WAIT_STATES 8

#define ATA_8BIT_MEMCFG ((MEMCFG_CLKENB|MEMCFG_WAIT_STATES(8-WAIT_STATES)|MEMCFG_BUS_WIDTH_8)<<8)
#define ATA_16BIT_MEMCFG ((MEMCFG_CLKENB|MEMCFG_WAIT_STATES(8-WAIT_STATES)|MEMCFG_BUS_WIDTH_16)<<8)

typedef struct PartitionInfo_S
{
    unsigned char Type;
    unsigned int StartSector;
    unsigned int EndSector;
} PartitionInfo_T;

typedef struct ATAPrivatePartitionInfo_S
{
    DeviceInfo_T Device;
    PartitionInfo_T Partition;
    struct ATAPrivatePartitionInfo_S * Next;
} ATAPrivatePartitionInfo_T;

typedef struct
{} IdentifyDeviceInfo_T;

typedef struct
{} IdentifyPacketDeviceInfo_T;

typedef struct
{
    DeviceInfo_T Device;
    unsigned short Cylinders;
    unsigned short Heads;
    unsigned short SectorsPerTrack;
    unsigned short BytesPerSector;
    unsigned int TotalSectors;
    char SerialNumber[40 + 1];
    char FirmwareRevision[8 + 1];
    char ModelNumber[20 + 1];
    ATAPrivatePartitionInfo_T * Partition;
    union
    {
	IdentifyDeviceInfo_T ATA;
	IdentifyPacketDeviceInfo_T ATAPI;
	unsigned short Raw[256];
    } Info;
} ATAPrivateDeviceInfo_T;

typedef struct
{
    /* Transfer data */
    unsigned short * Data;
    unsigned int DataLength;	/* NOTE DataLength is number of 16 bit words */

    /* Packet command data */
    unsigned char * PacketCommand;
    unsigned int PacketCommandLength;
    
    /* Task file registers */
    unsigned char Features;
    unsigned char SectorCount;
    unsigned char SectorNumber;
    unsigned char CylinderLow;
    unsigned char CylinderHigh;
    unsigned char DeviceHead;
    unsigned char DeviceControl;
    unsigned char Command;
} ATATaskFile_T;

/* ATA task file register addresses */
#define ATA_DATA_REG        0x50000010
#define ATA_ERR_REG         0x50000030
#define ATA_FEATURES_REG    ATA_ERR_REG
#define ATA_SEC_CNT_REG     0x50000012
#define ATA_SEC_NUM_REG     0x50000032
#define ATA_CYL_LOW_REG     0x50000014
#define ATA_CYL_HI_REG      0x50000034
#define ATA_DEVICE_HEAD_REG 0x50000016
#define ATA_STATUS_REG      0x50000036
#define ATA_COMMAND_REG     ATA_STATUS_REG
#define ATA_ALT_STATUS_REG  0x5000000e
#define ATA_DEVICE_CTL_REG  ATA_ALT_STATUS_REG
#define ATA_DRIVE_ADDR_REG  0x5000002e


/* Feature Register */
#define FEATURE_NODMA 0x00
#define FEATURE_DMA   0x01

/* Drive Select Register */
#define SELECT_MASTER  0x00
#define SELECT_SLAVE 0x10
#define SELECT_LBA 0x40

/* Error Register */
#define ERROR_WP   0x40
#define ERROR_UNC  0x40
#define ERROR_MC   0x20
#define ERROR_IDNF 0x10
#define ERROR_MCR  0x08
#define ERROR_ABRT 0x04
#define ERROR_NM   0x02

/* Error Register for EXECUTE DRIVE DIAGNOSTIC */
#define NO_ERROR_DETECTED 0x01
/* NOTE the following entries may only be valid for CF */
#define FORMATTER_DEVICE_ERROR 0x02
#define SECTOR_BUFFER_ERROR 0x03
#define ECC_CIRCUITRY_ERROR 0x04
#define CONTROLLING_uP_ERROR 0x05

/* Status Register */
#define STATUS_BSY  0x80
#define STATUS_DRDY 0x40
#define STATUS_DRQ  0x08
#define STATUS_ERR  0x01

/* Interrupt Reason Register */
#define INTERRUPT_COMMAND   0x01
#define INTERRUPT_IO        0x02

/* Command Register */
#define COMMAND_DEVICE_RESET  0x08
#define COMMAND_ATAPI_PACKET  0xA0
#define COMMAND_ATAPI_ID      0xA1
#define COMMAND_SLEEP         0xE6

/* Control Register */
#define CONTROL_IRQ           0x00
#define CONTROL_POLLED        0x02
#define CONTROL_SRST          0x04

/* ATA Commands */
#define ATA_DEVICE_RESET              0x08
#define ATA_EXECUTE_DEVICE_DIAGNOSTIC 0x90
#define ATA_IDENTIFY_DEVICE           0xec
#define ATA_MEDIA_EJECT               0xed
#define ATA_IDENTIFY_PACKET_DEVICE    0xa1
#define ATA_PACKET                    0xa0
#define ATA_WRITE_SECTORS             0x30
#define ATA_READ_SECTORS              0x20
#define ATA_CHECK_POWER_MODE          0xe5
#define ATA_SET_SLEEP_MODE            0xe6

#if defined(USE_IRQ)
extern cyg_sem_t ATASem;
#endif /* USE_IRQ */

/* ATA I/O macros */
#define _ReadReg8(reg,var)             \
        var = *((volatile cyg_uint8 *)reg)

#define _WriteReg8(reg,val)            \
        *((volatile cyg_uint8 *)reg) = (val&0xff)

#define _ReadReg16(reg,var)            \
        var = *((volatile cyg_uint16 *)reg)

#define _WriteReg16(reg,val)           \
        *((volatile cyg_uint16 *)reg) = (val&0xffff)

#if USE_MACROS
#define _ReadDataBulk(address,count)   \
      {                               \
        int i;                        \
        for(i=0;i<count;i++)        \
          ((volatile cyg_uint16 *)address)[i]=*((volatile cyg_uint16 *)ATA_DATA_REG);  \
      }

#define _WriteDataBulk(address, count) \
      {                               \
	  int i;                      \
	  for(i=0;i<count;i++)        \
            *((volatile cyg_uint16 *)ATA_DATA_REG) = ((volatile cyg_uint16 *)address)[i];   \
      }
#else /* USE_MACROS */
void _ReadDataBulk(cyg_uint16 * Buffer, unsigned int Length);
void _WriteDataBulk(cyg_uint16 * Buffer, unsigned int Length);
#endif /* USE_MACROS */

#define _ReadError(Variable) _ReadReg8(ATA_ERR_REG,Variable)
#define _ReadSectorCount(Variable) _ReadReg8(ATA_SEC_CNT_REG,Variable)
#define _ReadSectorNumber(Variable) _ReadReg8(ATA_SEC_NUM_REG,Variable)
#define _ReadCylinderLow(Variable) _ReadReg8(ATA_CYL_LOW_REG,Variable)
#define _ReadCylinderHigh(Variable) _ReadReg8(ATA_CYL_HI_REG,Variable)
#define _ReadDeviceHead(Variable) _ReadReg8(ATA_DEVICE_HEAD_REG,Variable)
#define _ReadStatus(Variable) _ReadReg8(ATA_STATUS_REG,Variable)
#define _ReadAlternateStatus(Variable) _ReadReg8(ATA_ALT_STATUS_REG,Variable)

#define _WriteData(Variable) _WriteReg8(ATA_DATA_REG,Variable)
#define _WriteFeatures(Variable) _WriteReg8(ATA_FEATURES_REG,Variable)
#define _WriteSectorCount(Variable) _WriteReg8(ATA_SEC_CNT_REG,Variable)
#define _WriteSectorNumber(Variable) _WriteReg8(ATA_SEC_NUM_REG,Variable)
#define _WriteCylinderLow(Variable) _WriteReg8(ATA_CYL_LOW_REG,Variable)
#define _WriteCylinderHigh(Variable) _WriteReg8(ATA_CYL_HI_REG,Variable)
#define _WriteDeviceHead(Variable) _WriteReg8(ATA_DEVICE_HEAD_REG,Variable)
#define _WriteCommand(Variable) _WriteReg8(ATA_COMMAND_REG,Variable)
#define _WriteDeviceControl(Variable) _WriteReg8(ATA_DEVICE_CTL_REG,Variable)

#define TIMEOUT_5S 500
#define TIMEOUT_1S 100
int PollStatus(unsigned char ClearBits, unsigned char SetBits, cyg_tick_count_t Timeout);

/* Check signature return values */
#define NOT_PACKET_DEVICE 0
#define PACKET_DEVICE     1
int CheckSignature(void);

#endif /* _ATA_H */
