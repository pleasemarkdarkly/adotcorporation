#include "Jukebox.h"
#include "JPlaylist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>

#define MAX_STRING_LENGTH 500
char buf[100];

void LoadFromWinMedPlayerFile(char* filename);
void LoadSongs(Jukebox &juke);
void FillMetadata(char* name, char* value, structMetadata &md);
void ClearMetadata(structMetadata &md);
void PrintMenu();
void PrintAddSongMenu();
void SortColumnsMenu();
char GetSelection();
void GetValue();
char GetChar();
void main() {
	
	structMetadata md;
	JPlaylist *myPlaylist;
	int index;


	cout << "           *********** JUKEBOX DEMO *********** " << endl;
	PrintMenu();


	char c = GetSelection();
while (c != 'Q') {
	switch (c) {

	//load songs from Windows Media Player File
	case 'L':
		cout << "Enter file name." << endl;
		GetValue();
		LoadFromWinMedPlayerFile(buf);
		Jukebox::GetInstance()->ViewDatabase();
		break;

	//Add Songs by hand
	case 'A':
		ClearMetadata(md);
		cout << "\n\n\n ** Add Song ** \n" << endl;
		PrintAddSongMenu();
		c = 'e';
		while ( c != 'J') {
			c = GetSelection();
			if (c != 'J' && c != 'M') GetValue();
			switch (c) {
			case 'A': strcpy(md.Album, buf);
				break;
			case 'R': strcpy(md.Artist, buf);
				break;
			case 'B': md.BitRate = atoi(buf);
				break;
			case 'D': strcpy(md.FileCreationDate, buf);
				break;
			case 'X': strcpy(md.FileName, buf);
				break;
			case 'Z': strcpy(md.FilePath, buf);
				break;
			case 'F': md.Flags |= atoi(buf);
				break;
			case 'G': strcpy(md.Genre, buf);
				break;
			case 'N': md.NumChannels = atoi(buf); 
				break;
			case 'P': md.PlayCount = atoi(buf);
				break;
			case 'Q': md.SampleFreq = atoi(buf);
				break;
			case 'I': md.SongId = atoi(buf);
				break;
			case 'L': strcpy(md.SongLength, buf);
				break;
			case 'T': strcpy(md.Title, buf);
				break;
			case 'Y': md.Year = atoi(buf);
				break;
			case 'M': PrintAddSongMenu();
				break;
			case 'J': Jukebox::GetInstance()->AddSong(md);
				cout << "Song Added!\n\n(Back in Main menu)" << endl;
				break;
			default: cout << "Not a valid selection" << endl;
			}
		}		break;

	//save the state of the jukebox
	case 'V': 
		Jukebox::GetInstance()->StoreDatabase();
		break;
	//delete a song from the jukebox
	case 'D':
		c = 'm';
		while (c != 'T' && c != 'I') {
			cout << "Delete by index (I) or by Title and Artist (T): " << endl;
			c = GetSelection();
			if (c == 'M') cout << "\n\nI (delete by index) or T (delete by Title and Artist)" << endl;
		}
		if ( c == 'I') {
			cout << "Index ";
			GetValue();
			index = atoi(buf);
			if (index < 0 || index >= Jukebox::GetInstance()->GetSize()) 
				cout << "** Invalid Index **" << endl;
			else {
				Jukebox::GetInstance()->DeleteSong(index);
				cout << "Deleted!" << endl;
			}
		}
		else {
			cout <<"Title ";
			char Temp[100];
			GetValue();
			strcpy(Temp, buf);
			cout <<"Artist ";
			GetValue();
			index = Jukebox::GetInstance()->FindSong(Temp, buf);
			if (index == -1 ) cout << "** Song not found ** " << endl;
			else {		
				Jukebox::GetInstance()->DeleteSong(index);
				cout << "Deleted!" << endl;
			}
		}
		
		break;
	//create a playlist (and add it to the jukebox so we can find it.)
	case 'C':
		cout << "Enter Playlist Name." << endl;
		GetValue();
		myPlaylist = Jukebox::GetInstance()->CreatePlaylist(buf);
		Jukebox::GetInstance()->ReleaseStoredPlaylist(myPlaylist);
		break;
	//add songs to a playlist 
	case 'Y': 
	{  
		cout << "Enter Playlist Name." << endl;
		GetValue();
		JPlaylist pl(buf);
		index = Jukebox::GetInstance()->FindPlaylistInMaster(buf);
		if (index != -1) {
			Jukebox::GetInstance()->RetrievePlaylistFromMaster(index, pl);
			pl.PrintSongs();
			c = 'm';
			cout << "\n" << endl;
			while (c != 'T' && c != 'I') {
				cout << "Add by index in Master (I) or by Title and Artist (T): " << endl;
				c = GetSelection();
				if (c == 'M') cout << "\n\nI (add by index) or T (add by Title and Artist)" << endl;
			}
			if ( c == 'I') {
				while(c != 'Q'){		
					cout << "Index ";
					GetValue();
					index = atoi(buf);
					if (index < 0 || index >= Jukebox::GetInstance()->GetSize()) {
						cout << " ** Invalid index **" << endl;
					} else {
						pl.AddItem(index);
						cout << "Song Added!" << endl;
						cout << "Quit adding songs (Q) or Add another song by index (A): " << endl;
						c = GetSelection();
						if (c == 'M') cout << "\n\nQ (Quit) or A (Add another song)" << endl;
					}
				}
			}
			else {
				while(c != 'Q'){
					cout <<"Title ";
					char Temp[100];
					GetValue();
					strcpy(Temp, buf);
					cout <<"Artist ";
					GetValue();
					index = Jukebox::GetInstance()->FindSong(Temp, buf);
					if (index == -1 ) cout << "** Song not found ** " << endl;
					else {		
						pl.AddItem(index);
						cout << "Song Added!" << endl;
					}
					cout << "Quit adding songs (Q) or Add another song by Title and Artist (A): " << endl;
					c = GetSelection();
					if (c == 'M') cout << "\n\nQ (Quit) or A (Add another song)" << endl;
				}
			}
		} else
			cout << "Playlist not found" << endl;
	}
		break;
	//delete songs from a playlist
	case 'Z':
		{
		cout << "Enter Playlist Name." << endl;
		GetValue();
		JPlaylist pl(buf);
		index = Jukebox::GetInstance()->FindPlaylistInMaster(buf);
		if (index != -1) {
			Jukebox::GetInstance()->RetrievePlaylistFromMaster(index, pl);
			pl.PrintSongs();
			c = 'm';
			cout << "\n" << endl;
			while (c != 'T' && c != 'I') {
				cout << "Delete by index in playlist (I) or by Title and Artist (T): " << endl;
				c = GetSelection();
				if (c == 'M') cout << "\n\nI (delete by index) or T (delete by Title and Artist)" << endl;
			}
			if ( c == 'I') {
				cout << "Index ";
				GetValue();
				index = atoi(buf);
				
			cout << "Index: " << index << endl;
			cout << "GetSize: " << pl.GetSize() << endl;
				if (index < 0 || index >= pl.GetSize()) 
					cout << " ** Invalid Index **" << endl;
				else {
					pl.DeleteItem(index);
					cout << "Deleted!" << endl;
				}
			}
			else {
				cout <<"Title ";
				char Temp[100];
				GetValue();
				strcpy(Temp, buf);
				cout <<"Artist ";
				GetValue();
				index = pl.FindSong(Temp, buf);
				if (index == -1 ) cout << "** Song not found ** " << endl;
				else {
					pl.DeleteItem(index);
					cout << "Deleted!" << endl;
				}
			}
		} else
			cout << "Playlist not found" << endl;
		}
		break;
	//Add a playlist to a playlist 
	case 'E':
		{
		cout << "Enter Parent Playlist Name." << endl;
		GetValue();
		JPlaylist pl(buf);
		index = Jukebox::GetInstance()->FindPlaylistInMaster(buf);
		if (index != -1) {
			Jukebox::GetInstance()->RetrievePlaylistFromMaster(index, pl);
			cout << "Enter Child Playlist Name." << endl;
			GetValue();
			JPlaylist pl2(buf);
			index = Jukebox::GetInstance()->FindPlaylistInMaster(buf);
			if (index != -1) {
				pl.AddItem(index);
				cout << "Playlist added!" << endl;
			} else 
				cout << "Playlist not found" << endl;
		} else
			cout << "Playlist not found" << endl;
		}
		break;
	//delete a playlist from a playlist
	case 'G':
		{
		cout << "Enter Parent Playlist Name." << endl;
		GetValue();
		JPlaylist pl(buf);
		index = Jukebox::GetInstance()->FindPlaylistInMaster(buf);
		if (index != -1) {
			Jukebox::GetInstance()->RetrievePlaylistFromMaster(index, pl);
			cout << "Enter Child Playlist Name." << endl;
			GetValue();
			JPlaylist pl2(buf);
			index = pl.FindPlaylist(pl2.GetName());
			if (index != -1) {
				pl.DeleteItem(index);
				cout << "Playlist deleted!" << endl;
			} else 
				cout << "Playlist not found" << endl;
		} else
			cout << "Playlist not found" << endl;
		}
		break;
		
	//character completion on entire jukebox
	case 'F':
		cout << "Choose column to search: \n\tTitle ......... T\n\tAlbum ......... A\n\tArtist ........ R" << endl;
		cout << "\tGenre ......... G\n\tFilename ...... F\n\tFilepath ...... I\n\tPlaylists ..... P\n" << endl;
		c = GetSelection();
		eColumn col;
		switch (c) {
		case 'T': col = TITLE; break;
		case 'A': col = ALBUM; break;
		case 'R': col = ARTIST; break;
		case 'G': col = GENRE; break;
		case 'F': col = FILENAME; break;
		case 'I': col = FILEPATH; break;
		case 'P': col = VIEWS; break;
		default: cout << "Not a valid column." << endl;
		}
		cout << "Enter the First Character." << endl;
		c = GetChar();
		int ret;
		if (col == GENRE || col == TITLE || col == ARTIST) { 
			//have a section greater than 1
			if ((ret = Jukebox::GetInstance()->StartCharComplete(c, col, Jukebox::GetInstance()->GetStandardPlaylist(col))) == -2) {
				Jukebox::GetInstance()->PrintSelection(Jukebox::GetInstance()->GetStandardPlaylist(col));
				int count = 1;
				while(true) {
					c = GetChar();
					if ((ret = Jukebox::GetInstance()->NextChar(c, count++, col, Jukebox::GetInstance()->GetStandardPlaylist(col))) >= 0) {
						cout << "Match!" << endl;
						JPlaylist theMatch("match");
						int index = Jukebox::GetInstance()->GetRealIndexOfSong(Jukebox::GetInstance()->GetStandardPlaylist(col), ret);
						theMatch.AddItem(index);
						Jukebox::GetInstance()->ViewPlaylist(theMatch);
						theMatch.DeleteItem(0);
						break;
					} else if (ret == -2) 
						Jukebox::GetInstance()->PrintSelection(Jukebox::GetInstance()->GetStandardPlaylist(col));
					else {
						cout << "No Matches!" << endl;
						break;
					}
				}
			}
			else if(ret == -1) {
				cout << "No entries beginning with that character" << endl;
			}
			else {
				cout << "Match!" << endl;
				JPlaylist theMatch("match");
				int index = Jukebox::GetInstance()->GetRealIndexOfSong(Jukebox::GetInstance()->GetStandardPlaylist(col), ret);
				cout << "index: " << index << endl;
				theMatch.AddItem(index);
				Jukebox::GetInstance()->ViewPlaylist(theMatch);
				theMatch.DeleteItem(0);
			}
		}
		else { //all other columns
			JPlaylist allSongs("allSongs");
			Jukebox::GetInstance()->cols.NumCols = 1;
			Jukebox::GetInstance()->cols.columns[0] = col;
			Jukebox::GetInstance()->GetAllSongs(allSongs, Jukebox::GetInstance()->cols);
			if ((ret = Jukebox::GetInstance()->StartCharComplete(c, col, allSongs)) == -2) {
				Jukebox::GetInstance()->PrintSelection(allSongs);
				int count = 1;
				while(true) {
					c = GetChar();
					if ((ret = Jukebox::GetInstance()->NextChar(c, count++, col, allSongs)) >= 0) {
						cout << "Match!" << endl;
						JPlaylist theMatch("match");
						int index = Jukebox::GetInstance()->GetRealIndexOfSong(allSongs, ret);
						theMatch.AddItem(index);
						Jukebox::GetInstance()->ViewPlaylist(theMatch);
						theMatch.DeleteItem(0);
						break;
					} else if (ret == -2) 
						Jukebox::GetInstance()->PrintSelection(allSongs);
					else {
						cout << "No Matches!" << endl;
						break;
					}
				}
			}
			else if(ret == -1) {
				cout << "No entries beginning with that character" << endl;
			}
			else {
				cout << "Match!" << endl;
				JPlaylist theMatch("match");
				int index = Jukebox::GetInstance()->GetRealIndexOfSong(allSongs, ret);
				cout << "index: " << index << endl;
				theMatch.AddItem(index);
				Jukebox::GetInstance()->ViewPlaylist(theMatch);
				theMatch.DeleteItem(0);
			}
		}
		break;

		//do character completion on a playlist 
	case 'K':
		{
		cout << "Enter Playlist Name." << endl;
		GetValue();
		JPlaylist charcomp(buf);
		index = Jukebox::GetInstance()->FindPlaylistInMaster(buf);
		if (index != -1) {
			Jukebox::GetInstance()->RetrievePlaylistFromMaster(index, charcomp);
			cout << "Choose column to search: \n\tTitle ......... T\n\tAlbum ......... A\n\tArtist ........ R" << endl;
			cout << "\tGenre ......... G\n\tFilename ...... F\n\tFilepath ...... I\n\tPlaylists ..... P\n" << endl;
			c = GetSelection();
			eColumn col;
			switch (c) {
			case 'T': col = TITLE; break;
			case 'A': col = ALBUM; break;
			case 'R': col = ARTIST; break;
			case 'G': col = GENRE; break;
			case 'F': col = FILENAME; break;
			case 'I': col = FILEPATH; break;
			case 'P': col = VIEWS; break;
			default: cout << "Not a valid column." << endl;
			}
			Jukebox::GetInstance()->cols.NumCols = 1;
			Jukebox::GetInstance()->cols.columns[0] = col;
			charcomp.SortOn(Jukebox::GetInstance()->cols);	

			cout << "Enter the First Character." << endl;
			c = GetChar();
			int ret;
			//have a section greater than 1
			if ((ret = charcomp.StartCharComplete(c, col)) == -2) {
				charcomp.PrintSelection();
				int count = 1;
				while(true) {
					c = GetChar();
					if ((ret = charcomp.NextChar(c, count++, col)) >= 0) {
						cout << "Match!" << endl;
						JPlaylist theMatch("match");
						int index = Jukebox::GetInstance()->GetRealIndexOfSong(charcomp, ret);
						theMatch.AddItem(index);
						Jukebox::GetInstance()->ViewPlaylist(theMatch);
						theMatch.DeleteItem(0);
						break;
					} else if (ret == -2) 
						charcomp.PrintSelection();
					else {
						cout << "No Matches!" << endl;
						break;
					}
				}
			}
			else if(ret == -1) {
				cout << "No entries beginning with that character" << endl;
			}
			else {
				cout << "Match!" << endl;
				JPlaylist theMatch("match");
				int index = Jukebox::GetInstance()->GetRealIndexOfSong(charcomp, ret);
				cout << "index: " << index << endl;
				theMatch.AddItem(index);
				Jukebox::GetInstance()->ViewPlaylist(theMatch);
				theMatch.DeleteItem(0);
			}
		}
		else {
			cout << "Playlist not found" << endl;
		}
		}
		break;
		//sort the entire jukebox on columns
	case 'S':
		{
		cout << "Enter the name of the playlist you wish to place results." << endl;
		GetValue();
		myPlaylist = Jukebox::GetInstance()->CreatePlaylist(buf);
		JPlaylist temp("temp");

		cout << " Choose the columns you would like to sort on." << endl;
		SortColumnsMenu();
		c = 'e';
		eColumn aCol = NOCOL;
		Jukebox::GetInstance()->cols.NumCols = 0;
		while ( c != 'S') {
			c = GetSelection();
			if (Jukebox::GetInstance()->cols.NumCols == 10) {
				cout << " ** Maximum columns reached ** " << endl;
				c = 'S';
			}
			switch (c) {
			case 'A': aCol = ALBUM;
				break;
			case 'R': aCol = ARTIST;
				break;
			case 'B': aCol = BITRATE;
				break;
			case 'D': aCol = CREATIONDATE;
				break;
			case 'X': aCol = FILENAME;
				break;
			case 'Z': aCol = FILEPATH;
				break;
			case 'F': aCol = FLAGS;
				break;
			case 'G': aCol = GENRE;
				break;
			case 'N': aCol = NUMCHANNELS; 
				break;
			case 'P': aCol = PLAYCOUNT;
				break;
			case 'Q': aCol = SAMPLEFREQ;
				break;
			case 'I': aCol = ID;
				break;
			case 'L': aCol = SONGLENGTH;
				break;
			case 'T': aCol = TITLE;
				break;
			case 'Y': aCol = YEAR;
				break;
			case 'M': SortColumnsMenu();
				break;
			case 'S': Jukebox::GetInstance()->GetAllSongs(temp, Jukebox::GetInstance()->cols);
				break;
			default: cout << "Not a valid selection" << endl;
				break;
			}
		Jukebox::GetInstance()->cols.columns[Jukebox::GetInstance()->cols.NumCols] = aCol;
		Jukebox::GetInstance()->cols.NumCols++;				
		}
		for (int i=0; i<temp.GetSize(); i++) {
			int index = Jukebox::GetInstance()->GetRealIndexOfSong(temp, i);
			(*myPlaylist).AddItem(index);
		}
		(*myPlaylist).PrintSongs();
		Jukebox::GetInstance()->ReleaseStoredPlaylist(myPlaylist);
		}
		break;

		//playlist sorting stuff here
	case 'R':
		{
		cout << "Enter Playlist Name." << endl;
		GetValue();
		JPlaylist pl(buf);
		index = Jukebox::GetInstance()->FindPlaylistInMaster(buf);
		if (index != -1) {
			Jukebox::GetInstance()->RetrievePlaylistFromMaster(index, pl);
			cout << " Choose the columns you would like to sort on." << endl;
			SortColumnsMenu();
			c = 'e';
			eColumn aCol = NOCOL;
			Jukebox::GetInstance()->cols.NumCols = 0;
			while ( c != 'S') {
				c = GetSelection();
				if (Jukebox::GetInstance()->cols.NumCols == 10) {
					cout << " ** Maximum columns reached ** " << endl;
					c = 'S';
				}
				switch (c) {
				case 'A': aCol = ALBUM;
					break;
				case 'R': aCol = ARTIST;
					break;
				case 'B': aCol = BITRATE;
					break;
				case 'D': aCol = CREATIONDATE;
					break;
				case 'X': aCol = FILENAME;
					break;
				case 'Z': aCol = FILEPATH;
					break;
				case 'F': aCol = FLAGS;
					break;
				case 'G': aCol = GENRE;
					break;
				case 'N': aCol = NUMCHANNELS; 
					break;
				case 'P': aCol = PLAYCOUNT;
					break;
				case 'Q': aCol = SAMPLEFREQ;
					break;
				case 'I': aCol = ID;
					break;
				case 'L': aCol = SONGLENGTH;
					break;
				case 'T': aCol = TITLE;
					break;
				case 'Y': aCol = YEAR;
					break;
				case 'M': SortColumnsMenu();
					break;
				case 'S': pl.SortOn(Jukebox::GetInstance()->cols);	
					break;
				default: cout << "Not a valid selection" << endl;
				}
			Jukebox::GetInstance()->cols.columns[Jukebox::GetInstance()->cols.NumCols] = aCol;
			Jukebox::GetInstance()->cols.NumCols++;				
			}
		pl.PrintSongs();
		} else {
			cout << "Playlist not found" << endl;
		}
		}
		break;

		//sort all the playlists 
	case 'T':
		Jukebox::GetInstance()->SortAllPlaylists();
		Jukebox::GetInstance()->ViewAllPLaylists();
		break;

		//this is a feature to help out testing, do it if you can 
	case 'W':
		{
		cout << "Enter Playlist Name." << endl;
		GetValue();
		JPlaylist pl(buf);
		index = Jukebox::GetInstance()->FindPlaylistInMaster(buf);
		if (index != -1) 
			pl.PrintSongsToFile("outfile.txt");
	
		else cout << "Playlist not found. " << endl;
		}
		break;

		//print out the database 
	case 'P': Jukebox::GetInstance()->ViewDatabase();
		break;
		//print out a playlist
	case 'X':
		{
		cout << "Enter Playlist Name." << endl;
		GetValue();
		JPlaylist pl(buf);
		index = Jukebox::GetInstance()->FindPlaylistInMaster(buf);
		if (index != -1) 
			pl.PrintSongs();
		else cout << "Playlist not found. " << endl;
		}
		break;
	//print out the options menu
	case 'M':
		PrintMenu();
		break;
	//sort on a value
	case 'O':
		{
			eColumn aCol;
			cout << "Enter the name of the playlist you wish to place results." << endl;
			GetValue();
			myPlaylist = Jukebox::GetInstance()->CreatePlaylist(buf);
			cout << "Choose a Column." << endl;
			cout << " Album ................................................ A" << endl;
			cout << " Artist ............................................... R" << endl;
			cout << " Bitrate .............................................. B" << endl;
			cout << " File Creation Date ................................... D" << endl;
			cout << " File Name ............................................ X" << endl;
			cout << " File Path ............................................ Z" << endl;
			cout << " Flag Value ........................................... F" << endl;
			cout << " Genre ................................................ G" << endl;
			cout << " Number of Channels ................................... N" << endl;
			cout << " Play Count ........................................... P" << endl;
			cout << " Sample Frequency Value ............................... Q" << endl;
			cout << " Unique Id Value ...................................... I" << endl;
			cout << " Length ............................................... L" << endl;
			cout << " Title ................................................ T" << endl;
			cout << " Production Year ...................................... Y" << endl;
			c = GetSelection();
			switch (c) {
			case 'A': aCol = ALBUM; break;
			case 'R': aCol = ARTIST; break;
			case 'B': aCol = BITRATE; break;
			case 'D': aCol = CREATIONDATE; break;
			case 'X': aCol = FILENAME; break;
			case 'Z': aCol = FILEPATH; break;
			case 'F': aCol = FLAGS; break;
			case 'G': aCol = GENRE; break;
			case 'N': aCol = NUMCHANNELS;  break;
			case 'P': aCol = PLAYCOUNT; break;
			case 'Q': aCol = SAMPLEFREQ; break;
			case 'I': aCol = ID; break;
			case 'L': aCol = SONGLENGTH; break;
			case 'T': aCol = TITLE; break;
			case 'Y': aCol = YEAR; break;
			default: cout << "Not a valid selection" << endl;
				break;
			}
			cout << "Enter value to select on." << endl;
			GetValue();
			JPlaylist temp("temp");
			MetakitManager::GetInstance()->SelectOn(temp, aCol, buf);
			for (int i=0; i < temp.GetSize(); i++) {
				int ret = Jukebox::GetInstance()->GetRealIndexOfSong(temp, i);
				(*myPlaylist).AddItem(ret);
			}
			(*myPlaylist).PrintSongs();
			Jukebox::GetInstance()->ReleaseStoredPlaylist(myPlaylist);	
		break;
		}
	default:
		cout << " ** Invalid selection ** " << endl;
	}
c = GetSelection();
}
}



void LoadFromWinMedPlayerFile(char* filename) {
	char buf[MAX_STRING_LENGTH];
	structMetadata md;
	ClearMetadata(md);

	//open input file
	FILE* fd;
	fd = fopen(filename, "r");
	if (fd == NULL) {
		printf("File not opened\n");
		return;
	}
	char *str, name[100], value[100];
	while (fgets(buf, MAX_STRING_LENGTH, fd)){
		//get to song beginning
		if (0==strcmp(buf, "<Entry> \n")) {
//			printf("%s", buf);
			while (fgets(buf, MAX_STRING_LENGTH, fd)) {
			//get to duration line
				if (buf[1] == 'D') {
					str = strtok(buf, "\"");
					str = strtok(NULL, "\"");
//					printf("Duration string is: %s\n", str);
					FillMetadata("Duration", str, md);
					while(fgets(buf, MAX_STRING_LENGTH, fd)){
						if (0==strcmp(buf, "</Entry>\n")) {
//							printf("%s", buf);
							break;
						}
						if (buf[1] == 'R') {
							sscanf(buf, "%*[A-Za-z <=]\"%[A-Za-z0-9\\_-&=\?><!#$%*() .:/'+]\"%*[*]", value);
//							printf("str: %s\n", value);
							FillMetadata("HREF", value, md);
						}
						else if (buf[0] != '\n') {
							sscanf(buf, "%*[A-Za-z <=]\"%[A-Za-z]\"%*[A-Za-z =]\"%[A-Za-z0-9\\_-&=\?><!#$%*() .:/'+]", name, value);
//							printf("name: %s\t", name);
//							printf("str: %s\n", value);
							FillMetadata(name, value, md);
							value[0] = '\0'; name[0] = '\0';
						}
					}
//printf("Title: %s \n Genre: %s\n %s %s %s\n%s %s %d %d %d\n", md.Title, md.Genre, md.Album, md.Artist, md.FileCreationDate, md.FilePath, md.SongLength, md.BitRate, md.Flags, md.PlayCount); 
					Jukebox::GetInstance()->AddSong(md);
					ClearMetadata(md);
					break;
				}
			}
		}
	}
}
	 

void FillMetadata(char* name, char* value, structMetadata &md){
	int val;
	static int id=0;
	if (name == NULL || value ==NULL) {
		printf("no data\n");
		return;
	}
	if (0==strcmp(name, "Name")){ 
		strcpy(md.Title, value);
		md.SongId = id++;
	}
	if (0==strcmp(name, "Genre"))
		strcpy(md.Genre, value);
	if (0==strcmp(name, "Artist"))
		strcpy(md.Artist, value);
	if (0==strcmp(name, "Album"))
		strcpy(md.Album, value);
	if (0==strcmp(name, "Bitrate")) {
		val = atoi(value);
		md.BitRate = val;
	}
	if (0==strcmp(name, "DigitallySecure")){
		val = atoi(value);
		md.Flags |= val;
	}
	if (0==strcmp(name, "PlayCount")) {
		val = atoi(value);
		md.PlayCount = val;
	}
	if (0==strcmp(name, "SourceURL"))
		strcpy(md.FilePath, value);
	if (0==strcmp(name, "CreationDate")) 
		strcpy(md.FileCreationDate, value);
	if (0==strcmp(name, "Duration"))
		strcpy(md.SongLength, value);
/*	if (0==strcmp(name, "HREF"))
	if (0==strcmp(name, "MediaType"));
	if (0==strcmp(name, "MediaAttribute"));
	if (0==strcmp(name, "TOC"));
	if (0==strcmp(name, "OriginalIndex"));
*/

}


void LoadSongs(Jukebox &juke){
	
	structMetadata md;
	char buf[MAX_STRING_LENGTH];
	
	//open input file
	FILE* fd;
	fd = fopen("testfile.txt", "r");
	if (fd == NULL) {
		printf("File not opened\n");
		exit(1);
	}
	int id = 0;
	while (fgets(buf, MAX_STRING_LENGTH, fd)){
//		printf("artist: %s", buf);
		strcpy(md.Artist, buf);
//		md.Artist[strlen(md.Artist)] = '\0';
		fgets(buf, MAX_STRING_LENGTH, fd);
//		printf("title: %s",buf);
		strcpy(md.Title, buf);
//		md.Title[strlen(md.Title)] = '\0';
		fgets(buf, MAX_STRING_LENGTH, fd);
//		printf("genre: %s", buf);
		strcpy(md.Genre, buf);
//		md.Genre[strlen(md.Genre)] = '\0';
//		printf("id: %d\n", id);
		md.SongId = id++;
	Jukebox::GetInstance()->AddSong(md);
	}
}

void ClearMetadata(structMetadata &md) {
	md.Album[0] = '\0';
	md.Artist[0] = '\0';
	md.BitRate = 0;
	md.FileCreationDate[0] = '\0';
	md.FileName[0] = '\0';
	md.FilePath[0] = '\0';
	md.Flags = 0;
	md.Genre[0] = '\0';
	md.NumChannels = 0;
	md.PlayCount = -1;
	md.SampleFreq = 0;
	md.SongId = -1;
	md.SongLength[0] = '\0';
	md.Title[0] = '\0';
	md.Year = 0;
}


void PrintMenu () {
	cout << "\n\n Testing Options Menu:" << endl;
	cout << " 1. Load Songs into Jukebox from a file ............... L" << endl;
	cout << " 2. Add Songs by hand to Jukebox ...................... A" << endl;
	cout << " 3. Delete Songs from Jukebox ......................... D" << endl;
	cout << " 4. Create a Playlist ................................. C" << endl;
	cout << " 5. Add Songs to a Playlist ........................... Y" << endl;
	cout << " 6. Delete Songs from the Playlist .................... Z" << endl;
	cout << " 7. Add a Playlist to a Playlist ...................... E" << endl;
	cout << " 8. Delete a Playlist from a Playlist ................. G" << endl;
	cout << " 9. Find a Song by Entering Characters ................ F" << endl;
	cout << "10. Find a Song in a Playlist by Entering Characters .. K" << endl;
	cout << "11. Sort the Jukebox .................................. S" << endl;
	cout << "12. Sort a Playlist ................................... R" << endl;
	cout << "13. Sort all Playlists .................................T" << endl;
	cout << "14. Select On a Value ................................. O" << endl;
	cout << "15. Write a Playlist to a File ........................ W" << endl;
	cout << "16. Save the State of the Jukebox ..................... V" << endl;
	cout << "17. Print the Contents of the Jukebox ................. P" << endl;
	cout << "18. Print the Contents of a Playlist .................. X" << endl;
	cout << "19. Print this Menu ................................... M" << endl;
	cout << "20. Quit .............................................. Q" << endl;
}

void PrintAddSongMenu() {
	cout << " Choose what values you wish to add to this song: " << endl;
	cout << " Add Album ............................................ A" << endl;
	cout << " Add Artist ........................................... R" << endl;
	cout << " Add Bitrate .......................................... B" << endl;
	cout << " Add File Creation Date ............................... D" << endl;
	cout << " Add File Name ........................................ X" << endl;
	cout << " Add File Path ........................................ Z" << endl;
	cout << " Change Flag Value .................................... F" << endl;
	cout << " Add Genre ............................................ G" << endl;
	cout << " Add Number of Channels ............................... N" << endl;
	cout << " Add Play Count ....................................... P" << endl;
	cout << " Add Sample Frequency Value ........................... Q" << endl;
	cout << " Add Unique Id Value .................................. I" << endl;
	cout << " Add Length ........................................... L" << endl;
	cout << " Add Title ............................................ T" << endl;
	cout << " Add Production Year .................................. Y" << endl;
	cout << " When all the values you want are added: \n" << endl;
	cout << " Add Song To Jukebox .................................. J\n" << endl;
}

void SortColumnsMenu() {
	cout << " Album ................................................ A" << endl;
	cout << " Artist ............................................... R" << endl;
	cout << " Bitrate .............................................. B" << endl;
	cout << " File Creation Date ................................... D" << endl;
	cout << " File Name ............................................ X" << endl;
	cout << " File Path ............................................ Z" << endl;
	cout << " Flag Value ........................................... F" << endl;
	cout << " Genre ................................................ G" << endl;
	cout << " Number of Channels ................................... N" << endl;
	cout << " Play Count ........................................... P" << endl;
	cout << " Sample Frequency Value ............................... Q" << endl;
	cout << " Unique Id Value ...................................... I" << endl;
	cout << " Length ............................................... L" << endl;
	cout << " Title ................................................ T" << endl;
	cout << " Production Year ...................................... Y" << endl;
	cout << " When all the values you want are added: \n" << endl;
	cout << " Sort ................................................. S\n" << endl;
}

void GetValue() {
	cout << "Value: " << flush;
	gets(buf);
}

char GetSelection () {
	char c;
	cout << "Choose an option (M to see options): " << flush;
	cin >> c;
	c = toupper(c);
	return c;
}

char GetChar () {
	char c;
	cout << "Enter a character: " << flush;
	cin >> c;
	return c;
}
/*

		//put entire jukebox, unsorted, into a playlist (works)
	case 2:
		break;

		//put genre sorted playlist into another playlist (works)
	case 3:
		if (1) {
			JPlaylist pl("mike");
			pl = Jukebox::GetInstance()->GetStandardPlaylist(GENRE);
			pl.PrintSongs();
		}
		break;
*/

