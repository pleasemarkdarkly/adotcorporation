//****************************************************************************
//
// MAVUSB.CPP - Maverick(tm) Internet Audio Player Interface application.
//
// Copyright (c) 2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************
#include "stdafx.h"
#include <afxres.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shellapi.h>
#include "auddat.h"
#include "mavcmd.h"
#include "resource.h"
#include "../../player/audible.h"
#include "../../player/usbven.h"

//****************************************************************************
//
// The length of each month of the year, in days.  February is consider to be
// 29 days and the code explicitly handles making sure that it is 28 days in
// non-leap years.
//
//****************************************************************************
const unsigned char ucMonthLength[12] = { 31, 29, 31, 30, 31, 30,
                                          31, 31, 30, 31, 30, 31 };

//****************************************************************************
//
// The string representation of the days of the week.
//
//****************************************************************************
const char pcWeekdays[] = "SunMonTueWedThuFriSat";

//****************************************************************************
//
// The string representation of the months of the year.
//
//****************************************************************************
const char pcMonths[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

//****************************************************************************
//
// The instance handle of the application.
//
//****************************************************************************
HINSTANCE g_hInstance;

//****************************************************************************
//
// The currently displayed drive number.
//
//****************************************************************************
unsigned long g_ulDriveNum = 0;

//****************************************************************************
//
// The handle of the list view window.
//
//****************************************************************************
HWND g_hListView;

//****************************************************************************
//
// The handle of the update progress dialog box.
//
//****************************************************************************
HWND g_hDlg;

//****************************************************************************
//
// The handle of the thread created for downloading or uploading file data.
//
//****************************************************************************
HANDLE g_hThread;

//****************************************************************************
//
// The handle of the file being downloaded to or uploaded from the device.
//
//****************************************************************************
HANDLE g_hFile;

//****************************************************************************
//
// The length of the file being downloaded to or uploaded from the device.
//
//****************************************************************************
unsigned long g_ulLength;

//****************************************************************************
//
// The device name of the file being downloaded to or uploaded from the
// device.
//
//****************************************************************************
char *g_pcFileName;

//****************************************************************************
//
// The buffer containing the current block of data being transferred to or
// from the device.
//
//****************************************************************************
void *g_pvData;

//****************************************************************************
//
// TimeToSeconds converts the specified time as month, day, year, hour,
// minute, and second into the number of seconds since January 1, 1970.
//
//****************************************************************************
unsigned long
TimeToSeconds(unsigned long ulYear, unsigned long ulMonth, unsigned long ulDay,
              unsigned long ulHour, unsigned long ulMinute,
              unsigned long ulSecond)
{
    unsigned long ulRet, ulIdx;

    //
    // Compute the number of days that have past in this year.
    //
    ulRet = ulDay - 1;
    ulIdx = ulMonth - 1;
    while(ulIdx--)
    {
        ulRet += ucMonthLength[ulIdx];
    }

    //
    // If this is a non-leap year and the day is past February 28th, then
    // subtract the count of days by one (since we treat each year as if it
    // were a leap year).
    //
    if(((ulYear & 3) != 0) && (ulRet >= (31 + 28)))
    {
        ulRet--;
    }

    //
    // Add the days for the years that have past.
    //
    ulIdx = ulYear - 1970;
    ulRet += (ulIdx * 365) + ((ulIdx + 1) / 4);

    //
    // Convert the days to hours and add the current hour.
    //
    ulRet = (ulRet * 24) + ulHour;

    //
    // Convert the hours to minutes and add the current minute.
    //
    ulRet = (ulRet * 60) + ulMinute;

    //
    // Convert the minutes to seconds and add the current second.
    //
    ulRet = (ulRet * 60) + ulSecond;

    //
    // Return the converted time.
    //
    return(ulRet);
}

//****************************************************************************
//
// TimeFromSeconds converts the specified time in seconds since January 1,
// 1970 into a time structure, giving the corresponding month, day, year,
// hour, minute, second, and weekday.
//
//****************************************************************************
void
TimeFromSeconds(unsigned long ulSeconds, unsigned long *pulYear,
                unsigned long *pulMonth, unsigned long *pulDay,
                unsigned long *pulHour, unsigned long *pulMinute,
                unsigned long *pulSecond, unsigned long *pulWeekday)
{
    long lYear, lIdx = 0;

    //
    // Extract the number of seconds from the time.
    //
    *pulSecond = ulSeconds % 60;
    ulSeconds /= 60;

    //
    // Extract the number of minutes from the time.
    //
    *pulMinute = ulSeconds % 60;
    ulSeconds /= 60;

    //
    // Extract the number of hours from the time.
    //
    *pulHour = ulSeconds % 24;
    ulSeconds /= 24;

    //
    // We now have days, so add the number of days between January 1, 1900 and
    // January 1, 1970.
    //
    ulSeconds += (70 * 365) + 18;

    //
    // The day of the week just happens to work out this way.
    //
    *pulWeekday = ulSeconds % 7;

    //
    // Compute the number of years in terms of group of years from leap year to
    // leap year.
    //
    lYear = 4 * (ulSeconds / ((365 * 4) + 1));
    ulSeconds %= (365 * 4) + 1;

    //
    // If there are more than 365 days left in the current count of days, then
    // subtract the days from the remaining non-leap years.
    //
    if(ulSeconds >= 366)
    {
        lYear += (ulSeconds - 1) / 365;
        ulSeconds = (ulSeconds - 1) % 365;
    }

    //
    // Save the computed year.
    //
    *pulYear = 1900 + lYear;

    //
    // If this is a non-leap year and the day is past February 28th, then
    // increment the count of days by one (i.e. act as if each year is a leap
    // year).
    //
    if(((lYear & 3) != 0) && (ulSeconds >= (31 + 28)))
    {
        ulSeconds++;
    }

    //
    // Subtract days for each month till we find the current month.
    //
    while(ulSeconds >= ucMonthLength[lIdx])
    {
        ulSeconds -= ucMonthLength[lIdx++];
    }

    //
    // Save the computed month and day.
    //
    *pulDay = ulSeconds + 1;
    *pulMonth = lIdx + 1;
}

//****************************************************************************
//
// Checks the clock on the player and updates it if it is "too far" out of
// sync with the host clock.
//
//****************************************************************************
void
CheckTime(void)
{
    SYSTEMTIME sLocalTime;
    unsigned long ulLength, ulTime, ulPlayerTime;

    //
    // Get the current time from the player.
    //
    if(Maverick_GetDescriptor(USB_Vendor_Descriptor_CurrentTime, 0,
                              (char *)&ulPlayerTime, &ulLength) == 0)
    {
        //
        // We could not get the current time from the player, so do nothing
        // further.
        //
        return;
    }

    //
    // Get the current host time.
    //
    GetLocalTime(&sLocalTime);

    //
    // Convert the current host time into seconds.
    //
    ulTime = TimeToSeconds(sLocalTime.wYear, sLocalTime.wMonth,
                           sLocalTime.wDay, sLocalTime.wHour,
                           sLocalTime.wMinute, sLocalTime.wSecond);

    //
    // See if the current player time is within 10 seconds on either side of
    // the current host time.
    //
    if(((ulTime < ulPlayerTime) && ((ulPlayerTime - ulTime) > 10)) ||
       ((ulTime > ulPlayerTime) && ((ulTime - ulPlayerTime) > 10)))
    {
        //
        // The current player time is more than 10 seconds different than the
        // current host time, so change the player time to the current host
        // time.
        //
        Maverick_SetDescriptor(USB_Vendor_Descriptor_CurrentTime, 0,
                               (char *)&ulTime, 4);
    }
}

//****************************************************************************
//
// Converts a value into it's comma separated string representation.
//
//****************************************************************************
char *
GetValueString(ULONG ulValue)
{
    static char pcBuf[16];
    unsigned long ulLoop, ulIdx;

    //
    // Start filling the string buffer from the beginning.
    //
    ulIdx = 0;

    //
    // Start with the most significant digit and work our way down to the least
    // significant digit.
    //
    for(ulLoop = 1000000000; ulLoop; ulLoop /= 10)
    {
        //
        // If the value is greater than the current digit, or we are at the
        // first digit, then we need to add a character to our string buffer.
        //
        if((ulValue >= ulLoop) || (ulLoop == 1))
        {
            //
            // Place the character for the current digit into the string
            // buffer.
            //
            pcBuf[ulIdx++] = '0' + (char)((ulValue / ulLoop) % 10);

            //
            // If the current digit is the billions, millions, or thousands,
            // then we need to add a comma after the digit.
            //
            if((ulLoop == 1000000000) || (ulLoop == 1000000) ||
               (ulLoop == 1000))
            {
                //
                // Place a comma into the string buffer.
                //
                pcBuf[ulIdx++] = ',';
            }
        }
    }

    //
    // NULL terminate the string in the buffer.
    //
    pcBuf[ulIdx] = '\0';

    //
    // Return a pointer to the string.
    //
    return(pcBuf);
}

//****************************************************************************
//
// Converts a time specified as the number of seconds since January 1, 1970
// into a string representation.
//
//****************************************************************************
char *
GetTimeString(ULONG ulSeconds)
{
    static char pcBuf[32];
    unsigned long ulYear, ulMonth, ulDay, ulHour, ulMin, ulSec, ulWeekday;
    unsigned long ulIdx, bIsAM;

    //
    // Convert the number of seconds into the date and time.
    //
    TimeFromSeconds(ulSeconds, &ulYear, &ulMonth, &ulDay, &ulHour, &ulMin,
                    &ulSec, &ulWeekday);

    //
    // The starting string offset is zero.
    //
    ulIdx = 0;

    //
    // Put the string representation of the day of the week into the buffer.
    //
    memcpy(pcBuf + ulIdx, pcWeekdays + (ulWeekday * 3), 3);
    ulIdx += 3;

    //
    // Add a space after the week day.
    //
    pcBuf[ulIdx++] = ' ';

    //
    // Put the string representation of the month into the buffer.
    //
    memcpy(pcBuf + ulIdx, pcMonths + ((ulMonth - 1) * 3), 3);
    ulIdx += 3;

    //
    // Add a space after the month.
    //
    pcBuf[ulIdx++] = ' ';

    //
    // Put the string representation of the day into the buffer.
    //
    if(ulDay > 9)
    {
        pcBuf[ulIdx++] = '0' + (char)(ulDay / 10);
    }
    pcBuf[ulIdx++] = '0' + (char)(ulDay % 10);

    //
    // Add a command and space after the day.
    //
    pcBuf[ulIdx++] = ',',
    pcBuf[ulIdx++] = ' ';

    //
    // Put the string representation of the year into the buffer.
    //
    pcBuf[ulIdx++] = '0' + (char)(ulYear / 1000);
    pcBuf[ulIdx++] = '0' + (char)((ulYear / 100) % 10);
    pcBuf[ulIdx++] = '0' + (char)((ulYear / 10) % 10);
    pcBuf[ulIdx++] = '0' + (char)(ulYear % 10);

    //
    // Add a space after the year.
    //
    pcBuf[ulIdx++] = ' ';

    //
    // See if the hour is "am" or "pm".
    //
    if(ulHour < 12)
    {
        //
        // The hour is "am".
        //
        bIsAM = 1;

        //
        // If the hour is 0, then it is actually 12am.
        //
        if(ulHour == 0)
        {
            ulHour = 12;
        }
    }
    else
    {
        //
        // The hour is "pm".
        //
        bIsAM = 0;

        //
        // If the hour is not 12, then it is actually 12 hours before (i.e.
        // convert military time to civilian time).
        //
        if(ulHour != 12)
        {
            ulHour -= 12;
        }
    }

    //
    // Put the string representation of the hour into the buffer.
    //
    if(ulHour > 9)
    {
        pcBuf[ulIdx++] = '0' + (char)(ulHour / 10);
    }
    pcBuf[ulIdx++] = '0' + (char)(ulHour % 10);

    //
    // Add a colon after the hour.
    //
    pcBuf[ulIdx++] = ':';

    //
    // Put the string representation of the minute into the buffer.
    //
    pcBuf[ulIdx++] = '0' + (char)(ulMin / 10);
    pcBuf[ulIdx++] = '0' + (char)(ulMin % 10);

    //
    // Add a colon after the minute.
    //
    pcBuf[ulIdx++] = ':';

    //
    // Put the string representation of the second into the buffer.
    //
    pcBuf[ulIdx++] = '0' + (char)(ulSec / 10);
    pcBuf[ulIdx++] = '0' + (char)(ulSec % 10);

    //
    // Add a space after the second.
    //
    pcBuf[ulIdx++] = ' ';

    //
    // Add a "am" if it is morning or "pm" if it is evening.
    //
    if(bIsAM)
    {
        pcBuf[ulIdx++] = 'a';
    }
    else
    {
        pcBuf[ulIdx++] = 'p';
    }
    pcBuf[ulIdx++] = 'm';

    //
    // Null terminate the string.
    //
    pcBuf[ulIdx++] = '\0';

    //
    // Return the string representation of the given time.
    //
    return(pcBuf);
}

//****************************************************************************
//
// Loads the list of files on the internet audio player and adds an icon to
// the list view control for each file.
//
//****************************************************************************
void
LoadFileList(void)
{
    LVITEM sItem;
    long lIdx;
    unsigned short pusBuffer[256];
    char pcBuffer[256];

    //
    // Remove all the items from the list view control/
    //
    ListView_DeleteAllItems(g_hListView);

    //
    // Fill in the members of the LVITEM structure which are common to all the
    // items.
    //
    sItem.mask = LVIF_IMAGE | LVIF_TEXT | LVIF_PARAM;
    sItem.iSubItem = 0;
    sItem.state = 0;
    sItem.stateMask = 0;
    sItem.pszText = pcBuffer;
    sItem.cchTextMax = 0;
    sItem.iImage = 0;
    sItem.iIndent = 0;

    //
    // Open the root directory on the device.
    //
    if(Maverick_OpenDir(g_ulDriveNum, L"\\") == 0)
    {
        return;
    }

    //
    // Loop through all the files in the root directory of the device.
    //
    lIdx = 0;
    while(Maverick_ReadDir(pusBuffer, 1) != 0)
    {
        //
        // Convert the device name from Unicode to ASCII.
        //
        WideCharToMultiByte(CP_ACP, 0, pusBuffer, wcslen(pusBuffer) + 1,
                            pcBuffer, 256, NULL, NULL);

        //
        // Fill in the item number.
        //
        sItem.iItem = lIdx;
        sItem.lParam = lIdx++;

        //
        // Add this file to the list view.
        //
        ListView_InsertItem(g_hListView, &sItem);

        //
        // Open the file.
        //
        Maverick_Open(g_ulDriveNum, pusBuffer, 1);

        //
        // Get the length of the file and set the length into the list view.
        //
        ListView_SetItemText(g_hListView, lIdx - 1, 1,
                             GetValueString(Maverick_Length()));

        //
        // Get the modification date/time of the file and set it into the list
        // view.
        //
        ListView_SetItemText(g_hListView, lIdx - 1, 2,
                             GetTimeString(Maverick_GetDate()));

        //
        // Close the file.
        //
        Maverick_Close();
    }

    //
    // Close the directory on the device.
    //
    Maverick_CloseDir();
}

//****************************************************************************
//
// The thread which transfers a file to the Internet audio player.
//
//****************************************************************************
DWORD WINAPI
DownloadThread(void *pvParameter)
{
    unsigned long ulRead, ulLength;

    //
    // Get the length of the file.
    //
    ulLength = g_ulLength;

    //
    // While there is more data in the host file, read data from the host and
    // write it to the device file.
    //
    while(ulLength)
    {
        //
        // Read data from the host file.
        //
        ReadFile(g_hFile, g_pvData, 1024 * 1024, &ulRead, NULL);

        //
        // Write data to the device file.
        //
        Maverick_Write(g_pvData, ulRead);

        //
        // Decrement the number of bytes still to be read.
        //
        ulLength -= ulRead;
    }

    //
    // Close the file on the device.
    //
    Maverick_Close();

    //
    // Close the host file.
    //
    CloseHandle(g_hFile);

    //
    // Free the file data buffer.
    //
    GlobalFree(g_pvData);

    //
    // Return TRUE.
    //
    return(TRUE);
}

//****************************************************************************
//
// The dialog box procedure for the download progress indicator.
//
//****************************************************************************
BOOL APIENTRY
DownloadProgressDlgProc(HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
    //
    // Determine what to do based on the message we received.
    //
    switch(message)
    {
        //
        // The dialog is being initialized.
        //
        case WM_INITDIALOG:
        {
            char pcMessage[256];

            //
            // Schedule a WM_TIMER message to be sent to ourself in 100ms.
            //
            SetTimer(hDlg, 0, 100, NULL);

            //
            // Format the download message in the dialog box.
            //
            wsprintf(pcMessage, "Downloading \"%s\"", g_pcFileName);

            //
            // Put the download message in the appropriate field of the dialog
            // box.
            //
            SetDlgItemText(hDlg, IDC_FILENAME, pcMessage);

            //
            // Indicate that the first control in the dialog box should receive
            // the keyboard focus.
            //
            return(TRUE);
        }

        //
        // A command button or menu item was selected.
        //
        case WM_TIMER:
        {
            DWORD dwExitCode;
            unsigned long ulCount;

            //
            // Get the exit code from the download thread.
            //
            GetExitCodeThread(g_hThread, &dwExitCode);

            //
            // See if the download thread is still active.
            //
            if(dwExitCode != STILL_ACTIVE)
            {
                //
                // The download thread has completed, so dismiss the progress
                // dialog box.
                //
                EndDialog(hDlg, TRUE);

                //
                // We've handled this message.
                //
                return(TRUE);
            }

            //
            // Get the current count of bytes transferred from the Maverick
            // USB driver.
            //
            ulCount = Maverick_GetTransferCount();

            //
            // Set the position of the progress indicator based on the number
            // of bytes transferred.
            //
            SendMessage(GetDlgItem(hDlg, IDC_PROGRESS), PBM_SETPOS,
                        (ulCount * 100) / g_ulLength, 0);

            //
            // Schedule another WM_TIMER message to be sent in 100ms.
            //
            SetTimer(hDlg, 0, 100, NULL);

            //
            // We're done handling this message.
            //
            return(TRUE);
        }
    }

    //
    // Indicate that we did not handle the message.
    //
    return(FALSE);
}

//****************************************************************************
//
// Download a file to the Internet audio player.
//
//****************************************************************************
BOOL
Download(HWND hWnd, char *pcFileName)
{
    DWORD dwThreadID;
    char *pcDeviceName;
    unsigned short pusFileName[128];

    //
    // Allocate memory for the file data.
    //
    g_pvData = GlobalAlloc(GMEM_FIXED, 1024 * 1024);
    if(!g_pvData)
    {
        return(FALSE);
    }

    //
    // Open the host file.
    //
    g_hFile = CreateFile(pcFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
                         OPEN_EXISTING, 0, NULL);
    if(g_hFile == INVALID_HANDLE_VALUE)
    {
        GlobalFree(g_pvData);
        return(FALSE);
    }

    //
    // Get the length of the file.
    //
    g_ulLength = GetFileSize(g_hFile, NULL);

    //
    // Find the last '\' character in the name of the host file.  The remainder
    // of the host file name will become the device file name.
    //
    pcDeviceName = strrchr(pcFileName, '\\');
    if(pcDeviceName)
    {
        g_pcFileName = pcDeviceName + 1;
    }
    else
    {
        g_pcFileName = pcFileName;
    }

    //
    // Convert the file name from ASCII to Unicode.
    //
    MultiByteToWideChar(CP_ACP, 0, g_pcFileName, strlen(g_pcFileName) + 1,
                        pusFileName, 128);

    //
    // Attempt to open the specified file on the device.
    //
    if(Maverick_Open(g_ulDriveNum, pusFileName, 6) == 0)
    {
        GlobalFree(g_pvData);
        CloseHandle(g_hFile);
        return(FALSE);
    }

    //
    // Reset the transfer count in the Maverick USB driver.
    //
    Maverick_ResetTransferCount();

    //
    // Create the thread which will actually download the data to the device.
    //
    g_hThread = CreateThread(NULL, 0, DownloadThread, NULL, 0, &dwThreadID);

    //
    // Create the progress indicator dialog box.
    //
    DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_PROGRESS), hWnd,
              DownloadProgressDlgProc);

    //
    // We've successfully downloaded the file.
    //
    return(TRUE);
}

//****************************************************************************
//
// Downloads the set of files specified in a play list.
//
//****************************************************************************
BOOL
DownloadPlayList(HWND hWnd, char *pcFileName, unsigned long bIsM3U)
{
    HANDLE hFile;
    char *pcBuffer, *pcLine;
    unsigned long ulOffset, ulLength;

    //
    // Open the host file.
    //
    hFile = CreateFile(pcFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, 0, NULL);
    if(hFile == INVALID_HANDLE_VALUE)
    {
        return(FALSE);
    }

    //
    // Get the length of the file.
    //
    ulLength = GetFileSize(hFile, NULL);

    //
    // Allocate memory for the file data.
    //
    pcBuffer = (char *)GlobalAlloc(GMEM_FIXED, ulLength);
    if(!pcBuffer)
    {
        CloseHandle(hFile);
        return(FALSE);
    }

    //
    // Read data from the host file.
    //
    ReadFile(hFile, pcBuffer, ulLength, &ulOffset, NULL);

    //
    // Loop through the entire buffer, replacing carriage return and line feed
    // characters with NULLs.
    //
    for(ulOffset = 0; ulOffset < ulLength; ulOffset++)
    {
        //
        // See if this is a carriage return or line feed character.
        //
        if((pcBuffer[ulOffset] == '\r') || (pcBuffer[ulOffset] == '\n'))
        {
            //
            // Replace this character with a NULL.
            //
            pcBuffer[ulOffset] = '\0';
        }
    }

    //
    // Look through each line of the file.
    //
    for(ulOffset = 0; ulOffset < ulLength;
        ulOffset += strlen(pcBuffer + ulOffset) + 1)
    {
        pcLine = pcBuffer + ulOffset;

        //
        // See if this is a M3U play list.
        //
        if(bIsM3U)
        {
            //
            // If this is a comment line then ignore it.
            //
            if(pcBuffer[ulOffset] == '#')
            {
                continue;
            }

            //
            // If this is a blank line then ignore it.
            //
            if(pcBuffer[ulOffset] == '\0')
            {
                continue;
            }
        }

        //
        // Otherwise, this is a PLS play list.
        //
        else
        {
            //
            // If this line does not start with "File" then ignore it.
            //
            if((pcBuffer[ulOffset] != 'F') ||
               (pcBuffer[ulOffset + 1] != 'i') ||
               (pcBuffer[ulOffset + 2] != 'l') ||
               (pcBuffer[ulOffset + 3] != 'e'))
            {
                continue;
            }

            //
            // Following "File" must be a sequence of digits.
            //
            for(ulOffset += 4; pcBuffer[ulOffset]; ulOffset++)
            {
                //
                // Stop looking at this line if this character is not a digit.
                //
                if((pcBuffer[ulOffset] < '0') || (pcBuffer[ulOffset] > '9'))
                {
                    break;
                }
            }

            //
            // Following the digits must be a '='; if not, then ignore this
            // line.
            //
            if(pcBuffer[ulOffset] != '=')
            {
                continue;
            }

            //
            // Skip the '='.
            //
            ulOffset++;
        }

        //
        // Download this file.
        //
        Download(hWnd, pcBuffer + ulOffset);
    }

    //
    // Free the memory buffer.
    //
    GlobalFree(pcBuffer);

    //
    // Close the host file.
    //
    CloseHandle(hFile);

    //
    // Success.
    //
    return(TRUE);
}

//****************************************************************************
//
// Determines how to download the specified file.
//
//****************************************************************************
BOOL
DoDownload(HWND hWnd, char *pcFileName)
{
    unsigned long ulLength;

    //
    // Get the length of the file name.
    //
    ulLength = strlen(pcFileName);

    //
    // See if the name of this file ends in ".m3u".
    //
    if((pcFileName[ulLength - 4] == '.') &&
       (pcFileName[ulLength - 3] == 'm') &&
       (pcFileName[ulLength - 2] == '3') &&
       (pcFileName[ulLength - 1] == 'u'))
    {
        //
        // Download the files specified in this M3U play list.
        //
        return(DownloadPlayList(hWnd, pcFileName, TRUE));
    }

    //
    // See if the name of this file ends in ".pls".
    //
    else if((pcFileName[ulLength - 4] == '.') &&
            (pcFileName[ulLength - 3] == 'p') &&
            (pcFileName[ulLength - 2] == 'l') &&
            (pcFileName[ulLength - 1] == 's'))
    {
        //
        // Download the files specified in this PLS play list.
        //
        return(DownloadPlayList(hWnd, pcFileName, FALSE));
    }

    //
    // Otherwise, this is a media file.
    //
    else
    {
        //
        // Simply download all other files.
        //
        return(Download(hWnd, pcFileName));
    }
}

//****************************************************************************
//
// The thread which transfers a file from the Internet audio player.
//
//****************************************************************************
DWORD WINAPI
UploadThread(void *pvParameter)
{
    unsigned long ulWrote, ulSize;

    //
    // Read data from the device file and write it to the host file.
    //
    while(1)
    {
        //
        // Read data from the device file.
        //
        ulSize = Maverick_Read(g_pvData, 1024 * 1024);
        if(!ulSize)
        {
            break;
        }

        //
        // Write data to the host file.
        //
        WriteFile(g_hFile, g_pvData, ulSize, &ulWrote, NULL);
    }

    //
    // Close the file on the device.
    //
    Maverick_Close();

    //
    // Close the host file.
    //
    CloseHandle(g_hFile);

    //
    // Free the file data buffer.
    //
    GlobalFree(g_pvData);

    //
    // Success.
    //
    return(TRUE);
}

//****************************************************************************
//
// The dialog box procedure for the upload progress indicator.
//
//****************************************************************************
BOOL APIENTRY
UploadProgressDlgProc(HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
    //
    // Determine what to do based on the message we received.
    //
    switch(message)
    {
        //
        // The dialog is being initialized.
        //
        case WM_INITDIALOG:
        {
            char pcMessage[256];

            //
            // Schedule a WM_TIMER message to be sent to ourself in 100ms.
            //
            SetTimer(hDlg, 0, 100, NULL);

            //
            // Format the upload message in the dialog box.
            //
            wsprintf(pcMessage, "Uploading \"%s\"", g_pcFileName);

            //
            // Put the download message in the appropriate field of the dialog
            // box.
            //
            SetDlgItemText(hDlg, IDC_FILENAME, pcMessage);

            //
            // Indicate that the first control in the dialog box should receive
            // the keyboard focus.
            //
            return(TRUE);
        }

        //
        // A command button or menu item was selected.
        //
        case WM_TIMER:
        {
            DWORD dwExitCode;
            unsigned long ulCount;

            //
            // Get the exit code from the upload thread.
            //
            GetExitCodeThread(g_hThread, &dwExitCode);

            //
            // See if the upload thread is still active.
            //
            if(dwExitCode != STILL_ACTIVE)
            {
                //
                // The upload thread has completed, so dismiss the progress
                // dialog box.
                //
                EndDialog(hDlg, TRUE);

                //
                // We've handled this message.
                //
                return(TRUE);
            }

            //
            // Get the current count of bytes transferred from the Maverick
            // USB driver.
            //
            ulCount = Maverick_GetTransferCount();

            //
            // Set the position of the progress indicator based on the number
            // of bytes transferred.
            //
            SendMessage(GetDlgItem(hDlg, IDC_PROGRESS), PBM_SETPOS,
                        (ulCount * 100) / g_ulLength, 0);

            //
            // Schedule another WM_TIMER message to be sent in 100ms.
            //
            SetTimer(hDlg, 0, 100, NULL);

            //
            // We're done handling this message.
            //
            return(TRUE);
        }
    }

    //
    // Indicate that we did not handle the message.
    //
    return(FALSE);
}

//****************************************************************************
//
// Uploads a file from the Internet audio player.
//
//****************************************************************************
BOOL
Upload(HWND hWnd, char *pcFileName, char *pcDeviceName)
{
    DWORD dwThreadID;
    unsigned short pusDeviceName[128];

    //
    // Allocate memory for the file data.
    //
    g_pvData = GlobalAlloc(GMEM_FIXED, 1024 * 1024);
    if(!g_pvData)
    {
        return(FALSE);
    }

    //
    // Convert the file name from ASCII to Unicode.
    //
    MultiByteToWideChar(CP_ACP, 0, pcDeviceName, strlen(pcDeviceName) + 1,
                        pusDeviceName, 128);

    //
    // Open the file on the device.
    //
    if(Maverick_Open(g_ulDriveNum, pusDeviceName, 1) == 0)
    {
        GlobalFree(g_pvData);
        return(FALSE);
    }

    //
    // Create the host file.
    //
    g_hFile = CreateFile(pcFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                         CREATE_NEW, 0, NULL);
    if(g_hFile == INVALID_HANDLE_VALUE)
    {
        GlobalFree(g_pvData);
        Maverick_Close();
        return(FALSE);
    }

    //
    // Get the length of the file.
    //
    g_ulLength = Maverick_Length();

    //
    // Save the name of the device file.
    //
    g_pcFileName = pcDeviceName;

    //
    // Reset the transfer count in the Maverick USB driver.
    //
    Maverick_ResetTransferCount();

    //
    // Create the thread which will actually upload the data from the device.
    //
    g_hThread = CreateThread(NULL, 0, UploadThread, NULL, 0, &dwThreadID);

    //
    // Create the progress indicator dialog box.
    //
    DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_PROGRESS), hWnd,
              UploadProgressDlgProc);

    //
    // We've successfully uploaded the file.
    //
    return(TRUE);
}

//****************************************************************************
//
// The thread which updates the software on the Internet audio player.
//
//****************************************************************************
DWORD WINAPI
UpdateThread(void *pvParameter)
{
    DWORD *pdwResult;
    unsigned long ulRead;

    //
    // Convert the parameter in a usable pointer.
    //
    pdwResult = (DWORD *)pvParameter;

    //
    // Create the "file" for the new code image.
    //
    if(Maverick_Create(0, L"", g_ulLength) == 0)
    {
        GlobalFree(g_pvData);
        CloseHandle(g_hFile);
        *pdwResult = FALSE;
        return(FALSE);
    }

    //
    // Indicate that we are currently programming the FLASH.
    //
    SetDlgItemText(g_hDlg, IDC_UPDATE_PROGRESS, "Programming the FLASH...");

    //
    // While there is more data in the host file, read data from the host and
    // write it to the device file.
    //
    while(g_ulLength)
    {
        //
        // Read data from the host file.
        //
        ReadFile(g_hFile, g_pvData, 1024 * 1024, &ulRead, NULL);

        //
        // Write data to the device file.
        //
        Maverick_Write(g_pvData, ulRead);

        //
        // Decrement the number of bytes still to be read.
        //
        g_ulLength -= ulRead;
    }

    //
    // Close the file on the device.
    //
    Maverick_Close();

    //
    // Close the host file.
    //
    CloseHandle(g_hFile);

    //
    // Free the file data buffer.
    //
    GlobalFree(g_pvData);

    //
    // Indicate that we are restarting the player.
    //
    SetDlgItemText(g_hDlg, IDC_UPDATE_PROGRESS, "Restarting the player...");

    //
    // Now, disconnect from the player.  This will cause it to reboot and run
    // the newly downloaded software image.
    //
    Maverick_CloseDevice();

    //
    // It takes a couple of seconds for the player to restart and reconnect to
    // the USB, so wait for 3 seconds.
    //
    SleepEx(3000, FALSE);

    //
    // Make a couple of attempts at reconnecting to the player.  We don't know
    // exactly how long it will take for the player to restart and reconnect
    // to the USB.
    //
    for(ulRead = 0; ulRead < 10; ulRead++)
    {
        //
        // Delay for 500ms.
        //
        SleepEx(500, FALSE);

        //
        // Attempt to reconnect to the player.
        //
        if(Maverick_OpenDevice() != 0)
        {
            //
            // We were able to reconnect, so the result is success.
            //
            *pdwResult = TRUE;
            return(TRUE);
        }

        //
        // Close the device just in case.
        //
        Maverick_CloseDevice();
    }

    //
    // We were unable to reconnect, so the result is failure.
    //
    *pdwResult = FALSE;
    return(FALSE);
}

//****************************************************************************
//
// The dialog box procedure for the update progress indicator.
//
//****************************************************************************
BOOL APIENTRY
UpdateProgressDlgProc(HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
    //
    // Determine what to do based on the message we received.
    //
    switch(message)
    {
        //
        // The dialog is being initialized.
        //
        case WM_INITDIALOG:
        {
            //
            // Save the dialog box handle.
            //
            g_hDlg = hDlg;

            //
            // Schedule a WM_TIMER message to be sent to ourself in 100ms.
            //
            SetTimer(hDlg, 0, 100, NULL);

            //
            // Indicate that we are currently erasing the FLASH.
            //
            SetDlgItemText(hDlg, IDC_UPDATE_PROGRESS, "Erasing the FLASH...");

            //
            // Indicate that the first control in the dialog box should receive
            // the keyboard focus.
            //
            return(TRUE);
        }

        //
        // A command button or menu item was selected.
        //
        case WM_TIMER:
        {
            DWORD dwExitCode;

            //
            // Get the exit code from the download thread.
            //
            GetExitCodeThread(g_hThread, &dwExitCode);

            //
            // See if the download thread is still active.
            //
            if(dwExitCode != STILL_ACTIVE)
            {
                //
                // The download thread has completed, so dismiss the progress
                // dialog box.
                //
                EndDialog(hDlg, TRUE);

                //
                // We've handled this message.
                //
                return(TRUE);
            }

            //
            // Schedule another WM_TIMER message to be sent in 100ms.
            //
            SetTimer(hDlg, 0, 100, NULL);

            //
            // We're done handling this message.
            //
            return(TRUE);
        }
    }

    //
    // Indicate that we did not handle the message.
    //
    return(FALSE);
}

//****************************************************************************
//
// Updates the software on the Internet audio player.
//
//****************************************************************************
BOOL
Update(HWND hWnd, char *pcFileName)
{
    DWORD dwThreadID, dwResult;
    unsigned long ulHardwareID;

    //
    // Allocate memory for the file data.
    //
    g_pvData = GlobalAlloc(GMEM_FIXED, 1024 * 1024);
    if(!g_pvData)
    {
        return(FALSE);
    }

    //
    // Open the host file.
    //
    g_hFile = CreateFile(pcFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
                         OPEN_EXISTING, 0, NULL);
    if(g_hFile == INVALID_HANDLE_VALUE)
    {
        GlobalFree(g_pvData);
        return(FALSE);
    }

    //
    // Get the length of the image.
    //
    g_ulLength = GetFileSize(g_hFile, NULL);

    //
    // Read data from the host file.
    //
    ReadFile(g_hFile, g_pvData, 1024, &dwResult, NULL);

    //
    // Seek back to the beginning of the file.
    //
    SetFilePointer(g_hFile, 0, NULL, FILE_BEGIN);

    //
    // Get the hardware ID from the player.
    //
    if(Maverick_GetDescriptor(USB_Vendor_Descriptor_HardwareID, 0,
                              (char *)&ulHardwareID, &dwResult) == 0)
    {
        CloseHandle(g_hFile);
        GlobalFree(g_pvData);
        return(FALSE);
    }

    //
    // Make sure that the hardware ID of the player matches the hardware ID
    // of the image being read, and that the length of the image makes sense.
    //
    if(((((unsigned long *)g_pvData)[9]) != g_ulLength) ||
       (((unsigned long *)g_pvData)[10] != ulHardwareID))
    {
        //
        // Display an error message in a message box.
        //
        if((((unsigned long *)g_pvData)[9]) == g_ulLength)
        {
            MessageBox(hWnd, "This software image is not compatible with the "
                       "player.", "Maverick(tm) Internet Audio Player", MB_OK);
        }
        else
        {
            MessageBox(hWnd, "Invalid software image file.",
                       "Maverick(tm) Internet Audio Player", MB_OK);
        }

        //
        // Return a failure.
        //
        CloseHandle(g_hFile);
        GlobalFree(g_pvData);
        return(FALSE);
    }

    //
    // Attempt to start the update.
    //
    if(Maverick_StartUpdate() == 0)
    {
        //
        // Tell the user that the player does not support software updates.
        //
        MessageBox(hWnd, "The player does not support software updates.",
                   "Maverick(tm) Internet Audio Player", MB_OK);

        //
        // Return a failure.
        //
        GlobalFree(g_pvData);
        CloseHandle(g_hFile);
        return(FALSE);
    }

    //
    // Create the thread which will actually update the software on the device.
    //
    g_hThread = CreateThread(NULL, 0, UpdateThread, (void *)&dwResult, 0,
                             &dwThreadID);

    //
    // Create the progress indicator dialog box.
    //
    DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_UPDATE), hWnd,
              UpdateProgressDlgProc);

    //
    // If we could not reconnect to the player, then display an error message
    // and then exit the application.
    //
    if(dwResult == FALSE)
    {
        //
        // Display an error message in the message box.
        //
        MessageBox(hWnd,
                   "Unable to reconnect to the Internet audio player..."
                   "exiting.", "Maverick(tm) Internet Audio Player", MB_OK);

        //
        // Exit the application.
        //
        PostQuitMessage(0);
    }

    //
    // We've successfully updated the software.
    //
    return(TRUE);
}

//****************************************************************************
//
// Displays an open file dialog and downloads the selected files to the
// device.
//
//****************************************************************************
void
OpenFiles(HWND hWnd)
{
    OPENFILENAME ofn;
    char pcBuffer[4096], *pcFileNameStart;
    unsigned long ulLength;

    //
    // Fill in the open file dialog box structure.
    //
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.hInstance = g_hInstance;
    ofn.lpstrFilter = "Music Files (.mp3;.wma;.wav)\0"
                      "*.mp3;*.wma;*.wav\0"
                      "Play Lists (.m3u;.pls)\0"
                      "*.m3u;*.pls\0"
                      "All Files (*.*)\0"
                      "*.*\0";
    ofn.lpstrCustomFilter = NULL;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = pcBuffer;
    ofn.nMaxFile = 4096;
    ofn.lpstrFileTitle = NULL;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = NULL;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_EXPLORER |
                OFN_ALLOWMULTISELECT;
    ofn.lpTemplateName = NULL;

    //
    // The initial file name is empty.
    //
    pcBuffer[0] = 0;

    //
    // Display the open file dialog box.
    //
    if(GetOpenFileName(&ofn) != 0)
    {
        //
        // A file (or files) were selected successfully.  See if a single file
        // or multiple files were selected.
        //
        if(ofn.lpstrFile[strlen(pcBuffer) + 1] == 0)
        {
            //
            // A single file was selected, so download it now.
            //
            DoDownload(hWnd, pcBuffer);
        }
        else
        {
            //
            // Multiple files were selected, so we need to construct the
            // individual file names and download them one at a time.  Get a
            // pointer to the end of the path name (which is the beginning of
            // the file name).
            //
            pcFileNameStart = pcBuffer + strlen(pcBuffer);

            //
            // While there are more file names, download the files.
            //
            while(*ofn.lpstrFile)
            {
                //
                // Generate the name of the file to download.
                //
                if(!*pcFileNameStart)
                {
                    //
                    // This is the first file, so simply replace the NULL at
                    // the end of the directory name with a '\'.
                    //
                    *pcFileNameStart = '\\';

                    //
                    // Move the file name pointer to the start of the next
                    // file name.
                    //
                    ofn.lpstrFile += strlen(pcBuffer) + 1;
                }
                else
                {
                    //
                    // Get the length of this file name.
                    //
                    ulLength = strlen(ofn.lpstrFile) + 1;

                    //
                    // Copy this file name to the end of the directory name.
                    //
                    strcpy(pcFileNameStart + 1, ofn.lpstrFile);

                    //
                    // Move the file name pointer to the start of the next
                    // file name.
                    //
                    ofn.lpstrFile += ulLength;
                }

                //
                // Download this file.
                //
                DoDownload(hWnd, pcBuffer);
            }
        }
    }

    //
    // Re-load the list of files on the device.
    //
    LoadFileList();
}

//****************************************************************************
//
// The following variables contain the values retrieved from the user via the
// Audible dialog box.
//
//****************************************************************************
char g_pcAudibleTitle[256];
unsigned long g_ulAudibleStart;
unsigned long g_ulAudibleSections;
unsigned long g_pulAudibleSection[5];

//****************************************************************************
//
// The message handler for our Audible dialog box.
//
//****************************************************************************
BOOL APIENTRY
AudibleDlgProc(HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
    //
    // Determine what to do based on the message we received.
    //
    switch(message)
    {
        //
        // The dialog is being initialized.
        //
        case WM_INITDIALOG:
        {
            //
            // Clear the text fields in the dialog box.
            //
            SetDlgItemText(hDlg, IDC_TITLE, "");
            SetDlgItemInt(hDlg, IDC_START_TIME, 0, FALSE);
            SetDlgItemInt(hDlg, IDC_NUM_SECTIONS, 0, FALSE);
            SetDlgItemInt(hDlg, IDC_SECTION1, 0, FALSE);
            SetDlgItemInt(hDlg, IDC_SECTION2, 0, FALSE);
            SetDlgItemInt(hDlg, IDC_SECTION3, 0, FALSE);
            SetDlgItemInt(hDlg, IDC_SECTION4, 0, FALSE);
            SetDlgItemInt(hDlg, IDC_SECTION5, 0, FALSE);

            //
            // Indicate that the first control in the dialog box should receive
            // the keyboard focus.
            //
            return(TRUE);
        }

        //
        // A command button or menu item was selected.
        //
        case WM_COMMAND:
        {
            //
            // See if the "OK" button was pressed, or escape was pressed.
            //
            if((LOWORD(wParam) == IDOK) || (LOWORD(wParam) == IDCANCEL))
            {
                //
                // See if the "OK" button was pressed.
                //
                if(LOWORD(wParam) == IDOK)
                {
                    //
                    // Retrieve the values from the dialog box.
                    //
                    GetDlgItemText(hDlg, IDC_TITLE, g_pcAudibleTitle, 256);
                    g_ulAudibleStart = GetDlgItemInt(hDlg, IDC_START_TIME,
                                                     NULL, FALSE);
                    g_ulAudibleSections = GetDlgItemInt(hDlg, IDC_NUM_SECTIONS,
                                                        NULL, FALSE);
                    g_pulAudibleSection[0] = GetDlgItemInt(hDlg, IDC_SECTION1,
                                                           NULL, FALSE);
                    g_pulAudibleSection[1] = GetDlgItemInt(hDlg, IDC_SECTION2,
                                                           NULL, FALSE);
                    g_pulAudibleSection[2] = GetDlgItemInt(hDlg, IDC_SECTION3,
                                                           NULL, FALSE);
                    g_pulAudibleSection[3] = GetDlgItemInt(hDlg, IDC_SECTION4,
                                                           NULL, FALSE);
                    g_pulAudibleSection[4] = GetDlgItemInt(hDlg, IDC_SECTION5,
                                                           NULL, FALSE);
                }

                //
                // We are done with this dialog box, so dismiss it.
                //
                EndDialog(hDlg, LOWORD(wParam));

                //
                // Indicate that we've handled this message.
                //
                return(TRUE);
            }

            //
            // We're done handling this message.
            //
            break;
        }
    }

    //
    // Indicate that we did not handle the message.
    //
    return(FALSE);
}

//****************************************************************************
//
// Displays an open file dialog and downloads the selected files to the
// device.
//
//****************************************************************************
void
OpenAudibleFile(HWND hWnd)
{
    OPENFILENAME ofn;
    char pcBuffer[4096], *pcDeviceName, cAudibleData[512];
    tAudibleMetaData *pAudibleData = (tAudibleMetaData *)cAudibleData;
    unsigned short pusDeviceName[128];

    //
    // Fill in the open file dialog box structure.
    //
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.hInstance = g_hInstance;
    ofn.lpstrFilter = "Music Files (.mp3;.wma;.wav)\0"
                      "*.mp3;*.wma;*.wav\0"
                      "All Files (*.*)\0"
                      "*.*\0";
    ofn.lpstrCustomFilter = NULL;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = pcBuffer;
    ofn.nMaxFile = 4096;
    ofn.lpstrFileTitle = NULL;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = NULL;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_EXPLORER;
    ofn.lpTemplateName = NULL;

    //
    // The initial file name is empty.
    //
    pcBuffer[0] = 0;

    //
    // Display the open file dialog box.
    //
    if(GetOpenFileName(&ofn) != 0)
    {
        //
        // Display the Audible information dialog box.
        //
        if(DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_AUDIBLE), hWnd,
                     AudibleDlgProc) == IDOK)
        {
            //
            // Find the last '\' character in the name of the host file.  The
            // remainder of the host file name will become the device file
            // name.
            //
            pcDeviceName = strrchr(pcBuffer, '\\');
            if(pcDeviceName)
            {
                pcDeviceName++;
            }
            else
            {
                pcDeviceName = pcBuffer;
            }

            //
            // Clear the Audible meta-data for this file.
            //
            memset(cAudibleData, 0, 512);

            //
            // Convert the title from ASCII to Unicode and place it in the
            // meta-data structure.
            //
            MultiByteToWideChar(CP_ACP, 0, g_pcAudibleTitle,
                                strlen(g_pcAudibleTitle) + 1,
                                pAudibleData->pusTitle, 128);

            //
            // Set the start of the Audible program in the meta-data.
            //
            pAudibleData->ulProgramStart = g_ulAudibleStart;

            //
            // Set the number of sections in the Audible program in the
            // meta-data.
            //
            pAudibleData->ulSections = g_ulAudibleSections;

            //
            // Save the offset of the sections in the meta-data.
            //
            pAudibleData->pulSectionStart[0] = g_pulAudibleSection[0];
            pAudibleData->pulSectionStart[1] = g_pulAudibleSection[1];
            pAudibleData->pulSectionStart[2] = g_pulAudibleSection[2];
            pAudibleData->pulSectionStart[3] = g_pulAudibleSection[3];
            pAudibleData->pulSectionStart[4] = g_pulAudibleSection[4];

            //
            // Convert the device file name from ASCII to Unicode.
            //
            MultiByteToWideChar(CP_ACP, 0, pcDeviceName,
                                strlen(pcDeviceName) + 1, pusDeviceName, 128);

            //
            // Add the Audible meta-data for this file to the device.
            //
            SetAudibleMetaData(g_ulDriveNum, pusDeviceName, cAudibleData, 508);

            //
            // A single file was selected, so download it now.
            //
            Download(hWnd, pcBuffer);
        }
    }

    //
    // Re-load the list of files on the device.
    //
    LoadFileList();
}

//****************************************************************************
//
// Displays a save file dialog and uploads the selected files from the device.
//
//****************************************************************************
void
SaveFiles(HWND hWnd)
{
    OPENFILENAME ofn;
    char pcPath[MAX_PATH], pcFile[256];
    LVITEM item;
    int iNumItems, iIdx;

    //
    // Get the number of items which are selected.
    //
    iNumItems = ListView_GetSelectedCount(g_hListView);

    //
    // Find the first selected item in the list view control.
    //
    iIdx = 0;
    while(1)
    {
        //
        // Fill in the LVITEM structure for this item.
        //
        item.mask = LVIF_STATE | LVIF_TEXT;
        item.iItem = iIdx++;
        item.iSubItem = 0;
        item.stateMask = LVIS_SELECTED;
        item.pszText = pcPath;
        item.cchTextMax = MAX_PATH;

        //
        // Get the item information.
        //
        ListView_GetItem(g_hListView, &item);

        //
        // Is this item selected?
        //
        if(item.state & LVIS_SELECTED)
        {
            break;
        }
    }

    //
    // Fill in the save file dialog box structure.
    //
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.hInstance = g_hInstance;
    ofn.lpstrFilter = "All files (*.*)\0*.*\0\0\0";
    ofn.lpstrCustomFilter = NULL;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = pcPath;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFileTitle = NULL;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_NOTESTFILECREATE;
    ofn.lpTemplateName = NULL;

    //
    // Display the save as dialog box.
    //
    if(GetSaveFileName(&ofn) != 0)
    {
        //
        // Remove the file name from the end of the path name.
        //
        pcPath[ofn.nFileOffset] = '\0';

        //
        // Loop through all the items to find the ones which are selected.
        //
        iIdx = 0;
        while(iNumItems)
        {
            //
            // Fill in the LVITEM structure for this item.
            //
            item.mask = LVIF_STATE | LVIF_TEXT;
            item.iItem = iIdx++;
            item.iSubItem = 0;
            item.stateMask = LVIS_SELECTED;
            item.pszText = pcFile;
            item.cchTextMax = 256;

            //
            // Get the item information.
            //
            ListView_GetItem(g_hListView, &item);

            //
            // Is this item selected?
            //
            if(item.state & LVIS_SELECTED)
            {
                //
                // Append the file name to the directory name.
                //
                strcpy(pcPath + ofn.nFileOffset, pcFile);

                //
                // Upload this file.
                //
                Upload(hWnd, pcPath, pcFile);

                //
                // Decrement the count of selected items.
                //
                iNumItems--;
            }
        }
    }
}

//****************************************************************************
//
// Pastes files from the clipboard to the device.
//
//****************************************************************************
void
Paste(HWND hWnd)
{
    HDROP hDrop;
    int iFiles, iIdx;
    char pcBuffer[256];

    //
    // Open the clipboard.
    //
    if(OpenClipboard(NULL) == 0)
    {
        return;
    }

    //
    // Try to get the file name list from the clipboard.
    //
    hDrop = (HDROP)GetClipboardData(CF_HDROP);
    if(hDrop == NULL)
    {
        CloseClipboard();
        return;
    }

    //
    // Get the number of files in the clipboard.
    //
    iFiles = DragQueryFile(hDrop, -1, NULL, 0);

    //
    // Loop through all the files in the clipboard.
    //
    for(iIdx = 0; iIdx < iFiles; iIdx++)
    {
        //
        // Get the name of this file.
        //
        DragQueryFile(hDrop, iIdx, pcBuffer, 256);

        //
        // Download this file to the player.
        //
        DoDownload(hWnd, pcBuffer);
    }

    //
    // Close the clipboard.
    //
    CloseClipboard();

    //
    // Re-load the list of files on the device.
    //
    LoadFileList();
}

//****************************************************************************
//
// Deletes the currently selected file(s).
//
//****************************************************************************
void
DeleteFiles(HWND hWnd)
{
    LVITEM item;
    int iNumItems, iIdx;
    char pcMsg[256];
    unsigned short pusFileName[128];

    //
    // Get the number of items which are selected.
    //
    iNumItems = ListView_GetSelectedCount(g_hListView);
    if(!iNumItems)
    {
        return;
    }

    //
    // Generate the confirmation message.
    //
    if(iNumItems == 1)
    {
        strcpy(pcMsg, "Are you sure you want to delete this file?");
    }
    else
    {
        wsprintf(pcMsg, "Are you sure you want to delete these %d files?",
                 iNumItems);
    }

    //
    // Display a message box and make sure that the user really wants to delete
    // these files.
    //
    if(MessageBox(hWnd, pcMsg, "Confirm File Delete",
                  MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2) != IDYES)
    {
        return;
    }

    //
    // Loop through all the items to find the ones which are selected.
    //
    iIdx = 0;
    while(iNumItems)
    {
        //
        // Fill in the LVITEM structure for this item.
        //
        item.mask = LVIF_STATE | LVIF_TEXT;
        item.iItem = iIdx++;
        item.iSubItem = 0;
        item.stateMask = LVIS_SELECTED;
        item.pszText = pcMsg;
        item.cchTextMax = 256;

        //
        // Get the item information.
        //
        ListView_GetItem(g_hListView, &item);

        //
        // Is this item selected?
        //
        if(item.state & LVIS_SELECTED)
        {
            //
            // Convert the file name from ASCII to Unicode.
            //
            MultiByteToWideChar(CP_ACP, 0, pcMsg, strlen(pcMsg) + 1,
                                pusFileName, 128);

            //
            // This item is selected, so delete it from the device.
            //
            Maverick_Delete(g_ulDriveNum, pusFileName);

            //
            // Remove any Audible meta-data associated with this file.
            //
            RemoveAudibleMetaData(g_ulDriveNum, pusFileName);

            //
            // Decrement the count of selected items.
            //
            iNumItems--;
        }
    }

    //
    // Re-load the list of files on the device.
    //
    LoadFileList();
}

//****************************************************************************
//
// Changes the view in the list view control.
//
//****************************************************************************
void
SetView(HWND hWnd, DWORD dwStyle, UINT uiMenuId)
{
    DWORD dwCurStyle;
    HMENU hMenu;

    //
    // Get the current window style.
    //
    dwCurStyle = GetWindowLong(g_hListView, GWL_STYLE);

    //
    // If the current window style is not the large icon style, then
    // we need to change the window style.
    //
    if((dwCurStyle & LVS_TYPEMASK) != dwStyle)
    {
        //
        // Change the window style to the large icon style.
        //
        SetWindowLong(g_hListView, GWL_STYLE,
                      (dwCurStyle & ~LVS_TYPEMASK) | dwStyle);

        //
        // Get the handle of the "view" menu.
        //
        hMenu = GetSubMenu(GetMenu(hWnd), 2);

        //
        // Set the check mark for the new view item and clear the check marks
        // for the other items.
        //
        CheckMenuRadioItem(hMenu, ID_VIEW_LARGEICONS, ID_VIEW_DETAILS,
                           uiMenuId, MF_BYCOMMAND);
    }
}

//****************************************************************************
//
// Changes the current drive.
//
//****************************************************************************
void
SetDrive(HWND hWnd, unsigned long ulDrive, UINT uiMenuId)
{
    HMENU hMenu;

    //
    // If we are already on the selected drive then there is nothing to do.
    //
    if(g_ulDriveNum == ulDrive)
    {
        return;
    }

    //
    // Change the drive number to the given drive.
    //
    g_ulDriveNum = ulDrive;

    //
    // Get the handle of the "drive" menu.
    //
    hMenu = GetSubMenu(GetMenu(hWnd), 3);

    //
    // Set the check mark for the "internal" item and clear the check
    // marks for the other items.
    //
    CheckMenuRadioItem(hMenu, ID_DRIVE_INTERNAL, ID_DRIVE_EXTERNAL,
                       uiMenuId, MF_BYCOMMAND);

    //
    // Re-load the file list.
    //
    LoadFileList();
}

//****************************************************************************
//
// Formats the file system on the device.
//
//****************************************************************************
void
Format(HWND hWnd)
{
    //
    // Make sure that the user really wants to format the device.
    //
    if(MessageBox(hWnd,
                  "Formatting the device will lose all the files stored on "
                  "the device.\nAre you sure you want to proceed?",
                  "Warning!",
                  MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2) == IDYES)
    {
        //
        // Format the device.
        //
        Maverick_Format(g_ulDriveNum);

        //
        // Re-load the list of files on the device.
        //
        LoadFileList();
    }
}

//****************************************************************************
//
// Updates the software on the internet audio player.
//
//****************************************************************************
BOOL
UpdateSoftware(HWND hWnd)
{
    OPENFILENAME ofn;
    char pcBuffer[MAX_PATH];

    //
    // Fill in the open file dialog box structure.
    //
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.hInstance = g_hInstance;
    ofn.lpstrFilter = "Software Package (.pkg)\0*.pkg\0";
    ofn.lpstrCustomFilter = NULL;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = pcBuffer;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFileTitle = NULL;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = NULL;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_EXPLORER;
    ofn.lpTemplateName = NULL;

    //
    // The initial file name is empty.
    //
    pcBuffer[0] = 0;

    //
    // Display the open file dialog box.
    //
    if(GetOpenFileName(&ofn) != 0)
    {
        //
        // Update the player software with the given image.
        //
        return(Update(hWnd, pcBuffer));
    }

    //
    // The user cancelled the open file dialog box, so return a failure.
    //
    return(FALSE);
}

//****************************************************************************
//
// The message handler for our about dialog box.
//
//****************************************************************************
BOOL APIENTRY
AboutDlgProc(HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
    //
    // Determine what to do based on the message we received.
    //
    switch(message)
    {
        //
        // The dialog is being initialized.
        //
        case WM_INITDIALOG:
        {
            //
            // Indicate that the first control in the dialog box should receive
            // the keyboard focus.
            //
            return(TRUE);
        }

        //
        // A command button or menu item was selected.
        //
        case WM_COMMAND:
        {
            //
            // See if the "OK" button was pressed, or escape was pressed.
            //
            if((LOWORD(wParam) == IDOK) || (LOWORD(wParam) == IDCANCEL))
            {
                //
                // We are done with this dialog box, so dismiss it.
                //
                EndDialog(hDlg, TRUE);

                //
                // Indicate that we've handled this message.
                //
                return(TRUE);
            }

            //
            // We're done handling this message.
            //
            break;
        }
    }

    //
    // Indicate that we did not handle the message.
    //
    return(FALSE);
}

//****************************************************************************
//
// The message handler for our status dialog box.
//
//****************************************************************************
BOOL APIENTRY
StatusDlgProc(HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
    //
    // Determine what to do based on the message we received.
    //
    switch(message)
    {
        //
        // The dialog is being initialized.
        //
        case WM_INITDIALOG:
        {
            unsigned long ulNumFiles, ulUsedSpace, ulFreeSpace, ulTotalSpace;

            //
            // Get the free and total space on the device.
            //
            ulFreeSpace = Maverick_FreeSpace(g_ulDriveNum);
            ulTotalSpace = Maverick_TotalSpace(g_ulDriveNum);

            //
            // Compute the used space.
            //
            ulUsedSpace = ulTotalSpace - ulFreeSpace;

            //
            // Get the number of files on the device.
            //
            ulNumFiles = ListView_GetItemCount(g_hListView);

            //
            // Convert the above values to strings and place them into the
            // appropriate static text controls in the dialog.
            //
            SetDlgItemText(hDlg, IDC_NUMFILES, GetValueString(ulNumFiles));
            SetDlgItemText(hDlg, IDC_BYTESAVAILABLE,
                           GetValueString(ulFreeSpace));
            SetDlgItemText(hDlg, IDC_BYTESUSED, GetValueString(ulUsedSpace));
            SetDlgItemText(hDlg, IDC_TOTALBYTES, GetValueString(ulTotalSpace));

            //
            // Indicate that the first control in the dialog box should receive
            // the keyboard focus.
            //
            return(TRUE);
        }

        //
        // A command button or menu item was selected.
        //
        case WM_COMMAND:
        {
            //
            // See if the "OK" button was pressed, or escape was pressed.
            //
            if((LOWORD(wParam) == IDOK) || (LOWORD(wParam) == IDCANCEL))
            {
                //
                // We are done with this dialog box, so dismiss it.
                //
                EndDialog(hDlg, TRUE);

                //
                // Indicate that we've handled this message.
                //
                return(TRUE);
            }

            //
            // We're done handling this message.
            //
            break;
        }
    }

    //
    // Indicate that we did not handle the message.
    //
    return(FALSE);
}

//****************************************************************************
//
// Handles the WM_CREATE message.
//
//****************************************************************************
void
HandleCreate(HWND hWnd)
{
    HIMAGELIST hImageListLarge, hImageListSmall;
    RECT rcl;
    HICON hIcon;
    HMENU hMenu;
    LV_COLUMN lvc;
    unsigned short pusDeviceName[256];
    char pcBuffer[256];

    //
    // Make sure that the common control DLL is loaded.
    //
    InitCommonControls();

    //
    // Get the size and position of the parent window.
    //
    GetClientRect(hWnd, &rcl);

    //
    // Create the list view window that starts out in large icon view.
    //
    g_hListView = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, "",
                                 WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_ICON |
                                 LVS_AUTOARRANGE | LVS_NOSORTHEADER,
                                 0, 0, rcl.right - rcl.left,
                                 rcl.bottom - rcl.top, hWnd, (HMENU)0,
                                 g_hInstance, NULL);
    if(g_hListView == NULL)
    {
        return;
    }

    //
    // Enable the full row select extended list view style.
    //
    ListView_SetExtendedListViewStyleEx(g_hListView, LVS_EX_FULLROWSELECT,
                                        LVS_EX_FULLROWSELECT);

    //
    // Create the image lists for the list view window.
    //
    hImageListLarge = ImageList_Create(GetSystemMetrics(SM_CXICON),
                                       GetSystemMetrics(SM_CYICON),
                                       ILC_COLORDDB | ILC_MASK, 1, 0);
    hImageListSmall = ImageList_Create(GetSystemMetrics(SM_CXSMICON),
                                       GetSystemMetrics(SM_CYSMICON),
                                       ILC_COLORDDB | ILC_MASK, 1, 0);

    //
    // Load the music icon and add it to the image lists.
    //
    hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_MUSIC));
    ImageList_AddIcon(hImageListLarge, hIcon);
    ImageList_AddIcon(hImageListSmall, hIcon);

    //
    // Supply the image lists to the list view control.
    //
    ListView_SetImageList(g_hListView, hImageListLarge, LVSIL_NORMAL);
    ListView_SetImageList(g_hListView, hImageListSmall, LVSIL_SMALL);

    //
    // Add the two columns to the list view control.
    //
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT;
    lvc.cx = 200;
    lvc.pszText = "Name";
    lvc.iSubItem = 0;
    ListView_InsertColumn(g_hListView, 0, &lvc);
    lvc.cx = 88;
    lvc.pszText = "Size";
    lvc.iSubItem = 1;
    ListView_InsertColumn(g_hListView, 1, &lvc);
    lvc.cx = 88;
    lvc.pszText = "Date";
    lvc.iSubItem = 2;
    ListView_InsertColumn(g_hListView, 2, &lvc);

    //
    // Get the name of the internet audio player.
    //
    if(Maverick_GetDeviceName(pusDeviceName, 256) == 1)
    {
        //
        // Convert the device name from Unicode to ASCII.
        //
        WideCharToMultiByte(CP_ACP, 0, pusDeviceName,
                            wcslen(pusDeviceName) + 1, pcBuffer, 256, NULL,
                            NULL);

        //
        // Set the device name as the title of the main window.
        //
        SetWindowText(hWnd, pcBuffer);
    }

    //
    // Indicate that the main window can accept drag and drop files.
    //
    DragAcceptFiles(hWnd, TRUE);

    //
    // Get the handle of the "view" menu.
    //
    hMenu = GetSubMenu(GetMenu(hWnd), 2);

    //
    // Set the check mark for the "large icons" item and clear the check marks
    // for the other items.
    //
    CheckMenuRadioItem(hMenu, ID_VIEW_LARGEICONS, ID_VIEW_DETAILS,
                       ID_VIEW_LARGEICONS, MF_BYCOMMAND);

    //
    // See if we have one or two drives.
    //
    if(Maverick_NumDrives() == 1)
    {
        //
        // Get the handle of the main menu.
        //
        hMenu = GetMenu(hWnd);

        //
        // We have only one drive, so disable the drive menu.
        //
        EnableMenuItem(hMenu, 3, MF_BYPOSITION | MF_GRAYED);
    }
    else
    {
        //
        // Get the handle of the "drive" menu.
        //
        hMenu = GetSubMenu(GetMenu(hWnd), 3);

        //
        // We have two drives, so set the check mark for the "internal" item
        // and clear the check marks for the other items.
        //
        CheckMenuRadioItem(hMenu, ID_DRIVE_INTERNAL, ID_DRIVE_EXTERNAL,
                           ID_DRIVE_INTERNAL, MF_BYCOMMAND);
    }

    //
    // Load the file list.
    //
    LoadFileList();
}

//****************************************************************************
//
// Handles the WM_INITMENUPOPUP message.
//
//****************************************************************************
void
HandleInitMenuPopup(HMENU hMenu)
{
    int iMenuItems, iPos;
    UINT uiId;

    //
    // Get the number of items in this menu.
    //
    iMenuItems = GetMenuItemCount(hMenu);

    //
    // Loop through all the items in this menu.
    //
    for(iPos = 0; iPos < iMenuItems; iPos++)
    {
        //
        // Get the control ID of this menu item.
        //
        uiId = GetMenuItemID(hMenu, iPos);

        //
        // Determine what to do based on the control ID of this menu item.
        //
        switch(uiId)
        {
            //
            // This is the "paste" item of the "edit" menu.
            //
            case ID_EDIT_PASTE:
            {
                //
                // We only support "paste" if the CF_HDROP format is on the
                // clipboard.
                //
                EnableMenuItem(hMenu, uiId,
                               IsClipboardFormatAvailable(CF_HDROP) ?
                               MF_ENABLED : MF_GRAYED);

                //
                // We're done handling this menu item.
                //
                break;
            }

            //
            // This is the "save as..." item of the "file" menu, or the
            // "delete" item of the "edit" menu.
            //
            case ID_FILE_SAVEAS:
            case ID_EDIT_DELETE:
            {
                //
                // We only support "save as..." and "delete" if there is at
                // least one selected item in the list view control.
                //
                EnableMenuItem(hMenu, uiId,
                               ListView_GetSelectedCount(g_hListView) == 0 ?
                               MF_GRAYED : MF_ENABLED);

                //
                // We're done handling this menu item.
                //
                break;
            }

            //
            // This is the "select all" item of the "edit" menu.
            //
            case ID_EDIT_SELECTALL:
            {
                //
                // We only support "select all" if there is at least one item
                // in the list view control.
                //
                EnableMenuItem(hMenu, uiId,
                               ListView_GetItemCount(g_hListView) == 0 ?
                               MF_GRAYED : MF_ENABLED);

                //
                // We're done handling this menu item.
                //
                break;
            }
        }
    }
}

//****************************************************************************
//
// Handles the WM_DROPFILES message.
//
//****************************************************************************
void
HandleDropFiles(HWND hWnd, HDROP hDropInfo)
{
    int iFiles, iIdx;
    char pcBuffer[256];

    //
    // Get the number of files dropped on our window.
    //
    iFiles = DragQueryFile(hDropInfo, -1, NULL, 0);

    //
    // Loop through all the files dropped on our window.
    //
    for(iIdx = 0; iIdx < iFiles; iIdx++)
    {
        //
        // Get the name of this file.
        //
        DragQueryFile(hDropInfo, iIdx, pcBuffer, 256);

        //
        // Download this file to the player.
        //
        DoDownload(hWnd, pcBuffer);
    }

    //
    // Indicate that we are done with the drag and drop operation.
    //
    DragFinish(hDropInfo);

    //
    // Reload the list of files on the player.
    //
    LoadFileList();
}

//****************************************************************************
//
// Handles the WM_COMMAND message.
//
//****************************************************************************
BOOL
HandleCommand(HWND hWnd, UINT wParam)
{
    //
    // Determine which menu item was selected.
    //
    switch(LOWORD(wParam))
    {
        //
        // The "open" item was selected from the "file" menu.
        //
        case ID_FILE_OPEN:
        {
            //
            // Open and download files.
            //
            OpenFiles(hWnd);

            //
            // We're done handling this menu item.
            //
            break;
        }

        //
        // The "open Audible" item was selected from the "file" menu.
        //
        case ID_FILE_OPEN_AUDIBLE:
        {
            //
            // Open and download an "Audible" file.
            //
            OpenAudibleFile(hWnd);

            //
            // We're done handling this menu item.
            //
            break;
        }

        //
        // The "save as" item was selected from the "file" menu.
        //
        case ID_FILE_SAVEAS:
        {
            //
            // Upload and save files.
            //
            SaveFiles(hWnd);

            //
            // We're done handling this menu item.
            //
            break;
        }

        //
        // The "exit" item was selected from the "file" menu.
        //
        case ID_FILE_EXIT:
        {
            //
            // Destroy our window.
            //
            DestroyWindow(hWnd);

            //
            // We're done handling this menu item.
            //
            break;
        }

        //
        // The "paste" item was selected from the "edit" menu.
        //
        case ID_EDIT_PASTE:
        {
            //
            // Paste the files from the clipboard.
            //
            Paste(hWnd);

            //
            // We're done handling this menu item.
            //
            break;
        }

        //
        // The "delete" item was selected from the "edit" menu.
        //
        case ID_EDIT_DELETE:
        {
            //
            // Delete the currently selected files.
            //
            DeleteFiles(hWnd);

            //
            // We're done handling this menu item.
            //
            break;
        }

        //
        // The "select all" item was selected from the "edit" menu.
        //
        case ID_EDIT_SELECTALL:
        {
            //
            // Select all the items in the list view control.
            //
            ListView_SetItemState(g_hListView, -1, LVIS_SELECTED,
                                  LVIS_SELECTED);

            //
            // We're done handling this menu item.
            //
            break;
        }

        //
        // The "large icons" item was selected from the "view" menu.
        //
        case ID_VIEW_LARGEICONS:
        {
            //
            // Change the list view control to display large icons.
            //
            SetView(hWnd, LVS_ICON, ID_VIEW_LARGEICONS);

            //
            // We're done handling this menu item.
            //
            break;
        }

        //
        // The "small icons" item was selected from the "view" menu.
        //
        case ID_VIEW_SMALLICONS:
        {
            //
            // Change the list view control to display small icons.
            //
            SetView(hWnd, LVS_SMALLICON, ID_VIEW_SMALLICONS);

            //
            // We're done handling this menu item.
            //
            break;
        }

        //
        // The "list" item was selected from the "view" menu.
        //
        case ID_VIEW_LIST:
        {
            //
            // Change the list view control to display the item list.
            //
            SetView(hWnd, LVS_LIST, ID_VIEW_LIST);

            //
            // We're done handling this menu item.
            //
            break;
        }

        //
        // The "details" item was selected from the "view" menu.
        //
        case ID_VIEW_DETAILS:
        {
            //
            // Change the list view control to display the detail view.
            //
            SetView(hWnd, LVS_REPORT, ID_VIEW_DETAILS);

            //
            // We're done handling this menu item.
            //
            break;
        }

        //
        // The "refresh" item was selected from the "view" menu.
        //
        case ID_VIEW_REFRESH:
        {
            //
            // Re-load the file list.
            //
            LoadFileList();

            //
            // We're done handling this menu item.
            //
            break;
        }

        //
        // The "internal" item was selected from the "drive" menu.
        //
        case ID_DRIVE_INTERNAL:
        {
            //
            // Change to the first drive.
            //
            SetDrive(hWnd, 0, ID_DRIVE_INTERNAL);

            //
            // We're done handling this menu item.
            //
            break;
        }

        //
        // The "external" item was selected from the "drive" menu.
        //
        case ID_DRIVE_EXTERNAL:
        {
            //
            // Change to the second drive.
            //
            SetDrive(hWnd, 1, ID_DRIVE_EXTERNAL);

            //
            // We're done handling this menu item.
            //
            break;
        }

        //
        // The "status" item was selected from the "tools" menu.
        //
        case ID_TOOLS_STATUS:
        {
            //
            // Display status information about the internet audio player.
            //
            DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_STATUS), hWnd,
                      StatusDlgProc);

            //
            // We're done handling this menu item.
            //
            break;
        }

        //
        // The "format" item was selected from the "tools" menu.
        //
        case ID_TOOLS_FORMAT:
        {
            //
            // Format the device.
            //
            Format(hWnd);

            //
            // We're done handling this menu item.
            //
            break;
        }

        //
        // The "update" item was selected from the "tools" menu.
        //
        case ID_TOOLS_UPDATE:
        {
            //
            // Update the device software.
            //
            UpdateSoftware(hWnd);

            //
            // Re-load the list of files on the device.
            //
            LoadFileList();

            //
            // We're done handling this menu item.
            //
            break;
        }

        //
        // The "about" item was selected from the "help" menu.
        //
        case ID_HELP_ABOUT:
        {
            //
            // Display the "About" dialog box.
            //
            DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_ABOUT), hWnd,
                      AboutDlgProc);

            //
            // We're done handling this menu item.
            //
            break;
        }

        //
        // All other commands.
        //
        default:
        {
            //
            // Indicate that we did not handle the command.
            //
            return(FALSE);
        }
    }

    //
    // Indicate that we did handle the command.
    //
    return(TRUE);
}

//****************************************************************************
//
// The message handler for our window.
//
//****************************************************************************
LONG APIENTRY
MainWndProc(HWND hWnd, UINT message, UINT wParam, LONG lParam)
{
    //
    // Determine what to do based on the message.
    //
    switch(message)
    {
        //
        // WM_CREATE is sent when the window is first created.
        //
        case WM_CREATE:
        {
            //
            // Handle the WM_CREATE message.
            //
            HandleCreate(hWnd);

            //
            // We're done with this message.
            //
            break;
        }

        //
        // WM_INITMENUPOPUP is sent when the popup menu is initialized.
        //
        case WM_INITMENUPOPUP:
        {
            //
            // Handle the WM_INITMENUPOPUP message.
            //
            HandleInitMenuPopup((HMENU)wParam);

            //
            // We're done with this message.
            //
            break;
        }

        //
        // WM_DROPFILES is sent when files are dropped on the window.
        //
        case WM_DROPFILES:
        {
            //
            // Handle the WM_DROPFILES message.
            //
            HandleDropFiles(hWnd, (HDROP)wParam);

            //
            // We're done with this message.
            //
            break;
        }

        //
        // WM_COMMAND is sent when a menu item is selected.
        //
        case WM_COMMAND:
        {
            //
            // Handle the WM_COMMAND message.
            //
            if(!HandleCommand(hWnd, wParam))
            {
                //
                // Send the message to the default window procedure.
                //
                return(DefWindowProc(hWnd, message, wParam, lParam));
            }

            //
            // We're done with this message.
            //
            break;
        }

        //
        // WM_SETFOCUS is sent when the window has gained the keyboard focus.
        //
        case WM_SETFOCUS:
        {
            //
            // Set the keyboard focus to our list view control.
            //
            SetFocus(g_hListView);

            //
            // We're done with this message.
            //
            break;
        }

        //
        // WM_SIZE is sent when the window is being resized.
        //
        case WM_SIZE:
        {
            //
            // Resize the list view window so that it takes the entire client
            // area of our window.
            //
            MoveWindow(g_hListView, 0, 0, LOWORD(lParam), HIWORD(lParam),
                       TRUE);

            //
            // We're done with this message.
            //
            break;
        }

        //
        // WM_SYSCOLORCHANGE is sent when the system GUI colors have been
        // changed.
        //
        case WM_SYSCOLORCHANGE:
        {
            //
            // Send this message to the list view control.
            //
            SendMessage(g_hListView, WM_SYSCOLORCHANGE, wParam, lParam);

            //
            // We're done with this message.
            //
            break;
        }

        //
        // WM_FONTCHANGE is sent when the system GUI fonts have been changed.
        //
        case WM_FONTCHANGE:
        {
            //
            // Send this message to the list view control.
            //
            SendMessage(g_hListView, WM_FONTCHANGE, wParam, lParam);

            //
            // We're done with this message.
            //
            break;
        }

        //
        // WM_SETTINGCHANGE is sent when a system setting has changed.
        //
        case WM_SETTINGCHANGE:
        {
            //
            // Send this message to the list view control.
            //
            SendMessage(g_hListView, WM_SETTINGCHANGE, wParam, lParam);

            //
            // We're done with this message.
            //
            break;
        }

        //
        // WM_DESTROY is sent when the window is being destroyed.
        //
        case WM_DESTROY:
        {
            //
            // Post a quit message so that the main message loop will
            // terminate.
            //
            PostQuitMessage(0);

            //
            // We're done handling this message.
            //
            break;
        }

        //
        // Send all other messages to the default window procedure.
        //
        default:
        {
            return(DefWindowProc(hWnd, message, wParam, lParam));
        }
    }

    //
    // Return zero to indicate that we've handled the message.
    //
    return(0);
}

//****************************************************************************
//
// WinMain is the main entry point for a Windows application.
//
//****************************************************************************
int APIENTRY
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
        int nCmdShow)
{
    HACCEL hAccel;
    WNDCLASSEX wc;
    HWND hWnd;
    MSG msg;

    //
    // Save our instance handle in a global.
    //
    g_hInstance = hInstance;

    //
    // Setup the structure which describes our window class.
    //
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = (WNDPROC)MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
    wc.lpszClassName = "MavusbWindowClass";
    wc.hIconSm = NULL;

    //
    // Register our window class.
    //
    if(RegisterClassEx(&wc) == 0)
    {
        char cMsg[256];
        wsprintf(cMsg, "RegisterClass failed with 0x%08x", GetLastError());
        MessageBox(0, cMsg, "Error!", MB_OK);
        return(FALSE);
    }

    //
    // See if we can find an instance of the internet audio player.
    //
    if(Maverick_OpenDevice() == 0)
    {
        MessageBox(0, "Unable to find the internet audio player.",
                   "Maverick(tm) Internet Audio Player", MB_OK);
        return(FALSE);
    }

    //
    // Check and possibly update the time on the player.
    //
    CheckTime();

    //
    // See if the software needs to be updated (which is the case if there are
    // no drives in the system).
    //
    if(Maverick_NumDrives() == 0)
    {
        //
        // Inform the user that they need to update the player software.
        //
        MessageBox(0, "The player software needs to be updated!",
                   "Maverick(tm) Internet Audio Player", MB_OK);

        //
        // Update the player software.
        //
        if(UpdateSoftware(0) == FALSE)
        {
            //
            // Tell the user that we are going to exit.
            //
            MessageBox(0, "The player software was not updated...exiting.",
                       "Maverick(tm) Internet Audio Player", MB_OK);

            //
            // Close the internet audio player.
            //
            Maverick_CloseDevice();

            //
            // Exit.
            //
            return(FALSE);
        }
    }

    //
    // Create our main window.
    //
    hWnd = CreateWindow("MavusbWindowClass",
                        "Maverick(tm) Internet Audio Player",
                        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                        400, 240, NULL, NULL, hInstance, NULL);
    if(!hWnd)
    {
        Maverick_CloseDevice();
        return(FALSE);
    }

    //
    // Show the window.
    //
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    //
    // Load our accelerators.
    //
    hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));

    //
    // Standard Windows message dispatch loop.
    //
    while(GetMessage(&msg, NULL, 0, 0))
    {
        if(TranslateAccelerator(hWnd, hAccel, &msg) == 0)
        {
            TranslateMessage(&msg);
        }
        DispatchMessage(&msg);
    }

    //
    // Close the internet audio player.
    //
    Maverick_CloseDevice();

    //
    // Return the result from the WM_QUIT message.
    //
    return(msg.wParam);
}
