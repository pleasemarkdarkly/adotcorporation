========================================================================
       Windows CE APPLICATION Workspace : FullAudioCE
========================================================================

This is a collection of useful tidbits of information when working with the
FullAudio for Windows CE workspace.  

OUTPUT DIRECTORIES FOR BINARIES

Release binaries output to the following directories for the given platforms:

	ARM		-		./v2/WinCE/Builds/ARM/Release
	SH3		-		./v2/WinCE/Builds/SH3/Release
	MIPS	-		./v2/WinCE/Builds/MIPS/Release

Debug binaries output to appropriate platform subdirectory under the 
respective projects.

FULLAUDIOCE CONFIGURATION

All binaries associated with FullAudioCE, namely DSS and its codecs, are
functionally complete.  All crippling is determined at compile-time of the
FullAudioCE project.  The supported configurations are determined by settings
in the stdafx.h header file.  Please refer to the header for more information
on configuring a build of FullAudioCE.

MEDIAPLAYER CONFIGURATION

Currently the downsampling rate for MP3 is hardcoded in CMediaPlayer::SetSong in
Mediaplayer.cpp in the FullAudioCE application (Lines 149-152 for SH3, 166-169 for
ARM).  This is done by enumerating through the destination formats supported by the
given source format, and making sure that the destination format's sampling frequency
is half that of the source.

 
GETTING WMA WORKING

Alright, so you've gotten the production version of WMA decoder for the ARM
and SH3 platforms.  You're almost done!  Decompress the package, and copy the
platform directory and its subdirectories into the WMA_DEC subdirectory in
the WMACodec project directory on your local machine, which can be found at:

					./v2/WinCE/WMACodec/WMA_DEC

Setup the linker to reference the new static libraries.  To make my life easier,
I abbreviated the decoder and DRM libraries as follows:

	Platform	Original Name							Abbreviation
	=========================================================================
	ARM			WMA_Dec_WinCE_arm_eval.lib				wmadec.lib
	ARM			WMA_Dec_WinCE_ARM_DRM_Eval.lib			wmadrm.lib
	SH3			WMA_Dec_WinCE_sh3_eval.lib				wmadec.lib
	SH3			WMA_Dec_WinCE_sh3_DRM_Eval.lib			wmadrm.lib

	ARM lib directory can be found at ./v2/WinCE/WMACodec/WMA_DEC/ARM/lib
	SH3 lib directory can be found at ./v2/WinCE/WMACodec/WMA_DEC/SH3/lib

PORTING TO OTHER CE PLATFORMS

In order to get the projects working on MIPS and SH4 platforms, use the project
settings for the ARM and SH3 platforms as guidelines for the include and link
directories.  You'll need to create codeccrypt and appcrypt libraries for each
platform as well, as well as the respective WMA codec.

DSS CLIENT/CODEC AUTHENTICATION

Here's the basics.  Each codec has a 64-bit key that it uses to encrypt an
authentication code.  Each application has an array of these keys internal to
itself which it enumerates through to decrypt the authentication code.  The
necessary logic for applications and codecs is created through the appcrypt and
codeccrypt projects, respectively.  These projects create static libraries that
DSS client applications and DSS codecs must link against.  DSS performs the 
necessary handshaking through the interfaces defined in these projects.  For ARM 
and SH3, I've provided the following libraries:

												CodecCrypt
		Platform		AppCrypt		MP3Codec			WMACodec
		=============================================================
		ARM				apparm.lib		ccarm.lib			ccarm.lib
		SH3				appsh3.lib		ccsh3.lib			ccsh3.lib

AppCrypt libraries are in $/FullAudioCE/v2/WinCE/FullAudioCE.
MP3Codec libraries are in $/FullAudioCE/v2/WinCE/MP3Codec.
WMACodec libraries are in $/FullAudioCE/v2/WinCE/WMACodec.

Currently, the codeccrypt libraries for MP3 and WMA have USE_KEY in Driver.cpp 
undefined, which means that these codecs are open to any application.  In order 
to set the key for a codec, change line the value of dwKey on line 38 of Driver.cpp.  
Below is an example of setting codeccrypt for WMA.  Make sure to define USE_KEY,
otherwise, your key won't be used.

	static DWORD dwKey[2] = { 0xDA7B632D, 0xB489461F }; //WMACodec

For applications, you can create an array of authentication keys, with each key
representing a codec that that application is permitted to work with.  In order to
do so, set the array beginning at line 14 of App.cpp to include all the keys you 
wish the application linking to the static library you're building to support.  
Below is an example of an application allowed to link to WMA and MP3.

	static const DWORD g_AppKeys[2][2] = {
		{ 0x8A7B8E22, 0x67A6480A }, //MP3Codec
		{ 0xDA7B632D, 0xB489461F }, //WMACodec
	};

JUKEBOX

The Jukebox project creates a static library containing the Jukebox and the metakit.
This library is currently being linked into the FullAudioCE application.  If you
undefine INCLUDE_JUKEBOX in FullAudioCE, you're ripping the guts out of the playlist
management.  This is done currently for purposes of being able to hand the code to
FullAudio.  I believe most if not all references to INCLUDE_JUKEBOX occur in MyCollection.h
and MyCollection.cpp.

