#*****************************************************************************
#
# MAKEFILE - Rules for building the buffer library.
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
TARGET = buffer

#
# The set of object files that comprise the buffer library.
#
OBJS = buffer.o

#
# Include the common makefile definitions.
#
include $(ROOT)/makedefs

#
# The default target.  We build the buffer library.
#
.PHONY: default
default: $(CROSSTOOLS)/$(TARGET).$(LIBEXT)

#
# Clean out the library directory.
#
clean::
	@rm -rf $(CROSSTOOLS)
