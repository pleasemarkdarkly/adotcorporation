//........................................................................................
//........................................................................................
//.. File Name: MetakitManager.cpp														..
//.. Date: 12/05/2000																	..
//.. Author(s): Donni Reitz-Pesek														..
//.. Description of content: Implementation of the MetakitManager class	 				..
//.. Usage: Used to implement jukebox functions on the metakit database					..
//.. Last Modified By: Donni Reitz-Pesek@iobjects.com									..	
//.. Modification date: 7/19/2001														..
//........................................................................................
//.. Copyright:(c) 1995-2000 Interactive Objects Inc.  									..
//..	 All rights reserved. This code may not be redistributed in source or linkable  ..
//.. 	 object form without the express written consent of Interactive Objects.        ..
//.. Contact Information: www.iobjects.com												..
//........................................................................................
//........................................................................................
#include "MetakitManager.h"
#include "JPlaylist.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

//#include <cyg/infra/diag.h>
#define diag_printf printf

static const char* sc_szDBSchema = "[Unique_id:I,UUID:S,URL:S,Genre:S,Artist:S,Album:S,Title:S,Filename:S,Filepath:S,VoteValue:I,Year:I,Song_length:I,Playcount:I,Sample_frequency:I,Bitrate:I,Num_channels:I,Flags:I,Embedded_playlist_name:S,Embedded_playlist:V,File_creation_date:S,Songlength:S,ArtistID:I,GenreID:I]";


	//initialize the metakit structure
MetakitManager::MetakitManager(char* filename) : m_ipId ("Unique_id"), m_spUUID("UUID"),  m_spURL("URL"),  m_spTitle ("Title"),  
	m_spArtist ("Artist"), m_spAlbum ("Album"), m_spGenre ("Genre"), m_spFname ("Filename"), 
	m_spFpath ("Filepath"), m_ipVoteValue ("Vote value"), m_ipYear ("Year"), m_ipPlaycount ("Playcount"), m_ipFlags ("Flags"), 
	m_ipSamplefreq ("Sample_frequency"), m_ipBitrate ("Bitrate"), m_ipNumchans ("Num_channels"), 
	m_vpEmbplaylist ("Embedded_playlist"), m_spEmbname ("Embedded_playlist_name"), 
	m_spSonglength ("Songlength"), m_spCreationDate ("File_creation_date"), m_ipDate ("Date"),
	m_ipTime ("Time"), m_ipTimeTenths ("Tenths"), m_ipArtistID ("Artist_id"), m_ipGenreID ("Genre_id"),
	m_sStorage (filename, true)
{
	char* szBlah = (char*)malloc(strlen(sc_szDBSchema) + 8);
	sprintf(szBlah, "Master%s", sc_szDBSchema);
	m_vMaster = m_sStorage.GetAs(szBlah);
	m_vArtist = m_sStorage.GetAs("zArtist[Artist_id:I,Artist:S]");
	m_vGenre = m_sStorage.GetAs("zGenre[Genre_id:I,Genre:S]");
	free(szBlah);
}

MetakitManager::~MetakitManager(){}


	//singleton
MetakitManager* MetakitManager::m_spMetakitmanager = 0;
MetakitManager* MetakitManager::GetInstance() {
	if (!m_spMetakitmanager) {
		m_spMetakitmanager = new MetakitManager("juke.dat");
	}
	return m_spMetakitmanager;
}

void MetakitManager::InitializeAPlaylist(JPlaylist &jp){
	char *str = (char*)malloc(strlen(jp.m_sName)+strlen(sc_szDBSchema)+1);
	strcpy(str, jp.m_sName);
	strcat(str, sc_szDBSchema);
	jp.m_vView = m_sStorage.GetAs(str);
}

int MetakitManager::AddSongToJukebox(const structMetadata& md){
	m_ipId(m_rAddSongRow) = md.SongId;
	m_spFpath(m_rAddSongRow) = md.FilePath;
	m_spFname(m_rAddSongRow) = md.FileName;
	m_ipFlags(m_rAddSongRow) = md.Flags;
	m_spUUID(m_rAddSongRow) = md.szUUID;
	m_spURL(m_rAddSongRow) = md.szURL;
	m_spTitle(m_rAddSongRow) = md.Title;
	m_spArtist(m_rAddSongRow) = md.Artist;
	m_spGenre(m_rAddSongRow) = md.Genre;
	m_spAlbum(m_rAddSongRow) = md.Album;
	m_ipVoteValue(m_rAddSongRow) = md.iVoteValue;
	m_ipYear(m_rAddSongRow) = md.Year;
	m_ipPlaycount(m_rAddSongRow) = md.PlayCount;
	m_ipSamplefreq(m_rAddSongRow) = md.SampleFreq;
	m_ipBitrate(m_rAddSongRow) = md.BitRate;
	m_ipNumchans(m_rAddSongRow) = md.NumChannels;
	m_spSonglength(m_rAddSongRow) = md.SongLength;
	m_spCreationDate(m_rAddSongRow) = md.FileCreationDate;
	m_ipDate(m_rAddSongRow) = md.Date;
	m_ipTime(m_rAddSongRow) = md.Time;
	m_ipTimeTenths(m_rAddSongRow) = md.TimeTenths;
    return m_vMaster.Add(m_rAddSongRow);	
}

int MetakitManager::AddRowToArtistTable(int id, char* artist) {
	m_ipArtistID(m_rDARRow) = id;
	m_spArtist(m_rDARRow) = artist;
	return m_vArtist.Add(m_rDARRow);
}

const char* MetakitManager::GetArtistFromArtistTable(int id) {
	m_ipArtistID(m_rDARFindRow) = id;
	int index = m_vArtist.Find(m_rDARFindRow);
	return m_spArtist(m_vArtist[index]);
}

int MetakitManager::AddRowToGenreTable(int id, char* genre) {
	m_ipGenreID(m_rDARRow) = id;
	m_spGenre(m_rDARRow) = genre;
	return m_vGenre.Add(m_rDARRow);
}

const char* MetakitManager::GetGenreFromGenreTable(int id) {
	m_ipGenreID(m_rDARFindRow) = id;
	int index = m_vGenre.Find(m_rDARFindRow);
	return m_spGenre(m_vGenre[index]);
}

void MetakitManager::AddPlaylistToJukebox(JPlaylist &pl, char* name) {
	m_vpEmbplaylist(m_rAddViewRow) = pl.m_vView;
	m_spEmbname(m_rAddViewRow) = name;
	m_vMaster.Add(m_rAddViewRow);
}

void MetakitManager::DeleteItemFromJukebox(int index) {
	m_vMaster.RemoveAt(index);
}

int MetakitManager::FindSongInJukebox(char* Title, char* Artist){
	m_spTitle(m_rFindSongRow) = Title;
	m_spArtist(m_rFindSongRow) = Artist;
	return m_vMaster.Find(m_rFindSongRow);
}

int MetakitManager::FindPlaylistInJukebox(char* name){ 
	m_spEmbname(m_rFindViewRow) = name;
	return m_vMaster.Find(m_rFindViewRow);
} 

//fill the metadata structure with the information on song at index in master playlist
void MetakitManager::FillMetadataFromMaster(structMetadata &md, int index){
	const char* str;
	str = m_spAlbum(m_vMaster[index]);
	strcpy(md.Album, str);
	str = m_spArtist(m_vMaster[index]);
	strcpy(md.Artist, str);
	md.BitRate = m_ipBitrate(m_vMaster[index]);
	str = m_spFname(m_vMaster[index]);
	strcpy(md.FileName, str);
	str = m_spFpath(m_vMaster[index]);
	strcpy(md.FilePath, str);
	md.Flags = m_ipFlags(m_vMaster[index]);
	str = m_spGenre(m_vMaster[index]);
	strcpy(md.Genre, str);
	md.NumChannels = m_ipNumchans(m_vMaster[index]);
	md.PlayCount = m_ipPlaycount(m_vMaster[index]);
	md.SampleFreq = m_ipSamplefreq(m_vMaster[index]);
	md.SongId = m_ipId(m_vMaster[index]);
	str = m_spSonglength(m_vMaster[index]);
	strcpy(md.SongLength, str);
	str = m_spUUID(m_vMaster[index]);
	strcpy(md.szUUID, str);
	str = m_spURL(m_vMaster[index]);
	strcpy(md.szURL, str);
	str = m_spTitle(m_vMaster[index]);
	strcpy(md.Title, str);
	md.iVoteValue = m_ipVoteValue(m_vMaster[index]);
	md.Year = m_ipYear(m_vMaster[index]);
	str = m_spCreationDate(m_vMaster[index]);
	strcpy(md.FileCreationDate, str);
}

void MetakitManager::AddItemToPlaylist(c4_View view, int indexInMaster) {
	view.Add(m_vMaster[indexInMaster]);
}

void MetakitManager::DeleteItemFromPlaylist(c4_View view, int indexInPlaylist) {
	view.RemoveAt(indexInPlaylist);
}

int MetakitManager::FindSongInPlaylist(c4_View view, char* Title, char* Artist){
	m_spTitle(m_rFindSongRow) = Title;
	m_spArtist(m_rFindSongRow)= Artist;
return view.Find(m_rFindSongRow);
}

int MetakitManager::FindPlaylistInPlaylist(JPlaylist &pl, char* name) {
	m_spEmbname(m_rFindViewRow) = name;
	return pl.m_vView.Find(m_rFindViewRow);	
}

void MetakitManager::GetEmbeddedPlaylist(int index, JPlaylist &jp) {
//	jp.m_vView = m_vpEmbplaylist(m_vMaster[index]);
	jp.m_vView.Concat(m_vpEmbplaylist(m_vMaster[index]));
}


void MetakitManager::SortMasterOn(JPlaylist &pl, structColumns &cols) {
	c4_View sortOn;
	eColumn cur_col;
	for(int i=0; i<cols.NumCols; ++i) {
		cur_col = cols.columns[i];
		switch (cur_col) {
		case UUID: sortOn.AddProperty(m_spUUID);
			break;
		case URL: sortOn.AddProperty(m_spURL);
			break;
		case TITLE: sortOn.AddProperty(m_spTitle);
			break;
		case ARTIST: sortOn.AddProperty(m_spArtist);
			break;
		case GENRE:	sortOn.AddProperty(m_spGenre);
			break;
		case ID: sortOn.AddProperty(m_ipId);
			break;
		case ALBUM: sortOn.AddProperty(m_spAlbum);
			break;
		case VOTEVALUE: sortOn.AddProperty(m_ipVoteValue);
			break; 
		case YEAR: sortOn.AddProperty(m_ipYear);
			break; 
		case FILEPATH: sortOn.AddProperty(m_spFpath);
			break;
		case FILENAME: sortOn.AddProperty(m_spFname);
			break;
		case SONGLENGTH: sortOn.AddProperty(m_spSonglength);
			break;
		case PLAYCOUNT: sortOn.AddProperty(m_ipPlaycount);
			break;
		case SAMPLEFREQ: sortOn.AddProperty(m_ipSamplefreq);
			break;
		case BITRATE: sortOn.AddProperty(m_ipBitrate);
			break;
		case NUMCHANNELS: sortOn.AddProperty(m_ipNumchans);
			break;
		case FLAGS: sortOn.AddProperty(m_ipFlags);
			break;
		case VIEWS: sortOn.AddProperty(m_spEmbname);
			break;
		case CREATIONDATE: sortOn.AddProperty(m_spCreationDate);
			break;
		default: diag_printf("Attempt to sort on unsupported eColumn\n"); 
			exit(1);
		}
	}
	pl.m_vView = m_vMaster.SortOn(sortOn);
//	PrintPlaylist(pl.m_vView);
}

	//return a copy of the specified playlist sorted on specified eColumns
c4_View MetakitManager::SortOn(c4_View &view, structColumns &cols){

		if (view == m_vMaster) return view;

	c4_View sortOn;
	eColumn cur_col;
	for(int i=0; i<cols.NumCols; ++i) {
		cur_col = cols.columns[i];
		switch (cur_col) {
		case UUID: sortOn.AddProperty(m_spUUID);
			break;
		case URL: sortOn.AddProperty(m_spURL);
			break;
		case TITLE: sortOn.AddProperty(m_spTitle);
			break;
		case ARTIST: sortOn.AddProperty(m_spArtist);
			break;
		case GENRE:	sortOn.AddProperty(m_spGenre);
			break;
		case ID: sortOn.AddProperty(m_ipId);
			break;
		case ALBUM: sortOn.AddProperty(m_spAlbum);
			break;
		case VOTEVALUE: sortOn.AddProperty(m_ipVoteValue);
			break; 
		case YEAR: sortOn.AddProperty(m_ipYear);
			break; 
		case FILEPATH: sortOn.AddProperty(m_spFpath);
			break;
		case FILENAME: sortOn.AddProperty(m_spFname);
			break;
		case SONGLENGTH: sortOn.AddProperty(m_spSonglength);
			break;
		case PLAYCOUNT: sortOn.AddProperty(m_ipPlaycount);
			break;
		case SAMPLEFREQ: sortOn.AddProperty(m_ipSamplefreq);
			break;
		case BITRATE: sortOn.AddProperty(m_ipBitrate);
			break;
		case NUMCHANNELS: sortOn.AddProperty(m_ipNumchans);
			break;
		case FLAGS: sortOn.AddProperty(m_ipFlags);
			break;
		case VIEWS: sortOn.AddProperty(m_spEmbname);
			break;
		case CREATIONDATE: sortOn.AddProperty(m_spCreationDate);
			break;
		default: diag_printf("Attempt to sort on unsupported eColumn\n"); 
			exit(1);
		}
	}
	sortOn = view.SortOn(sortOn);
	return sortOn;
}

//MMT - added playlist song
JSong MetakitManager::GetPlaylistSong(JPlaylist &jp, int index)
{
	return JSong(jp.m_vView[index]);
}

void MetakitManager::SetPlaylistFlags(JPlaylist& jp,int index,int iFlags)
{
	m_rCopyRow = jp.m_vView[index];
	int master_index = m_vMaster.Find(m_rCopyRow);
	m_ipFlags(m_vMaster[master_index]) = iFlags;
}


void MetakitManager::PrintPlaylist(c4_View view, bool viewsOnly){
	const char *str;
	int temp;
	if (viewsOnly) {
		for (int i=0;i<view.GetSize(); ++i) {
			str = m_spEmbname(view[i]);
			if (strlen(str) > 0)
				diag_printf("Stored Playlist: %s\n", str);
		}
	}
	else {
	for (int i=0;i<view.GetSize(); ++i) {
		temp = m_ipId(view[i]);
		diag_printf(" ****    ****    ****    ****    ****    ****    ****    ****\n");
		diag_printf("\nStored id: %d                ", temp);
		diag_printf("Index in Playlist: %d\n", i);
	    str = m_spUUID(view[i]);
		if (strlen(str) > 0)
			diag_printf("UUID: %s\n", str);
	    str = m_spURL(view[i]);
		if (strlen(str) > 0)
			diag_printf("URL: %s\n", str);
	    str = m_spTitle(view[i]);
		if (strlen(str) > 0)
			diag_printf("Stored Title: %s\n", str);
		str = m_spArtist(view[i]);
		if (strlen(str) > 0)
			diag_printf("Stored Artist: %s\n", str);
		str = m_spGenre(view[i]);
		if (strlen(str) > 0)
			diag_printf("Stored Genre: %s\n", str);
		str = m_spEmbname(view[i]);
		if (strlen(str) > 0)
			diag_printf("Stored Playlist: %s\n", str);
		str = m_spAlbum(view[i]);
		if (strlen(str) > 0)
			diag_printf("Stored Album: %s\n", str);
		str = m_spFname(view[i]);
		if (strlen(str) > 0)
			diag_printf("Stored Filename: %s\n", str);
		str = m_spFpath(view[i]);
		if (strlen(str) > 0)
			diag_printf("Stored Path: %s\n", str);
		str = m_spSonglength(view[i]);
		if (strlen(str) > 0)
			diag_printf("Stored SongLength: %s\n", str);
		str = m_spCreationDate(view[i]);
		if (strlen(str) > 0)
			diag_printf("Stored Creation Date: %s\n", str);
		temp = m_ipVoteValue(view[i]);
		diag_printf("Stored Vote Value: %d                ", temp);
		temp = m_ipYear(view[i]);
		diag_printf("Stored Year: %d                ", temp);
		temp = m_ipPlaycount(view[i]);
		diag_printf("Stored Playcount: %d\n", temp);
		temp = m_ipSamplefreq(view[i]);	
		diag_printf("Stored Samplefreq: %d            ", temp);
		temp = m_ipBitrate(view[i]);
		diag_printf("Stored Bitrate: %d\n", temp);
		temp = m_ipNumchans(view[i]);
		diag_printf("Stored Numchans: %d            ", temp);
		temp = m_ipFlags(view[i]);
		diag_printf("Stored Flags: %d\n", temp);
	}
	diag_printf("\nsize: %d\n", view.GetSize());
	}
}


/////////////////////////////////////////////////////////////////////////
// **** Overload this to write out to a file! *************************
/////////////////////////////////////////////////////////////////////////
void MetakitManager::PrintPlaylistToFile(c4_View view, char* name){	
	int temp;
	const char* str;
	FILE* fd = fopen(name, "w");
	for (int i=0;i<view.GetSize(); ++i) {
		temp = m_ipId(view[i]);
		fprintf(fd, " ****    ****    ****    ****    ****    ****    ****    ****\n");
		fprintf(fd, "\nStored id: %d                ", temp);
		fprintf(fd, "Index in Playlist: %d\n", i);
	    str = m_spUUID(view[i]);
		if (strlen(str) > 0)
			diag_printf("UUID: %s\n", str);
	    str = m_spURL(view[i]);
		if (strlen(str) > 0)
			diag_printf("URL: %s\n", str);
	    str = m_spTitle(view[i]);
		if (strlen(str) > 0)
			fprintf(fd, "Stored Title: %s\n", str);
		str = m_spArtist(view[i]);
		if (strlen(str) > 0)
			fprintf(fd, "Stored Artist: %s\n", str);
		str = m_spGenre(view[i]);
		if (strlen(str) > 0)
			fprintf(fd, "Stored Genre: %s\n", str);
		str = m_spEmbname(view[i]);
		if (strlen(str) > 0)
			fprintf(fd, "Stored Playlist: %s\n", str);
		str = m_spAlbum(view[i]);
		if (strlen(str) > 0)
			fprintf(fd, "Stored Album: %s\n", str);
		str = m_spFname(view[i]);
		if (strlen(str) > 0)
			fprintf(fd, "Stored Filename: %s\n", str);
		str = m_spFpath(view[i]);
		if (strlen(str) > 0)
			fprintf(fd, "Stored Path: %s\n", str);
		str = m_spSonglength(view[i]);
		if (strlen(str) > 0)
			fprintf(fd, "Stored SongLength: %s\n", str);
		str = m_spCreationDate(view[i]);
		if (strlen(str) > 0)
			fprintf(fd, "Stored Creation Date: %s\n", str);
		temp = m_ipVoteValue(view[i]);
		fprintf(fd, "Stored Vote Value: %d                ", temp);
		temp = m_ipYear(view[i]);
		fprintf(fd, "Stored Year: %d                ", temp);
		temp = m_ipPlaycount(view[i]);
		fprintf(fd, "Stored Playcount: %d\n", temp);
		temp = m_ipSamplefreq(view[i]);	
		fprintf(fd, "Stored Samplefreq: %d            ", temp);
		temp = m_ipBitrate(view[i]);
		fprintf(fd, "Stored Bitrate: %d\n", temp);
		temp = m_ipNumchans(view[i]);
		fprintf(fd, "Stored Numchans: %d            ", temp);
		temp = m_ipFlags(view[i]);
		fprintf(fd, "Stored Flags: %d\n", temp);
	}
	fprintf(fd, "\nsize: %d\n", view.GetSize());
	fclose(fd);
}

	//resort standard sorts and commit data
void MetakitManager::PrepToClose(){
	m_sStorage.Commit();
}

void MetakitManager::SaveDatabase() {
	m_sStorage.Commit();
}

void MetakitManager::PrintDatabase(){
	PrintPlaylist(m_vMaster);
}

int MetakitManager::GetIndexInMasterForCopy(int indexInThis, JPlaylist &jp) {
	m_rCopyRow = jp.m_vView[indexInThis];
	return m_vMaster.Find(m_rCopyRow);
}

int MetakitManager::GetPlaylistSize(JPlaylist &jp){
	return jp.m_vView.GetSize();
}

int MetakitManager::GetMasterSize(){
	return m_vMaster.GetSize();
}

void MetakitManager::SelectOn(JPlaylist &jp, eColumn col, char* value) {
	c4_Row row;
	int temp;
	temp = atoi(value);
	switch(col) {
		case UUID:	m_spUUID(row) = value;
			break;
		case URL:	m_spURL(row) = value;
			break;
		case TITLE:	m_spTitle(row) = value;
			break;
		case ARTIST: m_spArtist(row) = value;
			break;
		case GENRE:	m_spGenre(row) = value;
			break;
		case ID: m_ipId(row) = temp;
			break;
		case ALBUM: m_spAlbum(row) = value;
			break;
		case VOTEVALUE: m_ipVoteValue(row) = temp;
			break; 
		case YEAR: m_ipYear(row) = temp;
			break; 
		case FILEPATH: m_spFpath(row) = value;
			break;
		case FILENAME: m_spFname(row) = value;
			break;
		case SONGLENGTH: m_spSonglength(row) = value;
			break;
		case PLAYCOUNT: m_ipPlaycount(row) = temp;
			break;
		case SAMPLEFREQ: m_ipSamplefreq(row) = temp;
			break;
		case BITRATE: m_ipBitrate(row) = temp;
			break;
		case NUMCHANNELS: m_ipNumchans(row) = temp;
			break;
		case FLAGS: m_ipFlags(row) = temp;
			break;
		case VIEWS: m_spEmbname(row) = value;
			break;
		case CREATIONDATE: m_spCreationDate(row) = value;
			break;
		default: diag_printf("Attempt to sort on unsupported eColumn\n"); 
			exit(1);
	}
	jp.m_vView = m_vMaster.Select(row);
}

void MetakitManager::MakePlaylistForPlaying(c4_View view){}

//****************************************************************************
//** character completion stuff												**
//****************************************************************************

	

int MetakitManager::StartCharComplete(char ch, eColumn col, JPlaylist jp) {
	
	SetCharCompleteMarkers(jp.m_vView, col);
	//determine which m_iSection is needed
	if(!isalnum(ch)) 
		m_iSection = 0;
	else if(isdigit(ch))
		m_iSection = (ch - '0') + 1;
	else { 
		ch = tolower(ch);
		m_iSection = ch - 'a' + 11;
	}
	//determine m_iSize of m_iSection
	if (m_iCCMarkers[m_iSection] == -1) {
//		diag_printf("No values beginning with that character\n");
		return -1;
	} else {
		if (m_iSection != 36) {
			int i = m_iSection + 1;
			while(m_iCCMarkers[i++] == -1) {
				if (i == 37) {
					m_iSize = jp.m_vView.GetSize() - m_iCCMarkers[m_iSection];
					break;
				}
			}
			if (i != 37) 
				m_iSize = m_iCCMarkers[i-1] - m_iCCMarkers[m_iSection];	
		} else 
			m_iSize = jp.m_vView.GetSize() - m_iCCMarkers[m_iSection];
//		diag_printf("m_iSection: %d, m_iSize: %d\n", m_iSection, m_iSize);	
	}
	if (m_iSize == 1)
		return m_iCCMarkers[m_iSection];
	//set up variables 
	m_bMore = true;
	m_iCurStr = 0; 
	m_iCurIndex = m_iCCMarkers[m_iSection];
	return -2;
}


int MetakitManager::NextChar(char ch, int position, eColumn col, JPlaylist jp) { 

	//make case insensitive
	char ch2;
	if(!isalpha(ch))
		ch2 = ch;
	else {
		ch = tolower(ch);
		ch2 = toupper(ch);
	}

	m_iCurStr = 0;
	static int skips = 0;
	//skip over strings that were marked as too short last time
	m_iSize -= skips;
	m_iCurIndex += skips;
	skips = 0;
	LoadStrings(col, jp.m_vView);

	int begin = -1, end = -1, i = 0; 
	while(begin == -1) {
		//CurStr is sitting at the last string, it is reset by LoadStrings
		while (i < m_iCurStr) { //for each captured string
			if (((int)strlen(m_sStrings[i])) == position+1)
				skips++;
			if (m_sStrings[i][position] == ch || m_sStrings[i][position] == ch2) {
//				diag_printf("match\n");
				if (begin == -1) begin = end = i;
				else end = i;
			}
			++i;
		}
		if (begin == -1) {
			//free memory 
			for (int m=0; m < m_iCurStr; ++m) 
				free(m_sStrings[m]);
			if (!m_bMore) {
//				diag_printf("no match at all!\n");
				return -1; //no match
			}
			else { //no match, m_bMore to check
//				diag_printf("no match, m_bMore to check\n");
				m_iSize -= m_iCurStr;
				m_iCurStr = 0;

				LoadStrings(col, jp.m_vView);
				i = 0;
			}
		}
		else if (end == m_iCurStr-1 && begin != 0 && begin != end) {
			
			if (m_bMore){
//			diag_printf("need to add on rest of m_iSection, first move everything down");
			int j;
			for (j=0; begin+j < m_iCurStr; ++j) {
				free(m_sStrings[j]);
				m_sStrings[j] = m_sStrings[begin+j]; //move all the m_sStrings down
				m_sStrings[begin+j] = NULL;
			}
			m_iCurStr = j;
			//diag_printf("special load of m_sStrings, check this\n");
			LoadStrings(col, jp.m_vView);
			i = 0;
			m_iSize -= begin;
			m_iCurIndex += begin;
			begin = end = -1;
			}
			else { //no more to be added
			m_iCurIndex = (m_iCurIndex - m_iCurStr); //start at this m_iSection next letter
				for (int m=0; m < m_iCurStr; ++m) { 
					if (m_sStrings[m] != NULL)
						free(m_sStrings[m]);
				}
//				diag_printf("entire m_iSection matches\n");
				return -2; //entire m_iSection matches, must narrow down
			}
		}
		else if (end == begin) {
			for (int m=0; m < m_iCurStr; ++m) 
				free(m_sStrings[m]);
//			diag_printf("exact match, return value is index\n");	
			return (m_iCurIndex - (m_iCurStr - begin)); //exact match
		}
		else if(begin == 0 && end == m_iCurStr-1) {
			m_iCurIndex -= m_iCurStr; //start at this m_iSection next letter
			for (int m=0; m < m_iCurStr; ++m) { 
				if (m_sStrings[m] != NULL)
					free(m_sStrings[m]);
			}
//			diag_printf("entire m_iSection matches\n");
			return -2; //entire m_iSection matches, must narrow down
		}
		else {
			diag_printf("never should reach here!\n");
			return -9;
		}
	}
	return -9;
}

void MetakitManager::PrintSelection(JPlaylist jp) {
	const char *str;
	int temp, size;
	if (MAX_STRINGS < m_iSize) 
		size = m_iCurIndex+MAX_STRINGS;
	else
		size = m_iCurIndex+m_iSize;
//	for(int i=m_iCurIndex; i < m_iCurIndex+m_iCurStr;++i){
	for(int i=m_iCurIndex; i < size;++i){
		temp = m_ipId(jp.m_vView[i]);
		diag_printf(" ****    ****    ****    ****    ****    ****    ****    ****\n");
		diag_printf("\nStored id: %d                ", temp);
		diag_printf("Index in Playlist: %d\n", i);
	    str = m_spUUID(jp.m_vView[i]);
		if (strlen(str) > 0)
			diag_printf("Stored UUID: %s\n", str);
	    str = m_spURL(jp.m_vView[i]);
		if (strlen(str) > 0)
			diag_printf("Stored URL: %s\n", str);
	    str = m_spTitle(jp.m_vView[i]);
		if (strlen(str) > 0)
			diag_printf("Stored title: %s\n", str);
		str = m_spArtist(jp.m_vView[i]);
		if (strlen(str) > 0)
			diag_printf("Stored artist: %s\n", str);
		str = m_spGenre(jp.m_vView[i]);
		if (strlen(str) > 0)
			diag_printf("Stored genre: %s\n", str);
		str = m_spEmbname(jp.m_vView[i]);
		if (strlen(str) > 0)
			diag_printf("Stored Playlist: %s\n", str);
		str = m_spAlbum(jp.m_vView[i]);
		if (strlen(str) > 0)
			diag_printf("Stored Album: %s\n", str);
		str = m_spFname(jp.m_vView[i]);
		if (strlen(str) > 0)
			diag_printf("Stored Filename: %s\n", str);
		str = m_spFpath(jp.m_vView[i]);
		if (strlen(str) > 0)
			diag_printf("Stored Path: %s\n", str);
		str = m_spSonglength(jp.m_vView[i]);
		if (strlen(str) > 0)
			diag_printf("Stored SongLength: %s\n", str);
		str = m_spCreationDate(jp.m_vView[i]);
		if (strlen(str) > 0)
			diag_printf("Stored Creation Date: %s\n", str);
		temp = m_ipVoteValue(jp.m_vView[i]);
		diag_printf("Stored Vote Value: %d                ", temp);
		temp = m_ipYear(jp.m_vView[i]);
		diag_printf("Stored Year: %d                ", temp);
		temp = m_ipPlaycount(jp.m_vView[i]);
		diag_printf("Stored Playcount: %d\n", temp);
		temp = m_ipSamplefreq(jp.m_vView[i]);	
		diag_printf("Stored Samplefreq: %d            ", temp);
		temp = m_ipBitrate(jp.m_vView[i]);
		diag_printf("Stored Bitrate: %d\n", temp);
		temp = m_ipNumchans(jp.m_vView[i]);
		diag_printf("Stored Numchans: %d            ", temp);
		temp = m_ipFlags(jp.m_vView[i]);
		diag_printf("Stored Flags: %d\n", temp);
	}
	diag_printf("\nsize: %d\n", m_iSize);
}


void MetakitManager::SetCharCompleteMarkers(c4_View &view, eColumn col) {

	for (int j=0; j<37; j++)
		m_iCCMarkers[j] = -1;

	structColumns cols;
	cols.NumCols = 1;
	cols.columns[0] = col;
	view = SortOn(view, cols);
//	diag_printf("m_iSize of view: %d\n", view.GetSize());
	const char* str;
	char ch;

	//for each song in the database
	int mark = 0;
	for(int i=0; i<view.GetSize(); ++i) {
		switch (col) {
		case UUID: str = m_spUUID(view[i]);
			break;
		case URL: str = m_spURL(view[i]);
			break;
		case TITLE: str = m_spTitle(view[i]);
			break;
		case ARTIST:str = m_spArtist(view[i]);
			break;
		case GENRE:	str = m_spGenre(view[i]);
			break;
		case ALBUM: str = m_spAlbum(view[i]);
			break;
		case FILEPATH: str = m_spFpath(view[i]);
			break;
		case FILENAME: str = m_spFname(view[i]);
			break;
		case VIEWS: str = m_spEmbname(view[i]);
			break;
		default: diag_printf("Attempt to set Markers for unsupported case\n"); 
			exit(1);
		}
		ch = str[0];
//		diag_printf("the character is: %c\n", ch);

		// deal with non-alnumerics
		if (!isalnum(ch)) {
			if (i==0)
				m_iCCMarkers[mark++] = 0; 
		}
		else if (isdigit(ch)) {//digits
			int digit = ch - '0';
			while (mark < digit+2) { 
				mark++;
				if (mark == digit+2)
					m_iCCMarkers[mark-1] = i;
			}
		}
		else { //it's a letter
			int letter = tolower(ch);
			letter -= 'a';
			while (mark < letter+12) { 
				mark++;
				if (mark == letter+12)
					m_iCCMarkers[mark-1] = i;
			}
		}			
	}
//	for (int k=0; k<37; ++k) 
//		diag_printf("m_iCCMarkers[%d]: %d\n", k, m_iCCMarkers[k]);
}


//returns number of elements left to grab
int MetakitManager::LoadStrings(eColumn col, c4_View &view){
	const char* str;
	while (true) {
		//for each string in the m_iSection that fits
		while((m_iCurStr < m_iSize) && (m_iCurStr < MAX_STRINGS) && 
			  (m_iCurIndex < m_iCCMarkers[m_iSection]+m_iSize)) { 
			switch (col) {
				case UUID: str = m_spUUID(view[m_iCurIndex]);
					break;
				case URL: str = m_spURL(view[m_iCurIndex]);
					break;
				case TITLE: str = m_spTitle(view[m_iCurIndex]);
					break;
				case ARTIST:str = m_spArtist(view[m_iCurIndex]);
					break;
				case GENRE:	str = m_spGenre(view[m_iCurIndex]);
					break;
				case ALBUM: str = m_spAlbum(view[m_iCurIndex]);
					break;
				case FILEPATH: str = m_spFpath(view[m_iCurIndex]);
					break;
				case FILENAME: str = m_spFname(view[m_iCurIndex]);
					break;
				case VIEWS: str = m_spEmbname(view[m_iCurIndex]);
					break;
				default: diag_printf("Attempt to search m_iSection for unsupported case\n"); 
					exit(1);
			}
			// get the space you need
			m_sStrings[m_iCurStr] = (char*) malloc(strlen(str) + 1);
			if (m_sStrings[m_iCurStr] != NULL)
				// copy the string into your new place in memory
				strcpy(m_sStrings[m_iCurStr], str);
			else  //if it fails, break with current string and index captured
				break;
//			diag_printf("str: %s\n", str);
//			diag_printf("m_iCurStr: %d, m_iCurIndex: %d, m_sStrings[m_iCurStr]: %s\n", m_iCurStr, m_iCurIndex, m_sStrings[m_iCurStr]); 
			m_iCurStr++;
			m_iCurIndex++;
		
		}	
		if ((m_iCurIndex >= m_iCCMarkers[m_iSection]+m_iSize) || (m_iCurStr >= m_iSize)) {
			m_bMore = false;
//			diag_printf("No m_bMore!, m_iSize: %d\n", m_iSize);
			return 0; 
		}
		if (m_iCurStr == MAX_STRINGS) {
//			diag_printf("hit max\n");
			break;
		}
	}
	return m_iSize - m_iCurIndex;
}
