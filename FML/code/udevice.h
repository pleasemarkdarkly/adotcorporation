//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1997-2001.
//
//  File:       U D E V I C E. H
//
//  Contents:   Values specific to devices.
//              For each device, add function prototypes
//              and SERVICE_CTL entries to this header
//
//----------------------------------------------------------------------------

#ifndef _U_DEVICE_H_
#define _U_DEVICE_H_

// Device specific prototypes.  They are entry points for device-specific control.


// X10Light device function prototypes


DWORD Do_PwrDim_Init    (CHAR* StrEventUrl);


DWORD Do_PwrDim_Cleanup (CHAR* StrEventUrl);


DWORD Do_SetPowerOn (
                         CHAR*           StrEventUrl,
                         DWORD           cArgs,       
                         ARG*            rgArgs,      
                         PDWORD          pArgsOut,      
                         ARG_OUT*        rgArgsOut   
                     );


DWORD Do_SetPowerOff ( 
                         CHAR*           StrEventUrl,
                         DWORD           cArgs,       
                         ARG*            rgArgs,      
                         PDWORD          pArgsOut,      
                         ARG_OUT*        rgArgsOut   
                      );

DWORD Do_IncreaseLevel (
                         CHAR*           StrEventUrl,
                         DWORD           cArgs,       
                         ARG*            rgArgs,      
                         PDWORD          pArgsOut,      
                         ARG_OUT*        rgArgsOut   
                        );

DWORD Do_DecreaseLevel (  
                         CHAR*           StrEventUrl,
                         DWORD           cArgs,       
                         ARG*            rgArgs,      
                         PDWORD          pArgsOut,      
                         ARG_OUT*        rgArgsOut   
                        );



// Add SERVICE_CTL entries for each device.

// c_cDemoSvc must be set to the number of control IDs in c_rgSvc (below).
// Change this number if you add entries to c_rgSvc.
#define c_cDemoSvc 1

const SERVICE_CTL c_rgSvc[c_cDemoSvc] =
{
    
    {
        "mls",                               //Service control identifier.
            (PFNAI)Do_PwrDim_Init,              //Device-specific initialization function.
            (PFNAC)Do_PwrDim_Cleanup,           //Device-specific cleanup function.
            4,                                  //Number of actions this service implements.
        {                
            { "GetRootDirectory",  (PFNAS)Do_DecreaseLevel      },
            { "PowerOff",          (PFNAS)Do_DecreaseLevel     },
            { "IncreaseLevel",     (PFNAS)Do_DecreaseLevel   },
            { "DecreaseLevel",     (PFNAS)Do_DecreaseLevel   },
        },
    },
};  




// Define the product name that will appear in HTTP SERVER header.
const CHAR c_szProduct[] = "iML";
const CHAR c_szProductVersion[] = "1.0";



#endif