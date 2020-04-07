# Microsoft Developer Studio Generated NMAKE File, Based on mavspi.dsp
!IF "$(CFG)" == ""
CFG=mavspi - Win32 Debug
!MESSAGE No configuration specified. Defaulting to mavspi - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "mavspi - Win32 Debug" && "$(CFG)" != "mavspi - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mavspi.mak" CFG="mavspi - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mavspi - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mavspi - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "mavspi - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\mavspi.dll" "$(OUTDIR)\mavspi.pch" ".\Debug\regsvr32.trg"


CLEAN :
	-@erase "$(INTDIR)\device.obj"
	-@erase "$(INTDIR)\enumdev.obj"
	-@erase "$(INTDIR)\enumstor.obj"
	-@erase "$(INTDIR)\mavcmd.obj"
	-@erase "$(INTDIR)\mavinfo.obj"
	-@erase "$(INTDIR)\mavspi.obj"
	-@erase "$(INTDIR)\mavspi.pch"
	-@erase "$(INTDIR)\mavspi.res"
	-@erase "$(INTDIR)\servprov.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\storage.obj"
	-@erase "$(INTDIR)\storglob.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\mavspi.dll"
	-@erase "$(OUTDIR)\mavspi.exp"
	-@erase "$(OUTDIR)\mavspi.lib"
	-@erase "$(OUTDIR)\mavspi.pdb"
	-@erase ".\mavspi.h"
	-@erase ".\mavspi.tlb"
	-@erase ".\mavspi_i.c"
	-@erase ".\Debug\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Gz /ML /W3 /Z7 /Od /I "c:\progra~1\wmsdk\wmfsdk\include" /D "_DEBUG" /D "_ATL_STATIC_REGISTRY" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mavspi.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mavspi.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=msvcrtd.lib mssachlp.lib rsa32sdk2.lib randlib.lib bignumsdk.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib setupapi.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\mavspi.pdb" /debug /debugtype:coff /machine:I386 /nodefaultlib /def:".\mavspi.def" /out:"$(OUTDIR)\mavspi.dll" /implib:"$(OUTDIR)\mavspi.lib" /pdbtype:sept /libpath:"c:\progra~1\wmsdk\wmfsdk\lib" 
DEF_FILE= \
	".\mavspi.def"
LINK32_OBJS= \
	"$(INTDIR)\device.obj" \
	"$(INTDIR)\enumdev.obj" \
	"$(INTDIR)\enumstor.obj" \
	"$(INTDIR)\mavcmd.obj" \
	"$(INTDIR)\mavinfo.obj" \
	"$(INTDIR)\mavspi.obj" \
	"$(INTDIR)\servprov.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\storage.obj" \
	"$(INTDIR)\storglob.obj" \
	"$(INTDIR)\mavspi.res"

"$(OUTDIR)\mavspi.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\Debug
TargetPath=.\Debug\mavspi.dll
InputPath=.\Debug\mavspi.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
<< 
	

!ELSEIF  "$(CFG)" == "mavspi - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\mavspi.dll" ".\mavspi.tlb" ".\mavspi.h" ".\mavspi_i.c" "$(OUTDIR)\mavspi.pch" ".\Release\regsvr32.trg"


CLEAN :
	-@erase "$(INTDIR)\device.obj"
	-@erase "$(INTDIR)\enumdev.obj"
	-@erase "$(INTDIR)\enumstor.obj"
	-@erase "$(INTDIR)\mavcmd.obj"
	-@erase "$(INTDIR)\mavinfo.obj"
	-@erase "$(INTDIR)\mavspi.obj"
	-@erase "$(INTDIR)\mavspi.pch"
	-@erase "$(INTDIR)\mavspi.res"
	-@erase "$(INTDIR)\servprov.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\storage.obj"
	-@erase "$(INTDIR)\storglob.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\mavspi.dll"
	-@erase "$(OUTDIR)\mavspi.exp"
	-@erase "$(OUTDIR)\mavspi.ilk"
	-@erase "$(OUTDIR)\mavspi.lib"
	-@erase ".\mavspi.h"
	-@erase ".\mavspi.tlb"
	-@erase ".\mavspi_i.c"
	-@erase ".\Release\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Gz /ML /W3 /I "c:\progra~1\wmsdk\wmfsdk\include" /D "NDEBUG" /D "_ATL_STATIC_REGISTRY" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mavspi.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mavspi.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=msvcrt.lib mssachlp.lib rsa32sdk2.lib randlib.lib bignumsdk.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib setupapi.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\mavspi.pdb" /machine:I386 /nodefaultlib /def:".\mavspi.def" /out:"$(OUTDIR)\mavspi.dll" /implib:"$(OUTDIR)\mavspi.lib" /libpath:"c:\progra~1\wmsdk\wmfsdk\lib" 
DEF_FILE= \
	".\mavspi.def"
LINK32_OBJS= \
	"$(INTDIR)\device.obj" \
	"$(INTDIR)\enumdev.obj" \
	"$(INTDIR)\enumstor.obj" \
	"$(INTDIR)\mavcmd.obj" \
	"$(INTDIR)\mavinfo.obj" \
	"$(INTDIR)\mavspi.obj" \
	"$(INTDIR)\servprov.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\storage.obj" \
	"$(INTDIR)\storglob.obj" \
	"$(INTDIR)\mavspi.res"

"$(OUTDIR)\mavspi.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\Release
TargetPath=.\Release\mavspi.dll
InputPath=.\Release\mavspi.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
<< 
	

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("mavspi.dep")
!INCLUDE "mavspi.dep"
!ELSE 
!MESSAGE Warning: cannot find "mavspi.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "mavspi - Win32 Debug" || "$(CFG)" == "mavspi - Win32 Release"
SOURCE=.\device.cpp

"$(INTDIR)\device.obj" : $(SOURCE) "$(INTDIR)" ".\mavspi.h"


SOURCE=.\enumdev.cpp

"$(INTDIR)\enumdev.obj" : $(SOURCE) "$(INTDIR)" ".\mavspi.h"


SOURCE=.\enumstor.cpp

"$(INTDIR)\enumstor.obj" : $(SOURCE) "$(INTDIR)" ".\mavspi.h"


SOURCE=.\mavcmd.c

!IF  "$(CFG)" == "mavspi - Win32 Debug"

CPP_SWITCHES=/nologo /Gz /ML /W3 /Z7 /Od /I "c:\progra~1\wmsdk\wmfsdk\include" /D "_DEBUG" /D "_ATL_STATIC_REGISTRY" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\mavcmd.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "mavspi - Win32 Release"

CPP_SWITCHES=/nologo /Gz /ML /W3 /I "c:\progra~1\wmsdk\wmfsdk\include" /D "NDEBUG" /D "_ATL_STATIC_REGISTRY" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\mavcmd.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\mavinfo.cpp

"$(INTDIR)\mavinfo.obj" : $(SOURCE) "$(INTDIR)" ".\mavspi.h"


SOURCE=.\mavspi.cpp

"$(INTDIR)\mavspi.obj" : $(SOURCE) "$(INTDIR)" ".\mavspi.h" ".\mavspi_i.c"


SOURCE=.\mavspi.idl

!IF  "$(CFG)" == "mavspi - Win32 Debug"

MTL_SWITCHES=/tlb ".\mavspi.tlb" /h "mavspi.h" /iid "mavspi_i.c" /Oicf 

".\mavspi.tlb"	".\mavspi.h"	".\mavspi_i.c" : $(SOURCE) "$(INTDIR)"
	$(MTL) @<<
  $(MTL_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "mavspi - Win32 Release"

MTL_SWITCHES=/tlb ".\mavspi.tlb" /h "mavspi.h" /iid "mavspi_i.c" /Oicf 

".\mavspi.tlb"	".\mavspi.h"	".\mavspi_i.c" : $(SOURCE) "$(INTDIR)"
	$(MTL) @<<
  $(MTL_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\mavspi.rc

"$(INTDIR)\mavspi.res" : $(SOURCE) "$(INTDIR)" ".\mavspi.tlb"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\servprov.cpp

"$(INTDIR)\servprov.obj" : $(SOURCE) "$(INTDIR)" ".\mavspi.h"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "mavspi - Win32 Debug"

CPP_SWITCHES=/nologo /Gz /ML /W3 /Z7 /Od /I "c:\progra~1\wmsdk\wmfsdk\include" /D "_DEBUG" /D "_ATL_STATIC_REGISTRY" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /Fp"$(INTDIR)\mavspi.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\mavspi.pch" : $(SOURCE) "$(INTDIR)" ".\mavspi.h"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "mavspi - Win32 Release"

CPP_SWITCHES=/nologo /Gz /ML /W3 /I "c:\progra~1\wmsdk\wmfsdk\include" /D "NDEBUG" /D "_ATL_STATIC_REGISTRY" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /Fp"$(INTDIR)\mavspi.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\mavspi.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\storage.cpp

"$(INTDIR)\storage.obj" : $(SOURCE) "$(INTDIR)" ".\mavspi.h"


SOURCE=.\storglob.cpp

"$(INTDIR)\storglob.obj" : $(SOURCE) "$(INTDIR)" ".\mavspi.h"



!ENDIF 

