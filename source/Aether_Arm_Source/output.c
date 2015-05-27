/* THIS FILE CONTAINS THE METHOD THAT PERFORMS THE LOG DATA FUNCTIONALITY WHICH WRITES SPECIFIC DATA TO THE STORAGE CARD*/
void logData()
{ 
	call_sd(global_data.date);
	call_sd(",");
	call_sd(global_data.time);	 
	call_sd(",");
	call_sd(global_data.longitude);
	call_sd(",");
	call_sd(global_data.latitude);
	call_sd(",");	  
	call_sd(global_data.altitude);
	call_sd(",");	  
	call_sd(global_data.temperature);
	call_sd(",");	  
	call_sd(global_data.pressure);
	call_sd(",");	  
	call_sd(global_data.humidity);	
	call_sd(",");	
	call_sd(global_data.incoming_text_message);	
	call_sd(",\n");
}
