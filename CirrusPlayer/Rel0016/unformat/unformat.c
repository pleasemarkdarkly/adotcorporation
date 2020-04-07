//****************************************************************************
//
// UNFORMAT.C - Erases the contents of the NAND FLASH on the EP7209 board.
//
// Copyright (c) 1999,2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************
#include <stdio.h>
#include "bootcode.h"
#ifdef _WIN32
#include <windows.h>
#endif
#ifdef __linux__
#include <sys/ioctl.h>
#include <sys/timeb.h>
#include <fcntl.h>
#include <termios.h>
#endif

//****************************************************************************
//
// Define a string which describes the board on which this unformat program
// will work.
//
//****************************************************************************
#ifdef EP7209_Eval_Board
#define BOARD "EP7209 Evaluation Board"
#endif
#ifdef EP7212_Reference_Board_Rev_A
#define BOARD "EP7212 Reference Design, Rev. A"
#endif
#ifdef EP7212_Reference_Board_Rev_B
#define BOARD "EP7212 Reference Design, Rev. B"
#endif
#ifdef EP7309_Reference_Board_Rev_B
#define BOARD "EP7309 Reference Design, Rev. B"
#endif
#ifdef EP7212_Reference_Board_Rev_C
#define BOARD "EP7212 Reference Design, Rev. C"
#endif
#ifdef EP7309_Reference_Board_Rev_C
#define BOARD "EP7309 Reference Design, Rev. C"
#endif
#ifdef EP73XX_Eval_Board
#define BOARD "EP73XX Evaluation Board"
#endif
#ifdef EP7312_Eval_Board
#define BOARD "EP7312 Evaluation Board"
#endif

//****************************************************************************
//
// lSerialPort is the serial port which is being used.
//
//****************************************************************************
#ifdef _WIN32
HANDLE hSerialPort;
#endif
#ifdef __linux__
long lSerialPort;
#endif

//****************************************************************************
//
// OpenPort opens the specified serial port.
//
//****************************************************************************
int
OpenPort(long lPort)
{
#ifdef _WIN32
    char pcName[16];

    //
    // Create the device name for the given serial port.
    //
    sprintf(pcName, "COM%d", lPort);

    //
    // Open the serial port.
    //
    hSerialPort = CreateFile(pcName, GENERIC_READ | GENERIC_WRITE, 0, 0,
                             OPEN_EXISTING, 0, 0);
    if(hSerialPort == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "Could not open serial port %s.\n", pcName);
        return(0);
    }

    //
    // Success.
    //
    return(1);
#elif defined(__linux__)
    struct termios buf;
    char pcName[16];

    //
    // Create the device name for the given serial port.
    //
    sprintf(pcName, "/dev/ttyS%d", lPort - 1);

    //
    // Open the serial port.
    //
    lSerialPort = open(pcName, O_RDWR | O_NONBLOCK);
    if(!lSerialPort)
    {
        fprintf(stderr, "Could not open serial port %s.\n", pcName);
        return(0);
    }

    //
    // Get the attributes of the serial port.  This will fail if the serial
    // port does not exist (even though the open will succeed if the serial
    // port does not exist).
    //
    if(tcgetattr(lSerialPort, &buf))
    {
        fprintf(stderr, "Could not open serial port %s.\n", pcName);
        return(0);
    }

    //
    // Success.
    //
    return(1);
#else
    //
    // There is no serial port support for the target environment, so return
    // an error.
    //
    fprintf(stderr, "No serial port support!\n");
    return(0);
#endif
}

//****************************************************************************
//
// SetBaud sets the baud rate and data format of the serial port.
//
//****************************************************************************
void
SetBaud(long lRate)
{
#ifdef _WIN32
    DCB dcb;

    //
    // Purge any pending characters in the serial port.
    //
    PurgeComm(hSerialPort, (PURGE_TXABORT | PURGE_RXABORT |
                            PURGE_TXCLEAR | PURGE_RXCLEAR));

    //
    // Fill in the device control block.
    //
    dcb.DCBlength = sizeof(DCB);
    dcb.BaudRate = lRate;
    dcb.fBinary = TRUE;
    dcb.fParity = FALSE;
    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fDsrSensitivity = FALSE;
    dcb.fTXContinueOnXoff = TRUE;
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;
    dcb.fErrorChar = FALSE;
    dcb.fNull = FALSE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    dcb.fAbortOnError = FALSE;
    dcb.XonLim = 0;
    dcb.XoffLim = 0;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.XonChar = 17;
    dcb.XoffChar = 19;
    dcb.ErrorChar = 0;
    dcb.EofChar = 0;
    dcb.EvtChar = 0;
    dcb.wReserved = 0;

    //
    // Set the new serial port configuration.
    //
    SetCommState(hSerialPort, &dcb);
#endif
#ifdef __linux__
    struct termios buf;
    int rate;

    //
    // Get the current settings for the serial port.
    //
    if(tcgetattr(lSerialPort, &buf))
    {
        return;
    }

    //
    // Reset to the serial port to raw mode.
    //
    buf.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP |
                     INLCR | IGNCR | ICRNL | IXON);
    buf.c_oflag &= ~OPOST;
    buf.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    buf.c_cflag &= ~(CSIZE | PARENB);
    buf.c_cflag |= CS8;

    //
    // Get the baud rate.
    //
    switch(lRate)
    {
        case 9600:
        {
            rate = B9600;
            break;
        }

        case 19200:
        {
            rate = B19200;
            break;
        }

        case 38400:
        {
            rate = B38400;
            break;
        }

        case 57600:
        {
            rate = B57600;
            break;
        }

        case 115200:
        {
            rate = B115200;
            break;
        }
    }

    //
    // Set the input and output baud rate of the serial port.
    //
    cfsetispeed(&buf, rate);
    cfsetospeed(&buf, rate);

    //
    // Set data bits to 8.
    //
    buf.c_cflag &= ~CSIZE;
    buf.c_cflag |= CS8;

    //
    // Set stop bits to one.
    //
    buf.c_cflag &= ~CSTOPB;

    //
    // Disable parity.
    //
    buf.c_cflag &= ~(PARENB | PARODD);

    //
    // Set the new settings for the serial port.
    //
    if(tcsetattr(lSerialPort, TCSADRAIN, &buf))
    {
	return;
    }

    //
    // Wait until the output buffer is empty.
    //
    tcdrain(lSerialPort);
#endif
}

//****************************************************************************
//
// SendChar sends a character to the serial port.
//
//****************************************************************************
void
SendChar(char cChar)
{
#ifdef _WIN32
    DWORD dwLen;

    //
    // Send this character to the serial port.
    //
    WriteFile(hSerialPort, &cChar, 1, &dwLen, NULL);
#endif
#ifdef __linux__
    //
    // Send this character to the serial port.
    //
    write(lSerialPort, &cChar, 1);
#endif
}

//****************************************************************************
//
// ReceiveChar reads a character from the serial port.
//
//****************************************************************************
char
ReceiveChar(long lTimeout)
{
#ifdef _WIN32
    COMMTIMEOUTS sTimeouts;
    char cChar;
    DWORD dwLen;

    //
    // Fill in the timeout structure based on the timeout requested for this
    // read.
    //
    sTimeouts.ReadIntervalTimeout = 0;
    sTimeouts.ReadTotalTimeoutMultiplier = 0;
    sTimeouts.ReadTotalTimeoutConstant = lTimeout;
    sTimeouts.WriteTotalTimeoutMultiplier = 0;
    sTimeouts.WriteTotalTimeoutConstant = 0;

    //
    // Set the timeout for this read.
    //
    SetCommTimeouts(hSerialPort, &sTimeouts);

    //
    // Read a character.
    //
    if(!ReadFile(hSerialPort, &cChar, 1, &dwLen, NULL))
    {
        //
        // The read failed, so set the read character to a NULL.
        //
        cChar = 0;
    }

    //
    // If we did not read a character, then set the character to NULL.
    //
    if(dwLen != 1)
    {
        cChar = 0;
    }

    //
    // Return the character we read.
    //
    return(cChar);
#endif
#ifdef __linux__
    struct timeb sTime;
    char cChar;
    int res;
    time_t tStart, tNow;

    //
    // Get the current time.
    //
    ftime(&sTime);
    tStart = (sTime.time * 1000) + sTime.millitm;

    //
    // Read the next character from the serial port.
    //
    while(1)
    {
        //
        // Try to read a character from the serial port.
        //
        res = read(lSerialPort, &cChar, 1);
        if(res == 1)
        {
            //
            // We read a character, so break out of the loop.
            //
            break;
        }

        //
        // Get the current time.
        //
        ftime(&sTime);
        tNow = (sTime.time * 1000) + sTime.millitm;

        //
        // See if the timeout has expired.
        //
        if(lTimeout && ((tStart + lTimeout) < tNow))
        {
            cChar = 0;
            break;
        }
    }

    //
    // Return the character read from the serial port.
    //
    return(cChar);
#endif
}

//****************************************************************************
//
// WaitTillEmpty waits until the serial port's output buffer is empty.
//
//****************************************************************************
void
WaitTillEmpty(void)
{
#ifdef _WIN32
    //
    // Wait for 10ms so the output buffer can drain.
    //
    Sleep(10);
#endif
#ifdef __linux__
    //
    // Wait until the output buffer is empty.
    //
    tcdrain(lSerialPort);
#endif
}

//****************************************************************************
//
// WaitFor waits until a specific character is read from the serial port.
//
//****************************************************************************
void
WaitFor(char cWaitChar)
{
    char cChar;

    //
    // Wait until we read a specific character from the serial port.
    //
    while(1)
    {
        //
        // Read a character.
        //
        cChar = ReceiveChar(0);

        //
        // Stop waiting if we received the character.
        //
        if(cChar == cWaitChar)
        {
            break;
        }
    }
}

//****************************************************************************
//
// Prints out a usage message for the program.
//
//****************************************************************************
void
Usage(void)
{
    fprintf(stderr, "Usage: unformat {-h} {-v} {-p<port>}\n\n");
    fprintf(stderr, "Unformats the NAND FLASH on a Cirrus Logic " BOARD
                    ".\n\n");
    fprintf(stderr, "  -p <port>             Use the specified serial port "
                    "(default is \"1\").  Valid\n");
#ifdef _WIN32
    fprintf(stderr, "                        values are 1 (COM1), 2 (COM2), 3 "
                    "(COM3), and 4 (COM4).\n\n");
#endif
#ifdef __linux__
    fprintf(stderr, "                        values are 1 (/dev/ttyS0), 2 "
                    "(/dev/ttyS1), 3\n");
    fprintf(stderr, "                        (/dev/ttyS2), and 4 "
                    "(/dev/ttyS3).\n\n");
#endif
    fprintf(stderr, "  -v                    Prints the version of this "
                    "program.\n\n");
    fprintf(stderr, "  -h                    Prints this usage message.\n");
}

//****************************************************************************
//
// This program waits for the '<' character from the boot ROM, sends the boot
// code, waits for the '>' from the boot ROM, waits for the '?' from the boot
// code, changes the serial port rate (preferably to 115200), downloads the
// user data file, and then prints out progress status as the boot code writes
// the user data file to the NOR FLASH.
//
//****************************************************************************
int
main(int argc, char *argv[])
{
    long lPort = 1, lFileSize, lIdx, lLoop, lSum;
    char cChar, cFirstChar, cRateChar, cBuffer[1024];
    int bError = 0;
    FILE *pFile;

    //
    // First, set stdout to be unbuffered, so that our status messages are
    // always displayed immediately.
    //
    setbuf(stdout, NULL);

    //
    // See if there are any flags specified on the command line.
    //
    while(1)
    {
        //
        // If we are out of arguments, or this argument does not start with a
        // '-', then stop looking at command line arguments.
        //
        if((argc == 1) || (argv[1][0] != '-'))
        {
            break;
        }

        //
        // See if this argument is "-p".
        //
        if(argv[1][1] == 'p')
        {
            //
            // Get the port number from the command line.
            //
            lPort = atoi(argv[1] + 2);

            //
            // Make sure that the specified port number is valid.
            //
            if((lPort != 1) && (lPort != 2) && (lPort != 3) && (lPort != 4))
            {
                //
                // Tell the user that the port number is invalid.
                //
                fprintf(stderr, "Invalid serial port '%s'.\n\n", argv[1] + 2);

                //
                // Print the usage message.
                //
                Usage();

                //
                // We're done.
                //
                return(1);
            }
        }

        //
        // See if this argument is "-h".
        //
        else if(argv[1][1] == 'h')
        {
            //
            // Print the usage message.
            //
            Usage();

            //
            // We're done.
            //
            return(1);
        }

        //
        // See if this argument is "-v".
        //
        else if(argv[1][1] == 'v')
        {
            //
            // Print the version of this program.
            //
            printf(BOARD " Unformat Version 2.00 for ");
#ifdef _WIN32
            printf("Win32.\n");
#endif
#ifdef __linux__
            printf("Linux.\n");
#endif
            printf("Copyright (c) 1999-2001 Cirrus Logic, Inc.\n\n");
            printf("Report bugs to <epdapps@crystal.cirrus.com>.\n");

            //
            // We're done.
            //
            return(0);
        }

        //
        // An unrecognized flag was specifed.
        //
        else
        {
            //
            // Tell the user that the specified flag is invalid.
            //
            fprintf(stderr, "Invalid flag '%s'.\n\n", argv[1]);

            //
            // Print the usage message.
            //
            Usage();

            //
            // We're done.
            //
            return(1);
        }

        //
        // Skip to the next argument on the command line.
        //
        argv++;
        argc--;
    }

    //
    // Make sure that there were no additional arguments specified.
    //
    if(argc != 1)
    {
        //
        // Print the usage message.
        //
        Usage();

        //
        // We're done.
        //
        return(1);
    }

    //
    // Open the serial port to be used.
    //
    if(OpenPort(lPort) != 1)
    {
        return(1);
    }

    //
    // Program a baud rate of 9600, 8 data bits, and no parity.
    //
    SetBaud(9600);

    //
    // Tell the user to reset the board.
    //
    printf("Waiting for the board to wakeup...");

    //
    // Wait until we read a '<' from the comm port.
    //
    WaitFor('<');

    //
    // Tell the user that we are downloading the boot code.
    //
    printf("\r                                  \r");
    printf("Downloading unformat code...(  0%%)");

    //
    // Write the boot code to the comm port.
    //
    for(lIdx = 0; lIdx < 2048; lIdx++)
    {
        //
        // Write this character.
        //
        SendChar(pcBoot[lIdx]);

        //
        // Periodically print out our progress.
        //
        if((lIdx & 127) == 127)
        {
            //
            // Wait until the transmit buffer is empty.
            //
            WaitTillEmpty();

            //
            // Print the updated status.
            //
            printf("\b\b\b\b\b%3d%%)", ((lIdx + 1) * 100) / 2048);
        }
    }

    //
    // Wait until we read a '>' from the comm port.
    //
    WaitFor('>');

    //
    // Indicate that the unformat is proceeding.
    //
    printf("\r                                  \r");
    printf("Unformatting...");

    //
    // Wait until we read a '!' from the comm port.
    //
    WaitFor('!');

    //
    // Indicate that the unformat succeeded.
    //
    printf("\r               \r");
    printf("Successfully unformatted the NAND FLASH.\n");

    //
    // Success.
    //
    return(0);
}
