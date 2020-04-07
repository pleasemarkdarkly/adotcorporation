#*****************************************************************************
#
# MAKEFILE - Rules for building the software update code.
#
# Copyright (c) 2001 Cirrus Logic, Inc.
#
#*****************************************************************************

#
# The location of the root directory relative to this directory.
#
ROOT = ..

#
# Include the automatically generated make.inc.
#
-include $(ROOT)/make.inc

#
# The name of the image that we are building.
#
TARGET = bootcode

#
# The set of object files that comprise the software update code.
#
OBJS = mmu.o     \
       update.o  \
       usb.o     \
       vectors.o

#
# The set of library files required to build the software update code.
#
LIBS =

#
# If the hardware uses Intel B3 FLASH, add the Intel B3 FLASH programming code
# to the build.
#
ifdef FLASH_INTEL_B3
OBJS += intelb3.o
endif

#
# If the hardware uses Intel C3 FLASH, add the Intel C3 FLASH programming code
# to the build.
#
ifdef FLASH_INTEL_C3
OBJS += intelc3.o
endif

#
# If the hardware uses Intel J3 (Strata) FLASH, add the Intel J3 FLASH
# programming code to the build.
#
ifdef FLASH_INTEL_J3
OBJS += intelj3.o
endif

#
# If the user interface is the LCD, then add the LCD user interface code to the
# build.
#
ifdef UI_IS_LCD
OBJS += uilcd.o
endif

#
# If the user interface is the serial port, then add the serial port user
# interface code to the build.
#
ifdef UI_IS_SERIAL_PORT
OBJS += uiser.o
endif

#
# The read-only and read-write base for the image, and the area that should be
# placed first in the image.
#
ROBASE = 0x00100000
RWBASE = 0x00104000
FIRST = vectors.o(Text)

#
# The set of defines that should be include on the command line of the compiler
# and assembler.
#
DEFINES = $(BOARD) $(CROSSTOOLS)

#
# Include the common makefile definitions.
#
include $(ROOT)/makedefs

#
# The default target.
#
.PHONY: default
default: $(TARGET).rom

#
# A rule to build the usb.c file from the player.
#
usb.o: ../player/usb.c
	$(CC) $(CFLAGS) -c ../player/usb.c
