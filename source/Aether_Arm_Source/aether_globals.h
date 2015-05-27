/* THIS FILE CONTAINS ALL GLOBAL VARIABLES AND DATA STRUCTURES THAT IS USED BY THE PROGRAM*/
#define MAX_NUM_LEN 12
#define MAX_ALT_LEN 25
#define MAX_LONG_LEN 25
#define MAX_LAT_LEN 25
#define MAX_DATE_LEN 25
#define MAX_TIME_LEN 25	
#define MAX_PHONE_LEN 25
#define MAX_IN_MSG_LEN 1000
#define MAX_OUT_MSG_LEN 100
#define MAX_GPS_LEN 1000
#define MAX_SENSOR_LEN 15
#define MAX_TEMP_LEN 10
#define MAX_PRESSURE_LEN 10
#define MAX_HUMIDITY_LEN 10

int detachFlag = 0;

char tempInputStr[MAX_GPS_LEN] = {0};
char completeInputStr[MAX_GPS_LEN] = {0};

int loopValue = 0;
static unsigned int count = 0;
static unsigned int scount = 0;

short current_txt_msg_index = 1;
short fake_data_loop = 0;

typedef char* String;

enum EVENT { nothing, temperature_request, humidity_request, pressure_request, latitude_request,
             longitude_request, altitude_request, gps_request, set_minimum_temperature, set_maximum_temperature,
             set_minimum_humidity, set_maximum_humidity, set_minimum_pressure, set_maximum_pressure,
             set_maximum_altitude, set_phone_number, detach, store, normal_text, recovery_text };

typedef struct {
        //sensor data
        double temperature_d;
        double humidity_d;
        double pressure_d; 
        char temperature[MAX_TEMP_LEN];
        char humidity[MAX_HUMIDITY_LEN];
        char pressure[MAX_PRESSURE_LEN];

        //gps data
		char latitude[MAX_LAT_LEN];
        char longitude[MAX_LONG_LEN]; 
        char altitude[MAX_ALT_LEN];
        double altitude_d;
        char time[MAX_TIME_LEN];
		char date[MAX_DATE_LEN];
		char satellites[MAX_NUM_LEN];

        //communication data
        int alert;
        char incoming_phone_number[MAX_PHONE_LEN];
        char incoming_text_message[MAX_IN_MSG_LEN];
        char outgoing_text_message[MAX_OUT_MSG_LEN];
        double tempConfValue;
        
        //output data
        int storage;
        int detach;
        
        int mode;
        int mode_actor_count;
        
        enum EVENT event_value;
        
        } SystemInfo;
        
typedef struct {
        double minimum_temperature;
        double maximum_temperature;
        double minimum_humidity;
        double maximum_humidity;
        double minimum_pressure;
        double maximum_pressure;
        double maximum_altitude;
		char phone_number[MAX_PHONE_LEN];
        } Configuration;


SystemInfo global_data = { 0, 0, 0, "0", "0", "0", "0N", "0E", "0", 0, " "," ", "0", 5, "", " ", " ", 9999, 0, 0, 0, 0, nothing };
Configuration config = { -5000, 5000, -5000, 5000, -5000, 5000, 5000, "" };

int initial_time;
int increment = 30;
int stop = 0;
long int sp_loop = 0;
int send_sp = 0;

