#*****************************************************************************
#
# MAKEFILE - Used by nmake (from Visual C++) to build the unformat utility for
#            the various hardware platforms.
#
# Copyright (c) 2001 Cirrus Logic, Inc.
#
#*****************************************************************************

#
# VERSIONS is a list of the various versions of the unformat utility that can
# be built.  The dot at the end of the version name is replaced by macro
# expansions later in the makefile.
#
VERSIONS = edb7209.  \
           crd7209a. \
           crd7209b. \
           edb73xx.

#
# Set the flags for the assembler, which is dependent upon the version being
# built.
#
!if "$(VERSION)" == "edb7209"
ASMFLAGS = -PD "EP7209_Eval_Board SETA 1"
!elseif "$(VERSION)" == "crd7209a"
ASMFLAGS = -PD "EP7209_Reference_Board_Rev_A SETA 1"
!elseif "$(VERSION)" == "crd7209b"
ASMFLAGS = -PD "EP7209_Reference_Board_Rev_B SETA 1"
!elseif "$(VERSION)" == "edb73xx"
ASMFLAGS = -PD "EP73XX_Eval_Board SETA 1"
!else
ASMFLAGS =
!endif

#
# Set the flags for the linker.
#
LINKFLAGS = -noscanlib -remove -first entry.o(Assembly$$$$code) \
            -ro-base 0x10000000

#
# The default target, which builds every version of the unformat utility (one
# per board).
#
all: $(VERSIONS:.=)

#
# Builds the various versions of the unformat utiltiy.
#
$(VERSIONS:.=): ____make.it
        @echo $@:
        @if NOT EXIST $@\nul mkdir $@
        @nmake -nologo VERSION=$@ $@/unformat.exe
        @echo.

#
# A dummy target that never exists which causes the recursive call to nmake
# above always happen.
#
____make.it:

#
# Builds unformat.exe from the source code and the header file generated from
# the ARM image.
#
$(VERSION)/unformat.exe: unformat.c $(VERSION)/bootcode.h makefile
        cl -nologo -I$(VERSION) unformat.c -o $(VERSION)\unformat.exe
        @erase unformat.obj

#
# Builds the header file from the ARM image.
# 
$(VERSION)/bootcode.h: $(VERSION)/makeboot.exe $(VERSION)/bootcode.rom makefile
        $(VERSION)\makeboot $(VERSION)

#
# Builds makeboot.exe, which converts the ARM image into a header file.
#
$(VERSION)/makeboot.exe: makeboot.c makefile
        cl -nologo makeboot.c -o $(VERSION)\makeboot.exe
        @erase makeboot.obj

#
# Links the ARM object files into an ARM image.
#
$(VERSION)/bootcode.rom: $(VERSION)/entry.o $(VERSION)/nandsupp.o makefile
        armlink $(LINKFLAGS) -o $(VERSION)\bootcode.axf $(VERSION)\entry.o \
            $(VERSION)\nandsupp.o
        fromelf -nozeropad $(VERSION)\bootcode.axf -bin $(VERSION)\bootcode.rom

#
# Builds the two ARM source files.
#
$(VERSION)/entry.o: entry.s makefile
        armasm $(ASMFLAGS) entry.s $(VERSION)\entry.o

$(VERSION)/nandsupp.o: nandsupp.s makefile
        armasm $(ASMFLAGS) nandsupp.s $(VERSION)\nandsupp.o

#
# Cleans up after the build.
#
clean: $(VERSIONS:.=.clean)
        @if exist *~ erase *~

$(VERSIONS:.=.clean):
        @deltree /y $(@:.clean=) > NUL: > NUL:
