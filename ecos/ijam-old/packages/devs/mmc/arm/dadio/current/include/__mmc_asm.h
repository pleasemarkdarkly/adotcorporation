#ifndef __MMC_ASM_H__
#define __MMC_ASM_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*mmc_fp)(char*,int);
  
void mmc_write_cmd_c0( char* buf, int len );
void mmc_read_resp_c0( char* buf, int len );
void mmc_write_data_c0( char* buf, int len );
void mmc_read_data_c0( char* buf, int len );
int  mmc_exchange_data_c0( unsigned char odata, unsigned char* idata);
  
void mmc_write_cmd_c1( char* buf, int len );
void mmc_read_resp_c1( char* buf, int len );
void mmc_write_data_c1( char* buf, int len );
void mmc_read_data_c1( char* buf, int len );
int  mmc_exchange_data_c1( unsigned char odata, unsigned char* idata);
  

#ifdef __cplusplus
};
#endif

#endif
