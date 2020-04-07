#*****************************************************************************
#
# MAKEFILE - Rules for building the ADPCM library.
#
# Copyright (c) 2001 Cirrus Logic, Inc.
#
#*****************************************************************************

#
# The location of the root directory relative to this directory.
#
ROOT = ../..

#
# Include the automatically generated make.inc.
#
-include $(ROOT)/make.inc

#
# The name of the image that we are building.
#
TARGET = adpcm

#
# The set of object files that comprise the ADPCM library.
#
OBJS = g711.o \
       g721.o \
       g72x.o

#
# Include the common makefile definitions.
#
include $(ROOT)/makedefs

#
# The default target.  We build the ADPCM library.
#
.PHONY: default
default: $(CROSSTOOLS)/$(TARGET).$(LIBEXT)

#
# Clean out the library directory.
#
clean::
	@rm -rf $(CROSSTOOLS)
