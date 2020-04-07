*******************************************************************************


                              Cirrus Logic, Inc.


                                 Maverick(tm)


                             Internet Audio Player
                              Reference Software


                                  Release 16


                                 July 9, 2001


*******************************************************************************


                               ==> Warning <==

Be sure to carefully read this entire file to ensure that the software works
as expected.  Also, do not mix and match software components from different
releases (such as the player from one release with the USB driver from another)
as this will most likely not work correctly.

-------------------------------------------------------------------------------

                               ==> Features <==

* Supports the following codecs:
  - MP3 playback (decoder library included in this package).
  - WMA playback (evaluation decoder library included in this package, full
    decoder library and DRM library must be separately obtained from
    Microsoft).
  - AAC playback (decoder library must be separately obtained from Cirrus
    Logic).
  - ACELP.net playback (decoder library must be separately obtained from
    VoiceAge Corporation).
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

* Contains an evaluation version of the Cirrus Logic SSFDC filesystem,
  supporting the following media:
  - On-board NAND FLASH.
  - SmartMedia cards.
  - MMC cards.

* All strings within the player are stored in Unicode and communicated to the
  host in Unicode.

* Downloads files via the USB at ~400K bytes per second.

* Windows GUI for downloading files via the USB.

* Windows Media Device Manager (WMDM) Service Provider Interface (SPI),
  allowing content download via WMDM-compliant applications, such as Windows
  Media Player Version 7.

* Player software update via the USB.

* Software sample rate conversion for non-44100Hz audio streams.

* Supports the sample clock generator on the EP73xx, eliminating the need for
  the software sample rate converter for 32000Hz and 48000Hz streams.

* Supports the tone and volume controls in the CS43L43 (via communication over
  a two-wire serial interface).

* Supports configuring the CS53L32A (via communication over a two-wire serial
  interface).

* Software tone and volume controls if needed.

* Modular output processing architecture which allows easy addition of third
  party audio processing effects.

* Supports the following third party audio processing effects:
  - WOW(tm) from SRS Labs (audio processing library must be separately obtained
    from SRS).
  - Q2X from QSound Labs (audio processing library included in this package).
  - Digital OnTheGoFX from Spatializer Audio Laboratories (evaluation audio
    processing library included in this package, full version must be
    separately obtained from Spatializer).

* Runs on the EP7209 evaluation board, the EP7212 reference design (revision
  B and C), the EP7309 reference design (revision B and C), the EP73xx
  evaluation board, and the EP7312 evaluation board.

* Can be built with the ARM Software Development Toolkit, Version 2.5, with the
  ARM Developer Suite, Version 1.x, or with the GNU toolchain.

-------------------------------------------------------------------------------

                              ==> Limitations <==

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
  in player/nandfs.c) which only supports a single file and is not really
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

* The evaluation version of Digital OnTheGoFX has the following limitation:
  - A tone will be inserted into the audio stream approximately every 40
    seconds.

  Contact Spatializer to obtain the full version of the OnTheGoFX library.

* While the AudibleReady portable device requirements are implemented in the
  player software, there is no support in the PC software for accessing the
  full AudibleReady functionality of the player.

* While InterTrust's Rights/PD is supported, there is no support in the PC
  software for integrating with InterTrust's host side software.

* SmartMedia cards can only be hot plugged and unplugged on the EP7209
  reference design, not on the EP7209, EP73xx or EP7312 evaluation boards.
  This is due to a limitation in the design of the evaluation boards.

* MMC card support is very minimal.  Both single card sockets and two card
  stacked sockets (with both cards being used) are supported.  With a two card
  socket, the order of the two cards is dependent upon the IDs of the cards,
  not the slots in the socket.  Also, dynamic insertion/removal of MMC card(s)
  is not supported; the card(s) are enumerated at startup and are assumed to
  remain forever inserted.  Finally, there are no MMC sockets on any available
  board, so a socket must be breadboarded onto a board.

* When built with the ARM Developer Suite, the following features are not
  supported (due to the lack of libraries compatible with the ADS linker):
  - Rights/PD

* When built with the GNU toolchain, the following features are not supported
  (due to the lack of libraries compatible with the GNU linker):
  - WMA decode
  - ACELP.net decode
  - Digital OnTheGoFX audio processing

  Also, the GNU compiler is not very efficient about its use of the stack, so
  the default stack size is increased when compiling with the GNU toolchain,
  resulting in less SRAM available for other uses.

* There is not a USB driver for Linux, so while the player can be built on
  Linux and downloaded to the board on Linux, music can not be downloaded to
  the board on Linux.

-------------------------------------------------------------------------------

                               ==> Licensing <==

The following modules must be licensed before they can be included in a
product (the company to contact for licensing is listed as well):

* WMA (Microsoft)
* AAC (Cirrus Logic)
* ACELP.net (VoiceAge)
* WOW (SRS)
* Q2X (QSound)
* Digital OnTheGoFX (Spatializer)
* Cirrus Logic SSFDC filesystem (Cirrus Logic)
* Rights/PD (InterTrust)

Additionally, to use the twowire library, the I2C patent must be licensed from
Philips for use on the Maverick processor.

Obtaining proper licensing is the responsibility of the OEM.

-------------------------------------------------------------------------------

                         ==> Files In This Package <==

bin/* - Windows versions of Unix utilities needed to build the software under
        Windows.

bootcode/* - Source code for the bootup/update program.

crd7212c/* - Pre-built images for use with the EP7212 reference design,
             revision C.

crd7309c/* - Pre-built images for use with the EP7309 reference design,
             revision C.

download/* - Source code for the download utility.

download/download - Pre-built download executable for use on Linux.

download/download.exe - Pre-built download executable for use on Windows.

edb7209/* - Pre-built images for use with the EP7209 evaluation board.

edb7312/* - Pre-built images for use with the EP7312 evaluation board.

player/* - Source code for the Maverick Internet Audio Player.

tools/* - Source code for the tools used during the build process.

tools/buildimg - Pre-built buildimg executable for use on Linux.

tools/buildimg.exe - Pre-built buildimg executable for use on Windows.

tools/package - Pre-built package executable for use on Linux.

tools/package.exe - Pre-built package executable for use on Windows.

unformat/* - Source code for the program which unformats the NAND FLASH on the
             boards.

windows/* - Files which are only for use on Windows.

windows/mavspi/* - Source code for the WMDM SPI.

windows/mavusb/* - Source code for the Windows application for communicating
                   with the player over the USB.

windows/usbdrvr/* - Source code for the Windows 98/Windows NT USB driver for
                    the Maverick Internet Audio Player.

windows/mavspi.dll - Pre-built image of the WMDM SPI.

windows/mavusb.exe - Pre-built image of the Windows communication application.

windows/mavusb.inf - INF file used to install the USB driver.

windows/mavusb.sys - Pre-built image of the USB driver.

asmdefs.h - Definitions for the assembler directivies used by the assembly
            source.

config.h - Source file which contains the software configuration parameters.

ep72xx.h - Source file which contains the register definitions of the EP7209/
           EP7212 and the various external peripherals on the boards.

ep73xx.h - Source file which contains the register definitions of the EP7209/
           EP7312 and the various external peripherals on the boards.

getdefs.awk - Awk script which extracts "#define"s from a file and prints them
              in a syntax which can be read by make.  This is used as part of
              the build process.

hwdefs.h - Source file which includes the appropriate register definition file
           for the Maverick processor being used.

hwport.h - Source file which contains the board specific definitions for the
           player and the unformat code.

makedefs - The default rules used by all makefiles in the source tree.

makefile - The makefile which builds all of the software.

mkarray.awk - Awk script which converts a binary file into a C header file.
              This is used as part of the build process.

readme.txt - This file.

Since the player distribution is in a ZIP file, which does not retain file
permissions on Linux, the pre-built executables for Linux must be made
executable (via the "chmod" command) before they can be run.

-------------------------------------------------------------------------------

                        ==> Downloading The Player <==

Four pre-built player images are supplied in the release distribution:

  edb7209/player.img - For use on the EP7209 evaluation board

  crd7212c/player.img - For use on the EP7212 reference design (revision C)

  crd7309c/player.img - For use on the EP7309 reference design (revision C)

  edb7312/player.img - For use on the EP7312 evaluation board

Alternately, a player image can be built from the source and then downloaded.
Downloading a version of the player to a board other than the one on which it
is intended to run will have unpredictable results (though it will almost
certainly not fully function).  The procedure used to download is dependent
upon the board.

The download utility on Linux requires root authority (since it must open
/dev/ttyS?, which is normally installed with root-only read/write access).
Either run download as root, make download a set-userid root program, or change
the permissions on /dev/ttyS? to be less restrictive.

* EP7209 and EP73xx Evaluation Boards

  The following procedure is used to download to the EP7209 and EP73xx
  evaluation boards:

  - Power off the board.
  - Put a jumper on JP2.
  - Connect "serial port 0" of the board to an available serial port on the PC
    with a NULL modem cable.
  - Run "download {image file name}" on the PC.  If the PC serial port being
    used is not COM1, then specify the number after the name of the ROM (i.e.
    "download {image file name} 3" for COM3).
  - Power on the board.
  - Press the "WAKEUP" button on the board.
  - Wait until "download" says it has successfully downloaded the image.
  - Power off the board.
  - Remove the jumper from JP2.

* EP7212 and EP7309 Reference Designs

  The following procedure is used to download to the EP7212 and EP7309
  reference designs (all versions):

  - Connect the board's serial port to an available serial port on the PC with
    a NULL modem cable.
  - Run "download {image file name}" on the PC.  If the PC serial port being
    used is not COM1, then specify the number after the name of the ROM (i.e.
    "download {image file name} 3" for COM3).
  - Hold the nBROM button on the board.
  - Power on the board (or press and release the POR button if the board is
    already powered on).
  - Once "download" says it has started downloading, release the nBROM button.
  - Wait until "download" says it has successfully downloaded the image.

* EP7312 Evaluation Board

  The following procedure is used to download to the EP7312 evaluation board:

  - Power on the board.
  - Connect "serial port 0" of the board to an available serial port on the PC
    with a NULL modem cable.
  - Run "download {image file name}" on the PC.  If the PC serial port being
    used is not COM1, then specify the number after the name of the ROM (i.e.
    "download {image file name} 3" for COM3).
  - Press and hold the "CPU BROM" button on the board.
  - Press and release the "CPU POR" button on the board.
  - Release the "CPU BROM" button on the board.
  - Wait until "download" says it has successfully downloaded the image.

-------------------------------------------------------------------------------

                      ==> Unformatting The NAND FLASH <==

If a version of the player older than release 12 was previously running on the
board, or a complete erase of the NAND FLASH (restoring it to the factory
state) is desired, the unformat utility must be run before the player can be
run.  Eight pre-built unformat utilities are supplied in the release
distribution:

  edb7209/unformat - For use with the EP7209 evaluation board for use on Linux

  edb7209/unformat.exe - For use with the EP7209 evaluation board for use on
                         Windows

  crd7212c/unformat - For use with the EP7212 reference design (revision C) for
                      use on Linux

  crd7212c/unformat.exe - For use with the EP7212 reference design (revision C)
                          for use on Windows

  crd7309c/unformat - For use with the EP7309 reference design (revision C) for
                      use on Linux

  crd7309c/unformat.exe - For use with the EP7309 reference design (revision C)
                          for use on Windows

  edb7312/unformat - For use with the EP7312 evaluation board for use on Linux

  edb7312/unformat.exe - For use with the EP7312 evaluation board for use on
                         Windows

Using a version of the unformat utility on a board other than the one for
which it is intended to be used will have unpredictable results (though it will
almost certainly not erase the NAND FLASH).

The unformat utility on Linux requires root authority (since it must open
/dev/ttyS?, which is normally installed with root-only read/write access).
Either run unformat as root, make unformat a set-userid root program, or change
the permissions on /dev/ttyS? to be less restrictive.

The procedure for running the unformat utility is the same as running the
download utility (see above), except that "unformat" (with no file name
argument) is run instead of "download".  To specify a different PC serial port,
specify the port number (i.e. "unformat 3" for COM3).

-------------------------------------------------------------------------------

                          ==> Running The Player <==

After the player has been downloaded to the board, and the NAND FLASH has been
erased (if necessary), the player can be run.  The procedure for running the
player is dependent upon the board:

* EP7209 Evaluation Board and EP73xx Evaluation Board
  - Connect "serial port 0" of the board to an available serial port on the PC
    with a NULL modem cable.
  - Run a communications program (such as HyperTerminal) and configure it for
    the PC serial port, 115,200 baud, and 8-N-1.
  - Power on the board.
  - Press the "wakeup" button on the board.
  - A message will appear on the communications program saying "Stopped".

* EP7212 Reference Board and EP7309 Reference Board
  - Power on the board.
  - The Cirrus Logic logo will appear on the LCD, followed by the Maverick
    logo.

* EP7312 Evaluation Board
  - Connect "serial port 0" of the board to an available serial port on the PC
    with a NULL modem cable.
  - Run a communications program (such as HyperTerminal) and configure it for
    the PC serial port, 115,200 baud, and 8-N-1.
  - Power on the board.
  - A message will appear on the communications program saying "Stopped".

-------------------------------------------------------------------------------

                      ==> Installing The USB Drivers <==

Currently, USB drivers for the Internet Audio Player are only available for
Windows (98, NT 4, Me, and 2000).  It is not possible to utilize the USB
interface on the board when using Linux (and therefore it is not possible to
download music to the board using Linux).

Once the player is running on the board, connect the USB cable between the
board and the PC.  If Windows does not have a driver for the player, it will
start the Add New Hardware Wizard to install drivers (see the Installing For
The First Time section below for details).  If drivers are already installed,
they will be loaded by Windows.  If running a new release of the player
software for the first time, or if newly built drivers need to be installed,
see the Updating Existing Drivers section below for details.

If a previous version of the player was previously running on the board, then
the new USB drivers must be installed on the PC in order to ensure proper
operation of the mavusb program.

The procedure given for installing drivers was taken from Windows 98; the
procedure in other versions of Windows might be slightly different.  Also,
Windows might be configured to not allow installation of device drivers; if so,
consult your system administrator for help.

                     --> Installing For The First Time <--

If drivers do not exist for the Internet Audio Player, the Add New Hardware
Wizard will appear on the screen when the player is plugged into the USB port.
When it does, perform the following steps:

* Click on "Next".
* Specify "Search for the best driver..." and then click on "Next".
* Choose "Specify a location" and type the location of the windows directory
  within the player software in the location field.
* Click on "Next".
* It should fin the INF file for the Maverick Internet Audio Player.  If it
  does not, click on "Back" and verify the directory location.
* Click on "Next".
* When it finishes loading the driver, click on "Finish".

                       --> Updating Existing Drivers <--

To update existing drivers to the drivers supplied with this package, or to
install newly built drivers, perform the following steps:

* Make sure that the player is running and connected to the computer's USB
  port.
* Right click on "My Computer" and select "Properties".
* Select the "Device Manager" tab.
* Click on the "+" next to "Universal serial bus controller".
* Click on the "Cirrus Logic Maverick(tm) Internet Audio Player".
* Click on the "Properties" button at the bottom.
* Click on the "Driver" tab.
* Click on the "Update Driver" button.
* Click on "Next".
* Specify "Search for a better driver..." and then click on "Next".
* Choose "Specify a location" and type the location of the windows directory
  within the player software in the location field.
* Click on "Next".
* It should find the INF file for the Maverick Internet Audio Player.  If it
  does not, click on "Back" and verify the directory location.
* Click on "Next".
* When it finishes loading the new driver, click on "Finish".

-------------------------------------------------------------------------------

                           ==> Downloading Music <==

The mavusb.exe program is used to download music to the player.  With the
player running on the board and the USB cable connecting the board to the PC,
run the mavusb program.  Do not use a mavusb program from a previous version of
the player as it will most likely not work as expected.

Mavusb will display the set of files that are on the player.  Files can be
downloaded to the player in one of three ways: drag and drop a file from
someplace else onto the mavusb window, paste a file from the clipboard (Paste
from the Edit menu, or by pressing Ctrl-V), or opening a file with mavusb (Open
from the File menu, or by pressing Ctrl-O).  The mavusb application must be
closed before the player can be used to play the music that has been downloaded
to it.

The mavusb program can also be used to update the software that is running on
the player.  Selecting Update from the Tools menu will start the update
process.  After specifying the file containing the new software to be used, the
software on the player is updated and the player restarted.  The new software
must be packaged before it can be programmed into the player; this is done
automatically as part of the player build (producing player/player.pkg).

The mavusb menus are summarized below:

* File

  Open - Reads a file from the PC and writes it to the player.  Can also be
         accessed by pressing Ctrl-O.

  Save As... - Reads a file from the player and writes it to the PC.  This
               option is only available if one or more files in the mavusb
               window are selected.

  Exit - Exits the mavusb program.

* Edit

  Paste - Reads a file from the clipboard on the PC and writes it to the
          player.  This option is only available if there is a file on the
          clipboard.  Cal also be accessed by pressing Ctrl-V.

  Delete - Deletes the specified file(s) from the player.  This option is only
           available if one or more files in the mavusb window are selected.
           Can also be accessed by pressing Del.

  Select All - Selects all the files on the player.  Can also be accessed by
               pressing Ctrl-A.

* View

  Large Icons - Displays the files on the player using large icons.

  Small Icons - Displays the files on the player using small icons.

  List - Displays the files on the player using a list format.

  Details - Displays the files on the player using a list format with
            additional file details (file size and creation date/time).

  Refresh - Regenerates the list of files on the player by re-scanning the
            player.

* Drive

  Internal - Displays files from the on-board NAND FLASH.  This also becomes
             the location where files are copied to the player.

  External - Displays files from the SmartMedia card.  This also becomes the
             location where files are copied to the player.

* Tools

  Status - Displays the status of the currently selected drive.  The number of
           files, space used, space available, and total space are all
           displayed.

  Format - Formats the currently selected drive, erasing all files on the
           drive.

  Update - Writes new player software to the player.

* Help

  About - Displays a message box giving the version of the mavusb program.

-------------------------------------------------------------------------------

                           ==> Using The Player <==

The player controls are different on each board since each board has a
different set of buttons and a different display.  Therefore, the operation of
the player is different on each board:

* EP7209 Evaluation Board

  This board has six buttons and no display (some of these boards have a
  640x240 ALPS LCD, but this requires too much memory for a frame buffer and
  comsumes too much bus bandwidth to refresh, so it is not used).  Due to the
  lack of a usable display, a serial port is used to "display" status
  information as the player runs.  The six buttons on the board are arranged
  as follows:

      +-------+  +-------+  +-------+  +-------+  +-------+  +-------+
      | User1 |  | User2 |  | User3 |  | User4 |  | User5 |  | User6 |
      +-------+  +-------+  +-------+  +-------+  +-------+  +-------+

  The meanings of these buttons are as follows:

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
      current time is greater than 3 seconds) or to the previous track (if the
      current time is less than 3 seconds).
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

* EP7212 Reference Design (revision B and C) and EP7309 Reference Design
  (revision B and C)

  This board has eight buttons and a small LCD display (128x33).  The LCD is
  used to display the status of the player, and the eight buttons are arranged
  as follows:

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

  The meanings of these buttons are as follows:

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

* EP73xx Evaluation Board

  This board has six buttons and no display (some of these boards have a
  640x240 ALPS LCD, but this requires too much memory for a frame buffer and
  comsumes too much bus bandwidth to refresh, so it is not used).  Due to the
  lack of a usable display, a serial port is used to "display" status
  information as the player runs.  The six buttons on the board are arranged
  as follows:

      +-------+  +-------+  +-------+
      | User4 |  | User5 |  | User6 |
      +-------+  +-------+  +-------+

      +-------+  +-------+  +-------+
      | User1 |  | User2 |  | User3 |
      +-------+  +-------+  +-------+

  The definitions of the buttons are identical to the EP7209 evaluation board
  (the buttons are simply arranged differently), with one exception:

  - User2
    - Pressing this button will stop playback of the current track.
    - Holding this button for one second will start recording.  Recording is
      stopped by pressing the "stop" button (User2).

* EP7312 Evaluation Board

  This board has six buttons and a large color LCD display (320x240).  Since
  the LCD is so large, it requires too much memory for a frame buffer and
  consumes too much bus bandwidth to refresh, so it is not used.  A serial port
  is instead used to "display" status information as the player runs.  The six
  buttons are arranged as follows:

      +-------+  +-------+  +-------+
      | User2 |  | User4 |  | User6 |
      +-------+  +-------+  +-------+

      +-------+  +-------+  +-------+
      | User1 |  | User3 |  | User5 |
      +-------+  +-------+  +-------+

  The definitions of the buttons are identical to the EP73xx evaluation board
  (the buttons are simply arranged differently).

-------------------------------------------------------------------------------

                         ==> Building The Software <==

The software can be broken into two categories; the code that runs on the ARM
and the code that runs on the PC.  Separate toolchains are used to build each
set of code.  The software must be configured before it can be built, to
specify the toolchains to be used, the board on which the software is to run,
and the features that should be included in the software.

                              --> Configuring <--

The configuration of the software is contained in a file called config.h.  A
set of "#define"s are used to specify how to build the software, including the
toolchains to use and the software features to include.  This file should be
modified (if necessary) before the software is built.

The "#define"s are described below:

* CROSSTOOLS is the name of the toolchain that is used to cross-compile the
  ARM code.  The following toolchains are currently supported:

  gcc - The GNU toolchain is used to build the ARM code.  This was tested using
        version 2.95.3 of gcc and version 2.11 of binutils; other versions may
        or may not work.
  sdt25 - The ARM Software Development Toolkit, Version 2.5 (or 2.51) is used
          to build the ARM code.
  ads - The ARM Developer Suite is used to build the ARM code.

* NATIVETOOLS is the name of the toolchain that is used to compile the PC code.
  The following toolchains are currently supported:

  gcc - The GNU toolchain is used to build the PC code (for use under Linux).
  msvc - Microsoft VisualStudio is used to build the PC code (for use under
         Windows).

* BOARD is the type of board on which the ARM code is to be run.  This affects
  the code in the bootcode, player, and unformat directories.  The following
  boards are currently supported:

  EP7209_Eval_Board - The Cirrus Logic EP7209 Evaluation Board, part number
                      EDB7209-4.

  EP7212_Reference_Board_Rev_A - The Cirrus Logic EP7212 Reference Design, Rev.
                                 A (this board was never generally available).

  EP7212_Reference_Board_Rev_B - The Cirrus Logic EP7212 Reference Design, Rev.
                                 B (this board was never generally available).

  EP7309_Reference_Board_Rev_B - The Cirrus Logic EP7309 Reference Design, Rev.
                                 B (this board was never generally available).

  EP7212_Reference_Board_Rev_C - The Cirrus Logic EP7212 Reference Design, Rev.
                                 C, part number CRD7212-C.

  EP7309_Reference_Board_Rev_C - The Cirrus Logic EP7309 Reference Design, Rev.
                                 C, part number CRD7309-C.

  EP73XX_Eval_Board - The Cirrus Logic EP73xx Evaluation Board (this board was
                      never generally available).

  EP7312_Eval_Board - The Cirrus Logic EP7312 Evaluation Board, part number
                      EDB7312.

* SUPPORT_AAC specifies that the AAC codec should be included in the player.
  This should only be defined once the AAC library and header files have been
  obtained and placed into the player/aac directory.

* SUPPORT_ACELPNET specifies that the ACELP.net codec should be included in the
  player.  This should only be defined once the ACELP.net library and header
  files have been obtained and placed into the player/acelpnet directory.
  There are four variants of the ACELP.net codec that can be individually
  selected by defining one of the following "#define"s:

  - SUPPORT_ACELPNET_5K0 is a variant of ACELP.net that produces audio at 8kHz
    from a 5kbps bitstream.
  - SUPPORT_ACELPNET_6K5 is a variant of ACELP.net that produces audio at 8kHz
    from a 6.5kbps bitstream.
  - SUPPORT_ACELPNET_8K5 is a variant of ACELP.net that produces audio at 8kHz
    from a 8.5kbps bitstream.
  - SUPPORT_ACELPNET_16K0 is a variant of ACELP.net that produces audio at
    16kHz from a 16kbps bitstream.

* SUPPORT_G721 specifies that the G.721 codec should be included in the player.
  This codec produces a 8kHz audio stream from a 32kbps bitstream, and can also
  encode a 8kHz audio stream to a 32kbps bitstream.

* SUPPORT_MP3 specifies that the MP3 codec should be included in the player.

* SUPPORT_MSADPCM specifies that the Microsoft ADPCM codec should be included
  in the player.  This codec produces a 8kHz audio stream from a 32kbps
  bitstream, and can also encode a 8kHz audio stream to a 32kbps bitstream.

* SUPPORT_WMA specifies that the WMA codec should be included in the player.
  This should only be defined once the WMA library and header files have been
  obtained and placed into the player/wma directory (the evalutation version of
  the WMA libraries must be extracted from a Microsoft supplied Windows
  executable included in this distribution).

* SUPPORT_AUDIBLE specifies that support for the Audible Portable Device
  Requirements should be included in the player.

* SUPPORT_SRS specifies that support for SRS Labs' audio processing should be
  included in the player.  All audio will pass through the SRS audio processing
  algorithm.  This should only be defined once the SRS library and header files
  have been obtained and placed into the player/srs directory.

  There are four different variants of SRS' audio processing, one of which can
  be included at a time.  The variants are (in order from the most MIPS to the
  least...consult SRS' documentation for details of these variants):

  - SUPPORT_WOW_FULL - The full version of WOW, which includes stereo
                       enhancement and improved bass response.
  - SUPPORT_WOW_MINIMAL - A version of WOW that approximates the full version
                          of WOW, but uses 20% less processing.
  - SUPPORT_TRU_BASS - Provides improved bass response.
  - SUPPORT_SRS_HEADPHONE - Provides stereo enhancement.

* SUPPORT_QSOUND specifies that support for QSound Labs' audio processing
  should be included in the player.  All audio will pass through the QSound
  audio processing algorithm.

* SUPPORT_SPATIALIZER specifies that support for Spatializer Laboratories'
  audio processing should be included in the player.  All audio will pass
  through the Spatializer audio processing algorithm.

* SUPPORT_RIGHTS_PD specifies that support for InterTrust's Rights/PD should be
  included in the player.  This should only be defined once the Rights/PD
  library and header files have been obtained and placed into the player/itrust
  directory.

* SUPPORT_SPECTRUM_ANALYZER specifies that the software spectrum analyzer
  should be included in the player.  This should only be specified when
  building an image for one of the reference design boards as they are the only
  ones able to display the spectral information (though the spectrum analyzer
  will work on any board, processing power permitting).

                               --> Building <--

If the software is to be built under Windows, make sure that the bin directory
within the player software appears in the search path.  Since the bin directory
contains some tools which might already be on the system (such as make and
sort), it must appear early in the search path.  The makefiles require at least
version 3.77 of GNU make (version 3.78.1 is supplied with this package) and a
bourne shell compatible shell named sh.exe (which is also supplied).  The Unix
version of sort is required (the Windows sort command does not support the "-u"
flag).  These utilities are all pre-built GNU software extracted from a larger
package of common GNU Unix utilities; the full package (UnxUtils.zip) and
source is available on SourceForge (http://unxutils.sourceforge.net).

To build the software (either on Windows or on Linux), open a command prompt,
change directory into the main directory within the player software, and type
"make".  This will build the following components:

* The build tools.
* The player itself.
* The download utility.
* The unformat utility.

When complete, there will be a "player.img" in the main directory within the
player software; this is an image that can be downloaded to the board (as
described above) and run.  There will also be a "player.pkg" in the player
directory; this is an image that can be used by mavusb to update the software
on the board.

After the entire package has been built, individual components can be re-built
by simply running "make" within that component's directory.  The most common
way to do this would be to re-build the player directory (creating
"player.pkg"), updating the software on the board with mavusb, and repeating
as more changes/additions are made to the player.  Keep in mind that unless
"make" is run in a directory (or in one of it's parent directories), the build
objects within that directory are not necessarily up to date (i.e. if the
player directory is modified and then individually rebuilt, "player.pkg" is up
to date but "player.img" in the main directory is not).

To use the evaluation version of WMA supplied with the player, it must first be
installed.  To do so, run either player/wma/WMA_Dec_Emb_ARM_AIF_eval_setup or
player/wma/WMA_Dec_Emb_ARM_ELF_eval_setup, depending upon the version required.
The AIF version is used for ARM Software Development Toolkit 2.5, and the ELF
version is used for ARM Developer Suite 1.x.  The install directory should be
player/wma.  After installing, there will be a player/wma/lib directory which
must be renamed as appropriate for the toolchain being used to cross-compile:

    sdt25 - If using ARM Software Development Toolkit 2.5
    ads - If using ARM Developer Suite 1.x

Additionally, the libraries for the ELF version need to be renamed to have a
".a" extension instead of ".elf".

If the full version is obtained from Microsoft, the same procedure must be
followed to make the libraries available to the build.

Building the Windows specific software requires the use of Microsoft specific
tools and development environments:

* Building the USB driver (mavusb.sys) requires version 6 of Microsoft Visual
  C++ and the Windows NT 4.0 DDK.  The following steps will build the driver:
  - Open a command prompt.
  - Perform the steps specified by Microsoft to setup the NT build environment.
  - Change directories to the USB driver directory "windows/usbdrvr".
  - Type "build".
  
  Once completed, the location of the newly built driver is dependent upon
  whether a checked or free version was built.  The checked version will be
  located in windows/usbdrvr/objchk/i386/mavusb.sys and the free version will
  be located in windows/usbdrvr/objfre/i386/mavusb.sys.  Copy the newly built
  driver into the windows directory within the player software (which also
  contains the INF file) and perform the procedure given above for
  re-installing the drivers.

* Building the WMDM service provider (mavspi.dll) requires version 6 of
  Microsoft Visual C++.  The following steps will build the service provider:
  - Load VisualStudio.
  - Load workspace "windows/mavspi/mavspi.dsw".
  - Click on "Build" and select "Set Active Configuration".
  - Choose the configuration to be built.  The debug configuration is not
    optimized and contains information for source level debugging of the
    service provider.  The release configuration is optimized and does not
    contain source level debugging information.  Click on "OK" when done.
  - Click on "Build" and select "Build mavspi.dll".

  Once completed, the location of th newly built service provider is dependent
  upon the configuration built.  The debug version is located in
  windows/mavspi/debug/mavspi.dll and the release version is located in
  windows/mavspi/release/mavspi.dll.  Copy the newly built service provider
  into the windows directory within the player software (which also contains
  the INF file) and perform the steps given above for re-installing the drivers
  (which also installs the service provider).

* Building the USB application (mavusb.exe) requires version 6 of Microsoft
  Visual C++.  The following steps will build the application:
  - Load VisualStudio.
  - Load workspace "windows/mavusb/mavusb.dsw".
  - Click on "Build" and select "Set Active Configuration".
  - Choose the configuration to be built.  The debug configuration is not
    optimized and contains information for source level debugging of the
    application.  The release configuration is optimized and does contain
    source level debugging information.  Click on "OK" when done.
  - Click on "Build" and select "Build mavusb.exe".

  Once completed, the location of the newly built application is dependent upon
  the configuration built.  The debug version is located in
  windows/mavusb/debug/mavusb.exe and the release version is located in
  windows/mavusb/release/mavusb.exe.  The new version can be run directly by
  either typing the program name on the command line or double clicking on the
  program in Windows Explorer.

-------------------------------------------------------------------------------

                         ==> Resource Consumption <==

The following table shows the approximate ROM usages of the various modules in
the player, along with the exact RAM usage.

    +------------------------------------------------+----------+----------+
    | Module                                         |    ROM   |    RAM   |
    +------------------------------------------------+----------+----------+
    | Main Player                                    |   20,480 |    2,032 |
    | Filesystem                                     |   30,720 |      520 |
    | Internal NAND                                  |    3,072 |    1,080 |
    | SmartMedia                                     |    3,072 |    1,080 |
    | User Interface                                 |   16,384 |      172 |
    | Input Processing                               |    1,024 |       76 |
    | Output Processing                              |   18,432 |    1,644 |
    | USB                                            |    4,096 |       28 |
    +------------------------------------------------+----------+----------+
    | Software Tone Control                          |    1,536 |       64 |
    | Software Volume Control                        |    1,024 |       36 |
    | Spectrum Analyzer                              |    2,048 |      224 |
    | WOW                                            |    2,048 |      228 |
    | Q2X                                            |    2,048 |      360 |
    | Digital OnTheGoFX                              |    4,096 |      124 |
    +------------------------------------------------+----------+----------+
    | Rights/PD                                      |   44,032 |    2,096 |
    +------------------------------------------------+----------+----------+
    | MP3                                            |   32,768 |  *26,724 |
    | AAC                                            |   41,984 |   27,072 |
    | WMA                                            |  108,544 |   29,664 |
    | MS ADPCM                                       |    6,144 |   21,084 |
    | G.721                                          |    5,120 |   10,496 |
    | ACELP.net                                      |   28,672 |    5,656 |
    +------------------------------------------------+----------+----------+
    * The RAM usage of MP3 is not exactly quantifiable since there is a
      block of memory which must reside on a 2K byte boundary.  The figure
      given here is the actual memory required by MP3; up to 2K of
      additional memory may be needed to ensure that the block has the
      required 2K alignment.

The table is broken up into four groups; the first set of modules must always
exist and always consume the specified amount of ROM and RAM.  The remaining
modules are individually selectable, are optional, and consume no memory (ROM
or RAM) if not included in the player.  Any module included always consumes the
specified amount of ROM.  The second set of modules always consume the
specified amount of RAM if included in the player.  The third set (which
consists solely of Rights/PD) only comsumes the specified RAM when playing a
file which is encrypted with InterTrust.  The fourth set of modules are the
various codecs available; they consume the specified RAM only when decoding a
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
the ROM.  To provide an indication of the processing requirements, measurements
were taken using a single 16-bit wide Intel B3 FLASH, using a pair of 16-bit
wide Intel B3 FLASHes in a 32-bit wide configuration, using a single 64
megabit, 16-bit wide Intel J3 FLASH, and using a single 128 megabit, 16-bit
wide Intel J3 FLASH.  For the Intel B3 FLASH devices, the memory controller was
configured for 3 wait states per access (which is the fastest possible access
speed for the Intel B3 FLASH).  For the 64 megabit Intel J3 FLASH device, the
memory controller was configured for 3 wait states per random access and zero
wait state per sequential access, with sequential accesses enabled (which is
the fastest possible access speed for the 64 megabit Intel J3 FLASH).  For the
128 megabit Intel J3 FLASH device, the memory controller was configured for 5
wait states per random access and one wait state per sequential access, with
sequential accesses enabled (which is the fastest possible access speed for the
128 megabit J3 FLASH).  The following table shows the processing requirements
for the main modules of the player in these memory configurations:

                                 +----------+----------+----------+----------+
              random wait states |     3    |     3    |     3    |     5    |
          sequential wait states |     3    |     3    |     0    |     1    |
                           width |    16    |    32    |    16    |    16    |
  +------------------------------+----------+----------+----------+----------+
  | Module                       |          |          |          |          |
  +------------------------------+----------+----------+----------+----------+
  | Filesystem @ 32kbps          |  0.20MHz |  0.18MHz |  0.18MHz |        + |
  | Filesystem @ 64kbps          |  0.97MHz |  0.86MHz |  0.83MHz |        + |
  | Filesystem @ 128kbps         |  1.36MHz |  1.11MHz |  1.11MHz |  1.15MHz |
  | Filesystem @ 384kbps         |  3.13MHz |  3.08MHz |  2.93MHz |  3.11MHz |
  +------------------------------+----------+----------+----------+----------+
  | Rights/PD @ 128kbps          |  4.27MHz |  2.50MHz |  2.29MHz |        + |
  +------------------------------+----------+----------+----------+----------+
  | SRC (6-tap, stereo)          | 13.18MHz | 11.91MHz |  9.80MHz |        + |
  | SRC (13-tap, stereo)         | 18.62MHz | 16.87MHz | 15.26MHz |        + |
  +------------------------------+----------+----------+----------+----------+
  | DAI Interrupt                |  4.42MHz |  3.69MHz |  3.83MHz |  4.53MHz |
  +------------------------------+----------+----------+----------+----------+
  | MP3 @ 32kHz, 128kbps         | 34.70MHz | 27.78MHz | 27.25MHz |        + |
  | MP3 @ 44.1kHz, 128kbps       | 43.42MHz | 36.60MHz | 36.02MHz | 42.32MHz |
  | MP3 @ 48kHz, 128kbps         | 48.19MHz | 40.37MHz | 39.42MHz |        + |
  +------------------------------+----------+----------+----------+----------+
  | AAC @ 44.1kHz, 128kbps       | 26.80MHz | 23.00MHz | 22.54MHz |        + |
  | AAC @ 48kHz, 384kbps         | 30.66MHz | 26.17MHz | 25.86MHz | 29.73MHz |
  +------------------------------+----------+----------+----------+----------+
  | WMA @ 32kHz, 64kbps          | 25.04MHz | 21.93MHz | 21.58MHz |        + |
  | WMA @ 44.1kHz, 64kbps        | 32.99MHz | 29.37MHz | 28.58MHz |        + |
  | WMA @ 44.1kHz, 128kbps       | 36.15MHz | 32.08MHz | 31.39MHz |        + |
  +------------------------------+----------+----------+----------+----------+
  | MS ADPCM @ 8kHz, 32kbps      |  0.94MHz |  0.91MHz |  0.91MHz |        + |
  | MS ADPCM @ 11kHz, 45kbps     |  1.27MHz |  1.24MHz |  1.24MHz |        + |
  +------------------------------+----------+----------+----------+----------+
  | ACELP.net @ 8kHz, 5kbps      | 15.37MHz | 14.23MHz | 14.18MHz |        + |
  | ACELP.net @ 8kHz, 6.5kbps    |  9.98MHz |  9.05MHz |  8.97MHz |        + |
  | ACELP.net @ 8kHz, 8.5kbps    | 10.29MHz |  9.30MHz |  9.24MHz |        + |
  | ACELP.net @ 16kHz, 16kbps    | 30.59MHz | 28.80MHz | 28.75MHz |        + |
  +------------------------------+----------+----------+----------+----------+
  | SRS Headphone @ 44.1kHz      |        * | 12.23MHz | 10.11MHz |        * |
  | SRS TruBass @ 44.1kHz        |        * | 17.48MHz | 15.40MHz |        * |
  | SRS WOW Minimal @ 44.1kHz    |        * | 21.21MHz | 19.23MHz |        * |
  | SRS WOW @ 44.1kHz            |        * | 25.26MHz | 23.25MHz |        * |
  +------------------------------+----------+----------+----------+----------+
  | Q2X @ 44.1kHz                |        * | 19.19MHz | 17.30MHz |        * |
  +------------------------------+----------+----------+----------+----------+
  | Digital OnTheGoFX @ 44.1kHz  |        * | 11.06MHz |  8.16MHz |        * |
  +------------------------------+----------+----------+----------+----------+
  | Spectrum Analyzer @ 44.1kHz  |        * |        = | 16.27MHz |        * |
  +------------------------------+----------+----------+----------+----------+
  * There was not enough processing power in this configuration to run these
    output audio processing algorithms so no measurements were taken.
  + These measurements were not taken as the general performance of this
    configuration can be judged by the measurements that were taken.
  = The spectrum analyzer performance was not measured in this configuration
    (which is the EP7209 evaluation board) since it does not make sense to run
    the spectrum analyzer on that platform.

From this it can be seen that a system using faster memory (the 64 megabit J3)
in a 16-bit wide configuration is actually faster than a system using slower
memory (the B3) in a 32-bit wide configuration.  It can also be seen that to
play a 44.1kHz MP3 file encoded at 128kbps on a system with a single 16-bit
wide B3 FLASH takes approximately 49.2MHz (1.36MHz to read from the filesystem,
43.42MHz to decode the file, and 4.42MHz to play the audio).

There is a modest amount of system overhead that must also be considered when
determining the total processing required.  Additionally, cache effects make
all of the above numbers rough estimates; varying levels of cache thrashing
could change the processing requirement of each component by 5-10% in either
direction (the ROM shift caused by adding a single word to the image could have
dramatic effects on the level of cache thrashing).  Also, two different files
compressed using the same codec, at the same sample rate, and at the same bit
rate will not necessarily require the same processing to decode.

All of the filters in the sample rate converter are 13-tap filters with the
exception of the filter used to convert 48kHz to 44.1kHz.  So, the input rate
to the sample rate converter must be taken into account when determining the
processing requiements (for example, a 48kHz MP3 requires a 6-tap SRC,
consuming 13.18MHz on a B3 x16, while a 32kHz MP3 requires a 13-tap SRC,
consuming 18.62MHz on a B3 x16).

The processing requirement of a player built in a particular software
configuration and run on a particular hardware configuration can not be
accurately pre-determined, but the above measuresments can be used to make
informed predictions.

-------------------------------------------------------------------------------

                          ==> Contact Information <==

* Send e-mail to the EPD applications helpline (epdapps@crystal.cirrus.com) for
  assistance or further information about the player or the additional software
  libraries available from Cirrus Logic.

* Visit http://www.microsoft.com/windowsmedia for information about WMA and to
  obtain the full version of the WMA library with digital rights management.

* Send e-mail to Bruno Deslauriers at VoiceAge Corporation
  (brunod@voiceage.com) to obtain the ACELP.net library.

* Send e-mail to rightspd@intertrust.com to obtain the Rights/PD library.

* Send e-mail to Alan Kraemer at SRS Labs (alank@srslabs.com) to obtain the WOW
  library.

* Send e-mail to Craig Palmer at QSound Labs (craig.palmer@qsound.com) to
  license the Q2X library.

* Send e-mail to Randy Roscoe at Spatializer Audio Laboratories
  (randyr@spatializer.com) to obtain the full version of the Digital OnTheGoFX
  library.


*******************************************************************************
