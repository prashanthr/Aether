#include <18F4550.h>
#include <stdlib.h>

// configure a 20MHz crystal to operate at 48MHz
#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL5,CPUDIV1,VREGEN

#use delay(clock=48000000)
#use i2c(master, sda=PIN_B0, scl=PIN_B1, FORCE_HW)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

// include CDC API
#include <usb_cdc.h>

#define carr1 PIN_D0
#define SERVO_PIN PIN_D0
#define LED_PIN PIN_D1


// PIC USB Trainer board constants
const int TMP100_ADDRESS = 0x90;
//const int POTENTIOMETER_ADC_CHANNEL = 1;
const int HUMIDITY_ADC_CHANNEL = 1;
const int PRESSURE_ADC_CHANNEL = 2;

//const int GREEN_LED_PIN = PIN_A4;
//const int YELLOW_LED_PIN = PIN_B5;
//const int RED_LED_PIN = PIN_C0;

unsigned int temperature, pressure, humidity;

// function prototypes
unsigned int readTMP100(byte address);
unsigned int readPotentiometer();
int readADC(int);
void UART_COMM();
void autoDataCapture();
void detach();
double calculatePressure(byte);
void put_str(char *);

unsigned int readTMP100(byte i2c_address)
{
   byte datah = 0, datal = 0;

   i2c_start();
   i2c_write(i2c_address);
   i2c_write(0x00);
   i2c_start();
   i2c_write(i2c_address + 0x01);
   datah = i2c_read();
   datal = i2c_read(0);
   i2c_stop();

   // just return the high byte

   return datah;
}

unsigned int readTMP100_high(byte i2c_address)
{
   byte datah = 0, datal = 0;

   i2c_start();
   i2c_write(i2c_address);
   i2c_write(0x00);
   i2c_start();
   i2c_write(i2c_address + 0x01);
   datah = i2c_read();
   datal = i2c_read(0);
   i2c_stop();

   // just return the high byte

   return datah;
}

unsigned int readTMP100_low(byte i2c_address)
{
   byte datah = 0, datal = 0;

   i2c_start();
   i2c_write(i2c_address);
   i2c_write(0x00);
   i2c_start();
   i2c_write(i2c_address + 0x01);
   datah = i2c_read();
   datal = i2c_read(0);
   i2c_stop();

   // just return the high byte

   return datal;
}


unsigned int readADC(int ADC_CHANNEL)
{
   set_adc_channel(ADC_CHANNEL);
   delay_ms(1);
   return read_adc();
}

void put_str(char *str)
{
   int i;
   for(i=0; i<strlen(str); i++)
      putc(str[i]);
}

void UART_SEND()
{
   char * nL = "\r\n";
   double pr;
   char t[9] = {0};
   char p[9] = {0};
   char h[9] = {0};

   putc(temperature);  
   putc(',');
   
   putc(pressure);   
   putc(',');   

   putc(humidity);  
   putc(';');
   put_str(nL);
}

void detach()
{
   int count = 0,count2=0;
   
   output_high(LED_PIN);
   delay_ms(150);
   output_low(LED_PIN);
   
   while(count<150)
   {
      output_high(SERVO_PIN);
      delay_us(1000);
      output_low(SERVO_PIN);
      delay_us(25000);   
   }
      delay_ms(2400);
   
   while(count2<150)
   {
      output_high(SERVO_PIN);
      delay_us(2000);
      output_low(SERVO_PIN);
      delay_us(25000);   
   }   

}

double calculatePressure(int p)
{
    double pr;
    pr = (((double)p/256) * 5.0)*15.0/0.2;   
    return pr;
}

void autoDataCapture()
{
   temperature = readTMP100(TMP100_ADDRESS);
   pressure = readADC(PRESSURE_ADC_CHANNEL);
   humidity = readADC(HUMIDITY_ADC_CHANNEL); 
   
   UART_SEND();
}

// program entry point
void main()
{

   int temp;
   char buff[2],c;
   
   delay_ms(1000);

   // initialize USB communication
   usb_cdc_init();
   usb_init();

   // initialize A2D
   setup_adc_ports(AN0_TO_AN8);
   setup_adc(ADC_CLOCK_INTERNAL);


   while(1)
   {
      if (kbhit()) 
         buff[0] = getc();
      else
            buff[0] = 0;
      
      if(buff[0] == 'C')
      {   
         autoDataCapture();
      }
      else if(buff[0] == 'D')
      {
         detach();
      }
     
   }
   return;
}
