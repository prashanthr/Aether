//*****************************************************************************
//
// sd_card.c - Example program for reading files from an SD card.
//
// Copyright (c) 2007-2010 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 6288 of the EK-LM3S8962 Firmware Package.
//
//*****************************************************************************

#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "utils/cmdline.h"
#include "utils/uartstdio.h"
#include "third_party/fatfs/src/ff.h"
#include "third_party/fatfs/src/diskio.h"	   



//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>SD card using FAT file system (sd_card)</h1>
//!
//! This example application demonstrates reading a file system from
//! an SD card.  It makes use of FatFs, a FAT file system driver.  It
//! provides a simple command console via a serial port for issuing commands
//! to view and navigate the file system on the SD card.
//!
//! The first UART, which is connected to the FTDI virtual serial port
//! on the Stellaris LM3S6965 Evaluation Board, is configured for 115,200
//! bits per second, and 8-n-1 mode.  When the program is started a message
//! will be printed to the terminal.  Type ``help'' for command help.
//!
//! For additional details about FatFs, see the following site:
//! http://elm-chan.org/fsw/ff/00index_e.html

//*****************************************************************************
//
// The following are data structures used by FatFs.
//
//*****************************************************************************
static FATFS g_sFatFs;
//static DIR g_sDirObject;
//static FILINFO g_sFileInfo;
static FIL g_sFileObject;

//*****************************************************************************
//
// A structure that holds a mapping between an FRESULT numerical code,
// and a string represenation.  FRESULT codes are returned from the FatFs
// FAT file system driver.
//
//*****************************************************************************
typedef struct
{
    FRESULT fresult;
    char *pcResultStr;
}
tFresultString;

//*****************************************************************************
//
// A macro to make it easy to add result codes to the table.
//
//*****************************************************************************
#define FRESULT_ENTRY(f)        { (f), (#f) }

//*****************************************************************************
//
// A table that holds a mapping between the numerical FRESULT code and
// it's name as a string.  This is used for looking up error codes for
// printing to the console.
//
//*****************************************************************************
tFresultString g_sFresultStrings[] =
{
    FRESULT_ENTRY(FR_OK),
    FRESULT_ENTRY(FR_NOT_READY),
    FRESULT_ENTRY(FR_NO_FILE),
    FRESULT_ENTRY(FR_NO_PATH),
    FRESULT_ENTRY(FR_INVALID_NAME),
    FRESULT_ENTRY(FR_INVALID_DRIVE),
    FRESULT_ENTRY(FR_DENIED),
    FRESULT_ENTRY(FR_EXIST),
    FRESULT_ENTRY(FR_RW_ERROR),
    FRESULT_ENTRY(FR_WRITE_PROTECTED),
    FRESULT_ENTRY(FR_NOT_ENABLED),
    FRESULT_ENTRY(FR_NO_FILESYSTEM),
    FRESULT_ENTRY(FR_INVALID_OBJECT),
    FRESULT_ENTRY(FR_MKFS_ABORTED)
};

//*****************************************************************************
//
// A macro that holds the number of result codes.
//
//*****************************************************************************
#define NUM_FRESULT_CODES (sizeof(g_sFresultStrings) / sizeof(tFresultString))

//*****************************************************************************
//
// This function returns a string representation of an error code
// that was returned from a function call to FatFs.  It can be used
// for printing human readable error messages.
//
//*****************************************************************************
const char *
StringFromFresult(FRESULT fresult)
{
    unsigned int uIdx;

    //
    // Enter a loop to search the error code table for a matching
    // error code.
    //
    for(uIdx = 0; uIdx < NUM_FRESULT_CODES; uIdx++)
    {
        //
        // If a match is found, then return the string name of the
        // error code.
        //
        if(g_sFresultStrings[uIdx].fresult == fresult)
        {
            return(g_sFresultStrings[uIdx].pcResultStr);
        }
    }

    //
    // At this point no matching code was found, so return a
    // string indicating unknown error.
    //
    return("UNKNOWN ERROR CODE");
}

//*****************************************************************************
//
// This is the handler for this SysTick interrupt.  FatFs requires a
// timer tick every 10 ms for internal timing purposes.
//
//*****************************************************************************
void
SysTickHandler(void)
{
    //
    // Call the FatFs tick timer.
    //
    disk_timerproc();
}

int writeSD(char *file, char *data)
{
	unsigned short usBytesWrite;

	if (f_open(&g_sFileObject, file, FA_CREATE_NEW | FA_WRITE)==FR_OK)
	{
		//
		// write file
		//
		if(f_write(&g_sFileObject,data,strlen(data),&usBytesWrite)==FR_OK)
		{
			//UARTStringPutNonBlocking("Write file success");
			f_sync(&g_sFileObject);
		}
		else
		{
			//UARTStringPutNonBlocking("Write into file Error");
		}
	}
	else 
	{
			// if the file is exist
		if (f_open(&g_sFileObject, file, FA_CREATE_NEW | FA_WRITE)==FR_EXIST)
		{

			if (f_open(&g_sFileObject, file, FA_OPEN_EXISTING | FA_WRITE)==FR_OK)
			{
				//
				// write file
				//
				if(f_lseek(&g_sFileObject, g_sFileObject.fsize)==FR_OK)
				{
					if(f_write(&g_sFileObject,data,strlen(data),&usBytesWrite)==FR_OK)
					{
						//UARTStringPutNonBlocking("File append success");
						f_close(&g_sFileObject);
					}
					else
					{
						//UARTStringPutNonBlocking("Write into file Error");
					}
				}
			}
			else
			{
				//UARTStringPutNonBlocking("Append File ceation Error, file not found");
			}

		}
		else
			{}//UARTStringPutNonBlocking("New File ceation Error");
	} 
	

	return(0);
} 

//*****************************************************************************
//
// The program main function.  It performs initialization, then calls the
// writeSD method to write data to the FAT FS microSD card.
//
//*****************************************************************************

void
call_sd(char * data)
{   
    FRESULT fresult;        

    fresult = f_mount(0, &g_sFatFs);
    if(fresult != FR_OK)
    {        
    }
	writeSD("data.csv", data);	
}
