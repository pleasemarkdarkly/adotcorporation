/* mpgdata.h
 * Type definition for MPEG Audio decoder instance data
 *
 * Copyright (C) ARM Limited 1999. All Rights Reserved.
 */

#ifndef _MPGDATA_H_
#define _MPGDATA_H_

 /*******************************************************************
 * NOTE: For audio layer III, the instance data MUST be aligned on  *
 * a 2048-byte boundary. One instance has already been defined.     *
 * This should be adequate for most applications.                   *
 *******************************************************************/

typedef unsigned char tMPEGInstance[15060];

extern tMPEGInstance MPEGInstance;

#endif
