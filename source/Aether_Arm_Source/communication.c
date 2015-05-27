/* THIS FILE CONTAINS ALL FUNCTIONS THAT HANDLE SYSTEM COMMUNICATION (BETWEEN CONTROLLER AND GM862 VIA SERIAL TRANSFER). THIS IS EASY TO FOLLOW BY REFERRING TO PROJECT AETHER'S DETAILED DESIGN DOC*/
void parseMsgData()
{
	char cp[MAX_IN_MSG_LEN] = {0};
	char * tempo = NULL;
	char delimiters[] = "\""; // delimit commas
	int i = 0;
	int numSegments = 5;
	int quoteCount = 0;

	//if(!strncmp(completeInputStr,"+CMGR: \"REC READ\",",18))
	if(find_substr(completeInputStr, "+CMGR: \"REC READ\",") != -1)
	{
		strcpy(global_data.incoming_text_message, "old:");
		i=0;
		quoteCount=0;
		while(quoteCount <= 8 && i <= strlen(completeInputStr))
		{
			if(quoteCount >= 8)
			{ 
				strcpy(global_data.incoming_text_message+4, completeInputStr+i+2); 
				global_data.incoming_text_message[strlen(completeInputStr+i+2)+4-8] = '\0';
				break;
			}

			if(completeInputStr[i]=='\"') quoteCount++;

			i++;
		}
		

		strcpy(cp, completeInputStr);
		for(i=0; i<numSegments; i++)
	    {		
			if(i==0)					
	        	tempo = strtok(cp, delimiters); // delimite the first part
	      	else
				tempo = strtok(NULL, delimiters);
			
			if(i==3) // number
			{
				if(tempo[0] == '+')
					strcpy(global_data.incoming_phone_number, tempo+1);
				else
					strcpy(global_data.incoming_phone_number, tempo);
			}
		}
		/******REMOVE ABOVE UNCOMMENT BELOW*********/
		//strcpy(global_data.incoming_text_message, "old message\0");
		current_txt_msg_index++;
	}
	else if(find_substr(completeInputStr, "+CMGR: \"REC UNREAD\",") != -1)
	{
		i=0;
		quoteCount=0;
		while(quoteCount <= 8 && i <= strlen(completeInputStr))
		{
			if(quoteCount >= 8)
			{ 
				strcpy(global_data.incoming_text_message, completeInputStr+i+2);
				global_data.incoming_text_message[strlen(completeInputStr+i+2)-8] = '\0';
				break;
			}

			if(completeInputStr[i]=='\"') quoteCount++;

			i++;
		}

		strcpy(cp, completeInputStr);
		global_data.alert=1;

		for(i=0; i<numSegments; i++)
	    {		
			if(i==0)					
	        	tempo = strtok(cp, delimiters); // delimite the first part
	      	else
				tempo = strtok(NULL, delimiters);
			
			if(i==3) // number
			{
				if(tempo[0] == '+')
					strcpy(global_data.incoming_phone_number, tempo+1);
				else
					strcpy(global_data.incoming_phone_number, tempo);
			}
		}

		if(global_data.incoming_text_message==NULL) strcpy(global_data.incoming_text_message, "Message not found!\0");
		current_txt_msg_index++;
	}
	else
		strcpy(global_data.incoming_text_message, "-----\0");
}

//Make a command to send to the cellular module
int sendTextMsg(char * phoneNumber, char * textMessage)
{
	char command[MAX_OUT_MSG_LEN] = {0};
	char prefix[] = "AT+CMGS=\"";
	char mid[] = "\"\r";
	char send = 0x1A;   
	int index = 0;
	int number_len = 0;

	number_len = strlen(phoneNumber);
	if(number_len > 9 && number_len < 12)
	{
		strcpy(command+index, prefix);
		index += strlen(prefix); 
		   
		strcpy(command+index, phoneNumber);	
		index += strlen(phoneNumber);	
	
		strcpy(command+index, mid);
		index += strlen(mid); 
	 	command[index] = '\0';	

		UARTSendCmd(command); 
		delay(543210); // wait for >
	
		index = 0;
		strcpy(command+index, textMessage);	 
		index += strlen(textMessage);	
	
		command[index] = send;	 
		index++;
	 	command[index] = '\0';
			
		UARTSendCmd(command);
	}

	return 1;
}

int getTextMsg()
{
	char cmd[15] = {0};

   
	global_data.incoming_text_message[0] = '\0';
	global_data.incoming_phone_number[0] = '\0';

	sprintf(cmd, "at+cmgr=%d\r", current_txt_msg_index);
  
	UARTSendCmd2(cmd, "\r\nOK\r\n", "+CMS ERROR: 321\r\n");
	parseMsgData();	 

	return 1;
}

