#!/bin/bash
#
###################################################################################################
# build.sh: generic script to build an image
#
# danc@iobjects.com 12/26/2000
#
# DO NOT MODIFY THIS SCRIPT FOR YOUR INDIVIDUAL BUILD
# CHANGES TO THIS SCRIPT IMPACT ALL BUILDS
# MODIFY THE .config FILE FOR YOUR BUILD INSTEAD
#
###################################################################################################
#
# How setup a new build process
#
# First, you need to do some outside work for this thing to go. Specifically, you need to have a
# kernel configuration in the builds/ folder in your eCos kernel tree; you need to have a bootloader
# that you want to use for this image; you need to have a config file that you want to build the
# player with. Optionally you can use the default config file; the location of the kernel sources
# will automagically be set by this script, so you dont have to worry about it.
#
# Second, make a copy of the default.config script, and rename it to something that pertains
# to what you are building. For example, if you are building images for the iomega 16 bit player,
# then "iomega-16bit.config" might be a decent name.
#
# Third, adjust the important variables in the new config. Specifically, be ABSOLUTELY SURE to
# modify the BUILD_NAME to match your new build name. per the above example, "iomega-16bit"
# would be a good BUILD_NAME.
# The other variables should be self evident. If you're scared of the config, then adjust the
# ech= line so you can get a dump of what all it is going to do (this gets dumped to a log file).
#
# Try and respect the structure set up in this file. Do note that build options for the player
# work but build options for the ecos kernel do not; for the most part your build options should
# reside in the respective config files. Read the comments, too :)
#
###################################################################################################

############# default variable values
# quick hack, by default dont shift. this saves me from setting it in every config
SHIFT_DATA_SEGMENT=0

# the root dir
LOCAL_ROOT_DIR="c:/dadio"
# the vss root
VSS_ROOT_DIR="\$"
# the drop dir root
DROP_ROOT_DIR="c:/builds"

export LOCAL_ROOT_DIR VSS_ROOT_DIR DROP_ROOT_DIR

if [ "abc$1" = "abc" ]; then
    echo "Usage: $0 <build.config>"
    exit 1
fi

# check to make sure the file exists
if [ ! -f "$1" ]; then
    echo "$0: configuration $1 not found"
    exit 1
fi

source $1

##### script not-so variables


# location of vss executable
VSS_PATH="/c/PROGRA~1/MICROS~4/VSS/win32"

# location of eCos config tool
if [ $USE_OLD_CONFIGTOOL -eq 1 ]; then
  CONFIG_TOOL="/c/PROGRA~1/REDHAT~1/eCos/tools/bin/ecosconfig-old.exe"
else
  CONFIG_TOOL="/c/PROGRA~1/REDHAT~1/eCos/tools/bin/ecosconfig.exe -i"
fi

# location of the bcat utility
BCAT="$PLAYER_PROJECT_DIR/bootloader/src/bcat.exe"

# location of the mailer utility
BLAT_MAILER="/c/dadio/tools/blat/blat.exe"

# location of the count utility
BYTE_COUNT="/c/dadio/tools/count/count.exe"

# the name of our config without the .ecc
CFG_NAME=`echo $ECOS_CONFIG_NAME | cut -f 1 -d "."`

# template version control file
TMPL_VC_FILE=`pwd`
TMPL_VC_FILE="$TMPL_VC_FILE/version_control.tmpl"

# name of the version control file we write to
BUILD_VC_FILE="$DROP_ROOT_DIR/$BUILD_NAME/$BUILD_NAME"_version_control.txt

# a directory for us to keep working data about this build in
# note that $BUILD_NAME will be appended to this
BUILD_DATA_DIR="build_data"

# various stuff for our build data
BUILD_DATA="$BUILD_DATA_DIR/$BUILD_NAME"
COUNT_NUM=0
LOCK_FN="$LOCAL_ROOT_DIR/$BUILD_DATA/$BUILD_NAME.lock"
COUNT_FN="$LOCAL_ROOT_DIR/$BUILD_DATA/$BUILD_NAME.count"
LOG_FN="$LOCAL_ROOT_DIR/$BUILD_DATA/$BUILD_NAME.log"

# these variables are set later
BN=0
PLAYER_VER=0
BOOT_VER=0

# this is set later. where the eCos headers and libraries can be found
ECOS_INSTALL_DIR=""

# these values are set on error
ERROR_STRING=""
ERROR_CODE=0

##### end script variables

##### functions

# delete lock files
function delete_locks() {
    $ech rm -f $PLAYER_PROJECT_DIR/lock
    $ech rm -f $ECOS_PROJECT_DIR/lock
    $ech rm -f $ECOS_BUILD_DIR/lock
    $ech rm -f $LOCK_FN
}

# mail off a status message indicating what happened
function do_status() {
    if [ $ERROR_CODE -ne 0 ]; then
	echo "Build process failed: $ERROR_STRING" >> $LOG_FN 
	delete_locks
    else
	echo "Build process succeeded" >> $LOG_FN
    fi

    if [ ! -f "$BUILD_VC_FILE" ]; then
	$ech cp $TMPL_VC_FILE $BUILD_VC_FILE
    fi

    DATEVAR=`date "+%D %k:%M"`
    PVER_UPPER=0
    PVER_LOWER=0
    if [ $PLAYER_VER -ne 0 ]; then
	PVER_UPPER=$(($PLAYER_VER >> 16))
	PVER_LOWER=$(($PLAYER_VER & 0xffff))
    fi
    while [ ${#PVER_UPPER} -lt 5 ]; do PVER_UPPER=0"$PVER_UPPER"; done
    while [ ${#PVER_LOWER} -lt 5 ]; do PVER_LOWER=0"$PVER_LOWER"; done

    echo -ne "$DATEVAR\t$PVER_UPPER"."$PVER_LOWER\t$BUILD_NAME"_"$BN".dad"\t" >> $BUILD_VC_FILE
    if [ $ERROR_CODE -ne 0 ]; then
	echo -e "\tFailed" >> $BUILD_VC_FILE
	echo -e "$DATEVAR\n$BUILD_NAME build $BN failed.\n" > ./tmp.txt
    else
	echo -e "\tSucceeded" >> $BUILD_VC_FILE
	echo -e "$DATEVAR\n$BUILD_NAME build $BN succeeded.\n" > ./tmp.txt
    fi
    $BLAT_MAILER ./tmp.txt -to builds@iobjects.com -subject "$BUILD_NAME build $BN" >> $LOG_FN 2>&1
    rm ./tmp.txt

    return
}

####
# 0) get the log file going, lock the build, echo the build number
####

# make sure this exists
$ech mkdir -p $BUILD_DATA

# this too
$ech mkdir -p $DROP_ROOT_DIR/$BUILD_NAME

# figure out our build number
if [ -f "$COUNT_FN" ]; then
	BUILD_NUM=`cat $COUNT_FN`
fi

BUILD_NUM=$(($BUILD_NUM + 1))

if [ $BUILD_NUM -lt 10 ]; then
    BN=0"$BUILD_NUM"
else
    BN=$BUILD_NUM
fi


if [ -f "$LOG_FN" ]; then
    $ech rm -f $LOG_FN
fi

$ech touch $LOG_FN

if [ -f "$LOCK_FN" ]; then
    ERROR_STRING="Cannot build $BUILD_NAME; another build is currently running"
    ERROR_CODE=1
    do_status
    exit 1
fi

$ech touch $LOCK_FN

# obtain locks

if [ -f "$ECOS_PROJECT_DIR/lock" ]; then
    ERROR_STRING="The ecos source tree is currently locked"
    ERROR_CODE=1
    $ech rm -f $LOCK_FN
    do_status
    exit 1
else
    if [ ! -d "$ECOS_PROJECT_DIR" ]; then
	$ech mkdir -p "$ECOS_PROJECT_DIR"
    fi
    $ech touch $ECOS_PROJECT_DIR/lock
fi

if [ -f "$ECOS_BUILD_DIR/lock" ]; then
    ERROR_STRING="The kernel build tree is currently locked"
    ERROR_CODE=1
    $ech rm -f $ECOS_PROJECT_DIR/lock
    $ech rm -f $LOCK_FN
    do_status
    exit 1
else
    if [ ! -d "$ECOS_BUILD_DIR" ]; then
	$ech mkdir -p "$ECOS_BUILD_DIR"
    fi
    $ech touch $ECOS_BUILD_DIR/lock
fi

if [ -f "$PLAYER_PROJECT_DIR/lock" ]; then
    ERROR_STRING="The player directory is currently locked"
    ERROR_CODE=1
    $ech rm -f $ECOS_PROJECT_DIR/lock
    $ech rm -f $ECOS_BUILD_DIR/lock
    $ech rm -f $LOCK_FN
    do_status
    exit 1
else
    if [ ! -d "$PLAYER_PROJECT_DIR" ]; then
	$ech mkdir -p "$PLAYER_PROJECT_DIR"
    fi
    $ech touch $PLAYER_PROJECT_DIR/lock
fi

echo "Starting $BUILD_NAME build $BUILD_NUM" >> $LOG_FN

# even if the build fails from this point on, the build number is incremented
$ech echo $BUILD_NUM > $COUNT_FN

####
# 1) generate directory structure
####

$ech cd $LOCAL_ROOT_DIR

# check to see if the project directory exists. if so, junk it
if [ -d $ECOS_PROJECT_DIR ]; then
	echo "Removing old project directory" >> $LOG_FN
	$ech rm -rf $ECOS_PROJECT_DIR >> $LOG_FN 2>&1
fi

$ech mkdir -p $ECOS_PROJECT_DIR
$ech touch "$ECOS_PROJECT_DIR/lock"

# check to see if the build directory exists
if [ -d $ECOS_BUILD_DIR ]; then
	echo "Removing old build directory" >> $LOG_FN
	$ech rm -rf $ECOS_BUILD_DIR >> $LOG_FN 2>&1
fi

$ech mkdir -p $ECOS_BUILD_DIR
$ech touch "$ECOS_BUILD_DIR/lock"

# check to see if the player directory exists
if [ -d $PLAYER_PROJECT_DIR ]; then
	echo "Removing old player directory" >> $LOG_FN
	$ech rm -rf $PLAYER_PROJECT_DIR >> $LOG_FN 2>&1
fi

$ech mkdir -p $PLAYER_PROJECT_DIR
$ech touch "$PLAYER_PROJECT_DIR/lock"


####
# 2) obtain sources from VSS
####

# add it into our path
PATH=$PATH:$VSS_PATH

$ech cd $ECOS_PROJECT_DIR

echo "Getting latest sources from VSS" >> $LOG_FN

echo "eCos sources..." >> $LOG_FN
echo ""  >> $LOG_FN
# set the working folder
$ech ss workfold "$ECOS_PROJECT_NAME" "$ECOS_PROJECT_DIR" >> $LOG_FN 2>&1

ERROR_CODE=$?
if [ $ERROR_CODE -ne 0 ]; then
    ERROR_STRING="Failed to set the eCos working folder for VSS"
    do_status
    exit 1
fi

# grab the sourceages
$ech ss get "$ECOS_PROJECT_NAME" -R -I- >> $LOG_FN 2>&1

ERROR_CODE=$?
if [ $ERROR_CODE -ne 0 ]; then
    ERROR_STRING="Failed to check out eCos sources from VSS"
    do_status
    exit 1
fi

echo "done" >> $LOG_FN

echo "player sources..." >> $LOG_FN

$ech cd $PLAYER_PROJECT_DIR

$ech ss workfold "$PLAYER_PROJECT_NAME" "$PLAYER_PROJECT_DIR" >> $LOG_FN 2>&1
ERROR_CODE=$?

if [ $ERROR_CODE -ne 0 ]; then
    ERROR_STRING="Failed to set the player working folder for VSS"
    do_status
    exit 1
fi

$ech ss get "$PLAYER_PROJECT_NAME" -R -I- >> $LOG_FN 2>&1

ERROR_CODE=$?
if [ $ERROR_CODE -ne 0 ]; then
    ERROR_STRING="Failed to check out player sources from VSS"
    do_status
    exit 1
fi

echo "done" >> $LOG_FN


####
# 3) configure and build eCos
####

# switch to the build dir
$ech cd $ECOS_BUILD_DIR

# help out the eCos config tool by generating some directories. for some reason the cmd line
# version of the tool doesnt automagically do this

ECOS_INSTALL_DIR="$ECOS_BUILD_DIR"/"$CFG_NAME"_install

$ech mkdir -p "$CFG_NAME"_install
#$ech mkdir -p "$CFG_NAME"_mlt
$ech mkdir -p "$CFG_NAME"_build

echo "Building eCos configuration: $CFG_NAME" >> $LOG_FN 2>&1

# copy the config
$ech cp "$ECOS_PROJECT_DIR/builds/$ECOS_CONFIG_NAME" . >> $LOG_FN 2>&1

# strip the read-only bit
$ech chmod a+w "$ECOS_CONFIG_NAME" >> $LOG_FN 2>&1

ERROR_CODE=$?
if [ $ERROR_CODE -ne 0 ]; then
    ERROR_STRING="eCos kernel configuration not found. Check your filename?"
    do_status
    exit 1
fi

$ech cd "$CFG_NAME"_build

$ech $CONFIG_TOOL --config=../"$ECOS_CONFIG_NAME" --srcdir="$ECOS_PROJECT_DIR"/packages --prefix="$ECOS_INSTALL_DIR" --no-resolve tree >> $LOG_FN 2>&1

ERROR_CODE=$?
if [ $ERROR_CODE -ne 0 ]; then
    ERROR_STRING="Failed to build eCos kernel configuration"
    do_status
    exit 1
fi

# now that the configuration is generated, run make
$ech make >> $LOG_FN 2>&1

ERROR_CODE=$?
if [ $ERROR_CODE -ne 0 ]; then
    ERROR_STRING="Failed to build eCos kernel"
    do_status
    exit 1
fi

#
# TODO dirty hack
# since the target.ld comes out with the improper base for building images that use bootloaders, we
# need to sed it a bit

if [ $PAD_FOR_BOOTLOADER -eq 1 ]; then
    $ech cd $ECOS_INSTALL_DIR/lib
    $ech mv target.ld target_ld_orig
    $ech cat target_ld_orig | sed 's/0xe0000000/0xe0020000/' > target.ld
    $ech cd -
fi
if [ $SHIFT_DATA_SEGMENT -eq 1 ]; then
    $ech cd $ECOS_INSTALL_DIR/lib
    $ech mv target.ld target_ld_tmp
    $ech cat target_ld_tmp | sed 's/data   0x1000/data   0x4a0/' > target.ld
    $ech rm -f target_ld_tmp
    $ech cd -
fi

####
# 4) configure and build the player
####

# first cd into the dir
$ech cd $PLAYER_PROJECT_DIR

# the config file we use
PLAYER_CFG="$PLAYER_PROJECT_DIR"/"$PLAYER_CONFIG_NAME"

# make sure we can write to it
$ech chmod 644 $PLAYER_CFG

ERROR_CODE=$?
if [ $ERROR_CODE -ne 0 ]; then
    ERROR_STRING="Player config file not found. Check your filename?"
    do_status
    exit 1
fi

# change the PKG_INSTALL_DIR to match the directory we just built
# also add any config flags put in this file
echo "########################################" >> $PLAYER_CFG
echo "# The following lines are auto-generated" >> $PLAYER_CFG
echo "########################################" >> $PLAYER_CFG
echo ""                                         >> $PLAYER_CFG
echo "PKG_INSTALL_DIR := $ECOS_INSTALL_DIR"     >> $PLAYER_CFG

if [ abc$PLAYER_BUILD_FLAGS != abc ]; then
 echo "COMPILER_FLAGS += $PLAYER_BUILD_FLAGS"   >> $PLAYER_CFG
fi

echo ""                                         >> $PLAYER_CFG
echo "# End of auto-generated lines           " >> $PLAYER_CFG

# now that the config is there, time to make
$ech make CONFIG=$PLAYER_CONFIG_NAME >> $LOG_FN 2>&1

ERROR_CODE=$?
if [ $ERROR_CODE -ne 0 ]; then
    ERROR_STRING="Failed to build player"
    do_status
    exit 1
fi


# ok, if all is good, then we have a image built, so generate a player.dad and a
# player+bootloader.dad
# the image should be $PLAYER_PROJECT_DIR/dadio.exe.

if [ -f "dar.exe" ]; then
    mv dar.exe dadio.exe
fi

$ech arm-elf-objcopy -O binary dadio.exe dadio.bin >> $LOG_FN 2>&1

# check the resulting file size against our magic number
# make sure we dont overflow
#FSIZE=`$BYTE_COUNT dadio.bin`
#FSIZE=$(($FSIZE / 1024))
#
#if[ $FSIZE -ge $CODEC_LOCATION ]; then
#    ERROR_CODE=1
#    ERROR_STRING="Your dadio.bin is $FSIZE kbytes, and the codecs need to be located at $FSIZE kbytes"
#    do_status
#    exit 1
#fi

# append codecs
$ech cd make-image/pad
$ech ./pad_at_address.exe ../../dadio.bin $CODEC_LOCATION $CODEC_LIST ../../dadio+codecs.bin >> $LOG_FN 2>&1
$ech cd ../..

# see if we are building a .dad
# if so, we need to check out the version file
if [ $BUILD_DAD_IMAGE -eq 1 ]; then
    $ech cd make-image/SFVerifier

    if [ $UPDATE_VSS_VERSION -eq 1 ]; then
	$ech ss checkout "$PLAYER_PROJECT_NAME/make-image/SFVerifier/$IMAGE_VERSION_FN" -I- >> $LOG_FN 2>&1
	ERROR_CODE=$?
	if [ $ERROR_CODE -ne 0 ]; then
	    ERROR_STRING="Could not check out image version file: $IMAGE_VERSION_FN."
	    do_status
	    exit 1
	fi
    fi

    ./MarksLazyVersionMaker $IMAGE_VERSION_FN >> $LOG_FN 2>&1
    PLAYER_VER=`cat $IMAGE_VERSION_FN`

    if [ ! -f "../keys/$IMAGE_PRIVATEKEY" -o ! -f "../keys/$IMAGE_PUBLICKEY" ]; then
	ERROR_STRING="Could not find keys: ($IMAGE_PRIVATEKEY, $IMAGE_PUBLICKEY)"
	do_status
	exit 1
    fi

    $ech cp ../../dadio+codecs.bin dadio+codecs.bin
    $ech java mkimage $IMAGE_ENCODE_ARGS -e 1 -v $PLAYER_VER dadio+codecs.bin ../keys/$IMAGE_PRIVATEKEY ../keys/$IMAGE_PUBLICKEY dadio+codecs_enc.bin >> $LOG_FN 2>&1

    ./SFVEncodeImage.exe dadio+codecs_enc.bin dadio.dad >> $LOG_FN 2>&1
    $ech cp dadio.dad ../..

    if [ $UPDATE_VSS_VERSION -eq 1 ]; then
        echo "version updated for build" | ss checkin "$PLAYER_PROJECT_NAME/make-image/SFVerifier/$IMAGE_VERSION_FN" -I- >> $LOG_FN 2>&1
	ERROR_CODE=$?
	if [ $ERROR_CODE -ne 0 ]; then
	    ERROR_STRING="Could not check in image version file: $IMAGE_VERSION_FN."
	    do_status
	    exit 1
	fi
    fi

    $ech cd ../..
fi

# bcat a bootloader on the front of the image
if [ $BUILD_DAD_BOOTLOAD_IMAGE -eq 1 ]; then
    $ech $BCAT dadio+codecs+boot.bin make-image/bootloaders/$BOOTLOADER_NAME dadio+codecs.bin

    BOOT_VER=`cat make-image/bootloaders/$BOOTLOADER_VERSION_FN`

    $ech cd make-image/SFVerifier

    if [ ! -f "../keys/$BOOTLOADER_PRIVATEKEY" -o ! -f "../keys/$BOOTLOADER_PUBLICKEY" ]; then
	ERROR_STRING="Could not find keys: ($BOOTLOADER_PRIVATEKEY, $BOOTLOADER_PUBLICKEY)"
	do_status
	exit 1
    fi

    $ech cp ../../dadio+codecs+boot.bin .
    $ech java mkimage $BOOTLOADER_ENCODE_ARGS -e 1 -v $BOOT_VER dadio+codecs+boot.bin ../keys/$BOOTLOADER_PRIVATEKEY ../keys/$BOOTLOADER_PUBLICKEY dadio+codecs+boot_enc.bin >> $LOG_FN 2>&1

    ./SFVEncodeImage.exe dadio+codecs+boot_enc.bin dadio+boot.dad >> $LOG_FN 2>&1
    $ech cp dadio+boot.dad ../..

    $ech cd ../..
fi


####
# 5) copy the player images to the drop dir
####

# make the directory where we copy our stuff to

DROP_DIR=$DROP_ROOT_DIR/$BUILD_NAME/$BN

$ech mkdir -p $DROP_DIR

# if we built .dad files, then we should have dadio.dad and dadio+boot.dad
if [ $BUILD_DAD_IMAGE -eq 1 ]; then
    $ech cp dadio.dad $DROP_DIR/"$BUILD_NAME"_"$BN".dad
fi
if [ $BUILD_DAD_BOOTLOAD_IMAGE -eq 1 ]; then
    $ech cp dadio+boot.dad $DROP_DIR/"$BUILD_NAME"+boot_"$BN".dad
fi

# copy the original dadio.exe and dadio.bin
$ech cp dadio.exe $DROP_DIR/"$BUILD_NAME"_"$BN".exe
$ech cp dadio.bin $DROP_DIR/"$BUILD_NAME"_"$BN".bin
$ech cp dadio+codecs.bin $DROP_DIR/"$BUILD_NAME"_"$BN"+codecs.bin

if [ $BUILD_DAD_IMAGE -eq 0 ]; then
	if [ $BUILD_DAD_BOOTLOAD_IMAGE -eq 0 ]; then
		$ech cp dadio+codecs.bin $DROP_DIR/"$BUILD_NAME"_"$BN".dad
	fi
fi

# copy the config file we modified
$ech cp $PLAYER_CONFIG_NAME $DROP_DIR/config

# and the .ecc
$ech cp "$ECOS_PROJECT_DIR/builds/$ECOS_CONFIG_NAME" $DROP_DIR/

####
# 6) mail off the status message, save off the log file, unlock the build, etc
####

do_status
$ech cp $LOG_FN $DROP_DIR/
delete_locks


