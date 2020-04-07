#*****************************************************************************
#
# MAKEFILE - Rules for building the Internet Audio Player.
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
TARGET = player

#
# The set of subdirectories to be built in addition to the current directory.
#
DIRS = adpcm    \
       buffer   \
       support

#
# The set of object files that comprise the player.
#
OBJS = codec.o    \
       dai.o      \
       fatfs.o    \
       fs.o       \
       init.o     \
       irq.o      \
       main.o     \
       mmcfs.o    \
       mmcsupp.o  \
       mmu.o      \
       nand.o     \
       nandsupp.o \
       output.o   \
       play.o     \
       power.o    \
       smartmed.o \
       time.o     \
       usb.o      \
       vectors.o

#
# The set of library files required to build the player.
#
LIBS = buffer/$(CROSSTOOLS)/buffer.a   \
       fat/$(CROSSTOOLS)/fat.a         \
       ftl/$(CROSSTOOLS)/ftl.a         \
       src/$(CROSSTOOLS)/src.a         \
       support/$(CROSSTOOLS)/support.a

#
# If AAC support is required, add the AAC interface code and object library to
# the build.
#
ifdef SUPPORT_AAC
OBJS += aac.o
LIBS += aac/$(CROSSTOOLS)/aaclib.a
endif

#
# If ACELP.net support is required, add the ACELP.net interface code and object
# library to the build.
#
ifdef SUPPORT_ACELPNET
OBJS += acelpnet.o
LIBS += acelpnet/$(CROSSTOOLS)/common.a \
        acelpnet/$(CROSSTOOLS)/lib5k.a  \
        acelpnet/$(CROSSTOOLS)/lib85k.a \
        acelpnet/$(CROSSTOOLS)/lib16k.a
endif

#
# If Audible support is required, add the Audible code to the build.
#
ifdef SUPPORT_AUDIBLE
OBJS += audible.o
endif

#
# If G.721 support is required, add the G.721 interface code and object library
# to the build.
#
ifdef SUPPORT_G721
OBJS += g721.o
LIBS += adpcm/$(CROSSTOOLS)/adpcm.a
endif

#
# If ADC to DAC loopback test support is required, add the loopback code to the
# build.
#
ifdef REQUIRE_LOOPBACK
OBJS += loop.o
endif

#
# If MP3 support is required, add the MP3 interface code and object library to
# the build.
#
ifdef SUPPORT_MP3
OBJS += id3.o \
        mp3.o
LIBS += mp3/$(CROSSTOOLS)/mpeg3lib.a
endif

#
# If MS ADPCM support is required, add the MS ADPCM code to the build.
#
ifdef SUPPORT_MSADPCM
OBJS += msadpcm.o
endif

#
# If QSound support is required, add the QSound interface code and object
# library to the build.
#
ifdef SUPPORT_QSOUND
OBJS += qsound.o
LIBS += qsound/$(CROSSTOOLS)/q2x.a
endif

#
# If voice record support is required, add the voice record object to the
# build.
#
ifdef REQUIRE_RECORD
OBJS += input.o  \
        record.o
endif

#
# If Rights/PD support is required, add the Rights/PD interface code and object
# library to the build.
#
ifdef SUPPORT_RIGHTS_PD
OBJS += itrust.o
LIBS += itrust/$(CROSSTOOLS)/DFReader.a     \
        itrust/$(CROSSTOOLS)/NoHeapReader.o
endif

#
# If Spatializer support is required, add the Spatializer interface code and
# object library to the build.
#
ifdef SUPPORT_SPATIALIZER
OBJS += spatial.o
LIBS += spatial/$(CROSSTOOLS)/vibe.a
endif

#
# If the spectral analyzer is required, add the spectral analyzer code to the
# build.
#
ifdef SUPPORT_SPECTRUM_ANALYZER
OBJS += spectrum.o
endif

#
# If SRS support is required, add the SRS interface code and object library to
# the build.
#
ifdef SUPPORT_SRS
OBJS += srs.o
LIBS += srs/$(CROSSTOOLS)/srs_eda.a
endif

#
# If the software tone control is required, add the software tone control
# object library to the build.
#
ifdef SOFTWARE_TONE_CONTROL
LIBS += tonectrl/$(CROSSTOOLS)/tonectrl.a
endif

#
# If the two-wire interface is used, add the two-wire object library to the
# build.
#
ifdef HwPortABCD_Clock
LIBS += twowire/$(CROSSTOOLS)/twowire.a
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
# If the software volume control is required, add the software volume control
# code to the build.
#
ifdef SOFTWARE_VOLUME_CONTROL
OBJS += volume.o
endif

#
# If WMA support is required, add the WMA interface code and object library to
# the build.
#
ifdef SUPPORT_WMA
OBJS += wma.o
LIBS += wma/$(CROSSTOOLS)/WMA_Dec_Emb_ARM_full_low.a \
        wma/$(CROSSTOOLS)/drmd_eval.a
ifndef SUPPORT_ACELPNET
LIBS += wma/$(CROSSTOOLS)/acelp_dummy.a
endif
endif

#
# The read-only and read-write base for the image, and the area that should be
# placed first in the image.
#
ROBASE = $(HwPlayerAddress)
RWBASE = 0x00100000
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
# The default target.  We will build everything in the subdirectories, as well
# as the player itself.
#
.PHONY: default
default: $(TARGET).pkg

#
# This rule will take the player ROM and place it into a upgrade package.
#
$(TARGET).pkg: $(TARGET).rom
	../tools/package $(TARGET).rom $(TARGET).pkg

#
# Clean out the source tree.
#
clean::
	@rm -f $(TARGET).pkg
