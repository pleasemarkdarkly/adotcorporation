#*****************************************************************************
#
# MAKEFILE - Builds the unformat utility.
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
# The set of object files that comprise the unformat utility.
#
OBJS = entry.o    \
       nandsupp.o

#
# The set of variables that should be defined when building the source.
#
DEFINES = $(BOARD)

#
# The read-only base for the image and the area that should be placed first in
# the image.
#
ROBASE = 0x10000000
FIRST = entry.o(Text)

#
# Include the common makefile definitions.
#
include $(ROOT)/makedefs

#
# The default target, which builds every version of the unformat utility (one
# per board).
#
.PHONY: default
default: unformat$(PROGEXT)

#
# Builds unformat.exe from the source code and the header file generated from
# the ARM image.
#
unformat$(PROGEXT): unformat.c bootcode.h
	$(NATIVECC) -D$(BOARD) -o unformat$(PROGEXT) unformat.c

#
# Builds the header file from the ARM image.
# 
bootcode.h: $(TARGET).rom
	od -t u1 -v $(TARGET).rom | awk -f ../mkarray.awk > bootcode.h

#
# Cleans up after the build.
#
clean::
	@rm -f unformat$(PROGEXT) bootcode.h
