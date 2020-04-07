//=============================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// INSERT COPYRIGHT HERE
// -------------------------------------------
//
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   toddm
// Contributors:
// Date:        2000-06-12
// Purpose:     SCSI block commands constants, structures
//              
//####DESCRIPTIONEND####
//
//=============================================================================
#ifndef RBC_H
#define RBC_H
/* SPC commands */
/* operation codes */
#define FORMAT_UNIT                  0x04
#define INQUIRY                      0x12
#define MODE_SELECT                  0x15
#define MODE_SENSE                   0x1a
#define PERSISTENT_RESERVE_IN        0x5e
#define PERSISTENT_RESERVE_OUT       0x5f
#define PREVENT_ALLOW_MEDIUM_REMOVAL 0x1e
#define READ                         0x28
#define READ_CAPACITY                0x25
#define RELEASE                      0x17
#define REQUEST_SENSE                0x03
#define RESERVE                      0x16
#define START_STOP_UNIT              0x1b
#define SYNCHRONIZE_CACHE            0x35
#define TEST_UNIT_READY              0x00
#define VERIFY                       0x2f
#define WRITE                        0x2a
#define WRITE_BUFFER                 0x3b
#define READ_FORMAT_CAPACITIES       0x23 /* this is documented in the clik drive book */

/* SPC inquiry values */
#define PERIPHERAL_QUALIFIER_CONNECTED          0x00
#define PERIPHERAL_QUALIFIER_NOT_CONNECTED      0x01
#define PERIPHERAL_QUALIFIER_RESERVED           0x02
#define PERIPHERAL_QUALIFIER_NOT_CAPABLE        0x03
#define PERIPHERAL_DEVICE_TYPE_DIRECT_ACCESS    0x00
#define PERIPHERAL_DEVICE_TYPE_RBC              0x0E
#define RMB_NOT_REMOVABLE                       0x00
#define RMB_REMOVABLE                           0x01
#define VERSION_NO_CONFORMANCE                  0x00
#define VERSION_T10_1236D                       0x04
#define AERC_NOT_SUPPORTED                      0x00
#define AERC_SUPPORTED                          0x01
#define NORMACA_NOT_SUPPORTED                   0x00
#define NORMACA_SUPPORTED                       0x01
#define HISUP_NOT_SUPPORTED                     0x00
#define HISUP_SUPPORTED                         0x01
#define RESPONSE_DATA_FORMAT_T10_1236D          0x02
#define SCCS_NOT_SUPPORTED                      0x00
#define CMD_QUEUEING_NOT_SUPPORTED              0x00
#define ENCSERV_NO_COMPONENT                    0x00
#define MULTIP_NOT_MULTIPORT                    0x00
#define MCHNGR_NOT_IN_MEDIA_CHANGER             0x00

/* REQUEST SENSE data format values */
#define INFORMATION_IS_VALID                    0x01
#define CURRENT_ERRORS                          0x70
#define DEFERRED_ERRORS                         0x71
#define EOM_CONDITION_EXISTS                    0x01
#define INCORRECT_LENGTH_INDICATOR              0x01
#define NO_SENSE                                0x00
#define RECOVERED_ERROR                         0x01
#define NOT_READY                               0x02
#define MEDIUM_ERROR                            0x03
#define HARDWARE_ERROR                          0x04
#define ILLEGAL_REQUEST                         0x05
#define UNIT_ATTENTION                          0x06
#define DATA_PROTECT                            0x07
#define BLANK_CHECK                             0x08
#define VENDOR_SPECIFIC                         0x09
#define COPY_ABORTED                            0x0a
#define ABORTED_COMMAND                         0x0b
#define VOLUME_OVERFLOW                         0x0d
#define MISCOMPARE                              0x0e
#define UNIT_HAS_FAILED                         0x01

/* READ FORMAT CAPACITY */
/* capacity descriptors */
#define RESERVED                                0x00
#define UNFORMATTED_MEDIA                       0x01
#define FORMATTED_MEDIA                         0x02
#define NO_MEDIA_PRESENT                        0x03

/* PREVENT ALLOW MEDIUM REMOVAL */
#define PREVENT_FROM_DATA_TRANSPORT             0x01
#define PREVENT_FROM_MEDIA_CHANGER              0x02

/* vital product data page codes */
#define NUMBER_SUPPORTED_PAGES                  0x02
#define SUPPORTED_VPD_PAGES                     0x00
#define UNIT_SERIAL_NUMBER_PAGE                 0x80

/* ASC and ASCQ assignments */
#define ASC(X) (((X) & 0xff00) >> 8)
#define ASCQ(X) ((X) & 0x00ff)
#define RESET_OCCURRED                        0x2900
#define INVALID_FIELD_IN_CDB                  0x2400
#define NO_ADDITIONAL_SENSE_INFORMATION       0x0000
#define INVALID_COMMAND_OPERATION_CODE        0x2000
#define MEDIUM_NOT_PRESENT                    0x3a00
#define MEDIUM_CHANGED                        0x2800
#define WRITE_ERROR                           0x0c00
#define READ_ERROR                            0x1100
#define LOGICAL_UNIT_NOT_SUPPORTED            0x2500

/* optional */
typedef struct rbc_cmd_format_unit_s
{
    unsigned char operation_code;
    unsigned char reserved[3];	/* this could be 4, may be a typo in the spec */
    unsigned char control;
}__attribute__((packed)) rbc_cmd_format_unit_t;

/* mandatory */
typedef struct inquiry_cmd_s
{
    unsigned char operation_code;
    unsigned char evpd      :1;
    unsigned char cmddt     :1;
    unsigned char reserved0 :6;
    unsigned char page_op_code;
    unsigned char reserved1;
    unsigned char allocation_length;
    unsigned char control;
}__attribute__((packed)) inquiry_cmd_t;

typedef struct inquiry_data_s
{
    unsigned char peripheral_device_type :5;
    unsigned char peripheral_qualifier   :3;

    unsigned char reserved_0             :7;
    unsigned char rmb                    :1;

    unsigned char version;

    unsigned char response_data_format   :4;
    unsigned char hisup                  :1;
    unsigned char normaca                :1;
    unsigned char obsolete_0             :1;
    unsigned char aerc                   :1;

    unsigned char additional_length;

    unsigned char reserved_1             :7;
    unsigned char sccs                   :1;

    unsigned char addr16                 :1;
    unsigned char obsolete_1             :1;
    unsigned char obsolete_2             :1;
    unsigned char mchngr                 :1;
    unsigned char multip                 :1;
    unsigned char vs_0                   :1;
    unsigned char encserv                :1;
    unsigned char bque                   :1;

    unsigned char vs_1                   :1;
    unsigned char cmdque                 :1;
    unsigned char trandis                :1;
    unsigned char linked                 :1;
    unsigned char sync                   :1;
    unsigned char wbus16                 :1;
    unsigned char obsolete_3             :1;
    unsigned char reladr                 :1;

    char vendor_identification[8];
    char product_identification[16];
    char product_revision_level[4];
}__attribute__((packed)) inquiry_data_t;

typedef struct supported_vital_product_data_pages_s
{
    unsigned char peripheral_device_type :5;
    unsigned char peripheral_qualifier   :3;

    unsigned char page_code;
    unsigned char reserved0;
    unsigned char page_length;	/* n - 3 */
    unsigned char supported_page_list[0];
}__attribute__((packed)) supported_vital_product_data_pages_t;

typedef struct unit_serial_number_page_s
{
    unsigned char peripheral_device_type :5;
    unsigned char peripheral_qualifier   :3;

    unsigned char page_code;
    unsigned char reserved0;
    unsigned char page_length;	/* n - 3 */
    unsigned char product_serial_number[0];
}__attribute__((packed)) unit_serial_number_page_t;

/* mandatory */
typedef struct spc_cmd_mode_select_s
{
    unsigned char operation_code;
}__attribute__((packed)) spc_cmd_mode_select_t;

/* mandatory */
typedef struct mode_sense_cmd_s
{
    unsigned char operation_code;
    
    unsigned char reserved0 :3;
    unsigned char dbd       :1;
    unsigned char reserved1 :4;
    
    unsigned char page_code :6;
    unsigned char pc        :2;
    
    unsigned char reserved2;
    unsigned char allocation_length;
    unsigned char control;
}__attribute__((packed)) mode_sense_cmd_t;

typedef struct mode_sense_header_s
{
    unsigned char mode_data_length;
    unsigned char medium_type;
    unsigned char device_specific_parameter;
    unsigned char block_descriptor_length;
}__attribute__((packed)) mode_sense_header_t;

typedef struct mode_sense_da_block_descriptor_s /* for direct access device mode*/
{
    int number_of_blocks;	/* big-endian */
    unsigned char density_code;
    unsigned char msb_block_length; /* block length is a 3 byte parameter, big-endian */
    short block_length;
}__attribute__((packed)) mode_sense_da_block_descriptor_t;

typedef struct rw_error_recovery_page_s
{
    unsigned char page_code :6;
    unsigned char reserved0 :1;
    unsigned char ps        :1;
    unsigned char page_length;
    unsigned char dcr       :1;
    unsigned char dte       :1;
    unsigned char per       :1;
    unsigned char eer       :1;
    unsigned char rc        :1;
    unsigned char tb        :1;
    unsigned char arre      :1;
    unsigned char awre      :1;
    unsigned char read_retry_count;
    /* TODO here's where a difference between the sbc and iomega differ
     * i'll use iomega since that's the only reference i have */
#if 1 /* iomega version */
    unsigned char reserved1[4];
#else /* sbc version */
    unsigned char correction_span;
    unsigned char head_offset_count;
    unsigned char data_strobe_offset_count;
    unsigned char reserved1;
    unsigned char write_retry_count;
    unsigned char reserved2;
    short recover_time_limit; /* big-endian */
#endif
}__attribute__((packed)) rw_error_recovery_page_t;    

typedef struct flexible_disk_page_s
{
    unsigned char page_code  :6;
    unsigned char reserved   :1;    
    unsigned char ps         :1;    
    unsigned char page_length;
    short transfer_rate;	/* big-endian */
    unsigned char number_of_heads;
    unsigned char sectors_per_track;
    short data_bytes_per_sector; /* big-endian */
    short number_of_cylinders;	/* all shorts big-endian */
    short start_cyl_write_precompensation;
    short start_cyl_reduced_write_current;
    short device_step_rate;
    unsigned char device_step_pulse_width;
    short head_settle_delay;
    unsigned char motor_on_delay;
    unsigned char motor_off_delay;
    
    unsigned char reserved0 :5;
    unsigned char mo        :1;
    unsigned char ssn       :1;
    unsigned char trdy      :1;
    
    unsigned char spc       :4;
    unsigned char reserved1 :4;
    
    unsigned char write_compensation;
    unsigned char head_load_delay;
    unsigned char head_unload_delay;
    
    unsigned char pin_2     :4;
    unsigned char pin_34    :4;
    
    unsigned char pin_1     :4;
    unsigned char pin_4     :4;
    
    short medium_rotation_rate;
    unsigned char reserved2;
    unsigned char reserved3;
}__attribute__((packed)) flexible_disk_page_t;

typedef struct caching_page_s
{
    unsigned char page_code :6;
    unsigned char reserved0 :1;
    unsigned char ps        :1;
    
    unsigned char page_length;
    
    unsigned char rcd       :1;
    unsigned char mf        :1;
    unsigned char wce       :1;
    unsigned char size      :1;
    unsigned char disc      :1;
    unsigned char cap       :1;
    unsigned char abpf      :1;
    unsigned char ic        :1;
    
    unsigned char write_retention_priority       :4;
    unsigned char demand_read_retention_priority :4;
    short disable_pre_fetch_transfer_length;
    short minimum_pre_fetch;
    short maximum_pre_fetch;
    short maximum_pre_fetch_ceiling;
#if 0 /* the iomega version doesn't appear to use these values */
    unsigned char reserved1 :3;
    unsigned char vs0       :1;
    unsigned char vs1       :1;
    unsigned char dra       :1;
    unsigned char lbcss     :1;
    unsigned char fsw       :1;
    unsigned char number_of_cache_segments;
    short cache_segment_size;
    unsigned char reserved2;
    unsigned char non_cache_segment_size_msb; /* 3 byte value */
    short non_cache_segment_size;
#endif
}__attribute__((packed)) caching_page_t;

/* this is the vendor specific page as defined by iomega
 * in the clik docs */
typedef struct vendor_specific_page_s
{
    unsigned char page_code :6;
    unsigned char reserved0 :2;
    
    unsigned char page_length;
    
    unsigned char phsk      :1;
    unsigned char reserved1 :1;
    unsigned char offtr     :1;
    unsigned char ridi      :1;
    unsigned char ftme      :1;
    unsigned char vsc       :1;
    unsigned char scrub     :1;
    unsigned char dhs       :1;
    
    unsigned char spin_down_timer;
    unsigned char head_park_timer;
    
    unsigned char seek_retry_count :4;
    unsigned char reserved2        :4;
}__attribute__((packed)) vendor_specific_page_t;

/* optional */
typedef struct spc_cmd_persistent_reserve_in_s
{
}__attribute__((packed)) spc_cmd_persistent_reserve_in_t;

/* optional */
typedef struct spc_cmd_persistent_reserve_out_s
{
    unsigned char operation_code;
}__attribute__((packed)) spc_cmd_persistent_reserve_out_t;

/* mandatory for removable media */
typedef struct prevent_allow_medium_removal_cmd_s
{
    unsigned char operation_code;
    unsigned char reserved0[3];
    unsigned char prevent   :2;
    unsigned char reserved1 :6;
    unsigned char control;
}__attribute__((packed)) prevent_allow_medium_removal_cmd_t;

/* mandatory */
typedef struct read_cmd_s
{
    unsigned char operation_code;
    unsigned char reserved0;
    int lba;			/* this is big-endian */
    unsigned char reserved1;
    short transfer_length;	/* this is big-endian */
    unsigned char control;
}__attribute__((packed)) read_cmd_t;

/* mandatory */
typedef struct read_capacity_cmd_s
{
    unsigned char operation_code;
    unsigned char reserved[8];
    unsigned char control;
}__attribute__((packed)) read_capacity_cmd_t;

typedef struct read_capacity_data_s
{
    int lba;			/* this is big-endian */
    int blk_len;		/* this is big-endian */
}__attribute__((packed)) read_capacity_data_t;

/* optional */
typedef struct spc_cmd_release_s
{
    unsigned char operation_code;
}__attribute__((packed)) spc_cmd_release_t;

/* optional */
typedef struct request_sense_cmd_s
{
    unsigned char operation_code;
    unsigned char reserved[3];
    unsigned char allocation_length;
    unsigned char control;
}__attribute__((packed)) request_sense_cmd_t;

typedef struct request_sense_data_s
{
    unsigned char response_code   :7;
    unsigned char valid           :1;
    
    unsigned char segment_number;
    
    unsigned char sense_key       :4;
    unsigned char reserved0       :1;
    unsigned char ili             :1;
    unsigned char eom             :1;
    unsigned char filemark        :1;
    
    int information;
    unsigned char additional_length;
    int command_specific_information;
    unsigned char asc;
    unsigned char ascq;
    unsigned char field_replacable_unit_code;
    unsigned char sense_key_specific[3];
}__attribute__((packed)) request_sense_data_t;

/* optional */
typedef struct spc_cmd_reserve_s
{
    unsigned char operation_code;
}__attribute__((packed)) spc_cmd_reserve_t;

/* mandatory */
typedef struct rbc_cmd_start_stop_unit_s
{
    unsigned char operation_code;
    unsigned char immed            :1;
    unsigned char reserved0        :7;
    unsigned char reserved1[2];
    unsigned char start            :1;
    unsigned char loej             :1;
    unsigned char reserved2        :2;
    unsigned char power_conditions :4;
    unsigned char control;
}__attribute__((packed)) rbc_cmd_start_stop_unit_t;

/* optional */
typedef struct rbc_cmd_synchronize_cache_s
{
    unsigned char operation_code;
    unsigned char reserved[8];
    unsigned char control;
}__attribute__((packed)) rbc_cmd_synchronize_cache_t;

/* mandatory */
typedef struct test_unit_ready_cmd_s
{
    unsigned char operation_code;
    unsigned char reserved[4];
    unsigned char control;
}__attribute__((packed)) test_unit_ready_cmd_t;

/* mandatory */
typedef struct rbc_cmd_verify_s
{
    unsigned char operation_code;
    unsigned char reladr    : 1;
    unsigned char bytchk    : 1;
    unsigned char reserved0 : 2;
    unsigned char dpo       : 1;
    unsigned char reserved1 : 3;
    int lba;			/* this is big-endian */
    unsigned char reserved2;
    short verification_length;	/* this is big-endian */
    unsigned char control;
}__attribute__((packed)) rbc_cmd_verify_t;

/* mandatory */
typedef struct write_cmd_s
{
    unsigned char operation_code;
    
    unsigned char reserved0 :3;
    unsigned char fua       :1;
    unsigned char reserved1 :4;
    
    int lba;			/* this is big-endian */
    unsigned char reserved2;
    short transfer_length;	/* this is big-endian */
    unsigned char control;
}__attribute__((packed)) write_cmd_t;

/* optional for removable media */
typedef struct spc_cmd_write_buffer_s
{
    unsigned char operation_code;
}__attribute__((packed)) spc_cmd_write_buffer_t;

/* documented in clik drive book */
typedef struct read_format_capacities_cmd_s
{
    unsigned char operation_code;
    unsigned char reserved0[6];
    short allocation_length;
    unsigned char control;
}__attribute__((packed)) read_format_capacities_cmd_t;

typedef struct capacity_list_header_s
{
    unsigned char reserved0[3];
    unsigned char list_length;
}__attribute__((packed)) capacity_list_header_t;

typedef struct current_capacity_s
{
    int number_of_blocks;    
    unsigned char descriptor_code :2;
    unsigned char reserved0       :6;
    unsigned char block_length_msb; /* block length is 3 bytes */
    short block_length;
}__attribute__((packed)) current_capacity_t;

typedef struct formattable_capacity_s
{
    int number_of_blocks;
    unsigned char reserved0   :2; 
    unsigned char format_type :6;
    unsigned char param_msb; /* param is 3 bytes and the meaning depends on format_type */
    short param;
}__attribute__((packed)) formattable_capacity_t;

/* the following is a header of an rbc command that will then
 * be cast to the appropriate struct depending on the operation code */
typedef struct rbc_cmd_s
{
    unsigned char operation_code;
}__attribute__((packed)) rbc_cmd_t;

bool rbc_handler(cbw_t * cbw, csw_t * csw);

int spc_inquiry(unsigned char lun, inquiry_cmd_t * cmd, char * buf, int len);
int spc_read_format_capacities(unsigned char lun, read_format_capacities_cmd_t * cmd, char * buf, int len);
int spc_request_sense(unsigned char lun, request_sense_cmd_t * cmd, char * buf, int len);
int spc_mode_sense(unsigned char lun, mode_sense_cmd_t * cmd, char * buf, int len);
int spc_read_capacity(unsigned char lun, read_capacity_cmd_t * cmd, char * buf, int len);
int spc_read(unsigned char lun, read_cmd_t * cmd, char * buf, int len);
int spc_write(unsigned char lun, write_cmd_t * cmd, char * buf, int len);

#endif /* RBC_H */
