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
                                            void *pvValue,
                                            unsigned long *pulLength);
extern unsigned long Maverick_SetDescriptor(unsigned long ulDescriptor,
                                            unsigned long ulIndex,
                                            void *pvValue,
                                            unsigned long ulLength);
extern unsigned long Maverick_NumDrives(void);
extern unsigned long Maverick_Open(unsigned long ulDrive,
                                   const unsigned short *pusFileName,
                                   unsigned long ulFlags);
extern unsigned long Maverick_Create(unsigned long ulDrive,
                                     const unsigned short *pusFileName,
                                     unsigned long ulFileLength);
extern unsigned long Maverick_Read(void *pvBuffer, unsigned long ulNumBytes);
extern unsigned long Maverick_Write(void *pvBuffer, unsigned long ulNumBytes);
extern unsigned long Maverick_Seek(unsigned long ulPos);
extern unsigned long Maverick_Tell(void);
extern unsigned long Maverick_Length(void);
extern unsigned long Maverick_GetDate(void);
extern unsigned long Maverick_Close(void);
extern unsigned long Maverick_Delete(unsigned long ulDrive,
                                     const unsigned short *pusFileName);
extern unsigned long Maverick_OpenDir(unsigned long ulDrive,
                                      const unsigned short *pusDirName);
extern unsigned long Maverick_ReadDir(unsigned short *pusFileName,
                                      unsigned long ulType);
extern unsigned long Maverick_CloseDir(void);
extern unsigned long Maverick_MakeDir(unsigned long ulDrive,
                                      const unsigned short *pusDirName);
extern unsigned long Maverick_RemoveDir(unsigned long ulDrive,
                                        const unsigned short *pusDirName);
extern unsigned long Maverick_TotalSpace(unsigned long ulDrive);
extern unsigned long Maverick_FreeSpace(unsigned long ulDrive);
extern unsigned long Maverick_Format(unsigned long ulDrive);
extern unsigned long Maverick_StartUpdate(void);
extern void Maverick_ResetTransferCount(void);
extern unsigned long Maverick_GetTransferCount(void);
extern unsigned long Maverick_GetDeviceName(unsigned short *pusBuffer,
                                            unsigned long ulLength);
extern unsigned long Maverick_GetManufacturerName(unsigned short *pusBuffer,
                                                  unsigned long ulLength);

#ifdef __cplusplus
}
#endif
