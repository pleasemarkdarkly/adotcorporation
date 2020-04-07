//........................................................................................
//........................................................................................
//.. File Name:  DlgAcquire.cpp
//.. Date: 12/27/2000																	..
//.. Author(s): Matthew Tadd
//.. Description of content: Acquire dialog procedure
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
#include <Aygshell.h>
#include <commctrl.h>
#include <stdlib.h>

#include "resource.h"
#include "DlgAcquire.h"

struct ThingsToDo
{
	int iMinTimeToComplete;
	int iMaxTimeToComplete;
	int iProgress;
	TCHAR szText[100];
};

#define TIME_STEP			100

#define NUM_THINGSTODO		12
#define LAST_THINGTODO		NUM_THINGSTODO - 1

static ThingsToDo ttd[] =
{
	{ 500, 2000, 5, TEXT("Connecting to FullAudio Services...") },
	{ 200, 300, 10, TEXT("Connected to FullAudio.") } ,
	{ 500, 2000, 13, TEXT("Authenticating Client....") } ,
	{ 200, 300, 20, TEXT("FullAudio for Pocket PC Authenticated.") },
	{ 500, 2000, 25, TEXT("Verifying Profile Information...") },
	{ 200, 500, 30, TEXT("User Profile Found.") },
	{ 500, 1500, 34, TEXT("Checking for Requested Content...") },
	{ 200, 300, 37, TEXT("Content not available in specified encoded format.") },
	{ 300, 500, 40, TEXT("Preparing Encoded Buffer...") },
	{ 200, 300, 50, TEXT("Beginning Transfer....") },
	{ 2000, 6000, 100, TEXT("Transferring...") },
	{ 0, 0, 100, TEXT("File Transfer Complete.") }
};

int GenerateRandomNumber(int min, int max)
{
	float frand = float(rand()) / float(RAND_MAX);
	int middle = min + static_cast<int>(static_cast<float>(max-min) * frand);
	return middle - (middle % 100);
}

// Mesage handler for the About box.
LRESULT CALLBACK Acquire(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int doing_item;
	static int duration_of_item;
	static int done_so_far;
	static int progress;
	static HWND hwndStatic;
	static HWND hwndProgress;
	static HWND hwndButton;
	static int result;

	switch (message)
	{
		case WM_TIMER:
			done_so_far += 100;
			if(done_so_far > duration_of_item)
			{
				//set frame progress bar to max of done item
				progress = ttd[doing_item].iProgress;
				SendMessage(hwndProgress,PBM_SETPOS,progress,0);
				done_so_far = 0;
				doing_item++;
				//set text to new item
				SetWindowText(hwndStatic,ttd[doing_item].szText);
				if(doing_item < LAST_THINGTODO)
				{
					duration_of_item = GenerateRandomNumber(ttd[doing_item].iMinTimeToComplete,ttd[doing_item].iMaxTimeToComplete);
				}
				else
				{
					KillTimer(hDlg,1);
					SetWindowText(hwndButton,TEXT("OK"));
					result = 1;
				}
			}
			else
			{
				//if frame progress less than max of item, increment 1.
				if(progress < ttd[doing_item].iProgress)
				{
					progress++;
					SendMessage(hwndProgress,PBM_SETPOS,progress,0);
				}
			}
			return TRUE;
		case WM_INITDIALOG:
			SHINITDLGINFO shidi;
			// Create a Done button and size it.  
			shidi.dwMask = SHIDIM_FLAGS;
			 //shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN;
			 shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIPDOWN ;
			shidi.hDlg = hDlg;
			SHInitDialog(&shidi);

			hwndStatic = GetDlgItem(hDlg,IDC_TEXT);
			hwndProgress = GetDlgItem(hDlg,IDC_PROGRESS);
			hwndButton = GetDlgItem(hDlg,IDC_BUTTON);

			srand(GetTickCount());
			doing_item = 0;
			done_so_far = 0;
			duration_of_item = GenerateRandomNumber(ttd[doing_item].iMinTimeToComplete,ttd[doing_item].iMaxTimeToComplete);
			progress = 0;
			result = 0;

			SetWindowText(hwndStatic,ttd[doing_item].szText);
			SendMessage(hwndProgress,PBM_SETPOS,0,0);
			SetWindowText(hwndButton,TEXT("Cancel"));

			SetTimer(hDlg,1,TIME_STEP,NULL);
			return TRUE; 
		case WM_COMMAND:
			if (LOWORD(wParam) == IDC_BUTTON) {
				KillTimer(hDlg,1);
				EndDialog(hDlg, result);
				return TRUE;
			}
			break;
	}
    return FALSE;
}
