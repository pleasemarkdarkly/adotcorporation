//****************************************************************************
//
// MAVCMD.H - Prototypes for the functions in mavcmd.c.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
//
//****************************************************************************

#ifdef __cplusplus
extern "C"
{
#endif

extern unsigned long Maverick_OpenDevice(void);
extern void Maverick_CloseDevice(void);
extern unsigned long Maverick_GetDescriptor(unsigned long ulDescriptor,
                                            unsigned long ulIndex,
                                            char *pcValue,
                                            unsigned long *pulLength);
extern unsigned long Maverick_SetDescriptor(unsigned long ulDescriptor,
                                            unsigned long ulIndex,
                                            char *pcValue,
                                            unsigned long ulLength);
extern unsigned long Maverick_NumDrives(void);
extern unsigned long Maverick_Open(unsigned long ulDrive,
                                   const char *pcFileName,
                                   unsigned long ulFlags);
extern unsigned long Maverick_Create(unsigned long ulDrive,
                                     const char *pcFileName,
                                     unsigned long ulFileLength);
extern unsigned long Maverick_Read(void *pvBuffer, unsigned long ulNumBytes);
extern unsigned long Maverick_Write(void *pvBuffer, unsigned long ulNumBytes);
extern unsigned long Maverick_Seek(unsigned long ulPos);
extern unsigned long Maverick_Tell(void);
extern unsigned long Maverick_Length(void);
extern unsigned long Maverick_Close(void);
extern unsigned long Maverick_Delete(unsigned long ulDrive,
                                     const char *pcFileName);
extern unsigned long Maverick_OpenDir(unsigned long ulDrive,
                                      const char *pcDirName);
extern unsigned long Maverick_ReadDir(char *pcFileName, unsigned long ulType);
extern unsigned long Maverick_CloseDir(void);
extern unsigned long Maverick_MakeDir(unsigned long ulDrive,
                                      const char *pcDirName);
extern unsigned long Maverick_RemoveDir(unsigned long ulDrive,
                                        const char *pcDirName);
extern unsigned long Maverick_TotalSpace(unsigned long ulDrive);
extern unsigned long Maverick_FreeSpace(unsigned long ulDrive);
extern unsigned long Maverick_Format(unsigned long ulDrive);
extern unsigned long Maverick_StartUpdate(void);
extern void Maverick_ResetTransferCount(void);
extern unsigned long Maverick_GetTransferCount(void);
extern unsigned long Maverick_GetDeviceName(char *pcBuffer,
                                            unsigned long ulLength);

#ifdef __cplusplus
}
#endif
