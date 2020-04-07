//11/20/2000
#include "JPlaylist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

JPlaylist::JPlaylist(char* name) {
	strcpy(m_sName, name);
	MetakitManager::GetInstance()->InitializeAPlaylist(*this);
}

JPlaylist::~JPlaylist() {
}
	//add an item to this playlist
void JPlaylist::AddItem(int indexInMaster){
	MetakitManager::GetInstance()->AddItemToPlaylist(m_vView, indexInMaster);
}

	//find a song in this playlist
int JPlaylist::FindSong(char* title, char* artist){
	return MetakitManager::GetInstance()->FindSongInPlaylist(m_vView, title, artist);
}
	//deleta song from this playlist
void JPlaylist::DeleteItem(int indexInPlaylist){
	MetakitManager::GetInstance()->DeleteItemFromPlaylist(m_vView, indexInPlaylist);
}
	//return the name of this playlist
char* JPlaylist::GetName(){
	return m_sName;
}
	//find the embedded playlist who's name is
int JPlaylist::FindPlaylist(char *name) {
	return MetakitManager::GetInstance()->FindPlaylistInPlaylist(*this, name);
}

	//sort this playlist on specified eColumn
void JPlaylist::SortOn(structColumns &cols){
	m_vView = MetakitManager::GetInstance()->SortOn(m_vView, cols);
}
	//print songs
void JPlaylist::PrintSongs(bool viewsOnly){
	MetakitManager::GetInstance()->PrintPlaylist(m_vView, viewsOnly);
}
	
void JPlaylist::PrintSongsToFile(char* name) {
	MetakitManager::GetInstance()->PrintPlaylistToFile(m_vView, name);
}

JPlaylist& JPlaylist::operator= (const JPlaylist &jp){
	m_vView = jp.m_vView;
	return *this;
}

int JPlaylist::GetSize() {
	return MetakitManager::GetInstance()->GetPlaylistSize(*this);
}

int JPlaylist::StartCharComplete(char firstChar, eColumn col,JPlaylist& results) {
	return MetakitManager::GetInstance()->StartCharComplete(firstChar, col, *this, results);
}

int JPlaylist::NextChar(char ch, int position, eColumn col, JPlaylist& results){
	MetakitManager::GetInstance()->NextChar(ch, position, col, results);
	return -1;
}
	//prints the section that matches characters thus far (will only include those that fit 
	//in the strings[] array)
/*
void JPlaylist::PrintSelection(){
	MetakitManager::GetInstance()->PrintSelection(*this);
}
*/

JSong JPlaylist::GetSong(int index)
{
	return MetakitManager::GetInstance()->GetPlaylistSong(*this,index);
}

void JPlaylist::SetFlags(int index, int iFlags)
{
	MetakitManager::GetInstance()->SetPlaylistFlags(*this,index,iFlags);
}

	// (todo) creates Ed's playlist for playing
void JPlaylist::PlaySongs(){} 
