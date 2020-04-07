#ifndef _ATAPI_H
#define _ATAPI_H

#define MAX_PACKET_SIZE 16

/* Sense key codes */
#define UNIT_ATTENTION 0x06

/* SPC - SCSI primary command
 * SBC - SCSI block command
 * MMC - Multimedia command */

#define ATAPI_TEST_UNIT_READY 0x00 /* SPC */
#define ATAPI_REQUEST_SENSE 0x03 /* SPC */
#define ATAPI_INQUIRY 0x12	/* SPC */
#define ATAPI_START_STOP_UNIT 0x1b /* SBC */
#define ATAPI_READ10 0x28	/* SBC */
#define ATAPI_WRITE10 0x2a	/* SBC */
#define ATAPI_READ_TOC_PMA_ATIP 0x43 /* MMC p247 MMC-2 */
#define ATAPI_PLAY_AUDIO10 0x45 /* MMC p187 MMC-2 */
#define ATAPI_PAUSE_RESUME 0x46 /* MMC p186 MMC-2 */
#define ATAPI_PLAY_AUDIO_MSF 0x47 /* MMC p190 MMC-2 */
#define ATAPI_READ_DISC_INFORMATION 0x51 /* MMC p209 MMC-2 */

#endif /* _ATAPI_H */
