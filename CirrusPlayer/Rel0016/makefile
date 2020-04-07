#*****************************************************************************
#
# MAKEFILE - Builds the various components of the player software.
#
# Copyright (c) 2001 Cirrus Logic, Inc.
#
#*****************************************************************************

#
# The location of the root directory relative to this directory.
#
ROOT = .

#
# Include the automatically generated make.inc.
#
-include $(ROOT)/make.inc

#
# The set of subdirectories to be built in addition to the current directory.
# The "tools" directory must be first as it contains tools which are used by
# the remainder of the build.
#
DIRS = tools    \
       bootcode \
       download \
       player   \
       unformat

#
# Include the common makefile definitions.
#
include $(ROOT)/makedefs

#
# The default target.  We will build everything in the subdirectories.
#
.PHONY: default
default: player.img

#
# This rule will generate the player ROM image from the bootcode and the player
# image.
#
player.img: bootcode/bootcode.rom player/player.pkg
	tools/buildimg bootcode/bootcode.rom player/player.pkg player.img

#
# Clean out the source tree.
#
clean::
	@rm -f player.img
