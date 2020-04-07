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
// Purpose:     Mass storage class constants, structures
//              
//####DESCRIPTIONEND####
//
//=============================================================================
#ifndef MASS_STORAGE_H
#define MASS_STORAGE_H

#define MAX_LUN 2

#define CBW_SIGNATURE 0x43425355
#define CSW_SIGNATURE 0x53425355

#define CBW_FLAGS_DIRECTION    0x80 /* 0 = data out, 1 = data in */

#define CSW_STATUS_PASS        0x00
#define CSW_STATUS_FAIL        0x01
#define CSW_STATUS_PHASE_ERROR 0x02

typedef struct cbw_s		/* command block wrapper */
{
    unsigned int dCBWSignature;
    unsigned int dCBWTag;
    int dCBWDataTransferLength;
    unsigned char bmCBWFlags;
    char bCBWLUN                :4;
    unsigned char reserved0     :4;
    char bCBWCBLength           :5;
    unsigned char reserved1     :3;
    char CBWCB[16];
}__attribute__((packed)) cbw_t;

typedef struct csw_s
{
    unsigned int dCSWSignature;
    unsigned int dCSWTag;
    int dCSWDataResidue;
    unsigned char bCSWStatus;
}__attribute__((packed)) csw_t;

void bulk_handler(void);

#endif /* MASS_STORAGE_H */
