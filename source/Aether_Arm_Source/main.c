/* THIS FILE CONTAINS THE MAIN FUNCTION ALONG WITH OTHER FUNCTIONS, FUNCTION DECLARATION AND CALLS TO OTHER FUNCTIONS AND IS THE PRIMARY 
COORDINATOR OF ALL SYSTEM ACTIONS. THIS IS EASY TO FOLLOW BY REFERRING TO PROJECT AETHER'S DETAILED DESIGN DOC*/
void delay(unsigned long ulSeconds);
void UARTSendCmd(char *cmdBuffer);	
void UARTSendCmd2(char *cmd, char *d1, char *d2);
void UARTSendCmdPIC(char c, char *d1);
void UARTIntHandlerIMU(void);
void UARTIntHandlerUSB(void);
void enableInterrupts(void);
void disableInterrupts(void);
void initializeAether(void);
int find_substr(char *listPointer, char *itemPointer);
void call_sd(char * data);
void sd(void);
int convert_time(char *);
void detachModule(void);
void formatDateTime(void);
int parseSensorData(void);

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/pwm.h"
#include "drivers/rit128x96x4.h"
#include "utils/uartstdio.h"
#include "third_party/fatfs/src/diskio.h"
#include "third_party/fatfs/src/ff.h"
#include "string.h"
#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "aether_globals.h"	  
#include "communication.c"
#include "event_control.c"
#include "data_capture.c" 
#include "output.c"

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void __error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

void print_struct_data()
{
	char row[200] = {0};

	sprintf(row, "Sat:%s; Lon:%s;                    ", global_data.satellites, global_data.longitude);
	RIT128x96x4StringDraw(row, 0,  16, 15);	

	sprintf(row, "Lat:%s; D:%s;                    ", global_data.latitude, global_data.date);
	RIT128x96x4StringDraw(row, 0,  24, 15);

	sprintf(row, "A:%s; H:%s;                    ", global_data.altitude, global_data.humidity);
	RIT128x96x4StringDraw(row, 0,  32, 15); 

	sprintf(row, "Tmp:%s; Prs:%s;                    ", global_data.temperature, global_data.pressure);
	RIT128x96x4StringDraw(row, 0,  40, 15);    

	sprintf(row, "Time:%s; M:%d;                    ", global_data.time, global_data.mode);
	RIT128x96x4StringDraw(row, 0,  48, 15);    

	sprintf(row, "<T:%.2f; >T:%.2f;                    ", config.minimum_temperature, config.maximum_temperature);
	RIT128x96x4StringDraw(row, 0,  56, 15);  

	sprintf(row, "<H:%.2f; >H:%.2f;                    ", config.minimum_humidity, config.maximum_humidity);
	RIT128x96x4StringDraw(row, 0,  64, 15); 

	sprintf(row, "<P:%.2f; >P:%.2f;                    ", config.minimum_pressure, config.maximum_pressure); 
	RIT128x96x4StringDraw(row, 0,  72, 15); 
	 
	sprintf(row, "%d:%s;                    ", current_txt_msg_index, global_data.incoming_text_message);	
	RIT128x96x4StringDraw(row, 0,  80, 15);

	sprintf(row, "%s;                    ", global_data.incoming_phone_number);
	RIT128x96x4StringDraw(row, 0,  88, 15);
}


void fake_data()
{
	int i;
	char longitude[][MAX_LONG_LEN] = {"-97.113948W", "-97.114127W", "-97.114291W", "-97.114546W", "-97.114812W", "-97.115012W"};
	char latitude[][MAX_LAT_LEN] = {"32.731451N", "32.732014N", "32.732489N", "32.732746N", "32.732981N", "32.733231N"};
	char altitude[][MAX_ALT_LEN] = {"233.455", "25.22", "79.88", "253.11", "35.44", "412.55"};

	if(fake_data_loop > 5)	fake_data_loop = 0;
	i = fake_data_loop;
	fake_data_loop++;

	strcpy(global_data.longitude, longitude[i]); 
	strcpy(global_data.latitude, latitude[i]);
	strcpy(global_data.altitude, altitude[i]);

}

int convert_time( String time )
{
	int hri;
	int mini;
	int seci;
	int total_secs;
    char hr [3] = {0};
    char min [3] = {0};
    char sec [3] = {0};
    strncpy( hr, time, 2 );
    hri = atoi(hr);
    time = time+2;
    strncpy( min, time, 2);
    mini = atoi(min);
    time = time+2;
    strncpy( sec, time, 2);
    seci = atoi(sec);
   
    total_secs = (hri*60*60)+(mini*60)+seci;
	
	return total_secs; 
	
}

int main(void)
{
	long int loop = 0;	
	char num[10];

    // Set the clocking to run directly from the crystal.
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);

    // Initialize the OLED display and write status.
    //RIT128x96x4Init(1000000);
	//RIT128x96x4StringDraw("Team Aerotag", 0,  0, 15);	
    //RIT128x96x4StringDraw("Project Aether", 0, 8, 15);
	//RIT128x96x4Disable(); // Disable OLED Display to write to microSDcard

    // Enable the peripherals used by this program.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

	// Enable processor interrupts.
    IntMasterEnable();	 
    // Set GPIO {A0,A1} and {D2,D3} as UART pins.
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3);
	
    // Configure the UARTs for 115,200, 8-N-1 operation.
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));


   initializeAether();

    while(1)
    {
		dataCapture(); // data_capture.c
		if(loop==0) { initial_time = convert_time(global_data.time); }
		if(atoi(global_data.satellites)==0) { fake_data(); }
		getTextMsg(); // communication.c
		sensorDataCapture(); // data_capture.c
		formatDateTime();
		//print_struct_data();	
		event_control();   // event_control.c				
		logData(); // output.c	  
		
		if(detachFlag == 1 )	//Check Detachment		
		{
			 detachModule(); //main.c
		}

		sprintf(num, "%d", loop++);
		//RIT128x96x4StringDraw(num, 85,  0, 15);
	}
}


void detachModule()
{

	//PIC DETACH COMMAND
	delay(987654);
	enableInterrupts();
   	UARTCharPut(UART0_BASE, 'D');
	delay(987654);				   
	delay(987654);			   
	disableInterrupts();  

	detachFlag = 0; //Detachment Complete

}

//*****************************************************************************
// Interrupt Handler for UART 0 - PIC 
//*****************************************************************************
void UARTIntHandlerUSB(void) 
{ 
  unsigned long u1Status;
   char inByte;

    // Get the interrrupt status.
    u1Status = UARTIntStatus(UART0_BASE, true);

    // Clear the asserted interrupt.
    UARTIntClear(UART0_BASE, u1Status);
   
    // Loop while there are characters in the receive FIFOs.
    while(UARTCharsAvail(UART0_BASE))
    {
      // store the byte in the buffer and increment the counter
      inByte = UARTCharGetNonBlocking(UART0_BASE);
      UARTCharPutNonBlocking(UART0_BASE, inByte);
      tempInputStr[count++] = inByte;		
      tempInputStr[count] = '\0';
   } 
 }

//*****************************************************************************
// Interrupt Handler for UART 1 - GM862
//*****************************************************************************
void UARTIntHandlerIMU(void)	  
{
    unsigned long u1Status;
	char inByte;


    // Get the interrrupt status.
    u1Status = UARTIntStatus(UART1_BASE, true);

	// Clear the asserted interrupt.
    UARTIntClear(UART1_BASE, u1Status);	
	
    // Loop while there are characters in the receive FIFOs.
    if(UARTCharsAvail(UART1_BASE))
    {
		// store the byte in the buffer and increment the counter
		inByte = UARTCharGetNonBlocking(UART1_BASE);
		//UARTCharPutNonBlocking(UART0_BASE, inByte);
		tempInputStr[count++] = inByte;			
		tempInputStr[count] = '\0';
	}	
}	

void UARTSendCmd(char *cmdBuffer)
{
	long len = strlen(cmdBuffer);

	// Loop while there are more characters to send.
    while(len--)
    {
		// Write the next character to the UART.
        UARTCharPut(UART1_BASE, *cmdBuffer++);
    }	
}
			
void initializeAether()
{
	char setBnd[] = "at#bnd=1\r";
	char setMsgFormat[] = "at+cmgf=1\r"; 
	char deleteMsg[20] = {0};
	short int i = 35;

	//UARTSendCmd2(setBnd, "\r\nOK\r\n", "ERROR");
	//UARTSendCmd2(setMsgFormat, "\r\nOK\r\n", "ERROR");
	UARTSendCmd(setBnd);
	delay(987654);
	UARTSendCmd(setMsgFormat);
	delay(987654);

	while(i--)
	{
		sprintf(deleteMsg, "at+cmgd=%d\r", i);
		//UARTSendCmd(deleteMsg);
		//delay(987654);
		UARTSendCmd2(deleteMsg, "\r\nOK\r\n", "+CMS ERROR: 321\r\n");
	}
	call_sd(",,,Aether GPS DATA,\n\nDate,Time,Longitude,Latitude,Altitude (ft),Temperature (F),Pressure (atm),Humidity (%),Messages,\n");
}

void enableInterrupts()
{
	completeInputStr[0] = '\0';
	tempInputStr[0] = '\0';	
	count=0;

    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
	IntEnable(INT_UART1);
    UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);
}

void disableInterrupts()
{
    IntDisable(INT_UART0);
    UARTIntDisable(UART0_BASE, UART_INT_RX | UART_INT_RT);
	IntDisable(INT_UART1);
    UARTIntDisable(UART1_BASE, UART_INT_RX | UART_INT_RT);

	strcpy(completeInputStr, tempInputStr);
	tempInputStr[count] = '\0';
}

void delay(unsigned long ulSeconds)
{

    while(ulSeconds--)
    {  /*
        // Wait until the SysTick value is less than 1000.
        while(SysTickValueGet() > 1000){}

        // Wait until the SysTick value is greater than 1000.
        while(SysTickValueGet() < 1000){} */
    }	
}

int find_substr(char *listPointer, char *itemPointer)
{
  int t;
  char *p, *p2;

  for(t=0; listPointer[t]; t++) {
    p = &listPointer[t];
    p2 = itemPointer;

    while(*p2 && *p2==*p) {
      p++;
      p2++;
    }
    if(!*p2) return t; /* 1st return */
  }
   return -1; /* 2nd return */
}

void UARTSendCmd2(char *cmd, char *d1, char *d2)
{
	int sent=0;
	long int timeout = 32000;

	delay(55555);
	enableInterrupts();
	while(find_substr(tempInputStr, d1)==-1 && find_substr(tempInputStr, d2)==-1 && timeout--)
	{
		if(sent==0)
		{
			UARTSendCmd(cmd);
			sent++;
		}
	} 	
	disableInterrupts();
 }

void UARTSendCmdPIC(char c, char *d1)
{
	int sent=0;
	long int timeout = 32000;

	delay(55555);
	enableInterrupts();
	while(find_substr(tempInputStr, d1)==-1 && timeout--)
	{
		if(sent==0)
		{
			UARTCharPut(UART0_BASE, c);
			sent++;
		}
	}
	disableInterrupts();
 }
 
 void formatDateTime()
 {
 	int month = 0;
	int hr = 0;
	char day[5] = {0};
	char year[5] = {0};
	char hour[5] = {0};	 
	char min[5] = {0};
	char sec[5] = {0};
	char ms[5] = {0};
	char months[][5] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

 	if(!strncmp(global_data.date+2, "01", 2))
		month = 1; 
	else if(!strncmp(global_data.date+2, "02", 2))
		month = 2; 
	else if(!strncmp(global_data.date+2, "03", 2))
		month = 3; 
	else if(!strncmp(global_data.date+2, "04", 2))
		month = 4; 
	else if(!strncmp(global_data.date+2, "05", 2))
		month = 5; 
	else if(!strncmp(global_data.date+2, "06", 2))
		month = 6; 
	else if(!strncmp(global_data.date+2, "07", 2))
		month = 7; 
	else if(!strncmp(global_data.date+2, "08", 2))
		month = 8; 
	else if(!strncmp(global_data.date+2, "09", 2))
		month = 9; 
	else if(!strncmp(global_data.date+2, "10", 2))
		month = 10; 
	else if(!strncmp(global_data.date+2, "11", 2))
		month = 11;  
	else if(!strncmp(global_data.date+2, "12", 2))
		month = 12;

	if(month != 0)
	{
		strncpy(day, global_data.date, 2);
		strncpy(year, global_data.date+4, 2);
		sprintf(global_data.date, "%s %s 20%s",  months[month-1], day, year);
	}
	else
		strcpy(global_data.date, "Jan 00 0000");


	strncpy(hour, global_data.time, 2);
	strncpy(min, global_data.time+2, 2); 
	strncpy(sec, global_data.time+4, 2); 
	strncpy(ms, global_data.time+7, 2);

	hr = atoi(hour);

	if(hr<13)
	{
		//sprintf(global_data.time, "%2d:%2s:%2s:%2s A.M.", hr, min, sec, ms);
		sprintf(global_data.time, "%2d:%2s:%2s A.M.", hr, min, sec);
	}
	else
	{
		hr -= 12;
		//sprintf(global_data.time, "%2d:%2s:%2s:%2s P.M.", hr, min, sec, ms);
		sprintf(global_data.time, "%2d:%2s:%2s P.M.", hr, min, sec);
	}

	
 } 
 			