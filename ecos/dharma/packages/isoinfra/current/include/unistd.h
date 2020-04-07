#ifndef CYGONCE_ISO_UNISTD_H
#define CYGONCE_ISO_UNISTD_H
/*========================================================================
//
//      unistd.h
//
//      POSIX UNIX standard definitions
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     nickg
// Contributors:  
// Date:          2000-06-01
// Purpose:       This file provides the macros, types and functions
//                required by POSIX 1003.1.
// Description:   Much of the real contents of this file get set from the
//                configuration (set by the implementation)
// Usage:         #include <unistd.h>
//
//####DESCRIPTIONEND####
//
//======================================================================
*/

/* CONFIGURATION */

#include <pkgconf/isoinfra.h>          /* Configuration header */

/* INCLUDES */

#include <sys/types.h>

/* This is the "standard" way to get NULL and size_t from stddef.h,
 * which is the canonical location of the definitions.
 */
#define __need_NULL
#define __need_size_t
#include <stddef.h>

/* CONSTANTS */

/* Configuration constants */

#define _POSIX_VERSION          199506L

/* constants for access() */
#define R_OK	1
#define W_OK	2
#define X_OK	4
#define F_OK	8

/* constants for lseek */
#ifndef SEEK_SET
#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2
#endif 

#define	STDIN_FILENO	0
#define	STDOUT_FILENO	1
#define	STDERR_FILENO	2

/* Variable names for sysconf()				*/
#define _SC_AIO_LISTIO_MAX               1
#define _SC_AIO_MAX                      2
#define _SC_AIO_PRIO_DELTA_MAX           3
#define	_SC_ARG_MAX		         4
#define _SC_ASYNCHRONOUS_IO              5
#define	_SC_CHILD_MAX		         6
#define	_SC_CLK_TCK		         7
#define _SC_DELAYTIMER_MAX               8
#define	_SC_FSYNC		         9
#define _SC_GETGR_R_SIZE_MAX            10
#define _SC_GETPW_R_SIZE_MAX            11
#define	_SC_JOB_CONTROL                 12
#define _SC_LOGIN_NAME_MAX              13
#define _SC_MAPPED_FILES                14
#define _SC_MEMLOCK                     15
#define _SC_MEMLOCK_RANGE               16
#define _SC_MEMORY_PROTECTION           17
#define _SC_MESSAGE_PASSING             18
#define _SC_MQ_OPEN_MAX                 19
#define _SC_MQ_PRIO_MAX                 20
#define	_SC_NGROUPS_MAX		        21
#define	_SC_OPEN_MAX		        22
#define	_SC_PAGESIZE		        23
#define _SC_PRIORITIZED_IO              24
#define _SC_PRIORITY_SCHEDULING         25
#define _SC_REALTIME_SIGNALS            26
#define _SC_RTSIG_MAX                   27
#define _SC_SAVED_IDS                   28
#define _SC_SEMAPHORES                  29
#define _SC_SEM_NSEMS_MAX               30
#define _SC_SEM_VALUE_MAX               31
#define _SC_SHARED_MEMORY_OBJECTS       32
#define _SC_SIGQUEUE_MAX                33
#define	_SC_STREAM_MAX		        34
#define _SC_SYNCHRONIZED_IO             35
#define _SC_THREADS                     36
#define _SC_THREAD_ATTR_STACKADDR       37
#define _SC_THREAD_ATTR_STACKSIZE       38
#define _SC_THREAD_DESTRUCTOR_ITERATIONS 39
#define _SC_THREAD_KEYS_MAX             40
#define _SC_THREAD_PRIO_INHERIT         41
#define _SC_THREAD_PRIO_PROTECT         42
#define _SC_THREAD_PRIORITY_SCHEDULING  43
#define _SC_THREAD_PROCESS_SHARED       44
#define _SC_THREAD_SAFE_FUNCTIONS       45
#define _SC_THREAD_STACK_MIN            46
#define _SC_THREAD_THREADS_MAX          47
#define _SC_TIMERS                      48
#define _SC_TIMER_MAX                   49
#define _SC_TTY_NAME_MAX                50
#define	_SC_TZNAME_MAX		        51
#define	_SC_VERSION		        52

/* Variable names for pathconf()			*/
#define _PC_ASYNC_IO                     1
#define	_PC_CHOWN_RESTRICTED	         2
#define	_PC_LINK_MAX		         3
#define	_PC_MAX_CANON		         4
#define	_PC_MAX_INPUT		         5
#define	_PC_NAME_MAX		         6
#define	_PC_NO_TRUNC		         7
#define	_PC_PATH_MAX		         8
#define	_PC_PIPE_BUF		         9
#define _PC_PRIO_IO                     10
#define _PC_SYNC_IO                     11
#define	_PC_VDISABLE		        12


/* Standard function prototypes */


#ifdef __cplusplus
extern "C" {
#endif


extern int 	link(const char *path1, const char *path2);
extern int 	unlink(const char *path);
extern int 	rmdir(const char *path);
extern int 	rename(const char *old_name, const char *new_name);
extern int 	access(const char *path, int amode);
extern int 	chown(const char *path, uid_t owner, uid_t group);
extern long 	pathconf(const char *path, int name);
extern long 	fpathconf(int fd, int name);
extern int 	pipe(int fildes[2]);
extern int 	dup(int fd);
extern int 	dup2(int fd,int fd2);
extern int 	close(int fd);
extern ssize_t 	read(int fd, void *buf, size_t nbyte);
extern ssize_t 	write(int fd, const void *buf, size_t nbyte);
extern off_t 	lseek(int fd, off_t offset, int whence);
extern int      fsync( int fd );
extern int	ftruncate(int fd, unsigned long length);

extern int 	chdir(const char *path);
extern char 	*getcwd(char *buf, int size);

extern int 	getpid(void);
extern int 	getppid(void);
extern uid_t 	getuid(void);
extern uid_t 	geteuid(void);
extern uid_t 	getgid(void);
extern uid_t 	getegid(void);
extern int 	setuid(uid_t uid);
extern int 	setgid(uid_t uid);
extern int 	getgroups(int setsize, uid_t *list);
extern char	*getlogin(void);
extern char	*cuserid(char *s);
extern pid_t 	getpgrp(void);
extern pid_t 	setsid(void);
extern int 	setpgid(pid_t pid, pid_t pgid);
extern char 	*getenv(const char *name);
extern char 	*ctermid(char *s);
extern char	*ttyname(int fd);
extern int 	isatty(int fd);
extern long 	sysconf(int name);

extern int 	vfork(void);
extern int 	execl(char *path,...);
extern int 	execv(char *path,char **argv);
extern int 	execle(char *path,...);
extern int 	execlp(char *file,...);
extern int 	execvp(char *file, char **argv);
extern int 	execve(char *name, char **argv, char **envv);
extern void 	_exit(int code);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* CYGONCE_ISO_UNISTD_H multiple inclusion protection */

/* EOF unistd.h */
