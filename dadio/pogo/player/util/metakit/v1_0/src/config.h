#define HAVE_MMAP 0
#define HAVE_MEMMOVE 1
#define HAVE_BCOPY 0
#define SIZEOF_LONG 4
#define HAVE_LONG_LONG 0

#if 0

#if _MSC_VER >= 1000
	#define HAVE_MMAP 1
	#define HAVE_MEMMOVE 1
	#define HAVE_BCOPY 0
	#define SIZEOF_LONG 4
	#define LONG_LONG __int64
#else
	#define HAVE_MMAP 0
	#define HAVE_MEMMOVE 1
	#define HAVE_BCOPY 0
	#define SIZEOF_LONG 4
#endif


#endif
