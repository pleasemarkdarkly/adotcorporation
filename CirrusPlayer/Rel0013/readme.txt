*******************************************************************************

                              Cirrus Logic, Inc.

                                 Maverick(tm)

                             Internet Audio Player
                              Reference Software

                                  Release 13

                               October 26, 2000

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

* Evaluation version of the Cirrus Logic SSFDC filesystem.

* Downloads files via the USB at ~400K bytes per second.

* Windows GUI for downloading files via the USB.

* Software sample rate conversion for non-44100Hz audio streams.

* Supports the sample clock generator on the EP73xx, eliminating the need for
  the software sample rate converter for 32000Hz and 48000Hz streams.

* Supports the tone and volume controls in the CS43L43 (via communication over
  the I2C bus).

* Supports configuring the CS53L32A (via communication over the I2C bus).

* Software tone and volume controls if needed.

* Modular output processing architecture which allows easy addition of third
  party audio processing effects.

* Supports the following third party audio processing effects:
  - WOW from SRS Labs (audio processing library must be separately obtained
    from SRS Labs).
  - Q2X from QSound (audio processing library must be separately obtained from
    QSound).

* Runs on the EP7209 evaluation board, the EP7209 reference design, and the
  EP73xx evaluation board.

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

* SmartMedia cards can only be hot plugged and unplugged on the EP7209
  reference design, not on the EP7209 or EP73xx evaluation boards.  This is
  due to a limitation in the design of the evaluation boards.

-------------------------------------------------------------------------------

==> File In This Package <==

crd7209\*.* - Pre-built images for use with the EP7209 reference design.

edb7209\*.* - Pre-built images for use with the EP7209 evaluation board.

edb73xx\*.* - Pre-built images for use with the EP73xx evaluation board.

mavusb\*.* - Source code for the Windows application for communicating with the
             player over the USB.

mavusb\mavusb.exe - Pre-built image of the Windows communication application.

player\*.* - Source code for the Maverick(tm) Internet Audio Player.

unformat\*.* - Source code for the program which unformats the NAND FLASH on
               the boards.

usbdrvr\*.* - Source code for the Windows 98/Windows NT USB driver for the
              Maverick(tm) Internet Audio Player.

usbdrvr\mavusb.inf - INF file used to install the USB driver.

usbdrvr\mavusb.sys - Pre-built image of the USB driver.

config.inc & config.h - Source files which contain the board specific
                        definitions for the player and the unformat code.

dl_b3.exe - Program to download data into an Intel B3 NOR FLASH.

dl_c3.exe - Program to download data into an Intel C3 NOR FLASH.

dl_j3.exe - Program to download data into an Intel J3 NOR FLASH (StrataFLASH).

hwdefs.h & hwdefs.inc - Sources files which contain the register definitions
                        of the Maverick(tm) device and the various external
                        peripherals on the boards.

readme.txt - This file.

-------------------------------------------------------------------------------

==> How To Use The Player <==

Three player images are supplied in the release distribution:

    edb7209\player.rom - for use on the EP7209 evaluation board

    crd7209\player.rom - for use on the EP7209 reference design (revision B)

    edb73xx\player.rom - for use on the EP73xx evaluation board

The first thing that must be done is to download the appropriate player image
onto the board.  There are three programs which can be used to download the
player image, based on the NOR FLASH on the board:

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

* EP7209 reference design
  - Power off the board.
  - Connect the board's serial port to an available serial port on the PC with
    a NULL modem cable.
  - Run "dl_j3 crd7209\player.rom" on the PC.  If the PC serial port being used
    is not COM1, then specify the number after the name of the ROM (i.e. "dl_j3
    crd7209\player.rom 3" for COM3).
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

Once the image is downloaded, it is almost time to run the player.  First, if
a previous version of the player was already on the board then the NAND FLASH
needs to be unformatted.  The unformat procedure for the three boards is as
follows:

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

* EP7209 reference design
  - Power off the board.
  - Connect the board's serial port to an available serial port on the PC with
    a NULL modem cable.
  - Run "crd7209\unformat" on the PC.  If the PC serial port being used is not
    COM1, then specify the number on the command line (i.e. "crd7209\unformat
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
cable.  Windows will ask for a driver, which is located in the "usbdrvr"
directory.  Once the driver is loaded, you can then run "mavusb\mavusb", which
will allow you to download files to the NAND FLASH of the board.

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

Once you have files on the player, it is time to play the files.  Once the
Windows application is closed, the status display of the player should have
been updated to indicate the currently loaded file.  You can now press "play"
to play the song.

There are six buttons on the evaluation board, and eight on the reference
design.  These buttons have the (almost) same meanings on all three board, but
are arranged differently on the boards.  The following is the arrangement of
the buttons on each board:

* EP7209 evaluation board

      +-------+  +-------+  +-------+  +-------+  +-------+  +-------+
      | User1 |  | User2 |  | User3 |  | User4 |  | User5 |  | User6 |
      +-------+  +-------+  +-------+  +-------+  +-------+  +-------+

* EP7209 reference design

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

* EP73xx evaluation board

      +-------+  +-------+  +-------+
      | User4 |  | User5 |  | User6 |
      +-------+  +-------+  +-------+

      +-------+  +-------+  +-------+
      | User1 |  | User2 |  | User3 |
      +-------+  +-------+  +-------+

The buttons have the following functions:

* User1
  - Holding this button for one and one half seconds will put the player into
    low power mode.
  - Pressing this button on the EP7209 and EP73xx evaluation boards will toggle
    a 12dB treble boost.
  - Pressing this button on the EP7209 reference design will take you into and
    out of menu mode.

* User2
  - Pressing this button will start playback of the current track, or pause if
    the track is currently playing.
  - While in menu mode on the EP7209 reference design, pressing this button
    will adjust up the value of the currently selected menu item.  Holding this
    button will continue the adjustment.

* User3
  - Pressing this button will stop playback of the current track.
  - While in menu mode on the EP7209 reference design, pressing this button
    will adjust down the value of the currently selected menu item.  Holding
    this button will continue the adjustment.

* User4
  - Pressing this button will return to the beginning of this track (if the
    current time is greater than 3 seconds) or will skip to the previous track
    (if the current time is less than 3 seconds).
  - Holding this button will seek backwards within the track, if the track is
    being played or is paused, and will continue skipping to the previous
    track, if the track is not being played.
  - While in menu mode on the EP7209 reference design, pressing this button
    will move left within the menu items.  Holding this button will continue
    to move left.
  - If playing Audible content, pressing this button will return to the
    beginning of the current section (if the current time is greater than 3
    seconds into the section) or will skip to the previous section (if the
    current time is less than 3 seconds).  If the current time is within 3
    seconds of the beginning of the track, pressing this button will skip to
    the previous track.

* User5
  - Pressing this button will advance to the next track.
  - Holding this button will seek forwards within the track, if the track is
    being played or is paused, and will continue skipping to the next track, if
    the track is not being played.
  - While in menu mode on the EP7209 reference design, pressing this button
    will move right within the menu items.  Holding this button will continue
    to move right.
  - If playing Audible content, pressing this button will advance to the next
    section.  If the last section is already being played, it will advance to
    the next track.

* User6
  - Pressing this button will turn the volume down one step.
  - Holding this button will continue the volume down adjustment.

* User7
  - Holding this button for one second will start recording.  Recording is
    stopped by pressing the "stop" button (User3).

* User8
  - Pressing this button will turn the volume up one step.
  - Holding this button will continue the volume up adjustment.

Although User7 and User8 do not exist on the evaluation board, they still
function within the player.  While recording is not supported on the evaluation
boards, pressing User7 will toggle a 12dB bass boost, and User8 works as on the
reference design.  User7 and User8 can be simulated by using the ";" and "P"
buttons (respectively) on the Cherry keyboard that is supplied with some of the
evaluation boards.

-------------------------------------------------------------------------------

==> Building The Software <==

* Building the device code (player.rom) requires version 2.5 of the ARM
  Software Development Toolkit.  The following steps will build the player:
  - Load "player\player.apj" into the ARM Project Manager.
  - Select the variant that you want to build.  The "Debug" variant is not
    optimized and contains full symbol tables and source line information; it
    can be used to perform source level debugging of the player (through the
    use of a JTAG debugger, such as MultiICE).  The "Image" variant is fully
    optimized and does not contain source level debugging information.
  - Click on the "Build" button at the bottom of the player.apj window.

  Once completed, the location of the newly built player image is dependent
  upon the variant built.  The debug variant is located in
  player\debug\player.rom and the image variant is located in
  player\image\player.rom.  The new player image can be downloaded via the
  steps described above.

  In order to build the player for a specific Cirrus evaluation or reference
  board, the following must be done (the code as supplied will build for the
  EP7209 evaluation board):
  - Edit both "config.h" and "config.inc".
  - Find the set of defines near the top for "EP7209_Eval_Board",
    "EP7209_Reference_Board_Rev_A", "EP7209_Reference_Board_Rev_B", and
    "EP73XX_Eval_Board".  One of these four should be uncommented and the other
    three commented.  The uncommented line should match between "config.h" and
    "config.inc".  Uncomment "EP7209_Eval_Board" if you have an EP7209
    evaluation board (number "EDB7209-??.?"), "EP7209_Reference_Board_Rev_A" if
    you have an EP7209 reference design rev. A (number "CRDEP7209A.0"),
    "EP7209_Reference_Board_Rev_B" if you have an EP7209 reference design rev.
    B (number "CRD7209B.0"), or "EP73XX_Eval_Board" if you have an EP73XX
    evaluation board (number "EDB73XX-??.?").
  - Save the new version of "config.h" and "config.inc".
  - Rebuild the player as described above.

* Building the USB driver (mavusb.sys) requires version 6 of Microsoft Visual
  C++ and the Windows NT 4.0 DDK.  The following steps will build the driver:
  - Perform the steps specified by Microsoft to setup the NT build environment.
  - Change directories to the USB driver directory "usbdrvr".
  - Type "build".
  
  Once completed, the location of the newly built driver is dependent upon
  whether a checked or free version was built.  The checked version will be
  located in usbdrvr\objchk\i386\mavusb.sys and the free version will be
  located in usbdrvr\objfre\i386\mavusb.sys.  Copy the newly built driver into
  the usbdrvr directory (which also contains the INF file) and perform the
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
  - Specify "Specify a location" and type the location of the usbdrvr directory
    in the location field.
  - Click on "Next".
  - It should find the INF file for the Maverick(tm) Internet Audio Player.  If
    it does not, click on "Back" and verify the usbdrvr directory location.
  - Click on "Next".
  - When it finishes loading the new driver, click on "Finish".

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

==> Contact Information <==

* Send e-mail to the EPD applications helpline (epdapps@crystal.cirrus.com) for
  assistance or further information about the player or the additional software
  libraries available from Cirrus Logic.

* Visit http://www.microsoft.com/windowsmedia for information about WMA and to
  obtain the full version of the WMA library with digital rights management.

* Send e-mail to Alan Kraemer at SRS Labs (alank@srslabs.com) to obtain the WOW
  library.

* Send e-mail to Craig Palmer at QSound (craig.palmer@qsound.com) to obtain the
  Q2X library.

*******************************************************************************
