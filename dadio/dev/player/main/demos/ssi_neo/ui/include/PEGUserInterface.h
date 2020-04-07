// SerialUserInterface.h: demo user interface over serial port
// danc@iobjects.com 07/28/01
// (c) Interactive Objects

#ifndef __SIMPLEGUI_USERINTERFACE_H__
#define __SIMPLEGUI_USERINTERFACE_H__

#include <main/ui/common/UserInterface.h>
#include <gui/peg/peg.hpp>

class CPEGUserInterface : public IUserInterface
{
public:
    CPEGUserInterface();
    ~CPEGUserInterface();

    void SetBanner( const char* pBanner );
    void ShowDirections();
    
    void NotifyPlaying();
    void NotifyPaused();
    void NotifyStopped();
    
    void NotifyNextTrack();
    void NotifyPreviousTrack();

    void NotifySeekFwd();
    void NotifySeekRew();

    void NotifyMediaInserted( int MediaIndex );
    void NotifyMediaRemoved( int MediaIndex );
    
    // Set track data
    void SetTrack( set_stream_event_data_t* pSSED );
    void SetTrackTime( int Seconds );

    void SetPlaylistMode( IPlaylist::PlaylistMode mode );

    void SetVolume( int iVolume );
    void SetBass( int iBass );
    void SetTreble( int iTreble );

    // Set a one-line text message to be displayed to the user
    void SetMessage( const char* szMessage, ... );
    void ClearTrack();

private:
};


#endif // __SIMPLEGUI_USERINTERFACE_H__