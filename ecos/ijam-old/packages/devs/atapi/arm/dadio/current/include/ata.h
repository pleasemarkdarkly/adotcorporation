#ifndef ATA_H
#define ATA_H

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_edb7xxx.h>
#include <dadio/io/atapi.h>

extern unsigned char sleep_flag;

typedef struct ata_task_file_s
{
    /* transfer data */
    unsigned short * data;
    unsigned int data_len;	/* NOTE data_len is number of 16bit words */
    
    /* task file registers */
    unsigned char features;
    unsigned char sec_cnt;
    unsigned char sec_num;
    unsigned char cyl_low;
    unsigned char cyl_hi;
    unsigned char device_head;
    unsigned char device_ctl;
    unsigned char cmd;
} ata_task_file_t;

/* capabilites word in identify info */
#define STANDBY_TIMER_SUPPORT 0x2000
#define IORDY_SUPPORT         0x0800
#define LBA_SUPPORT           0x0200
#define DMA_SUPPORT           0x0100

typedef struct ata_identify_device_s
{
    /* any comments regarding retired, obsolete are in reference to ATA/ATAPI-5.
     * parameters are taken from CFA spec at that point */
    unsigned short general_config;
    unsigned short num_cyl;
    unsigned short specific_config;
    unsigned short num_hds;
    unsigned short num_unformatted_bytes_trk; /* retired */
    unsigned short num_unformatted_bytes_sec; /* retired */
    unsigned short num_sec_trk;
    unsigned int num_sec_card; /* CF only, word 7 = MSW, word 8 = LSW */
    unsigned short vendor0;
    unsigned char serial_num[20]; /* right justified */
    unsigned short buffer_type;	/* retired */
    unsigned short buffer_size;	/* buffer size in 512 byte increments, retired */
    unsigned short num_ecc_bytes; /* extra bytes passed on read,write long commands, obsolete */
    unsigned char firmware_rev[8];
    unsigned char model_num[40]; /* left justified */
    unsigned short max_rw_mult_sec_cnt;
    unsigned short dbl_word_support; /* reserved */
    unsigned short capabilities;
    unsigned short capabilities1; /* CF marks this as reserved */
    unsigned short pio_timing;	/* obsolete */
    unsigned short dma_timing;	/* obsolete */
    unsigned short translation_params_valid;
    unsigned short cur_num_cyl;
    unsigned short cur_num_hds;
    unsigned short cur_sec_trk;
    unsigned int total_sec;	/* word 57 = LSW, word 58 = MSW */
    unsigned short mult_sec_setting;
    unsigned int total_lba_sec;
    unsigned short reserved0[66]; /* TODO fill this out per ATA/ATAPI-5 spec */
    unsigned short security_status; /* TODO beginning with reserved0, the total bytes column and the word address do
				     * not agree in terms of size in the cf spec , so i'm using word address since
				     * total bytes only adds up to 500 */
    unsigned short vendor1[31];
    unsigned short power_mode;
    unsigned short reserved1[95];
}__attribute__((packed)) ata_identify_device_t;

/* TODO generalize this */
#define MediaPresent() (~(*((volatile cyg_uint8 *)PBDR) & 0x40)) /* nCD1, for compact flash */

unsigned char init_ata(void);
unsigned char read_ata(unsigned int lba, unsigned short length, unsigned char * data);
unsigned char write_ata(unsigned int lba, unsigned short length, unsigned char * data);

unsigned char ata_identify(ata_identify_device_t * data);
unsigned char ata_get_geometry(drive_geometry_t * dg);
/* values for data param to ata_check_power_mode */
#define STANDY_POWER_MODE         0x00
#define ACTIVE_OR_IDLE_POWER_MODE 0xff
#define IDLE_POWER_MODE           0x80
unsigned char ata_check_power_mode(unsigned char * data);

#endif /* ATA_H */
