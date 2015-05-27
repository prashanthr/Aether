/* THIS FILE CONTAINS ALL FUNCTIONS THAT HANDLE SYSTEM DATA CAPTURE AND ARE EASY TO FOLLOW BY REFERRING TO PROJECT AETHER'S DETAILED DESIGN DOC*/
//Validate input string
void validateGpsInput()
{
	int i = 0;
	int new_str_pos = 0;
	char gps_prefix[] = "$GPSACP: ";
	char invalid[]  = "n/a,n/a,n/a,n/a,n/a,n/a,n/a,n/a,n/a,n/a,n/a,";
	char new_str[MAX_GPS_LEN];

	//if(!strncmp(completeInputStr,gps_prefix,9))
	i = find_substr(completeInputStr, gps_prefix);
	if(i != -1)
	{
		for(i+=9; i<strlen(completeInputStr); i++)
		{
			if(completeInputStr[i]==',' && completeInputStr[i+1]==',')
			{
				strcpy(new_str+new_str_pos, ",n/a");
				new_str_pos += 4;
			}
			else if(completeInputStr[i]=='\r' || completeInputStr[i]=='\n')
			{
				strcpy(new_str+new_str_pos, ",\0");
				new_str_pos += 2;
			}
			else
			{
				new_str[new_str_pos] = completeInputStr[i];
				new_str_pos++;
			}
		}
	}
	else
		strncpy(new_str, invalid, strlen(invalid));

	strncpy(completeInputStr, new_str, strlen(new_str));
	completeInputStr[strlen(new_str)] = '\0';

	return;
}

//Parse the GPS data into separate strings
int parseGpsData()
{
	char cp[MAX_GPS_LEN] = {0};
	char * tempo = NULL;
	char delimiters[] = ","; // delimit commas
	int i;
	int numSegments = 12;

	validateGpsInput();
 	strcpy(cp, completeInputStr);
	
	for(i=0; i<numSegments; i++)
    {		
		if(i==0)					
        	tempo = strtok(cp, delimiters); // delimite the first part
      	else
			tempo = strtok(NULL, delimiters);
	
		if(i==0) // time
		{
			strcpy(global_data.time, tempo);
		}
		
		if(i==1) // latitude
		{
			strcpy(global_data.latitude, tempo);
		}
		
		if(i==2) // longitude
		{	
			strcpy(global_data.longitude, tempo);
		}
		
		if(i==4) // altitude
		{	
			strcpy(global_data.altitude, tempo);
		}
		
		if(i==9) // date
		{
			strcpy(global_data.date, tempo);
		}

		if(i==10) // satellites
		{
			strcpy(global_data.satellites, tempo);
		}
	}
	
	return 1;	
}

int dataCapture()
{
	char gpsRequest[] = "at$gpsacp\r";

	global_data.latitude[0] = '\0';
    global_data.longitude[0] = '\0'; 
    global_data.altitude[0] = '\0';
    global_data.time[0] = '\0';
	global_data.date[0] = '\0';	 
	global_data.satellites[0] = '\0';
	
	/*delay(987654);
    enableInterrupts();
	UARTSendCmd(gpsRequest);
	delay(987654);
	disableInterrupts(); */
	UARTSendCmd2(gpsRequest, "\r\nOK\r\n", "ERROR");
	parseGpsData();





	return 1;
}

int sensorDataCapture()
{
 	global_data.temperature_d = 0.0;
   	global_data.humidity_d = 0.0;
	global_data.pressure_d = 0.0;

	// PIC data
	delay(987654);
	enableInterrupts();
   	UARTCharPut(UART0_BASE, 'C');
	delay(987654);				   
	delay(987654);			   
	disableInterrupts();  
	parseSensorData();
	
	return 1;

}

int parseSensorData()
{
	char t[MAX_TEMP_LEN] = {0}, h[MAX_PRESSURE_LEN] = {0}, p[MAX_HUMIDITY_LEN] = {0};
	double temp=0.0, press=0.0, hum=0.0;


	if(strlen(completeInputStr) < 5)
	{
		temp = 72.654;
		global_data.temperature_d = temp;
		sprintf(global_data.temperature, "%3.2f", temp);
				
		press = .95;
		global_data.pressure_d = press;
		sprintf(global_data.pressure, "%3.2f", press);

		hum = 45.21;
		global_data.humidity_d = hum;
		sprintf(global_data.humidity, "%3.2f", hum);
	}
	else
	{
		//temp	
		sprintf(t, "%u", completeInputStr[0]);
		temp = atoi(t);
		temp = (((temp*(9.0/5.0))+32.0) - 12.0); //Calibrated temperature value based on standard units
		global_data.temperature_d = temp;
		sprintf(global_data.temperature, "%3.2f", temp);
	
		//pressure
		sprintf(p, "%u", completeInputStr[2]);
		press = atoi(p); 
		press = (((press/256.0)*5.0)*(115.0/4.7)); //Calibrated pressure value based on standard units and data sheet conversion
		press = press/101.32500;
		global_data.pressure_d = press;
		sprintf(global_data.pressure, "%3.2f", press);
	
		//humidity
		sprintf(h, "%u", completeInputStr[4]);
		hum = atoi(h);
		hum = ((hum - 0.1515) / ( 5.00636 )); //Calibrated humidity value based on standard units and data sheet conversion
		global_data.humidity_d = hum;
		//hum = (hum) * (0.00636(sensorRH) + 0.1515)
		sprintf(global_data.humidity, "%3.2f", hum);
	}

	return 1;
}

