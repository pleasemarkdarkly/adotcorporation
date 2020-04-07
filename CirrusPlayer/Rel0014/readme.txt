*******************************************************************************

                              Cirrus Logic, Inc.

                                 Maverick(tm)

                             Internet Audio Player
                              Reference Software

                                  Release 14

                                 March 1, 2001

*******************************************************************************

==> Features <==

* Supports the following codecs:
  - MP3 playback (decoder library included in this package).
  - WMA playback (evaluation decoder library included in this package, full
    decoder library and DRM library must be separately obtained from
    Microsoft).
  - AAC playback (decoder library must be separately obtained from Cirrus
    Logic).
  - ACELP.net playback (decoder library must be separately obtained from Cirrus
    Logic).
  - Microsoft ADPCM playback and record (encoder/decoder source included in
    this package).  Files recorded with MS ADPCM can be played back on the PC.
  - G.721 playback and record (encoder/decoder source included in this
    package).  Files recorded with G.721 can not be played back on the PC.

* Supports the following AudibleReady portable device requirements and
  recommendations (see the AudibleReady Portable Device Requirements
  specification for details):
  - Indication of Audible programs.
  - Maintenance of playback position.
  - Navigation of program sections.
  - Storage of program meta-data.
  - Long duration time display.
  - Accelerated navigation.

* Supports InterTrust Technologies Corp.'s Rights/PD(tm) (decryption library
  must be separately obtained from InterTrust).

* Contains an evaluation version of the Cirrus Logic SSFDC filesystem.

* All strings within the player are stored in Unicode and communicated to the
  host in Unicode.

* Downloads files via the USB at ~400K bytes per second.

* Windows GUI for downloading files via the USB.

* Windows Media Device Manager (WMDM) Service Provider Interface (SPI),
  allowing content download via WMDM-compliant applications, such as Windows
  Media Player Version 7.

* Software sample rate conversion for non-44100Hz audio streams.

* Supports the sample clock generator on the EP73xx, eliminating the need for
  the software sample rate converter for 32000Hz and 48000Hz streams.

* Supports the tone and volume controls in the CS43L43 (via communication over
  a I2C-compatible bus).

* Supports configuring the CS53L32A (via communication over a I2C-compatible
  bus).

* Software tone and volume controls if needed.

* Modular output processing architecture which allows easy addition of third
  party audio processing effects.

* Supports the following third party audio processing effects:
  - WOW(tm) from SRS Labs Inc. (audio processing library must be separately
    obtained from SRS).
  - Q2X from QSound Labs, Inc. (audio processing library must be separately
    obtained from QSound).
  - Spatializer from Spatializer Audio Laboratories, Inc. (audio processing
    library must be separately obtained from Spatialzier).

* Runs on the EP7209 evaluation board, the EP7209 reference design (revision
  B), and the EP73xx evaluation board.

-------------------------------------------------------------------------------

==> Limitations <==

The following are know limitations in the software:

* The evaluation version of the Cirrus Logic SSFDC filesystem has the following
  limitations:
  - It can only access 4MB, 8MB, and 16MB NAND FLASH/SmartMedia devices.
  - It will only store 10 files in the root directory.  If more than 10 files
    exist in the root directory, only the first 10 can be accessed.
  - It will only store 1MB of data in each file.  If more than 1MB of data
    exists in a file, only the first 1MB of data can be accessed.
  - It can not add, remove, or view any subdirectories, or access any files in
    subdirectories.

  The full version can access 4MB, 8MB, 16MB, 32MB, 64MB, and 128MB NAND FLASH
  devices, has no limitations on file size, quantity, or location (other than
  the actual limits of the media), and has full subdirectory support.  Contact
  your Cirrus Logic sales representative for details on obtaining the full
  version.

  Also included in this package is a very rudimentary file system (contained
  in player\nandfs.c) which only supports a single file and is not really
  usable for a real product.  It's only limitation is the fact that it stores
  only a single file; it can access any sized NAND FLASH, but is not compatible
  with the file system used on SmartMedia cards.  If the limitations of the
  evaluation version of the Cirrus Logic SSFDC file system are not acceptable,
  the rudimentary file system can be used instead.

* The evaluation version of WMA has the following limitations:
  - It will only play one minute's worth of audio for each track.  More
    specifically, it will decode (60 * sample rate) samples from each track,
    regardless of any seeking that is done (i.e. if you play 30 seconds, then
    seek forward 5 seconds, it will then play another 30 seconds and then
    stop).
  - Digital Rights Management is not supported.

  Contact Microsoft to obtain the full version of the WMA library.

* While the AudibleReady portable device requirements are implemented in the
  player software, there is no support in the PC software for accessing the
  full AudibleReady functionality of the player.

* While InterTrust's Rights/PD is supported, there is no support in the PC
  software for integrating with InterTrust's host side software.

* SmartMedia cards can only be hot plugged and unplugged on the EP7209
  reference design, not on the EP7209 or EP73xx evaluation boards.  This is
  due to a limitation in the design of the evaluation boards.

-------------------------------------------------------------------------------

==> File In This Package <==

crd7209b\*.* - Pre-built images for use with the EP7209 reference design,
               revision B.

edb7209\*.* - Pre-built images for use with the EP7209 evaluation board.

edb73xx\*.* - Pre-built images for use with the EP73xx evaluation board.

mavspi\*.* - Source code for the WMDM SPI.

mavusb\*.* - Source code for the Windows application for communicating with the
             player over the USB.

player\*.* - Source code for the Maverick Internet Audio Player.

unformat\*.* - Source code for the program which unformats the NAND FLASH on
               the boards.

usbdrvr\*.* - Source code for the Windows 98/Windows NT USB driver for the
              Maverick Internet Audio Player.

config.inc & config.h - Source files which contain the board specific
                        definitions for the player and the unformat code.

dl_b3.exe - Program to download data into an Intel B3 NOR FLASH.

dl_c3.exe - Program to download data into an Intel C3 NOR FLASH.

dl_j3.exe - Program to download data into an Intel J3 NOR FLASH (StrataFLASH).

hwdefs.h & hwdefs.inc - Sources files which contain the register definitions
                        of the Maverick device and the various external
                        peripherals on the boards.

mavusb.inf - INF file used to install the USB driver.

mavusb.sys - Pre-built image of the USB driver.

mavspi.dll - Pre-built image of the WMDM SPI.

mavusb.exe - Pre-built image of the Windows communication application.

readme.txt - This file.

-------------------------------------------------------------------------------

==> How To Use The Player <==

Three player images are supplied in the release distribution:

    edb7209\player.rom - for use on the EP7209 evaluation board

    crd7209b\player.rom - for use on the EP7209 reference design (revision B)

    edb73xx\player.rom - for use on the EP73xx evaluation board

First, download the appropriate player image onto the board.  There are three
programs which can be used to do this, based on the NOR FLASH on the board:

    dl_b3.exe - for use with Intel B3 FLASH devices (as used on the EP7209
                evaluation board and the EP73xx evaluation board)

    dl_c3.exe - for use with Intel C3 FLASH devices (not currently used on any
                Cirrus Logic board)

    dl_j3.exe - for use with Intel J3 FLASH devices, also known as StrataFLASH
                (as used on the EP7209 reference design)

The download procedure for the three boards is as follows:

* EP7209 evaluation board:
  - Power off the board.
  - Put a jumper on JP2.
  - Connect "serial port 0" of the board to an available serial port on the PC
    with a NULL modem cable.
  - Run "dl_b3 edb7209\player.rom" on the PC.  If the PC serial port being used
    is not COM1, then specify the number after the name of the ROM (i.e. "dl_b3
    edb7209\player.rom 3" for COM3).
  - Power on the board.
  - Press the "wakeup" button on the board.
  - Wait until "dl_b3" says it has successfully downloaded the image.
  - Power off the board.
  - Remove the jumper from JP2.

* EP7209 reference design (revision B)
  - Power off the board.
  - Connect the board's serial port to an available serial port on the PC with
    a NULL modem cable.
  - Run "dl_j3 crd7209b\player.rom" on the PC.  If the PC serial port being
    used is not COM1, then specify the number after the name of the ROM (i.e.
    "dl_j3 crd7209b\player.rom 3" for COM3).
  - Hold the nBROM button on the board.
  - Power on the board.
  - Once "dl_j3" says it has started downloading, release the nBROM button.
  - Wait until "dl_j3" says it has successfully downloaded the image.
  - Power off the board.

* EP73xx evaluation board
  - Power off the board.
  - Put a jumper on JP2.
  - Connect "serial port 0" of the board to an available serial port on the PC
    with a NULL modem cable.
  - Run "dl_b3 edb73xx\player.rom" on the PC.  If the PC serial port being used
    is not COM1, then specify the number after the name of the ROM (i.e. "dl_b3
    edb73xx\player.rom 3" for COM3).
  - Power on the board.
  - Press the "wakeup" button on the board.
  - Wait until "dl_b3" says it has successfully downloaded the image.
  - Power off the board.
  - Remove the jumper from JP2.

Once the image is downloaded, it is almost time to run the player.  If a
previous version of the player that did not include the Cirrus Logic SSFDC
filesystem was already on the board then the NAND FLASH needs to be
unformatted.  The unformat procedure for the three boards is as follows:

* EP7209 evaluation board:
  - Power off the board.
  - Put a jumper on JP2.
  - Connect "serial port 0" of the board to an available serial port on the PC
    with a NULL modem cable.
  - Run "edb7209\unformat" on the PC.  If the PC serial port being used is not
    COM1, then specify the number on the command line (i.e. "edb7209\unformat
    3" for COM3).
  - Power on the board.
  - Press the "wakeup" button on the board.
  - Wait until "unformat" says it has successfully unformatted the NAND FLASH.
  - Power off the board.
  - Remove the jumper from JP2.

* EP7209 reference design (revision B)
  - Power off the board.
  - Connect the board's serial port to an available serial port on the PC with
    a NULL modem cable.
  - Run "crd7209b\unformat" on the PC.  If the PC serial port being used is not
    COM1, then specify the number on the command line (i.e. "crd7209b\unformat
    3" for COM3).
  - Hold the nBROM button on the board.
  - Power on the board.
  - Once "unformat" says it has started downloading, release the nBROM button.
  - Wait until "unformat" says it has successfully unformatted the NAND FLASH.
  - Power off the board.

* EP73xx evaluation board
  - Power off the board.
  - Put a jumper on JP2.
  - Connect "serial port 0" of the board to an available serial port on the PC
    with a NULL modem cable.
  - Run "edb73xx\unformat" on the PC.  If the PC serial port being used is not
    COM1, then specify the number on the command line (i.e. "edb73xx\unformat
    3" for COM3).
  - Power on the board.
  - Press the "wakeup" button on the board.
  - Wait until "unformat" says it has successfully unformatted the NAND FLASH.
  - Power off the board.
  - Remove the jumper from JP2.

Now, it is time to run the player.  Do the following:

* EP7209 evaluation board and EP73xx evaluation board:
  - Run a communications program (such as HyperTerminal) and configure it for
    the PC serial port, 115,200 baud, and 8-N-1.
  - Power on the board.
  - Press the "wakeup" button on the board.
  - A message will appear on the communications program saying "Stopped".

* EP7209 reference design
  - Power on the board.
  - The Cirrus Logic logo will appear on the LCD.

The player is now running.  Before it will do anything very interesting, you'll
need to load some songs onto it.  Connect the board to the computer with a USB
cable.  Windows will ask for a driver, which is located in the main directory.
If you were using a previous version of the Windows driver, be sure to install
the new driver (using the directions supplied below for installing a newly
built driver), otherwise the PC will not be able to properly communicate with
the player.  Once the driver is loaded, you can then run "mavusb", which will
allow you to download files to the NAND FLASH of the board (again, do not use a
previous version of mavusb.exe as it will not work correctly).

The Windows application for downloading files to the player is very similar to
Windows Explorer in its operation.  You can drop files on the application to
download them, and you can paste files into the application.  If you use the
menu to "open" a file, it will be downloaded to the player.  If you use the
menu to "save" a file, it will be uploaded from the player and saved on your
PC.  At present, there is no capability to drag a file from the application or
copy a file from the application to the clip-board.  In order to evaluate the
AudibleReady support in the player software, the "open Audible" menu item will
allow you to specify the Audible information which the player itself cares
about, and will download that information to the player along with a media
file.  The file need not contain Audible information, it can be any media file
that you choose to use.

Once you have files on the player, it is time to play the files.  After closing
the Windows application, the status display of the player should have been
updated to indicate the currently loaded file.  You can now press "play" to
play the song.

There are six buttons on the evaluation boards, and eight on the reference
design.  The arrangement and function of the buttons is different on each
board:

* EP7209 evaluation board

      +-------+  +-------+  +-------+  +-------+  +-------+  +-------+
      | User1 |  | User2 |  | User3 |  | User4 |  | User5 |  | User6 |
      +-------+  +-------+  +-------+  +-------+  +-------+  +-------+

  - User1
    - Pressing this button will start playback of the current track, or pause
      if the track is currently playing.
    - Holding this button for one and one half seconds will put the player into
      lower power mode.

  - User2
    - Pressing this button will stop playback of the current track.

  - User3
    - Pressing this button will change the current meaning of User5 and User6,
      advancing backwards through the list of possible meanings.
    - Holding this button will successively advance backwards through the
      meanings of User5 and User6.

  - User4
    - Pressing this button will change the current meaning of User5 and User6,
      advancing forwards through the list of possible meanings.
    - Holding this button will successively advance forwards through the
      meanings of User5 and User6.

  - User5
    - Pressing this button will decrement the value of the currently selected
      item.  For example, it will turn down the volume.  If the current item
      is "seek", it will skip to the beginning of the current track (if the
      current time is less than 3 seconds) or to the previous track (if the
      current time is greater than 3 seconds).
    - Holding this button will successively decrement the value of the current
      item.  If the current item is "seek", it will seek backwards within the
      track.
    - If playing Audible content and the current item is "seek", pressing this
      button will return to the beginning of the current section (if the
      current time is greater than 3 seconds into the section) or will skip to
      the previous section (if the current time is less than 3 seconds).  If
      the current time is within 3 seconds of the beginning of the track,
      pressing this button will skip to the previous track.

  - User6
    - Pressing this button will increment the value of the currently selected
      item.  For example, it will turn up the volume.  If the current item
      is "seek", it will skip to the next track.
    - Holding this button will successively increment the value of the current
      item.  If the current item is "seek", it will seek forwards within the
      track.
    - If playing Audible content and the current item is "seek", pressing this
      button will advance to the next section.  If the last section is already
      being played, it will advance to the next track.

* EP7209 reference design (revision B)

                 +-------+
                 | User2 |
                 +-------+

      +-------+  +-------+  +-------+
      | User4 |  | User1 |  | User5 |
      +-------+  +-------+  +-------+

                 +-------+
                 | User3 |
                 +-------+

      +-------+  +-------+  +-------+
      | User6 |  | User7 |  | User8 |
      +-------+  +-------+  +-------+

  - User1
    - Holding this button for one and one half seconds will put the player into
      low power mode.
    - Pressing this button will take you into and out of menu mode.

  - User2
    - Pressing this button will start playback of the current track, or pause
      if the track is currently playing.
    - While in menu mode, pressing this button will adjust up the value of the
      currently selected menu item.  Holding this button will continue the
      adjustment.

  - User3
    - Pressing this button will stop playback of the current track.
    - While in menu mode, pressing this button will adjust down the value of
      the currently selected menu item.  Holding this button will continue the
      adjustment.

  - User4
    - Pressing this button will return to the beginning of this track (if the
      current time is greater than 3 seconds) or will skip to the previous
      track (if the current time is less than 3 seconds).
    - Holding this button will seek backwards within the track, if the track is
      being played or is paused, and will continue skipping to the previous
      track, if the track is not being played.
    - While in menu mode, pressing this button will move left within the menu
      items.  Holding this button will continue to move left.
    - If playing Audible content, pressing this button will return to the
      beginning of the current section (if the current time is greater than 3
      seconds into the section) or will skip to the previous section (if the
      current time is less than 3 seconds).  If the current time is within 3
      seconds of the beginning of the track, pressing this button will skip to
      the previous track.

  - User5
    - Pressing this button will advance to the next track.
    - Holding this button will seek forwards within the track, if the track is
      being played or is paused, and will continue skipping to the next track,
      if the track is not being played.
    - While in menu mode, pressing this button will move right within the menu
      items.  Holding this button will continue to move right.
    - If playing Audible content, pressing this button will advance to the next
      section.  If the last section is already being played, it will advance to
      the next track.

  - User6
    - Pressing this button will turn the volume down one step.
    - Holding this button will continue the volume down adjustment.

  - User7
    - Holding this button for one second will start recording.  Recording is
      stopped by pressing the "stop" button (User3).

  - User8
    - Pressing this button will turn the volume up one step.
    - Holding this button will continue the volume up adjustment.

* EP73xx evaluation board

      +-------+  +-------+  +-------+
      | User4 |  | User5 |  | User6 |
      +-------+  +-------+  +-------+

      +-------+  +-------+  +-------+
      | User1 |  | User2 |  | User3 |
      +-------+  +-------+  +-------+

  The definitions of the buttons are identical to the EP7209 evaluation board
  (the buttons are simply arranged differently).

-------------------------------------------------------------------------------

==> Building The Software <==

* Building the device code (player.rom) requires version 2.5 of the ARM
  Software Development Toolkit.  The following steps will build the player:
  - Load "player\player.apj" into the ARM Project Manager.
  - Select the variant that you want to build.  There are several variants to
    choose from:
    - edb7209:
      Builds an image which runs on the EP7209 evaluation board.
    - edb7209_debug:
      Builds an image which runs on the EP7209 evaluation board and contains
      source level debugging information.
    - crd7209b:
      Builds an image which runs on the EP7209 reference design (revision B).
    - crd7209b_debug:
      Builds an image which runs on the EP7209 reference design (revision B)
      and contains source level debugging information.
    - edb73xx:
      Builds an image which runs on the EP73xx evaluation board.
    - edb73xx_debug:
      Builds an image which runs on the EP73xx evaluation board and contains
      source level debugging information.
    The "..._debug" variants are not optimized and contain full symbol tables
    and source line information; it can be used to perform source level
    debugging of the player (through the use of a JTAG debugger, such as
    MultiICE).
  - Click on the "Build" button at the bottom of the player.apj window.

  Once completed, the location of the newly built player image is dependent
  upon the variant built (player\<variant name>\player.rom).  The new player
  image can be downloaded via the steps described above.

* Building the USB driver (mavusb.sys) requires version 6 of Microsoft Visual
  C++ and the Windows NT 4.0 DDK.  The following steps will build the driver:
  - Perform the steps specified by Microsoft to setup the NT build environment.
  - Change directories to the USB driver directory "usbdrvr".
  - Type "build".
  
  Once completed, the location of the newly built driver is dependent upon
  whether a checked or free version was built.  The checked version will be
  located in usbdrvr\objchk\i386\mavusb.sys and the free version will be
  located in usbdrvr\objfre\i386\mavusb.sys.  Copy the newly built driver into
  the main directory (which also contains the INF file) and perform the
  following steps to install the new driver (these steps are for Windows 98):
  - Right click on "My Computer" and select "Properties".
  - Select the "Device Manager" tab.
  - Click on the "+" next to "Universal serial bus controller".
  - Click on the "Cirrus Logic Maverick(tm) Internet Audio Player" (if it does
    not exist, make sure that the player is powered on and running).
  - Click on the "Properties" button at the bottom.
  - Click on the "Driver" tab.
  - Click on the "Update Driver" button.
  - Click on "Next".
  - Specify "Search for a better driver..." and then click on "Next".
  - Choose "Specify a location" and type the location of the main directory in
    the location field.
  - Click on "Next".
  - It should find the INF file for the Maverick Internet Audio Player.  If it
    does not, click on "Back" and verify the usbdrvr directory location.
  - Click on "Next".
  - When it finishes loading the new driver, click on "Finish".

* Building the WMDM service provider (mavspi.dll) requires version 6 of
  Microsoft Visual C++.  The following steps will build the service provider:
  - Load VisualStudio.
  - Load workspace "mavspi\mavspi.dsw".
  - Choose the configuration to be built.  The debug configuration is not
    optimized and contains information for source level debugging of the
    service provider.  The release configuration is optimized and does not
    contain source level debugging information.  Click on "OK" when done.
  - Click on "Build" and select "Build mavspi.dll".

  Once completed, the location of th newly built service provider is dependent
  upon the configuration built.  The debug version is located in
  mavspi\debug\mavspi.dll and the release version is located in
  mavspi\release\mavspi.dll.  Copy the newly built service provider into the
  main directory (which also contains the INF file) and perform the steps given
  above for installing the driver (which also installs the service provider).

* Building the USB application (mavusb.exe) requires version 6 of Microsoft
  Visual C++.  The following steps will build the application:
  - Load VisualStudio.
  - Load workspace "mavusb\mavusb.dsw".
  - Click on "Build" and select "Set Active Configuration".
  - Choose the configuration to be built.  The debug configuration is not
    optimized and contains information for source level debugging of the
    application.  The release configuration is optimized and does contain
    source level debugging information.  Click on "OK" when done.
  - Click on "Build" and select "Build mavusb.exe".

  Once completed, the location of the newly built application is dependent upon
  the configuration built.  The debug version is located in
  mavusb\debug\mavusb.exe and the release version is located in
  mavusb\release\mavusb.exe.  The new version can be run directly by either
  typing the program name on the command line or double clicking on the
  program in Windows Explorer.

-------------------------------------------------------------------------------

==> Resource Consumption <==

The following table shows the approximate ROM usages of the various modules in
the player, along with the exact RAM usage.

    +----------------------------------+----------+----------+
    | Module                           |    ROM   |    RAM   |
    +----------------------------------+----------+----------+
    | Main Player                      |   20,480 |    2,032 |
    | Filesystem                       |   30,720 |      520 |
    | Internal NAND                    |    3,072 |    1,080 |
    | SmartMedia                       |    3,072 |    1,080 |
    | User Interface                   |   16,384 |      172 |
    | Input Processing                 |    1,024 |       76 |
    | Output Processing                |   18,432 |    1,644 |
    | USB                              |    4,096 |       28 |
    +----------------------------------+----------+----------+
    | Software Tone Control            |    1,536 |       64 |
    | Software Volume Control          |    1,024 |       36 |
    | WOW                              |    2,048 |      228 |
    | Q2X                              |    2,048 |      360 |
    | Spatializer                      |    4,096 |      572 |
    +----------------------------------+----------+----------+
    | Rights/PD                        |   44,032 |    2,096 |
    +----------------------------------+----------+----------+
    | MP3                              |   32,768 |  *26,724 |
    | AAC                              |   41,984 |   27,072 |
    | WMA                              |  108,544 |   29,664 |
    | MS ADPCM                         |    6,144 |   21,084 |
    | G.721                            |    5,120 |   10,496 |
    | ACELP.net                        |   28,672 |    5,656 |
    +----------------------------------+----------+----------+

    * The RAM usage of MP3 is not exactly quantifiable since
      there is a block of memory which must reside on a 2K
      byte boundary.  The figure given here is the actual
      memory required by MP3; up to 2K of additional memory
      may be needed to ensure that the block has the required
      2K alignment.

The table is broken up into four pieces; the first set of modules must always
exist and always consume the specified amount of ROM and RAM.  The remaining
modules are individually selectable, are optional, and consume no memory (ROM
or RAM) if not included in the player.  Any module included always consumes the
specified amount of ROM.  The second set of modules always consume the
specified amount of RAM if included in the player.  The third set (which
consists solely of Rights/PD) only comsumes the specified RAM when playing a
file which is encrypted with InterTrust.  The fourth set of modules are the
various codecs availble; they consume the specified RAM only when decoding a
file.

So, a player image which supports only MP3 and MS ADPCM (such as the pre-built
images included in this release) will require approximately 136,192 bytes of
ROM (the images themselves are actually 131,096 bytes) and will consume 6,632
bytes of RAM.  It will then require an additional 26,724 bytes of RAM to decode
an MP3 file (for a total of 33,356 bytes of RAM) or an additional 21,084 bytes
of RAM to decode an MS ADPCM file (for a total of 27,716 bytes of RAM).  Since
there is a total of 38,400 bytes of internal SRAM on the EP7209, we have
sufficient RAM to decode MP3 or MS ADPCM files.

The processing requirements of the various modules is a bit more complicated
since it is dependent upon the speed and width of the memory used to contain
the ROM.  To provide a indication of the processing requirements, measurements
were taken using a single 16-bit wide Intel B3 FLASH (B3 x16), using a pair of
16-bit wide Intel B3 FLASHes in a 32-bit wide configuration (B3 x32), and using
a single 16-bit wide Intel J3 FLASH (J3 x16).  For the Intel B3 FLASH devices,
the memory controller was configured for 3 wait states per access (which is the
fastest possible access speed for the Intel B3 FLASH).  For the Intel J3 FLASH
devices, the memory controller was configured for 3 wait states per random
access and zero wait state per sequential access, with sequential accesses
enabled (which is the fastest possible access speed for the Intel J3 FLASH).
The following table shows the processing requirements for the main modules of
the player in these memory configurations:

    +----------------------------------+----------+----------+----------+
    | Module                           |  B3 x16  |  B3 x32  |  J3 x16  |
    +----------------------------------+----------+----------+----------+
    | Filesystem @ 32kbps              |  0.20MHz |  0.18MHz |  0.18MHz |
    | Filesystem @ 64kbps              |  0.97MHz |  0.86MHz |  0.83MHz |
    | Filesystem @ 128kbps             |  1.36MHz |  1.11MHz |  1.11MHz |
    | Filesystem @ 384kbps             |  3.13MHz |  3.08MHz |  2.93MHz |
    +----------------------------------+----------+----------+----------+
    | Rights/PD @ 128kbps              |  4.27MHz |  2.50MHz |  2.29MHz |
    +----------------------------------+----------+----------+----------+
    | SRC (6-tap, stereo)              | 13.18MHz | 11.91MHz |  9.80MHz |
    | SRC (13-tap, stereo)             | 18.62MHz | 16.87MHz | 15.26MHz |
    +----------------------------------+----------+----------+----------+
    | DAI Interrupt                    |  4.42MHz |  3.69MHz |  3.83MHz |
    +----------------------------------+----------+----------+----------+
    | MP3 @ 32kHz, 128kbps             | 34.70MHz | 27.78MHz | 27.25MHz |
    | MP3 @ 44.1kHz, 128kbps           | 43.42MHz | 36.60MHz | 36.02MHz |
    | MP3 @ 48kHz, 128kbps             | 48.19MHz | 40.37MHz | 39.42MHz |
    +----------------------------------+----------+----------+----------+
    | AAC @ 44.1kHz, 128kbps           | 26.80MHz | 23.00MHz | 22.54MHz |
    | AAC @ 48kHz, 384kbps             | 30.66MHz | 26.17MHz | 25.86MHz |
    +----------------------------------+----------+----------+----------+
    | WMA @ 32kHz, 64kbps              | 25.04MHz | 21.93MHz | 21.58MHz |
    | WMA @ 44.1kHz, 64kbps            | 32.99MHz | 29.37MHz | 28.58MHz |
    | WMA @ 44.1kHz, 128kbps           | 36.15MHz | 32.08MHz | 31.39MHz |
    +----------------------------------+----------+----------+----------+
    | MS ADPCM @ 8kHz, 32kbps          |  0.94MHz |  0.91MHz |  0.91MHz |
    | MS ADPCM @ 11kHz, 45kbps         |  1.27MHz |  1.24MHz |  1.24MHz |
    +----------------------------------+----------+----------+----------+
    | ACELP.net @ 8kHz, 5kbps          | 15.37MHz | 14.23MHz | 14.18MHz |
    | ACELP.net @ 8kHz, 6.5kbps        |  9.98MHz |  9.05MHz |  8.97MHz |
    | ACELP.net @ 8kHz, 8.5kbps        | 10.29MHz |  9.30MHz |  9.24MHz |
    | ACELP.net @ 16kHz, 16kbps        | 30.59MHz | 28.80MHz | 28.75MHz |
    +----------------------------------+----------+----------+----------+
    | SRS Headphone @ 44.1kHz          |        * | 12.23MHz | 10.11MHz |
    | SRS TruBass @ 44.1kHz            |        * | 17.48MHz | 15.40MHz |
    | SRS WOW Minimal @ 44.1kHz        |        * | 21.21MHz | 19.23MHz |
    | SRS WOW @ 44.1kHz                |        * | 25.26MHz | 23.25MHz |
    +----------------------------------+----------+----------+----------+
    | Q2X @ 44.1kHz                    |        * | 19.19MHz | 17.30MHz |
    +----------------------------------+----------+----------+----------+
    | Spatializer @ 44.1kHz            |        * | 27.07MHz | 19.98MHz |
    +----------------------------------+----------+----------+----------+

    * There was not enough processing power in this configuration to
      run these output audio processing algorithms so no measurements
      were taken.

From this it can be seen that a system using faster memory (the J3) in a 16-bit
wide configuration is actually faster than a system using slower memory (the
B3) in a 32-bit wide configuration.  It can also be seen that to play a 44.1kHz
MP3 file encoded at 128kbps on a system with a single 16-bit wide B3 FLASH
takes approximately 49.2MHz (1.36MHz to read from the filesystem, 43.42MHz to
decode the file, and 4.42MHz to play the audio).

There is a modest amount of system overhead that must also be considered when
determining the total processing required.  Additionally, cache effects make
all of the above numbers rough estimates; varying levels of cache thrashing
could change the processing requirement of each component by 5-10% in either
direction (the ROM shift caused by adding a single word to the image could have
dramatic effects on the level of cache thrashing).  Also, two different files
compressed using the same codec, at the same sample rate, and at the same bit
rate will not necessarily require the same processing to decode.

A quick word about the sample rate converter.  All of the filters in the sample
rate converter are 13-tap filters with the exception of the filter used to
convert 48kHz to 44.1kHz.  So, the input rate to the sample rate converter must
be taken into account when determining the processing requiements (for example,
a 48kHz MP3 requires a 6-tap SRC, consuming 13.18MHz on a B3 x16, while a 32kHz
MP3 requires a 13-tap SRC, consuming 18.62MHz on a B3 x16).

The processing requirement of a player built in a particular software
configuration and run on a particular configuration can not be accurately
determined, but the above measuresments can be used to make informed
predictions.

-------------------------------------------------------------------------------

==> Contact Information <==

* Send e-mail to the EPD applications helpline (epdapps@crystal.cirrus.com) for
  assistance or further information about the player or the additional software
  libraries available from Cirrus Logic.

* Visit http://www.microsoft.com/windowsmedia for information about WMA and to
  obtain the full version of the WMA library with digital rights management.

* Send e-mail to rightspd@intertrust.com to obtain the Rights/PD library.

* Send e-mail to Alan Kraemer at SRS Labs, Inc. (alank@srslabs.com) to obtain
  the WOW library.

* Send e-mail to Craig Palmer at QSound Labs, Inc. (craig.palmer@qsound.com) to
  obtain the Q2X library.

* Send e-mail to Randy Roscoe at Spatializer Audio Laboratories, Inc.
  (randyr@spatializer.com) to obtain the Spatializer library.

*******************************************************************************
