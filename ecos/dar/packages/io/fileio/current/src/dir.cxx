//==========================================================================
//
//      dir.cxx
//
//      Fileio directory support
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
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):           nickg
// Contributors:        nickg
// Date:                2000-05-25
// Purpose:             Fileio directory support
// Description:         Support for directory operations.
//                      
//              
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/kernel.h>
#include <pkgconf/io_fileio.h>

#include <cyg/kernel/ktypes.h>         // base kernel types
#include <cyg/infra/cyg_trac.h>        // tracing macros
#include <cyg/infra/cyg_ass.h>         // assertion macros

#include <stdarg.h>                     // for fcntl()

#include "fio.h"                       // Private header

#include <dirent.h>                    // struct dirent

//==========================================================================

#define DIROPEN_RETURN_ERR( err )               \
CYG_MACRO_START                                 \
    errno = err;                                \
    CYG_REPORT_RETVAL( NULL );                  \
    return NULL;                                \
CYG_MACRO_END

//==========================================================================
// Implement filesystem locking protocol. 

#define LOCK_FS( _mte_ ) cyg_fs_lock( _mte_, (_mte_)->fs->syncmode)

#define UNLOCK_FS( _mte_ ) cyg_fs_unlock( _mte_, (_mte_)->fs->syncmode)

//==========================================================================
// Open a directory for reading

extern DIR *opendir( const char *dirname )
{
    FILEIO_ENTRY();

    CYG_CANCELLATION_POINT;

    int ret = 0;
    int fd;
    cyg_file *file;
    cyg_mtab_entry *mte = cdir_mtab_entry;
    cyg_dir dir = cdir_dir;
    const char *name = dirname;

    fd = cyg_fd_alloc(1); // Never return fd 0

    if( fd < 0 )
        DIROPEN_RETURN_ERR(EMFILE);
    
    file = cyg_file_alloc();

    if( file == NULL )
    {
        cyg_fd_free(fd);
        DIROPEN_RETURN_ERR(ENFILE);
    }
    
    ret = cyg_mtab_lookup( &dir, &name, &mte );
    
    if( 0 != ret )
    {
        cyg_fd_free(fd);
        cyg_file_free(file);
        DIROPEN_RETURN_ERR(ENOENT);
    }

    LOCK_FS( mte );
    
    ret = mte->fs->opendir( mte, dir, name, file );
    
    UNLOCK_FS( mte );
    
    if( 0 != ret )
    {
        cyg_fd_free(fd);
        cyg_file_free(file);
        DIROPEN_RETURN_ERR(ret);
    }

    file->f_flag |= CYG_FDIR|CYG_FREAD;
    file->f_mte = mte;
    file->f_syncmode = mte->fs->syncmode;
    
    cyg_fd_assign( fd, file );

    DIR *dirp = (DIR *)fd;
    
    FILEIO_RETURN_VALUE(dirp);
}

//==========================================================================
// Read a directory entry.
// This is the thread-unsafe version that uses a static result buffer.
// It just calls the thread-safe version to do the work.

extern struct dirent *readdir( DIR *dirp )
{
    FILEIO_ENTRY();
    
    static struct dirent ent;
    struct dirent *result;
    int err;

    err = readdir_r( dirp, &ent, &result );

    if( err != 0 )
    {
        errno = err;
        FILEIO_RETURN_VALUE( NULL );
    }
    
    return result;
}

//==========================================================================

extern int readdir_r( DIR *dirp, struct dirent *entry, struct dirent **result )
{
    FILEIO_ENTRY();

    int fd = (int)dirp;    
    ssize_t res;

    *result = NULL;
    
    res = read( fd, (void *)entry, sizeof(struct dirent));

    if( res < 0 )
        return errno;
    
    if( res > 0 )
        *result = entry;
    
    FILEIO_RETURN( ENOERR );
}

//==========================================================================

extern void rewinddir( DIR *dirp )
{
    FILEIO_ENTRY();

    int fd = (int)dirp;

    lseek( fd, 0, SEEK_SET );
    
    FILEIO_RETURN_VOID();
}

//==========================================================================

extern int closedir( DIR *dirp )
{
    FILEIO_ENTRY();

    int fd = (int)dirp;
    int err = close( fd );

    FILEIO_RETURN_VALUE( err );
}

// -------------------------------------------------------------------------
// EOF dir.cxx
