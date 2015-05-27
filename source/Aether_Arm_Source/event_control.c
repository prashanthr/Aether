/* THIS FILE CONTAINS ALL FUNCTIONS THAT HANDLE SYSTEM EVENTS AND ARE EASY TO FOLLOW BY REFERRING TO PROJECT AETHER'S DETAILED DESIGN DOC*/
//function declarations
void event_control(void);
void mode_bit_check(double, double, double, double, String);
void auto_detach_detector(double, double, double, double);
void text_request_interpreter(String);
void mode_actor(void);
void status_assigner(enum EVENT);
void status_check(enum EVENT);
void message_assembler(enum EVENT);
void configuration_setter(enum EVENT);
void output_data_compiler(enum EVENT);
void communication_data_compiler( String, enum EVENT );

void event_control(void)
{
	 double temperature;
     double humidity;
     double pressure;
     double altitude;
     char in_text[MAX_IN_MSG_LEN] = {0};

	 strcpy(in_text, global_data.incoming_text_message);
	 temperature = global_data.temperature_d;
     humidity = global_data.humidity_d;
     pressure = global_data.pressure_d;
     altitude = global_data.altitude_d;

	 mode_bit_check(temperature, humidity, pressure, altitude, in_text);
}

void mode_bit_check(double temp, double humid, double press, double alt, String text)
{  
     if(global_data.mode == 0)
     {
		 if(global_data.alert == 1)
		 {
			 text_request_interpreter(text);
			global_data.alert = 0;
		 }
         auto_detach_detector(temp, humid, press, alt);
         mode_actor(); 
     }
     else if( (global_data.mode == 1) && (strncasecmp(text, "stop", 4) == 0))
     { 
        send_sp = 0;	
	 	stop = 1;
		mode_actor();
     }
	 else
	 {
	 	 mode_actor();
	 } 
}

void auto_detach_detector( double current_temp, double current_humid, 
                           double current_press, double current_alt)
{     
     if( ( current_temp <= config.minimum_temperature ) || 
         ( current_temp >= config.maximum_temperature ) ||
         ( current_humid <= config.minimum_humidity ) || 
         ( current_humid >= config.maximum_humidity ) ||
         ( current_press <= config.minimum_pressure ) || 
         ( current_press >= config.maximum_pressure ) ||
         ( current_alt >= config.maximum_altitude ) )
     { 
         
         status_assigner(detach);
		 global_data.mode = 1;
		 detachFlag = 1;
     }
     
}

void text_request_interpreter( String in_message )
{
	  String value_substring;
	
     if( strncasecmp(in_message, "request temperature", 19) == 0 )
     {
         status_assigner(temperature_request);
     }
     else if( strncasecmp(in_message, "request humidity", 16) == 0 )
     {
         status_assigner(humidity_request);
     }
     else if( strncasecmp(in_message, "request pressure", 16) == 0 )
     {
         status_assigner(pressure_request);
     }
     else if( strncasecmp(in_message, "request latitude", 16) == 0 )
     {
         status_assigner(latitude_request);
     }
     else if( strncasecmp(in_message, "request longitude", 17) == 0 )
     {
         status_assigner(longitude_request);
     }
     else if( strncasecmp(in_message, "request altitude", 16) == 0 )
     {
          status_assigner(altitude_request);
     }
	 else if( strncasecmp(in_message, "request gps", 11) == 0 )
     {
          status_assigner(gps_request);
     }
     else if( ( strlen(in_message) > 19 ) && ( strncasecmp(in_message, "set min temperature", 19) == 0) )
     {
          value_substring = in_message+22;
          global_data.tempConfValue = atof(value_substring);
          status_assigner(set_minimum_temperature);
     }
     else if( ( strlen(in_message) > 19 ) && ( strncasecmp(in_message, "set max temperature", 19) == 0) )
     {
          value_substring = in_message+22;
          global_data.tempConfValue = atof(value_substring);
          status_assigner(set_maximum_temperature);
     }
     else if( ( strlen(in_message) > 16 ) && ( strncasecmp(in_message, "set min humidity", 16) == 0) )
     {
          value_substring = in_message+19;
          global_data.tempConfValue = atof(value_substring);
          status_assigner(set_minimum_humidity);
     }
     else if( ( strlen(in_message) > 16 ) && ( strncasecmp(in_message, "set max humidity", 16) == 0) )
     {
          value_substring = in_message+19;
          global_data.tempConfValue = atof(value_substring);
          status_assigner(set_maximum_humidity);
     }
     else if( ( strlen(in_message) > 16 ) && ( strncasecmp(in_message, "set min pressure", 16) == 0) )
     {
          value_substring = in_message+19;
          global_data.tempConfValue = atof(value_substring);
          status_assigner(set_minimum_pressure);
     }
     else if( ( strlen(in_message) > 16 ) && ( strncasecmp(in_message, "set max pressure", 16) == 0) )
     {
          value_substring = in_message+19;
          global_data.tempConfValue = atof(value_substring);
          status_assigner(set_maximum_pressure);
     }
     else if( ( strlen(in_message) > 16 ) && ( strncasecmp(in_message, "set max altitude", 16) == 0) )
     {
          value_substring = in_message+19;
          global_data.tempConfValue = atof(value_substring);
          status_assigner(set_maximum_altitude);
     }
     else if( strncasecmp(in_message, "sp", 2) == 0 )
     {
          strcpy(config.phone_number, global_data.incoming_phone_number);
		  status_assigner(normal_text);
		  stop = 0;
		  send_sp = 1;
     }
     else if( strncasecmp(in_message, "detach", 6) == 0 )
     {
          status_assigner(detach);
		  global_data.mode = 1;
		  detachFlag = 1;
		  stop = 0;
		  send_sp = 1;
     }
	 else if( strncasecmp(in_message, "stop", 4) == 0 )
     {
	 	send_sp = 0;	
	 	stop = 1;
	 }
     
}
void mode_actor(void)
{
	int current_time;
	if(global_data.mode == 0)
	{ 
		//normal store event
		status_assigner(store);               
	}
	
	global_data.mode_actor_count++; 
	
		if( global_data.mode_actor_count == 1 )
		{
			if( global_data.mode == 0 )
			{
				//text_user_enviromnental_var
				status_assigner(normal_text);
			}
			else if( global_data.mode == 1 )
			{
				//text_user_positional_var
				status_assigner(recovery_text);
			}
		}
		
		current_time = convert_time(global_data.time);    
		//if( (current_time - initial_time) >= increment )
		if((send_sp==1) && (sp_loop++ % 6 == 0))
		{
			if( global_data.mode == 0 )
			{
				//text_user_enviromnental_var
				status_assigner(normal_text);
			}
			else if( global_data.mode == 1 )
			{
				//text_user_positional_var
				status_assigner(recovery_text);
			}
			increment += 30;
		}
	//}
}

void status_assigner( enum EVENT status )
{
     if( (status >= temperature_request) && (status <= recovery_text) )
     {
         global_data.event_value = status;
         status_check(global_data.event_value);
     }
}

void status_check( enum EVENT event_val )
{
     if( (( event_val >= temperature_request ) && ( event_val <= gps_request ) )
          || (event_val == normal_text) || (event_val == recovery_text) ) 
     {
          message_assembler( event_val );
     }
     else if( (event_val >= set_minimum_temperature ) && ( event_val <= set_maximum_altitude ) )
     {
          configuration_setter( event_val );
     }
     else if( (event_val == detach) || ( event_val == store ) )
     {
          output_data_compiler( event_val );
     }
}

void message_assembler( enum EVENT status_value )
{
     char temp_message[120] = {0};
     
     switch( status_value )
     {
          case temperature_request:
               sprintf(temp_message, "Temperature = %s", global_data.temperature );
               communication_data_compiler( temp_message, temperature_request );
               break;
          case humidity_request:
               sprintf(temp_message, "Humidity = %s", global_data.humidity );
               communication_data_compiler( temp_message, humidity_request );
               break;
          case pressure_request:
               sprintf(temp_message, "Pressure = %s", global_data.pressure );
               communication_data_compiler( temp_message, pressure_request );
               break;
          case latitude_request:
               sprintf(temp_message, "Latitude = %s", global_data.latitude );
               communication_data_compiler( temp_message, latitude_request );
               break;
          case longitude_request:
               sprintf(temp_message, "Longitude = %s", global_data.longitude );
               communication_data_compiler( temp_message, longitude_request );
               break;
          case altitude_request:
               sprintf(temp_message, "Altitude = %s", global_data.altitude );
               communication_data_compiler( temp_message, altitude_request );
               break;
		  case gps_request:
               sprintf(temp_message, "Latitude = %s\r Longitude = %s\r Altitude = %s", global_data.latitude, global_data.longitude, global_data.altitude );
               communication_data_compiler( temp_message, gps_request );
               break;
          case normal_text:
               sprintf(temp_message, "Temperature = %s\r Humidity = %s\r Pressure = %s\r", global_data.temperature, global_data.humidity, global_data.pressure);
			   communication_data_compiler( temp_message, normal_text );
               break;
          case recovery_text:
               sprintf(temp_message, "Latitude = %s\r Longitude = %s\r Altitude = %s", global_data.latitude, global_data.longitude, global_data.altitude );
               communication_data_compiler( temp_message, recovery_text );
               break;
          default:
                  break;
          
     }
     //outgoing_text_message must be set to null in communication layer
}

void configuration_setter(enum EVENT status_value)
{

     switch( status_value )
     {
          case set_minimum_temperature:
               config.minimum_temperature = global_data.tempConfValue;
               break;
          case set_maximum_temperature:
               config.maximum_temperature = global_data.tempConfValue;
               break;
          case set_minimum_humidity:
               config.minimum_humidity = global_data.tempConfValue;
               break;
          case set_maximum_humidity:
               config.maximum_humidity = global_data.tempConfValue;
               break;
          case set_minimum_pressure:
               config.minimum_pressure = global_data.tempConfValue;
               break;
          case set_maximum_pressure:
               config.maximum_pressure = global_data.tempConfValue;
               break;
          case set_maximum_altitude:
               config.maximum_altitude = global_data.tempConfValue;
               break;
          default:
                  break;      
     }
}

void output_data_compiler(enum EVENT status_value)
{
     switch( status_value )
     {
          case detach:
               global_data.detach = 1;
               break;
          case store:
               global_data.storage = 1;
               break;
          default:
                  break;
     }
}

void communication_data_compiler( String reply_message, enum EVENT status_value )
{
     if( ( (status_value == normal_text) || (status_value == recovery_text) ) &&(!strlen(config.phone_number) < 10) )
	 {
           sendTextMsg(config.phone_number, reply_message);
	 }
	 if(  (status_value != normal_text) && (status_value != recovery_text) )
	 {		 
			sendTextMsg(global_data.incoming_phone_number, reply_message);
	 }		   
}

