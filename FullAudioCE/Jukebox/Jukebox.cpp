#include "Jukebox.h"
#include "JPlaylist.h"
#include <stdio.h>
#include <stdlib.h>
#include "string.h"

Jukebox::Jukebox() : m_plSortOnGenre("sortOnGenre"), m_plSortOnTitle("sortOnTitle"), 
					 m_plSortOnArtist("sortOnArtist"), m_plSortOnAlbum("sortOnAlbum"), m_plEmbeddedPlaylists("embeddedPlaylists")
										
{
	InitJukebox();
}

Jukebox::~Jukebox(){}


	//singleton
Jukebox* Jukebox::m_spJukebox = 0;
Jukebox* Jukebox::GetInstance() {
	if (!m_spJukebox) {
		m_spJukebox = new Jukebox();
	}
	return m_spJukebox;
}

void Jukebox::InitJukebox(){
	cols.NumCols = 1;
	cols.columns[0] = GENRE;
	MetakitManager::GetInstance()->SortMasterOn(m_plSortOnGenre, cols);
	cols.columns[0] = TITLE;
	MetakitManager::GetInstance()->SortMasterOn(m_plSortOnTitle, cols);
	cols.columns[0] = ARTIST;
	MetakitManager::GetInstance()->SortMasterOn(m_plSortOnArtist, cols);
	cols.columns[0] = ALBUM;
	MetakitManager::GetInstance()->SortMasterOn(m_plSortOnAlbum, cols);
}

void Jukebox::GetAllSongs(JPlaylist &pl, structColumns &cols){
	if (cols.NumCols == 1 && cols.columns[0] == GENRE) 
		pl = m_plSortOnGenre;
	else if (cols.NumCols == 1 && cols.columns[0] == TITLE) 
		pl = m_plSortOnTitle;
	else if (cols.NumCols == 1 && cols.columns[0] == ARTIST) 
		pl = m_plSortOnArtist;
	else 
		MetakitManager::GetInstance()->SortMasterOn(pl, cols);
}

int Jukebox::FindSong(char* title, char* artist){
	return MetakitManager::GetInstance()->FindSongInJukebox(title, artist);
}

void Jukebox::AddSong(structMetadata md) {
	MetakitManager::GetInstance()->AddSongToJukebox(md);
}

void PlayerAddFile(const char* szFilename, const char* szFullFilename, unsigned short usDate, unsigned short usTime, unsigned short ubTimeTenths)
{
	structMetadata md;
	strncpy(md.FileName, szFilename, MAX_STRING_SIZE);
	strncpy(md.FilePath, szFullFilename, MAX_STRING_SIZE); 
	md.Date = usDate;
	md.Time = usTime;
	md.TimeTenths =ubTimeTenths;
	Jukebox::GetInstance()->AddSong(md);
}

void Jukebox::DeleteSong(int index) {
	MetakitManager::GetInstance()->DeleteItemFromJukebox(index);
}


JPlaylist* Jukebox::CreatePlaylist(char* name){
	JPlaylist* pl = new JPlaylist(name);
	MetakitManager::GetInstance()->AddPlaylistToJukebox(*pl, name);
	return pl;
}
//Use in conjunction with CreatePlaylist, when you want to keep the playlist in the 
//jukebox but free the memory used to create it
void Jukebox::ReleaseStoredPlaylist(JPlaylist* pl) {
	delete pl;
}
//Use in conjunction with CreatePlaylist, when you want to completely remove a playlist from 
//the jukebox and memory
void Jukebox::DestroyPlaylist(JPlaylist* pl) {
	int index = MetakitManager::GetInstance()->FindPlaylistInJukebox((*pl).GetName()); 
	MetakitManager::GetInstance()->DeleteItemFromJukebox(index);
	delete pl;
}

void Jukebox::ViewPlaylist(JPlaylist pl){
	pl.PrintSongs();
}


void Jukebox::ViewDatabase(){
	MetakitManager::GetInstance()->PrintDatabase();
}

void Jukebox::StoreDatabase(){
	MetakitManager::GetInstance()->SaveDatabase();
}

int Jukebox::FindPlaylistInMaster(char* name){
	return MetakitManager::GetInstance()->FindPlaylistInJukebox(name);
}

void Jukebox::RetrievePlaylistFromMaster(int index, JPlaylist &jp){
	MetakitManager::GetInstance()->GetEmbeddedPlaylist(index, jp);
}

int Jukebox::FindPlaylistInPlaylist(JPlaylist &parent_pl, char* name) {
	return MetakitManager::GetInstance()->FindPlaylistInPlaylist(parent_pl, name);
}


void Jukebox::AddPlaylistToPlaylist(JPlaylist &parent_pl, int indexInMaster){
	parent_pl.AddItem(indexInMaster);
}

void Jukebox::DeletePlaylistFromPlaylist(JPlaylist &parent_pl, int indexInPlaylist){
	parent_pl.DeleteItem(indexInPlaylist);
}


JPlaylist Jukebox::GetStandardPlaylist(eColumn col){
	switch (col) {
	case TITLE: return m_plSortOnTitle;
	case ARTIST: return m_plSortOnArtist;
	case GENRE: return m_plSortOnGenre;
	case ALBUM: return m_plSortOnAlbum;
	default: printf("Unsupported column\n"); exit(1);
	}
}

void Jukebox::SortAllPlaylists(){
	cols.NumCols = 1;
	cols.columns[0] = VIEWS;
	MetakitManager::GetInstance()->SortMasterOn(m_plEmbeddedPlaylists, cols);
}

int Jukebox::GetRealIndexOfSong(JPlaylist &pl, int indexInPlaylist){
	return MetakitManager::GetInstance()->GetIndexInMasterForCopy(indexInPlaylist, pl);
}

void Jukebox::ViewAllPLaylists(){
	m_plEmbeddedPlaylists.PrintSongs(true);
}

int Jukebox::StartCharComplete(char firstChar, eColumn col, JPlaylist &pl, JPlaylist& results){
	return MetakitManager::GetInstance()->StartCharComplete(firstChar, col, pl, results);
}
int Jukebox::NextChar(char ch, int position, eColumn col, JPlaylist &pl , JPlaylist& results){
	MetakitManager::GetInstance()->NextChar(ch, position, col, results);
	return -1;
}

/*
void Jukebox::PrintSelection(JPlaylist &pl){
	MetakitManager::GetInstance()->PrintSelection(pl);
}
*/

int Jukebox::GetSize() {
	return MetakitManager::GetInstance()->GetMasterSize();
}

//(todo) this function will need to implemented to use ed's playlist that is created
void Jukebox::PlayPlaylist(JPlaylist pl){}