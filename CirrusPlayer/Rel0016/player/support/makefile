#*****************************************************************************
#
# MAKEFILE - Rules for building the support library.
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
TARGET = support

#
# The set of object files that comprise the support library.
#
OBJS = _dfixu.o   \
       _dmul.o    \
       _f2d.o     \
       _ffltu.o   \
       _memcpy.o  \
       divtest.o  \
       lladd.o    \
       lland.o    \
       llcmpeq.o  \
       llcmpge.o  \
       llcmpne.o  \
       llcmpu.o   \
       llfroml.o  \
       llfromu.o  \
       llmul.o    \
       llscmpgt.o \
       llscmplt.o \
       llsdiv.o   \
       llshl.o    \
       llsshr.o   \
       llsub.o    \
       lltol.o    \
       llucmpgt.o \
       lludiv.o   \
       llushr.o   \
       memcmp.o   \
       memcpy.o   \
       memmove.o  \
       memset.o   \
       sdiv.o     \
       sdiv10.o   \
       strcmp.o   \
       strlen.o   \
       strncmp.o  \
       udiv.o     \
       udiv10.o   \
       uread4.o   \
       uwrite4.o  \
       wcscmp.o   \
       wcslen.o   \
       wcsncmp.o

#
# Include the common makefile definitions.
#
include $(ROOT)/makedefs

#
# The default target.  We build the support library.
#
.PHONY: default
default: $(CROSSTOOLS)/$(TARGET).$(LIBEXT)

#
# Clean out the library directory.
#
clean::
	@rm -rf $(CROSSTOOLS)
