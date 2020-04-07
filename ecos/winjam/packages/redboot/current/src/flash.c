//==========================================================================
//
//      flash.c
//
//      RedBoot - FLASH memory support
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-07-28
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <redboot.h>
#include <cyg/io/flash.h>
#include <fis.h>

// Exported CLI functions
RedBoot_cmd("fis", 
            "Manage FLASH images", 
            "{cmds}",
            do_fis
    );

// Internal commands
local_cmd_entry("init",
                "Initialize FLASH Image System [FIS]",
                "[-f]",
                fis_init,
                FIS_cmds
    );

#ifdef CYGSEM_REDBOOT_FIS_CRC_CHECK
# define FIS_LIST_OPTS "[-c] [-d]"
#else
# define FIS_LIST_OPTS "[-d]"
#endif

local_cmd_entry("list",
                "Display contents of FLASH Image System [FIS]",
		FIS_LIST_OPTS,
                fis_list,
                FIS_cmds
    );
local_cmd_entry("free",
                "Display free [available] locations within FLASH Image System [FIS]",
                "",
                fis_free,
                FIS_cmds
    );
local_cmd_entry("erase",
                "Erase FLASH contents",
                "-f <flash_addr> -l <length>",
                fis_erase,
                FIS_cmds
    );
#if 0 < CYGHWR_IO_FLASH_BLOCK_LOCKING // This is an *interface*
local_cmd_entry("lock",
                "LOCK FLASH contents",
                "-f <flash_addr> -l <length>",
                fis_lock,
                FIS_cmds
    );
local_cmd_entry("unlock",
                "UNLOCK FLASH contents",
                "-f <flash_addr> -l <length>",
                fis_unlock,
                FIS_cmds
    );
#endif
local_cmd_entry("delete",
                "Display an image from FLASH Image System [FIS]",
                "name",
                fis_delete,
                FIS_cmds
    );

static char fis_load_usage[] = 
#ifdef CYGPKG_COMPRESS_ZLIB
                      "[-d] "
#endif
                      "[-b <memory_load_address>] [-c] name";

local_cmd_entry("load",
                "Load image from FLASH Image System [FIS] into RAM",
                fis_load_usage,
                fis_load,
                FIS_cmds
    );
local_cmd_entry("create",
                "Create an image",
                "-b <mem_base> -l <image_length> [-s <data_length>]\n"
                "      [-f <flash_addr>] [-e <entry_point>] [-r <ram_addr>] [-n] <name>",
                fis_create,
                FIS_cmds
    );
local_cmd_entry("write",
                "Write raw data directly to FLASH",
                "-f <flash_addr> -b <mem_base> -l <image_length>",
                fis_write,
                FIS_cmds
    );

// Define table boundaries
CYG_HAL_TABLE_BEGIN( __FIS_cmds_TAB__, FIS_cmds);
CYG_HAL_TABLE_END( __FIS_cmds_TAB_END__, FIS_cmds);

extern struct cmd __FIS_cmds_TAB__[], __FIS_cmds_TAB_END__;

// Local data used by these routines
static void *flash_start, *flash_end;
static int block_size, blocks;
static void *fis_work_block;

struct fis_image_desc *
fis_lookup(char *name)
{
    int i;
    void *fis_addr;
    struct fis_image_desc *img;

    fis_addr = (void *)((unsigned long)flash_end - block_size);
    memcpy(fis_work_block, fis_addr, block_size);
    img = (struct fis_image_desc *)fis_work_block;
    for (i = 0;  i < block_size/sizeof(*img);  i++, img++) {
        if ((img->name[0] != (unsigned char)0xFF) && (strcmp(name, img->name) == 0)) {
            return img;
        }
    }
    return (struct fis_image_desc *)0;
}

static void
fis_usage(char *why)
{
    printf("*** invalid 'fis' command: %s\n", why);
    cmd_usage(__FIS_cmds_TAB__, &__FIS_cmds_TAB_END__, "fis ");
}

static void
fis_init(int argc, char *argv[])
{
    int stat;
    struct fis_image_desc *img;
    void *fis_base, *err_addr;
#ifdef CYGSEM_REDBOOT_CONFIG
    void *cfg_base;
#endif
    bool full_init = false;
    struct option_info opts[1];
    unsigned long redboot_image_size, redboot_flash_start;

    init_opts(&opts[0], 'f', false, OPTION_ARG_TYPE_FLG, 
              (void **)&full_init, (bool *)0, "full initialization, erases all of flash");
    if (!scan_opts(argc, argv, 2, opts, 1, 0, 0, ""))
    {
        return;
    }

    if (!verify_action("About to initialize [format] FLASH image system")) {
        printf("** Aborted\n");
        return;
    }
    printf("*** Initialize FLASH Image System\n");

#define MIN_REDBOOT_IMAGE_SIZE CYGBLD_REDBOOT_MIN_IMAGE_SIZE
    redboot_image_size = block_size > MIN_REDBOOT_IMAGE_SIZE ? block_size : MIN_REDBOOT_IMAGE_SIZE;

    // Create a pseudo image for RedBoot
    img = (struct fis_image_desc *)fis_work_block;
#ifdef CYGOPT_REDBOOT_FIS_RESERVED_BASE
    memset(img, 0, sizeof(*img));
    strcpy(img->name, "(reserved)");
    img->flash_base = (unsigned long)flash_start;
    img->mem_base = (unsigned long)flash_start;
    img->size = CYGNUM_REDBOOT_FLASH_RESERVED_BASE;
    img++;
#endif
    redboot_flash_start = (unsigned long)flash_start + CYGBLD_REDBOOT_FLASH_BOOT_OFFSET;
#ifdef CYGOPT_REDBOOT_FIS_REDBOOT
    memset(img, 0, sizeof(*img));
    strcpy(img->name, "RedBoot");
    img->flash_base = redboot_flash_start;
    img->mem_base = redboot_flash_start;
    img->size = redboot_image_size;
    img++;
    redboot_flash_start += redboot_image_size;
#endif
#ifdef CYGOPT_REDBOOT_FIS_REDBOOT_POST
#ifdef CYGNUM_REDBOOT_FIS_REDBOOT_POST_OFFSET
    // Take care to place the POST entry at the right offset:
    redboot_flash_start = (unsigned long)flash_start + CYGNUM_REDBOOT_FIS_REDBOOT_POST_OFFSET;
#endif
    memset(img, 0, sizeof(*img));
    strcpy(img->name, "RedBoot[post]");
    img->flash_base = redboot_flash_start;
    img->mem_base = redboot_flash_start;
    img->size = redboot_image_size;
    img++;
    redboot_flash_start += redboot_image_size;
#endif
#ifdef CYGOPT_REDBOOT_FIS_REDBOOT_BACKUP
    // And a backup image
    memset(img, 0, sizeof(*img));
    strcpy(img->name, "RedBoot[backup]");
    img->flash_base = redboot_flash_start;
    img->mem_base = redboot_flash_start;
    img->size = redboot_image_size;
    img++;
    redboot_flash_start += redboot_image_size;
#endif
#ifdef CYGIMP_REDBOOT_CONFIG_STORE_FLASH
    // And a descriptor for the configuration data
    memset(img, 0, sizeof(*img));
    strcpy(img->name, "RedBoot config");
    cfg_base = (void *)((unsigned long)flash_end - (2*block_size));
    img->flash_base = (unsigned long)cfg_base;
    img->mem_base = (unsigned long)cfg_base;
    img->size = block_size;
    img++;
#endif
    // And a descriptor for the descriptor table itself
    memset(img, 0, sizeof(*img));
    strcpy(img->name, "FIS directory");
    fis_base = (void *)((unsigned long)flash_end - block_size);
    img->flash_base = (unsigned long)fis_base;
    img->mem_base = (unsigned long)fis_base;
    img->size = block_size;
    img++;

    // Do this after creating the initialized table because that inherently
    // calculates where the high water mark of default RedBoot images is.

    if (full_init) {
        unsigned long erase_start, erase_size;
	// Erase everything except default RedBoot images, fis block, and config block.
	// FIXME! This still assumes that fis and config blocks can use top of FLASH.

        // first deal with the possible first part, before RedBoot images:
        erase_start = (unsigned long)flash_start + CYGNUM_REDBOOT_FLASH_RESERVED_BASE;
        erase_size =  (unsigned long)flash_start + CYGBLD_REDBOOT_FLASH_BOOT_OFFSET;
        if ( erase_size > erase_start ) {
            erase_size -= erase_start;
	    if ((stat = flash_erase((void *)erase_start, erase_size,
				    (void **)&err_addr)) != 0) {
		printf("   initialization failed %p: 0x%x(%s)\n",
		       err_addr, stat, flash_errmsg(stat));
            }
        }
        // second deal with the larger part in the main:
        erase_start = redboot_flash_start; // high water of created images
#ifdef CYGIMG_REDBOOT_CONFIG_STORE_FLASH
        erase_size = (unsigned long)cfg_base - erase_start; // the gap between HWM and config data
#else
        erase_size = (unsigned long)fis_base - erase_start; // the gap between HWM and fis data
#endif
        if ((stat = flash_erase((void *)erase_start, erase_size,
                                (void **)&err_addr)) != 0) {
            printf("   initialization failed %p: 0x%x(%s)\n",
                   err_addr, stat, flash_errmsg(stat));
        }
    } else {
        printf("    Warning: device contents not erased, some blocks may not be usable\n");
    }

#ifdef CYGSEM_REDBOOT_FLASH_LOCK_SPECIAL
    // Ensure [quietly] that the directory is unlocked before trying to update
    flash_unlock((void *)fis_base, block_size, (void **)&err_addr);
#endif
    if ((stat = flash_erase(fis_base, block_size, (void **)&err_addr)) != 0) {
            printf("   initialization failed %p: 0x%x(%s)\n", err_addr, stat, flash_errmsg(stat));
    } else {
        if ((stat = flash_program(fis_base, fis_work_block, 
                                  (unsigned long)img - (unsigned long)fis_work_block,
                                  (void **)&err_addr)) != 0) {
            printf("Error writing image descriptors at %p: 0x%x(%s)\n", 
                   err_addr, stat, flash_errmsg(stat));
        }
    }
#ifdef CYGSEM_REDBOOT_FLASH_LOCK_SPECIAL
    // Ensure [quietly] that the directory is locked after the update
    flash_lock((void *)fis_base, block_size, (void **)&err_addr);
#endif
}

static void
fis_list(int argc, char *argv[])
{
    struct fis_image_desc *img;
    int i;
    bool show_cksums = false;
    bool show_datalen = false;
    struct option_info opts[2];

    init_opts(&opts[0], 'd', false, OPTION_ARG_TYPE_FLG, 
              (void **)&show_datalen, (bool *)0, "display data length");
#ifdef CYGSEM_REDBOOT_FIS_CRC_CHECK
    init_opts(&opts[1], 'c', false, OPTION_ARG_TYPE_FLG, 
              (void **)&show_cksums, (bool *)0, "display checksums");
    i = 2;
#else
    i = 1;
#endif
    if (!scan_opts(argc, argv, 2, opts, i, 0, 0, ""))
    {
        return;
    }

    img = (struct fis_image_desc *)((unsigned long)flash_end - block_size);
    // Let printf do the formatting in both cases, rather than cnouting
    // cols by hand....
    printf("%-16s  %-10s  %-10s  %-10s  %-s\n",
           "Name","FLASH addr",
           show_cksums ? "Checksum" : "Mem addr",
           show_datalen ? "Datalen" : "Length",
           "Entry point" );
    for (i = 0;  i < block_size/sizeof(*img);  i++, img++) {
        if (img->name[0] != (unsigned char)0xFF) {
            printf("%-16s  0x%08lX  0x%08lX  0x%08lX  0x%08lX\n", img->name, 
                   img->flash_base, 
#ifdef CYGSEM_REDBOOT_FIS_CRC_CHECK
                   show_cksums ? img->file_cksum : img->mem_base, 
                   show_datalen ? img->data_length : img->size, 
#else
                   img->mem_base, 
                   img->size, 
#endif
                   img->entry_point);
        }
    }
}

static void
fis_free(int argc, char *argv[])
{
    unsigned long *fis_ptr, *fis_end;
    unsigned long *area_start;

    // Do not search the area reserved for pre-RedBoot systems:
    fis_ptr = (unsigned long *)((unsigned long)flash_start + CYGNUM_REDBOOT_FLASH_RESERVED_BASE);
    fis_end = (unsigned long *)(unsigned long)flash_end;
    area_start = fis_ptr;
    while (fis_ptr < fis_end) {
        if (*fis_ptr != (unsigned long)0xFFFFFFFF) {
            if (area_start != fis_ptr) {
                // Assume that this is something
                printf("  0x%08lX .. 0x%08lX\n",
                       (unsigned long)area_start, (unsigned long)fis_ptr);
            }
            // Find next blank block
            area_start = fis_ptr;
            while (area_start < fis_end) {
                if (*area_start == (unsigned long)0xFFFFFFFF) {
                    break;
                }
                area_start += block_size / sizeof(unsigned long);
            }
            fis_ptr = area_start;
        } else {
            fis_ptr += block_size / sizeof(unsigned long);
        }
    }
    if (area_start != fis_ptr) {
        printf("  0x%08lX .. 0x%08lX\n", 
               (unsigned long)area_start, (unsigned long)fis_ptr);
    }
}

// Find the first unused area of flash which is long enough
static bool
fis_find_free(unsigned long *addr, unsigned long length)
{
    unsigned long *fis_ptr, *fis_end;
    unsigned long *area_start;

    // Do not search the area reserved for pre-RedBoot systems:
    fis_ptr = (unsigned long *)((unsigned long)flash_start + CYGNUM_REDBOOT_FLASH_RESERVED_BASE);
    fis_end = (unsigned long *)(unsigned long)flash_end;
    area_start = fis_ptr;
    while (fis_ptr < fis_end) {
        if (*fis_ptr != (unsigned long)0xFFFFFFFF) {
            if (area_start != fis_ptr) {
                // Assume that this is something
                if ((fis_ptr-area_start) >= length) {
                    *addr = (unsigned long)area_start;
                    return true;
                }
            }
            // Find next blank block
            area_start = fis_ptr;
            while (area_start < fis_end) {
                if (*area_start == (unsigned long)0xFFFFFFFF) {
                    break;
                }
                area_start += block_size / sizeof(unsigned long);
            }
            fis_ptr = area_start;
        } else {
            fis_ptr += block_size / sizeof(unsigned long);
        }
    }
    if (area_start != fis_ptr) {
        if ((fis_ptr-area_start) >= length) {
            *addr = (unsigned long)area_start;
            return true;
        }
    }
    return false;
}

static void
fis_create(int argc, char *argv[])
{
    int i, stat;
    unsigned long mem_addr, exec_addr, flash_addr, entry_addr, length, img_size;
    char *name;
    bool mem_addr_set = false;
    bool exec_addr_set = false;
    bool entry_addr_set = false;
    bool flash_addr_set = false;
    bool length_set = false;
    bool img_size_set = false;
    bool no_copy = false;
    void *fis_addr, *err_addr;
    struct fis_image_desc *img;
    bool slot_found, defaults_assumed;
    struct option_info opts[7];
    bool prog_ok;

    init_opts(&opts[0], 'b', true, OPTION_ARG_TYPE_NUM, 
              (void **)&mem_addr, (bool *)&mem_addr_set, "memory base address");
    init_opts(&opts[1], 'r', true, OPTION_ARG_TYPE_NUM, 
              (void **)&exec_addr, (bool *)&exec_addr_set, "ram base address");
    init_opts(&opts[2], 'e', true, OPTION_ARG_TYPE_NUM, 
              (void **)&entry_addr, (bool *)&entry_addr_set, "entry point address");
    init_opts(&opts[3], 'f', true, OPTION_ARG_TYPE_NUM, 
              (void **)&flash_addr, (bool *)&flash_addr_set, "FLASH memory base address");
    init_opts(&opts[4], 'l', true, OPTION_ARG_TYPE_NUM, 
              (void **)&length, (bool *)&length_set, "image length [in FLASH]");
    init_opts(&opts[5], 's', true, OPTION_ARG_TYPE_NUM, 
              (void **)&img_size, (bool *)&img_size_set, "image size [actual data]");
    init_opts(&opts[6], 'n', false, OPTION_ARG_TYPE_FLG, 
              (void **)&no_copy, (bool *)0, "don't copy from RAM to FLASH, just update directory");
    if (!scan_opts(argc, argv, 2, opts, 7, (void *)&name, OPTION_ARG_TYPE_STR, "file name"))
    {
        fis_usage("invalid arguments");
        return;
    }

    defaults_assumed = false;
    if (name) {
        // Search existing files to acquire defaults for params not specified:
        fis_addr = (void *)((unsigned long)flash_end - block_size);
        memcpy(fis_work_block, fis_addr, block_size);
        img = (struct fis_image_desc *)fis_work_block;
        for (i = 0;  i < block_size/sizeof(*img);  i++, img++) {
            if ((img->name[0] != (unsigned char)0xFF) && (strcmp(name, img->name) == 0)) {
                // Found it, so get any unset but necessary params from there:
                if (!length_set) {
                    length_set = true;
                    length = img->size;
                    defaults_assumed = true;
                }
                // img_size can also be handled below, set from length
                if (!img_size_set && 0 < img->data_length) {
                    img_size_set = true;
                    img_size = img->data_length;
                    defaults_assumed = true;
                }
                if (!exec_addr_set) {
                    // Preserve "normal" behaviour
                    exec_addr_set = true;
                    exec_addr = flash_addr_set ? flash_addr : mem_addr;
                }           
                if (!flash_addr_set) {
                    flash_addr_set = true;
                    flash_addr = img->flash_base;
                    defaults_assumed = true;
                }           
                break;
            }
        }
    }

    if ((!no_copy && !mem_addr_set) || (no_copy && !flash_addr_set) ||
        !length_set || !name) {
        fis_usage("required parameter missing");
        return;
    }
    if (!img_size_set) {
        img_size = length;
    }
    // 'length' is size of FLASH image, 'img_size' is actual data size
    // Round up length to FLASH block size
#ifndef CYGPKG_HAL_MIPS // FIXME: compiler is b0rken
    length = ((length + block_size - 1) / block_size) * block_size;
    if (length < img_size) {
        printf("Invalid FLASH image size/length combination\n");
        return;
    }
#endif
    if (flash_addr_set &&
        ((stat = flash_verify_addr((void *)flash_addr)) ||
         (stat = flash_verify_addr((void *)(flash_addr+img_size-1))))) {
        printf("Invalid FLASH address: %p (%s)\n", (void *)flash_addr, flash_errmsg(stat));
        printf("   valid range is %p-%p\n", (void *)flash_start, (void *)flash_end);
        return;
    }
    if (strlen(name) >= sizeof(img->name)) {
        printf("Name is too long, must be less than %d chars\n", (int)sizeof(img->name));
        return;
    }
    if (!no_copy) {
        if ((mem_addr < (unsigned long)ram_start) ||
            ((mem_addr+img_size) >= (unsigned long)ram_end)) {
            printf("** WARNING: RAM address: %p may be invalid\n", (void *)mem_addr);
            printf("   valid range is %p-%p\n", (void *)ram_start, (void *)ram_end);
        }
        if (!flash_addr_set && !fis_find_free(&flash_addr, length)) {
            printf("Can't locate %ld bytes free in FLASH\n", length);
            return;
        }
    }
    // Find a slot in the directory for this entry
    // First, see if an image by this name is already present
    slot_found = false;
    fis_addr = (void *)((unsigned long)flash_end - block_size);
    memcpy(fis_work_block, fis_addr, block_size);
    img = (struct fis_image_desc *)fis_work_block;
    for (i = 0;  i < block_size/sizeof(*img);  i++, img++) {
        if ((img->name[0] != (unsigned char)0xFF) && (strcmp(name, img->name) == 0)) {
            if (flash_addr_set && (img->flash_base != flash_addr)) {
                printf("Image found, but FLASH address incorrect\n");
                return;
            }
            if (img->size != length) {
                printf("Image found, but LENGTH is incorrect (0x%lx != 0x%lx)\n", img->size, length);
                return;
            }
            if (!verify_action("An image named '%s' exists", name)) {
                return;
            } else {                
                slot_found = true;
                if (defaults_assumed) {
                    if (!verify_action("* CAUTION * about to program '%s'\n            at %p..%p from %p", 
                                       name, (void *)flash_addr, (void *)(flash_addr+img_size-1),
                                       (void *)mem_addr)) {
                        return;  // The guy gave up
                    }
                }
                break;
            }
        }
    }
    // If not found, try and find an empty slot
    if (!slot_found) {
        img = (struct fis_image_desc *)fis_work_block;
        for (i = 0;  i < block_size/sizeof(*img);  i++, img++) {
            if (img->name[0] == (unsigned char)0xFF) {
                slot_found = true;
                break;
            }
        }
    }
    if (!no_copy) {
        // Safety check - make sure the address range is not within the code we're running
        if (flash_code_overlaps((void *)flash_addr, (void *)(flash_addr+img_size-1))) {
            printf("Can't program this region - contains code in use!\n");
            return;
        }
        prog_ok = true;
        if (prog_ok) {
            // Erase area to be programmed
            if ((stat = flash_erase((void *)flash_addr, length, (void **)&err_addr)) != 0) {
                printf("Can't erase region at %p: 0x%x(%s)\n", err_addr, stat, flash_errmsg(stat));
                prog_ok = false;
            }
        }
        if (prog_ok) {
            // Now program it
            if ((stat = flash_program((void *)flash_addr, (void *)mem_addr, img_size, (void **)&err_addr)) != 0) {
                printf("Can't program region at %p: 0x%x(%s)\n", err_addr, stat, flash_errmsg(stat));
                prog_ok = false;
            }
        }
    }
    // Update directory
    memset(img, 0, sizeof(*img));
    strcpy(img->name, name);
    img->flash_base = flash_addr;
    img->mem_base = exec_addr_set ? exec_addr : (flash_addr_set ? flash_addr : mem_addr);
    img->entry_point = entry_addr_set ? entry_addr : (unsigned long)entry_address;  // Hope it's been set
    img->size = length;
    img->data_length = img_size;
#ifdef CYGSEM_REDBOOT_FIS_CRC_CHECK
    img->file_cksum = crc32((unsigned char *)flash_addr, img_size);
#endif
#ifdef CYGSEM_REDBOOT_FLASH_LOCK_SPECIAL
    // Insure [quietly] that the directory is unlocked before trying to update
    flash_unlock((void *)fis_addr, block_size, (void **)&err_addr);
#endif
    if ((stat = flash_erase((void *)fis_addr, block_size, (void **)&err_addr)) != 0) {
        printf("Error erasing at %p: 0x%x(%s)\n", err_addr, stat, flash_errmsg(stat));
        // Don't try to program if the erase failed
    } else {
        // Now program it
        if ((stat = flash_program((void *)fis_addr, (void *)fis_work_block, block_size, (void **)&err_addr)) != 0) {
            printf("Error programming at %p: 0x%x(%s)\n", err_addr, stat, flash_errmsg(stat));
        }
    }
#ifdef CYGSEM_REDBOOT_FLASH_LOCK_SPECIAL
    // Insure [quietly] that the directory is locked after the update
    flash_lock((void *)fis_addr, block_size, (void **)&err_addr);
#endif
}

static void
fis_write(int argc, char *argv[])
{
    int stat;
    unsigned long mem_addr, flash_addr, length;
    bool mem_addr_set = false;
    bool flash_addr_set = false;
    bool length_set = false;
    void *err_addr;
    struct option_info opts[3];
    bool prog_ok;

    init_opts(&opts[0], 'b', true, OPTION_ARG_TYPE_NUM, 
              (void **)&mem_addr, (bool *)&mem_addr_set, "memory base address");
    init_opts(&opts[1], 'f', true, OPTION_ARG_TYPE_NUM, 
              (void **)&flash_addr, (bool *)&flash_addr_set, "FLASH memory base address");
    init_opts(&opts[2], 'l', true, OPTION_ARG_TYPE_NUM, 
              (void **)&length, (bool *)&length_set, "image length [in FLASH]");
    if (!scan_opts(argc, argv, 2, opts, 3, 0, 0, 0))
    {
        fis_usage("invalid arguments");
        return;
    }

    if (!mem_addr_set || !flash_addr_set || !length_set)
        fis_usage("required parameter missing");

    // Round up length to FLASH block size
#ifndef CYGPKG_HAL_MIPS // FIXME: compiler is b0rken
    length = ((length + block_size - 1) / block_size) * block_size;
#endif
    if (flash_addr_set &&
        ((stat = flash_verify_addr((void *)flash_addr)) ||
         (stat = flash_verify_addr((void *)(flash_addr+length-1))))) {
        printf("Invalid FLASH address: %p (%s)\n", (void *)flash_addr, flash_errmsg(stat));
        printf("   valid range is %p-%p\n", (void *)flash_start, (void *)flash_end);
        return;
    }
    if ((mem_addr < (unsigned long)ram_start) ||
        ((mem_addr+length) >= (unsigned long)ram_end)) {
        printf("** WARNING: RAM address: %p may be invalid\n", (void *)mem_addr);
        printf("   valid range is %p-%p\n", (void *)ram_start, (void *)ram_end);
    }
    // Safety check - make sure the address range is not within the code we're running
    if (flash_code_overlaps((void *)flash_addr, (void *)(flash_addr+length-1))) {
        printf("Can't program this region - contains code in use!\n");
        return;
    }
    if (!verify_action("* CAUTION * about to program FLASH at %p..%p from %p", 
                       (void *)flash_addr, (void *)(flash_addr+length-1),
                       (void *)mem_addr)) {
        return;  // The guy gave up
    }
    prog_ok = true;
    if (prog_ok) {
        // Erase area to be programmed
        if ((stat = flash_erase((void *)flash_addr, length, (void **)&err_addr)) != 0) {
            printf("Can't erase region at %p: 0x%x(%s)\n", err_addr, stat, flash_errmsg(stat));
            prog_ok = false;
        }
    }
    if (prog_ok) {
        // Now program it
        if ((stat = flash_program((void *)flash_addr, (void *)mem_addr, length, (void **)&err_addr)) != 0) {
            printf("Can't program region at %p: 0x%x(%s)\n", err_addr, stat, flash_errmsg(stat));
            prog_ok = false;
        }
    }
}

static void
fis_erase(int argc, char *argv[])
{
    int stat;
    unsigned long flash_addr, length;
    bool flash_addr_set = false;
    bool length_set = false;
    void *err_addr;
    struct option_info opts[2];

    init_opts(&opts[0], 'f', true, OPTION_ARG_TYPE_NUM, 
              (void **)&flash_addr, (bool *)&flash_addr_set, "FLASH memory base address");
    init_opts(&opts[1], 'l', true, OPTION_ARG_TYPE_NUM, 
              (void **)&length, (bool *)&length_set, "length");
    if (!scan_opts(argc, argv, 2, opts, 2, (void **)0, 0, ""))
    {
        fis_usage("invalid arguments");
        return;
    }

    if (!flash_addr_set || !length_set) {
        fis_usage("missing argument");
        return;
    }
    if (flash_addr_set &&
        ((stat = flash_verify_addr((void *)flash_addr)) ||
         (stat = flash_verify_addr((void *)(flash_addr+length-1))))) {
        printf("Invalid FLASH address: %p (%s)\n", (void *)flash_addr, flash_errmsg(stat));
        printf("   valid range is %p-%p\n", (void *)flash_start, (void *)flash_end);
        return;
    }
    // Safety check - make sure the address range is not within the code we're running
    if (flash_code_overlaps((void *)flash_addr, (void *)(flash_addr+length-1))) {
        printf("Can't erase this region - contains code in use!\n");
        return;
    }
    if ((stat = flash_erase((void *)flash_addr, length, (void **)&err_addr)) != 0) {
        printf("Error erasing at %p: 0x%x(%s)\n", err_addr, stat, flash_errmsg(stat));
    }
}

#if 0 < CYGHWR_IO_FLASH_BLOCK_LOCKING // This is an *interface*

static void
fis_lock(int argc, char *argv[])
{
    int stat;
    unsigned long flash_addr, length;
    bool flash_addr_set = false;
    bool length_set = false;
    void *err_addr;
    struct option_info opts[2];

    init_opts(&opts[0], 'f', true, OPTION_ARG_TYPE_NUM, 
              (void **)&flash_addr, (bool *)&flash_addr_set, "FLASH memory base address");
    init_opts(&opts[1], 'l', true, OPTION_ARG_TYPE_NUM, 
              (void **)&length, (bool *)&length_set, "length");
    if (!scan_opts(argc, argv, 2, opts, 2, (void **)0, 0, ""))
    {
        fis_usage("invalid arguments");
        return;
    }

    if (!flash_addr_set || !length_set) {
        fis_usage("missing argument");
        return;
    }
    if (flash_addr_set &&
        ((stat = flash_verify_addr((void *)flash_addr)) ||
         (stat = flash_verify_addr((void *)(flash_addr+length-1))))) {
        printf("Invalid FLASH address: %p (%s)\n", (void *)flash_addr, flash_errmsg(stat));
        printf("   valid range is %p-%p\n", (void *)flash_start, (void *)flash_end);
        return;
    }
    if ((stat = flash_lock((void *)flash_addr, length, (void **)&err_addr)) != 0) {
        printf("Error locking at %p: 0x%x(%s)\n", err_addr, stat, flash_errmsg(stat));
    }
}

static void
fis_unlock(int argc, char *argv[])
{
    int stat;
    unsigned long flash_addr, length;
    bool flash_addr_set = false;
    bool length_set = false;
    void *err_addr;
    struct option_info opts[2];

    init_opts(&opts[0], 'f', true, OPTION_ARG_TYPE_NUM, 
              (void **)&flash_addr, (bool *)&flash_addr_set, "FLASH memory base address");
    init_opts(&opts[1], 'l', true, OPTION_ARG_TYPE_NUM, 
              (void **)&length, (bool *)&length_set, "length");
    if (!scan_opts(argc, argv, 2, opts, 2, (void **)0, 0, ""))
    {
        fis_usage("invalid arguments");
        return;
    }

    if (!flash_addr_set || !length_set) {
        fis_usage("missing argument");
        return;
    }
    if (flash_addr_set &&
        ((stat = flash_verify_addr((void *)flash_addr)) ||
         (stat = flash_verify_addr((void *)(flash_addr+length-1))))) {
        printf("Invalid FLASH address: %p (%s)\n", (void *)flash_addr, flash_errmsg(stat));
        printf("   valid range is %p-%p\n", (void *)flash_start, (void *)flash_end);
        return;
    }
    if ((stat = flash_unlock((void *)flash_addr, length, (void **)&err_addr)) != 0) {
        printf("Error unlocking at %p: 0x%x(%s)\n", err_addr, stat, flash_errmsg(stat));
    }
}
#endif

static void
fis_delete(int argc, char *argv[])
{
    char *name;
    int i, stat;
    void *fis_addr, *err_addr;
    struct fis_image_desc *img;
    bool slot_found;

    if (!scan_opts(argc, argv, 2, 0, 0, (void **)&name, OPTION_ARG_TYPE_STR, "image name"))
    {
        fis_usage("invalid arguments");
        return;
    }

    slot_found = false;
    fis_addr = (void *)((unsigned long)flash_end - block_size);
    memcpy(fis_work_block, fis_addr, block_size);
    img = (struct fis_image_desc *)fis_work_block;
    i = 0;
#ifdef CYGOPT_REDBOOT_FIS_RESERVED_BASE
    i++;
#endif
#ifdef CYGOPT_REDBOOT_FIS_REDBOOT
    i++;
#endif
#ifdef CYGOPT_REDBOOT_FIS_REDBOOT_BACKUP
    i++;
#endif
#ifdef CYGOPT_REDBOOT_FIS_REDBOOT_POST
    i++;
#endif
#ifdef CYGIMP_REDBOOT_CONFIG_STORE_FLASH
    i++;
#endif
#if 1 // And the descriptor for the descriptor table itself
    i++;
#endif
    img += i;  // Skip reserved files

    for ( /* i, img */;  i < block_size/sizeof(*img);  i++, img++) {
        if ((img->name[0] != (unsigned char)0xFF) && (strcmp(name, img->name) == 0)) {
            if (!verify_action("Delete image '%s'", name)) {
                return;
            } else {
                slot_found = true;
                break;
            }
        }
    }
    if (!slot_found) {
        printf("No image '%s' found\n", name);
        return;
    }
    // Erase Data blocks (free space)
    if ((stat = flash_erase((void *)img->flash_base, img->size, (void **)&err_addr)) != 0) {
        printf("Error erasing at %p: 0x%x(%s)\n", err_addr, stat, flash_errmsg(stat));
    }
#ifdef CYGSEM_REDBOOT_FLASH_LOCK_SPECIAL
    // Insure [quietly] that the directory is unlocked before trying to update
    flash_unlock((void *)fis_addr, block_size, (void **)&err_addr);
#endif
    // Update directory
    memset(img, 0xFF, sizeof(*img));
    if ((stat = flash_erase((void *)fis_addr, block_size, (void **)&err_addr)) != 0) {
        printf("Error erasing at %p: 0x%x(%s)\n", err_addr, stat, flash_errmsg(stat));
        // Don't try to program if the erase failed
    } else {
        // Now program it
        if ((stat = flash_program((void *)fis_addr, (void *)fis_work_block, block_size, (void **)&err_addr)) != 0) {
            printf("Error programming at %p: 0x%x(%s)\n", err_addr, stat, flash_errmsg(stat));
        }
    }
#ifdef CYGSEM_REDBOOT_FLASH_LOCK_SPECIAL
    // Insure [quietly] that the directory is locked after the update
    flash_lock((void *)fis_addr, block_size, (void **)&err_addr);
#endif
}

static void
fis_load(int argc, char *argv[])
{
    char *name;
    struct fis_image_desc *img;
    unsigned long mem_addr;
    bool mem_addr_set = false;
    bool show_cksum = false;
    struct option_info opts[3];
    unsigned long cksum;
    int num_options;
    bool decompress = false;

    init_opts(&opts[0], 'b', true, OPTION_ARG_TYPE_NUM, 
              (void **)&mem_addr, (bool *)&mem_addr_set, "memory [load] base address");
    init_opts(&opts[1], 'c', false, OPTION_ARG_TYPE_FLG, 
              (void **)&show_cksum, (bool *)0, "display checksum");
    num_options = 2;
#ifdef CYGPKG_COMPRESS_ZLIB
    init_opts(&opts[2], 'd', false, OPTION_ARG_TYPE_FLG, 
              (void **)&decompress, 0, "decompress");
    num_options++;
#endif

    if (!scan_opts(argc, argv, 2, opts, num_options, (void **)&name, OPTION_ARG_TYPE_STR, "image name"))
    {
        fis_usage("invalid arguments");
        return;
    }
    if ((img = fis_lookup(name)) == (struct fis_image_desc *)0) {
        printf("No image '%s' found\n", name);
        return;
    }
    if (!mem_addr_set) {
        mem_addr = img->mem_base;
    }
    // Load image from FLASH into RAM
    if ((mem_addr < (unsigned long)user_ram_start) ||
        ((mem_addr+img->size) >= (unsigned long)user_ram_end)) {
        printf("Not a loadable image\n");
        return;
    }
#ifdef CYGPKG_COMPRESS_ZLIB
    if (decompress) {
        int err;
        _pipe_t fis_load_pipe;
        _pipe_t* p = &fis_load_pipe;
        p->out_buf = (unsigned char*) mem_addr;
        p->out_size = -1;
        p->in_buf = (unsigned char*) img->flash_base;
        p->in_avail = img->data_length;

        err = (*_dc_init)(p);

        if (0 == err)
            err = (*_dc_inflate)(p);

        // Free used resources, do final translation of
        // error value.
        err = (*_dc_close)(p, err);

        if (0 != err && p->msg)
            printf("zlib: %s\n", p->msg);

    } else // dangling block
#endif
    {
        memcpy((void *)mem_addr, (void *)img->flash_base, img->data_length);
    }
    entry_address = (unsigned long *)img->entry_point;
#ifdef CYGPKG_REDBOOT_FIS_CRC_CHECK
    cksum = crc32((unsigned char *)mem_addr, img->data_length);
    if (show_cksum) {
        printf("Checksum: 0x%08lx\n", cksum);
    }
    // When decompressing, leave CRC checking to decompressor
    if (!decompress && img->file_cksum) {
        if (cksum != img->file_cksum) {
            printf("** Warning - checksum failure.  stored: 0x%08lx, computed: 0x%08lx\n",
                   img->file_cksum, cksum);
        }
    }
#endif
}

static bool
do_flash_init(void)
{
    int stat;
    static int init = 0;

    if (!init) {
        init = 1;
        if ((stat = flash_init((void *)(workspace_end-FLASH_MIN_WORKSPACE), 
                               FLASH_MIN_WORKSPACE)) != 0) {
            printf("FLASH: driver init failed!, status: 0x%x\n", stat);
            return false;
        }
        flash_get_limits((void *)0, (void **)&flash_start, (void **)&flash_end);
        flash_get_block_info(&block_size, &blocks);
        printf("FLASH: %p - %p, %d blocks of %p bytes each.\n", 
               flash_start, flash_end, blocks, (void *)block_size);
        fis_work_block = (unsigned char *)(workspace_end-FLASH_MIN_WORKSPACE-block_size);
        workspace_end = fis_work_block;
    }
    return true;
}

void
do_fis(int argc, char *argv[])
{
    struct cmd *cmd;

    if (argc < 2) {
        fis_usage("too few arguments");
        return;
    }
    if (!do_flash_init()) return;
    if ((cmd = cmd_search(__FIS_cmds_TAB__, &__FIS_cmds_TAB_END__, 
                          argv[1])) != (struct cmd *)0) {
        (cmd->fun)(argc, argv);
        return;
    }
    fis_usage("unrecognized command");
}

#ifdef CYGIMP_REDBOOT_CONFIG_STORE_FLASH
#include <config.h>
  
bool
cs_init(void)
{
    return do_flash_init();
}

void
cs_write_config(struct _config *config) 
{
    int stat;
    void *cfg_base, *err_addr;
    
    cfg_base = (void *)((unsigned long)flash_end - (2*block_size));
#ifdef CYGSEM_REDBOOT_FLASH_LOCK_SPECIAL
    // Insure [quietly] that the config page is unlocked before trying to update
    flash_unlock((void *)cfg_base, block_size, (void **)&err_addr);
#endif
    if ((stat = flash_erase(cfg_base, block_size, (void **)&err_addr)) != 0) {
	printf("   initialization failed %p: 0x%x(%s)\n", err_addr, stat, flash_errmsg(stat));
    } else {
	if ((stat = flash_program(cfg_base, (void *)config, 
				  sizeof(*config), (void **)&err_addr)) != 0) {
	    printf("Error writing config data at %p: 0x%x(%s)\n", 
		   err_addr, stat, flash_errmsg(stat));
	}
    }
#ifdef CYGSEM_REDBOOT_FLASH_LOCK_SPECIAL
    // Insure [quietly] that the config data is locked after the update
    flash_lock((void *)cfg_base, block_size, (void **)&err_addr);
#endif
}
  
void
cs_load_config(struct _config *config)
{
    void *cfg_base;
    
    cfg_base = (void *)((unsigned long)flash_end - (2*block_size));
    memcpy(config, cfg_base, sizeof(*config));
}

#endif

// EOF flash.c