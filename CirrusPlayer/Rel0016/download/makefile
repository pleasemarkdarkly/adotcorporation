#*****************************************************************************
#
# MAKEFILE - Builds the download utility.
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
TARGET = flasher

#
# The set of object files that comprise the download utility.
#
OBJS = flasher.o

#
# The read-only base for the image.
#
ROBASE = 0x10000000

#
# Include the common makefile definitions.
#
include $(ROOT)/makedefs

#
# The default rule, which builds the download utility.
#
.PHONY: default
default: download$(PROGEXT)


# A rule to build the download utility.
#
download$(PROGEXT): download.c flasher.h
	$(NATIVECC) -o download$(PROGEXT) download.c

#
# A rule to create the FLASH code header file from the binary image.
#
flasher.h: $(TARGET).rom
	od -t u1 -v $(TARGET).rom | awk -f ../mkarray.awk > flasher.h

#
# A rule to remove the build products.
#
clean::
	@rm -f download$(PROGEXT) flasher.h
