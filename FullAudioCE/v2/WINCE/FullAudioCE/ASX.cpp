//........................................................................................
//........................................................................................
//.. File Name:  ASX.cpp
//.. Date: 12/27/2000																	..
//.. Author(s): Matthew Tadd
//.. Description of content: ASX file parser for IO Jukebox
//.. Usage: Controls main menu															..
//.. Last Modified By: Matthew Tadd mattt@iobjects.com						..	
//.. Modification date: 12/27/2000														..
//........................................................................................
//.. Copyright:(c) 1995-2000 Interactive Objects Inc.  									..
//..	 All rights reserved. This code may not be redistributed in source or linkable  ..
//.. 	 object form without the express written consent of Interactive Objects.        ..
//.. Contact Information: www.iobjects.com												..
//........................................................................................
//........................................................................................

#include "stdafx.h"

#ifdef INCLUDE_JUKEBOX

#include "ASX.h"
#include <windows.h>
#include <stdio.h>


bool GetLine(FILE* pFile, unsigned char *buf, bool bReset);

int ASX::Load(Jukebox* pJukeBox,const char* pszFileName)
{
	unsigned char buf[1000];
	structMetadata md;
	int songs = 0;

//	FILE* pFile;
	HANDLE hFile;
#ifndef UNDER_CE
	int ret;
#endif

/*
	if (pJukeBox->GetSize() != 0) {
		DEBUG_ASX("Deleting the database\n");
		pJukeBox->ClearDatabase();
	}
*/

//	CFileInputStream in;
//	if((ret = in.Open(pszFileName, false))==-1) 
	TCHAR szFileName[MAX_PATH];
	MultiByteToWideChar(CP_ACP,0,pszFileName,-1,szFileName,MAX_PATH);
	hFile = CreateFile(szFileName,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
//	pFile = fopen(pszFileName,"rt");
//	if(!pFile)
	if(hFile == INVALID_HANDLE_VALUE)

	{
//		DEBUG_ASX("file not opened\n");
		return 0;
	}

	// reset the jukebox
	
//	DEBUG_ASX("file opened\n");
	md.Title[0] = '\0';
	md.Artist[0] = '\0';
	md.Album[0] = '\0';
	md.Genre[0] = '\0';
	md.FileCreationDate[0] = '\0';
	md.FilePath[0] = '\0';
	md.FileName[0] = '\0';
	md.SongLength[0] = '\0';
	md.SongId = -1;
	md.Flags = 0;

//	while(GetLine(in, buf)) {
	while(GetLine(hFile,buf,false)) 
	{
		if (buf[0] == '<' && buf[1] == 'P') {
			//Title
			int i = 0, j = 0;
			if (buf[15] == 'N'){
				while (buf[i++] != '"');
				while (buf[i++] != '"');
				while (buf[i++] != '"');
				while (buf[i] != '"')
					md.Title[j++] = (int)buf[i++];
				md.Title[j] = '\0';
			}
			//Genre
			if (buf[15] == 'G'){
				while (buf[i++] != '"');
				while (buf[i++] != '"');
				while (buf[i++] != '"');
				while (buf[i] != '"')
					md.Genre[j++] = (int)buf[i++];
				md.Genre[j] = '\0';
			}
			//Album
			if (buf[15] == 'A' && buf[16] == 'l'){
				while (buf[i++] != '"');
				while (buf[i++] != '"');
				while (buf[i++] != '"');
				while (buf[i] != '"')
					md.Album[j++] = (int)buf[i++];
				md.Album[j] = '\0';
			}
			//Artist
			if (buf[15] == 'A' && buf[16] == 'r'){
				while (buf[i++] != '"');
				while (buf[i++] != '"');
				while (buf[i++] != '"');
				while (buf[i] != '"')
					md.Artist[j++] = (int)buf[i++];
				md.Artist[j] = '\0';
			}
			//Filename
			if (buf[15] == 'S' && buf[16] == 'o'){
				while (buf[i++] != '"');
				while (buf[i++] != '"');
				while (buf[i++] != '"');
				while (buf[i] != '"')
					md.FileName[j++] = (int)buf[i++];
				md.FileName[j] = '\0';
/*
				while (buf[i++] != '\\');
				int lastSlash = i-1;
				while (buf[i] != '.'){
					if (buf[i] == '\\') 
						lastSlash = i;
					i++;
				}
				lastSlash++;
				while(lastSlash < i) 
					md.FileName[j++] = (int)buf[lastSlash++];
				md.FileName[j++] = '.';
				if (buf[++lastSlash] != '"') md.FileName[j++] = (int)buf[lastSlash++];
				if (buf[lastSlash] != '"') md.FileName[j++] = (int)buf[lastSlash++];
				if (buf[lastSlash] != '"') md.FileName[j++] = (int)buf[lastSlash++];
				md.FileName[j] = '\0';
*/
//		DEBUG_ASX("Filename: [%s]\n", md.FileName);
				
				//check for file in playlist:
//				CPlaylistEntry* pEntry = CPlaylist::GetPlaylist()->GetEntryByFilename(md.FileName);
//				if (pEntry) {
//					if (pEntry->GetUserPlaylistIndex() != -1)

//WMA
#if defined(INCLUDE_WMA) || defined(x86) || defined(SHOW_WMA_FILES)
				if(md.FileName[j-1] == 'a' || md.FileName[j-1] == 'A')
				{
						pJukeBox->AddSong(md);
						songs++;
				}
#endif
//MP3
#if defined(INCLUDE_MP3) || defined(x86) || defined(SHOW_MP3_FILES)
				if(md.FileName[j-1] == '3')
				{
						pJukeBox->AddSong(md);
						songs++;
				}
#endif
//				}
				md.Title[0] = '\0';
				md.Artist[0] = '\0';
				md.Album[0] = '\0';
				md.Genre[0] = '\0';
				md.FileCreationDate[0] = '\0';
				md.FilePath[0] = '\0';
				md.FileName[0] = '\0';
				md.SongLength[0] = '\0';
			}
		}
	}

	CloseHandle(hFile);
	return songs;
}

#if 0
bool GetLine(CFileInputStream &in, unsigned char *buf, bool bReset) {
/*	int curChar = 0;
	unsigned char ch[1];
	while(in.Read(ch, 1)) {
		if (ch[0] != '\n') buf[curChar++] = ch[0];
		else {
			buf[curChar] = '\0';
			return true;
		}
	}
	return false;
*/
	static bool once = false;
	static char* Start = 0;
	static int index = 0;
	static int len = 0;

	if (bReset)
	{
		DEBUG("Reset GetLine\n");
		once = false;
		Start = 0;
		index = 0;
		len = 0;
		return false;
	}

	if (!once) {
		len = in.Length() ;
		if (len != -1) {
			Start = (char*)malloc(len);
			in.Read((unsigned char*)Start, len);
		}
		once = true;
	}
	int bufIndex=0;
	while(Start[index] != '\n' && index < len) 
		buf[bufIndex++] = Start[index++];
	
	buf[bufIndex] = '\0';
	if (index++ == len) 
	{
		free(Start);
		return false;
	}
	
	return true;
}

bool GetLine(FILE* pFile, unsigned char *buf, bool bReset) {
/*	int curChar = 0;
	unsigned char ch[1];
	while(fread(ch,1,1,pFile)) {
		if (ch[0] != '\n') buf[curChar++] = ch[0];
		else {
			buf[curChar] = '\0';
			return true;
		}
	}
	return false;
*/
	static bool once = false;
	static char* Start = 0;
	static int index = 0;
	static int len = 0;

	if (bReset)
	{
//		DEBUG("Reset GetLine\n");
		once = false;
		Start = 0;
		index = 0;
		len = 0;
		return false;
	}

	if (!once) {
		//len = in.Length() ;
		fseek(pFile,0,SEEK_END);
		len = ftell(pFile);

		if (len != -1) {
			Start = (char*)malloc(len);
//			in.Read((unsigned char*)Start, len);
			fseek(pFile,0,SEEK_SET);
			fread((unsigned char*)Start,len,len,pFile);
		}
		once = true;
	}
	int bufIndex=0;
	while(Start[index] != '\n' && index < len) 
		buf[bufIndex++] = Start[index++];
	
	buf[bufIndex] = '\0';
	if (index++ == len) 
	{
		free(Start);
		return false;
	}
	
	return true;
}
#endif

bool GetLine(HANDLE hFile, unsigned char *buf, bool bReset) {
/*	int curChar = 0;
	unsigned char ch[1];
	while(fread(ch,1,1,pFile)) {
		if (ch[0] != '\n') buf[curChar++] = ch[0];
		else {
			buf[curChar] = '\0';
			return true;
		}
	}
	return false;
*/
	static bool once = false;
	static char* Start = 0;
	static int index = 0;
	static int len = 0;

	if (bReset)
	{
//		DEBUG("Reset GetLine\n");
		once = false;
		Start = 0;
		index = 0;
		len = 0;
		return false;
	}

	if (!once) {
		//len = in.Length() ;

		len = GetFileSize(hFile,NULL);

		if (len != -1) {
			Start = (char*)malloc(len);
//			in.Read((unsigned char*)Start, len);
			SetFilePointer(hFile,0,NULL,FILE_BEGIN);
			DWORD dwBytesRead;
			ReadFile(hFile,(LPVOID)Start,len,&dwBytesRead,NULL);
		}
		once = true;
	}
	int bufIndex=0;
	while(Start[index] != '\n' && index < len) 
		buf[bufIndex++] = Start[index++];
	
	buf[bufIndex] = '\0';
	if (index++ == len) 
	{
		free(Start);
		return false;
	}
	
	return true;
}


#endif //INCLUDE_JUKEBOX