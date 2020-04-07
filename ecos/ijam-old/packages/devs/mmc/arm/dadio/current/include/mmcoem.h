#ifndef __MMCOEM_H__
#define __MMCOEM_H__

#define PD_MMC_CLK1   0x01
#define PD_MMC_CMD1   0x02
#define PD_MMC_DATA1  0x04
#define PD_MMC_CARD1  0x08

#define PD_MMC_CARD2  0x10
#define PD_MMC_CLK2   0x20
#define PD_MMC_CMD2   0x40
#define PD_MMC_DATA2  0x80

#define USE_ASM

// macros for controller 0
#define DDR_DATA1_INPUT()   *(( volatile unsigned char*) PDDDR ) |= PD_MMC_DATA1
#define DDR_DATA1_OUTPUT()  *(( volatile unsigned char*) PDDDR ) &= ~(PD_MMC_DATA1)

#define DDR_CMD1_INPUT()    *(( volatile unsigned char*) PDDR ) &= ~(PD_MMC_CMD1); \
                            *(( volatile unsigned char*) PDDDR ) |= PD_MMC_CMD1

#define DDR_CMD1_OUTPUT()   *(( volatile unsigned char*) PDDDR ) &= ~(PD_MMC_CMD1)

#define DDR_CLK1_INPUT()    *(( volatile unsigned char*) PDDDR ) |= PD_MMC_CLK1
#define DDR_CLK1_OUTPUT()   *(( volatile unsigned char*) PDDDR ) &= ~(PD_MMC_CLK1)

#ifdef USE_ASM
// note that the clock bit must be clear to use these macros
#define MMC_READ_PORT_C0(var,tmp)  \
              asm volatile("mov  r4, #0x80000003;"      \
			   "ldrb r5, [r4];"             \
			   "orr  r6, r5, #0x01;"        \
                           "bic  r5, r5, #0x01;"        \
			   "strb r6, [r4];"             \
			   "ldrb r6, [r4];"             \
			   "strb r5, [r4];"             \
			   "mov  %0, r6"                \
			   : "=r" (var)                 \
			   :                            \
			   : "r4", "r5", "r6" )
#define MMC_WRITE_PORT_C0(var)    \
              asm volatile("mov  r4, #0x80000003;"      \
                           "mov  r5, %0;"               \
                           "orr  r6, r5, #0x01;"        \
                           "bic  r5, r5, #0x01;"        \
                           "strb r5, [r4];"             \
			   "strb r6, [r4];"             \
			   "strb r5, [r4]"              \
                           :                            \
			   : "r" (var)                  \
			   : "r4", "r5", "r6" )

#else

#define MMC_READ_PORT_C0(var,tmp)   tmp = *(( volatile unsigned char* ) PDDR ); \
                                    *(( volatile unsigned char* ) PDDR ) = ( tmp | PD_MMC_CLK1 ); \
                                    var = *(( volatile unsigned char* ) PDDR ); \
                                    *(( volatile unsigned char* ) PDDR ) = ( tmp & ~PD_MMC_CLK1 )
#define MMC_WRITE_PORT_C0(var)      *(( volatile unsigned char* ) PDDR ) = ( var & ~PD_MMC_CLK1); \
                                    *(( volatile unsigned char* ) PDDR ) = ( var | PD_MMC_CLK1 ); \
                                    *(( volatile unsigned char* ) PDDR ) = ( var & ~PD_MMC_CLK1 )
#endif

// macros for controller 1
#define DDR_DATA2_INPUT()   *(( volatile unsigned char*) PDDDR ) |= PD_MMC_DATA2
#define DDR_DATA2_OUTPUT()  *(( volatile unsigned char*) PDDDR ) &= ~(PD_MMC_DATA2)

#define DDR_CMD2_INPUT()    *(( volatile unsigned char*) PDDR ) &= ~(PD_MMC_CMD2); \
                            *(( volatile unsigned char*) PDDDR ) |= PD_MMC_CMD2

#define DDR_CMD2_OUTPUT()   *(( volatile unsigned char*) PDDDR ) &= ~(PD_MMC_CMD2)

#define DDR_CLK2_INPUT()    *(( volatile unsigned char*) PDDDR ) |= PD_MMC_CLK2
#define DDR_CLK2_OUTPUT()   *(( volatile unsigned char*) PDDDR ) &= ~(PD_MMC_CLK2)

#ifdef USE_ASM
#define MMC_READ_PORT_C1(var,tmp)  \
              asm volatile(                             \
                           "mov  r4, #0x80000003;"      \
			   "ldrb r5, [r4];"             \
			   "orr  r6, r5, #0x20;"        \
                           "bic  r5, r5, #0x20;"        \
			   "strb r6, [r4];"             \
			   "ldrb r6, [r4];"             \
			   "strb r5, [r4];"             \
			   "mov  %0, r6"                \
			   : "=r" (var)                 \
			   :                            \
			   : "r4","r5","r6" )
#define MMC_WRITE_PORT_C1(var)    \
              asm volatile("mov  r4, #0x80000003;"      \
                           "mov  r5, %0;"               \
                           "orr  r6, r5, #0x20;"        \
                           "bic  r5, r5, #0x20;"        \
                           "strb r5, [r4];"             \
			   "strb r6, [r4];"             \
			   "strb r5, [r4]"              \
                           :                            \
			   : "r" (var)                  \
			   : "r4", "r5", "r6" )
#else
#define MMC_READ_PORT_C1(var,tmp)   tmp = *(( volatile unsigned char* ) PDDR ); \
                                    *(( volatile unsigned char* ) PDDR ) = ( tmp | PD_MMC_CLK2 ); \
                                    var = *(( volatile unsigned char* ) PDDR ); \
                                    *(( volatile unsigned char* ) PDDR ) = ( tmp & ~PD_MMC_CLK2 )
#define MMC_WRITE_PORT_C1(var)      *(( volatile unsigned char* ) PDDR ) = ( var & ~PD_MMC_CLK2); \
                                    *(( volatile unsigned char* ) PDDR ) = ( var | PD_MMC_CLK2 ); \
                                    *(( volatile unsigned char* ) PDDR ) = ( var & ~PD_MMC_CLK2 )
#endif
// general macros

#if (WORD_ACCESS_ONLY)
#define DUMMY_DATA              0xFFFF
#define BUS_SIZE                16

#else
#define DUMMY_DATA              0xFF
#define BUS_SIZE                8

#endif

#define R2_BYTE_LENGTH          17      /* Length in bytes of MMC card R2 */
#define R2_BIT_LENGTH           136     /* Length in bit of MMC card R2 */
#define BYTE_LENGTH             6       /* Length in bytes of MMC responses  */
#define R1R3_BYTE_LENGTH        6       /* Length in byte of MMC card R1, R3 */
#define R1R3_BIT_LENGTH         48      /* Length in bits of MMC responses R1, R3 */


#ifdef __cplusplus
extern "C" {
#endif
  
#include "sdapi.h"
  
    // functions for controller 0 in mmcoem_c0.c

  MMC_CC  MMCExchangeData_c0( UCHAR odata, UCHAR* idata );
  UCHAR   MMCGetData_c0( SDVOID );
  SDVOID  MMCSendData_c0( UCHAR odata );
  SDVOID  MMCSendCommand_c0( UINT32 Arg, UINT16 Cmd, UINT16 crcData );
  MMC_CC  MMCReceive_c0( UCHAR* dBuf, UINT16 dataLength );
  MMC_CC  MMCTransmit_c0( UCHAR* dBuf, UINT16 dataLength );
  MMC_CC  mmc_get_response_c0( UCHAR* resp_bytes, UINT16 respBitLength );

    // slow functions for controller 0
  MMC_CC  MMCExchangeDataSlow_c0( UCHAR odata, UCHAR* idata );
  SDVOID  MMCSendCommandSlow_c0( UINT32 Arg, UINT16 Cmd, UINT16 crcData );
  MMC_CC  mmc_get_response_slow_c0( UCHAR* dBuf, UINT16 dataLength );
  
    // functions for controller 1 in mmcoem_c1.c

  MMC_CC  MMCExchangeData_c1( UCHAR odata, UCHAR* idata );
  UCHAR   MMCGetData_c1( SDVOID );
  SDVOID  MMCSendData_c1( UCHAR odata );
  SDVOID  MMCSendCommand_c1( UINT32 Arg, UINT16 Cmd, UINT16 crcData );
  MMC_CC  MMCReceive_c1( UCHAR* dBuf, UINT16 dataLength );
  MMC_CC  MMCTransmit_c1( UCHAR* dBuf, UINT16 dataLength );
  MMC_CC  mmc_get_response_c1( UCHAR* resp_bytes, UINT16 respBitLength );
  
    // slow functions for controller 1
  MMC_CC  MMCExchangeDataSlow_c1( UCHAR odata, UCHAR* idata );
  SDVOID  MMCSendCommandSlow_c1( UINT32 Arg, UINT16 Cmd, UINT16 crcData );
  MMC_CC  mmc_get_response_slow_c1( UCHAR* dBuf, UINT16 dataLength );
  

    // generic routines in mmcoem.c
  SDVOID  mmcPowerOnOff( INT16 controllerno, INT16 power_state );
  SDVOID  mmc_hw_init( INT16 controllerno );
  SDVOID  resetMMCController( UINT16 ctrlno );
  SDVOID  startMMCClock( SDVOID );
  SDBOOL  setMMCClockRate( UINT16 clkrate );
  SDVOID  startMMC80Clocks( INT16 controller_no );
  SDVOID  MMCSelectController( INT16 ctrlNo );
  SDBOOL  MMCPrepareAndSetup( UINT32 Arg, UINT16 Cmd, UINT16 noBlocks, UINT16 Resp );
  MMC_CC  getMMCResponseInfo( UCHAR* pRespBuff, UINT16 respLength, UINT16 respType );
  MMC_CC  checkCardBusy( INT16 controller_no );
  
#ifdef __cplusplus
};
#endif // __cplusplus

#endif /* __MMCOEM_H__ */
