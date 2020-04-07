#*****************************************************************************
#
# MAKEFILE - Builds the tools used by the build.
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
# Include the common makefile definitions.
#
include $(ROOT)/makedefs

#
# The default rule, which builds the download utility.
#
.PHONY: default
default: buildimg$(PROGEXT) package$(PROGEXT)

#
# A rule to build the buildimg utility.
#
buildimg$(PROGEXT): buildimg.c makefile $(ROOT)/make.inc $(ROOT)/makedefs
	$(NATIVECC) -o buildimg$(PROGEXT) buildimg.c

#
# A rule to build the package utility.
#
package$(PROGEXT): package.c makefile $(ROOT)/make.inc $(ROOT)/makedefs
	$(NATIVECC) -o package$(PROGEXT) package.c

#
# A rule to remove the build products.
#
clean::
	@rm -f buildimg$(PROGEXT) package$(PROGEXT)
