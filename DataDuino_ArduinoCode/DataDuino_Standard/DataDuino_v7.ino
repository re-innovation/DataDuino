/********************************************************
/****** DataDuino - Arduino DAQ UNIT ********************
/****** by Matt Little **********************************
/****** Date: 30/10/12 **********************************
/****** info@re-innovation.co.uk ************************
/****** www.re-innovation.co.uk *************************
/********************************************************

  See www.re-innovation.co.uk for information and construction details
  
  This is sample code for the DataDuino.

/*************Details of Code*****************************

  The DataDuino is an Arduino based SD card datalogger.
  A PCF8563 Realt Time Clock is used to timestamp the data.
  
  Pin D4 is set up to record a DS18B20 1 wire temp sensor (up to 4 sensors can be attached) 
  Pin D3 is set up to cound pulses from a sensor (such as a anemometer or flow sensor)
  Pins D7,D8,D9 are set up to record digital information (0 or 1)
  Pins A0 to A3 are set up to record analogue information (0 to 1024)
  
  Each logger has a reference (user adjustable from 00-99).
  
  Data is written to a .csv file created on an SD card.
  A new file is created each day. If file alreay present then data is appended.
  The file name is created from the reference number and the date in the format:
  RXXDXXXX.csv, where RXX is the reference number and DXXXX is the date in the format DDMM. 
  
  Data is stored with human readable headers:
  "Reference, Time, Date, Pulses, Temp1, Temp2, Temp3, Temp4, D7,D8,D9,A0,A1,A2,A3"
  
  You can adjust the parameters of the device using serial commands. These parameters are stored in EEPROM.
  These are:
  R??E
  This will change the reference number to ??
  T??????E
  This will change the time to HHMMSS
  D??????E
  This will change the date to DDMMYY
  S?????E
  This will change the sample period to ????? seconds. Set to 00001 for 1 second data, set to 03600 for 1 hour data.
  The minimum is 1 second data. The maximum is 99999 seconds
 
  Updates: 
  30/10/12 Code written - Matt Little
  31/10/12 writing code description and sorting out new sample timings - Matt Little
  19/9/13  Getting code to work on Arduino 1.0.5 - Matt Little
  30/1/14  Getting code to work on 1.0.5 IDE - Matt Little
  23/2/15  Adding digital reads (D3/4/7/8/9) - Matt Little
  
 
 //*********SD CARD DETAILS***************************	
 The SD card circuit:
 SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 10
 ** Card detect - pin 6
 
 SD card code details:
 created  24 Nov 2010
 updated 2 Dec 2010
 by Tom Igoe
 
 //************ Real Time Clock code*******************
 A PCF8563 RTC is attached to pins:
 ** A4 - SDA (serial data)
 ** A5 - SDC (serial clock)
 ** D2 - Clock out - This gives a 1 second pulse to record the data
 
 RTC PCF8563 code details:
 By Joe Robertson, jmr
 orbitalair@bellsouth.net
 
**********************************************************************************************************/


/************ External Libraries*****************************/
#include <OneWire.h>
#include <DallasTemperature.h> 
#include <stdlib.h>
#include <Wire.h>          // Required for RTC
#include <Rtc_Pcf8563.h>   // RTC library
#include <SD.h>            // SD card library
#include <avr/pgmspace.h>  // Library for putting data into program memory
#include <EEPROM.h>        // For writing values to the EEPROM
#include <avr/eeprom.h>

/************User variables and hardware allocation**********************************************/

/******* SD CARD*************/
const int chipSelect = 10; // The SD card Chip Select pin 10
const int cardDetect = 6;  // The SD card detect is on pin 6
// The other SD card pins (D11,D12,D13) are all set within SD.h
int cardDetectOld = LOW;  // This is the flag for the old reading of the card detect

/*************Real Time Clock*******/
Rtc_Pcf8563 rtc;
#define I2C_RTC 0x51 // 7 bit address (without last bit - look at the datasheet)
int RTCinterrupt = 0;  // RTC interrupt - This is pin 2 of ardunio - which is INT0

/********* Output LED *************/
const int LEDred = 5;  // The output led is on pin 5

/********* Digital Data Storage  ***********/
int D3 = 0;  // Digital input
int D4 = 0;  // Digital input
int D7 = 0;  // Digital input
int D8 = 0;  // Digital input
int D9 = 0;  // Digital input

/********** Analogue Data Storage ************/
int analog0 = 0;  // Analog input
int analog1 = 0;  // Analog input
int analog2 = 0;  // Analog input
int analog3 = 0;  // Analog input

///********* Dallas DS18B20 1 wire temp sensor****************/
//// This does not have to be used - this is just an example
//// Parasitic mode is used.
//// On the DS18B20 pins 1 and 3 are grounded. Pin 2 is the data wire and must have a 4k7 pull up.
//// This is designed to be connected to pin D4 of the Arduino
//#define ONE_WIRE_BUS 4    // OneWire on pin 4
//#define TEMPERATURE_PRECISION 12  // Precision is 12 bits. Precision can be 9 to 12 Bits. Higher precision = longer read time
//
//OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
//DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.
//int numberOfDevices; // Number of temperature devices found - this is used in the initalisation.
//DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address
//
//// In this example up to 4 DS18B20 sensors can be attached to the 1 wire bus.
//// Each sensor can take 750mS (0.75 seconds) to return data. 
//// The datalogging frequency must be adjusted to cope with that (ie. if data is rcorded every second then only one temp sensor should be used)
//char Temp1 [6];    // To store the temperature as a string
//char Temp2 [6];    // To store the temperature as a string
//char Temp3 [6];    // To store the temperature as a string
//char Temp4 [6];    // To store the temperature as a string
//float temp;        // Temporary store for float

// ****** Serial Data Read***********
// Variables for the serial data read
char inByte;         // incoming serial char
String str_buffer = "";  // This is the holder for the string which we will display


//********Variables for the Filename*******************

char filename[] = "RXXDXXXX.csv";  // This is a holder for the full file name
//int refnumber;     // The house number here, which is stored in EEPROM
char deviceID[3]; // A buffer to hold the device ID
File datafile;   // The logging file
String dataString;    // This is the holder for the data as a string. Start as blank
int counter = 0;   // Clue is in the name - its a counter.
long dataCounter = 0;  // This holds the number of seconds since the last data store
long sampleTime = 10;  // This is the time between samples for the DAQ

// Variables for the Pulse Counter
int pulseinterrupt = 1;  // Pulse Counter Interrupt - This is pin 3 of arduino - which is INT1
volatile long pulsecounter = 0;  // This counts pulses from the flow sensor  - Needs to be long to hold number
volatile long pulsecounterold = 0;  // This is storage for the old flow sensor - Needs to be long to hold number


volatile int writedataflag = HIGH;  // A flag to tell the code when to write data

// Varibales for writing to EEPROM
int hiByte;      // These are used to store longer variables into EERPRPROM
int loByte;

// These next ints are for the filename conversion
//int ref1 = 0;
//int ref2 = 0;
int day_int =0;      // To find the day from the Date for the filename
int day_int1 =0;
int day_int2 =0;
int month_int = 0;
int month_int1 = 0;
int month_int2 = 0;
int year_int = 0;  // Year
int hour_int = 0;
int min_int = 0;
int sec_int = 0;

//**********STRINGS TO USE****************************
String comma = ",";
String date;        // The stored date from filename creation
String newdate;     // The new date, read every time 

// These are Char Strings - they are stored in program memory to save space in data memory
// These are a mixutre of error messages and serial printed information
const char headers[] PROGMEM = "Ref,Date,Time,D3,D4,D7,D8,D9,A0,A1,A2,A3";  // Headers for the top of the file
const char headersOK[] PROGMEM = "Headers OK";
const char erroropen[] PROGMEM = "Error opening file";
const char error[] PROGMEM = "ERROR ERROR ERROR";
const char initialisesd[] PROGMEM = "Initialising SD card...";
const char dateerror[] PROGMEM = "Dates are not the same - create new file";
const char locate[] PROGMEM = "Locating devices....";
const char parasitic[] PROGMEM = "Parasitic power is:";
const char reference[] PROGMEM = "The ref number is:";
const char noSD[] PROGMEM = "No SD card present";

#define MAX_STRING 80      // Sets the maximum length of string probably could be lower
char stringBuffer[MAX_STRING];  // A buffer to hold the string when pulled from program memory

//****************INITIALISE ROUTINE******************************
void setup()
{
  Serial.begin(9600);    // Set up a serial output for data display and changing parameters
  
  //******Real Time Clock Set - up********
  // A4 and A5 are used as I2C interface.
  // D2 is connected to CLK OUT from RTC. This triggers an interrupt to take data
  // We need to enable pull up resistors
  pinMode(A4, INPUT);           // set pin to input
  digitalWrite(A4, HIGH);       // turn on pullup resistors
  pinMode(A5, INPUT);           // set pin to input
  digitalWrite(A5, HIGH);       // turn on pullup resistors
  pinMode(2,INPUT);    // Set D2 to be an input for the RTC CLK-OUT   
  //initialise the real time clock
  Rtc_Pcf8563 rtc; 
  
  // Read the reference number from the EEROM
  deviceID[0] = EEPROM.read(0);
  deviceID[1] = EEPROM.read(1);
  
  // Read the sample Time value from EEPROM
  hiByte = EEPROM.read(2);
  loByte = EEPROM.read(3);
  sampleTime = (hiByte << 8)+loByte;  // Get the sensor calibrate value 
 
  initialiseSD();    // Inisitalise the SD card
  createfilename();  // Create the corrct filename (from date)
//  initialisetemp();  // Initialise the temperature sensors
//  attachInterrupt(pulseinterrupt, pulse, RISING);  // This sets up our Interrupt Service Routine (ISR) for flow
  attachInterrupt(RTCinterrupt, RTC, FALLING);  // This sets up our Interrupt Service Routine (ISR) for RTC
  pinMode(LEDred,OUTPUT);    // Set D5 to be an output LED
  pinMode(cardDetect,INPUT);  // D6 is the SD card detect on pin 6.
 
  //Set up digital data lines
  pinMode(3,INPUT);  
  pinMode(4,INPUT); 
  pinMode(7,INPUT);  
  pinMode(8,INPUT);  
  pinMode(9,INPUT);  
  
  // This section configures the RTC to have a 1Hz output.
  // Its a bit strange as first we read the data from the RTC
  // Then we load it back again but including the correct second flag  
  rtc.formatDate(RTCC_DATE_WORLD);
  rtc.formatTime();
  
  year_int = rtc.getYear();
  day_int = rtc.getDay();
  month_int = rtc.getMonth();  
  hour_int = rtc.getHour();
  min_int = rtc.getMinute();
  sec_int = rtc.getSecond(); 
  
  Wire.begin(); // Initiate the Wire library and join the I2C bus as a master
  Wire.beginTransmission(I2C_RTC); // Select RTC
  Wire.write(0);        // Start address
  Wire.write(0);     // Control and status 1
  Wire.write(0);     // Control and status 2
  Wire.write(DecToBcd(sec_int));     // Second
  Wire.write(DecToBcd(min_int));    // Minute
  Wire.write(DecToBcd(hour_int));    // Hour
  Wire.write(DecToBcd(day_int));    // Day
  Wire.write(DecToBcd(2));    // Weekday
  Wire.write(DecToBcd(month_int));     // Month (with century bit = 0)
  Wire.write(DecToBcd(year_int));    // Year
  Wire.write(0b10000000);    // Minute alarm (and alarm disabled)
  Wire.write(0b10000000);    // Hour alarm (and alarm disabled)
  Wire.write(0b10000000);    // Day alarm (and alarm disabled)
  Wire.write(0b10000000);    // Weekday alarm (and alarm disabled)
  Wire.write(0b10000011);     // Output clock frequency enabled (1 Hz) ***THIS IS THE IMPORTANT LINE**
  Wire.write(0);     // Timer (countdown) disabled
  Wire.write(0);     // Timer value
  Wire.endTransmission();
}
//
////*********** The flow interrupt**************
//void pulse()
//{
//  // Increment the pulse counter
//  pulsecounter++;
//}

//**************The RTC interrupt****************
// I use the CLK_OUT from the RTC to give me exact 1Hz signal
// To do this I changed the initialise the RTC with the CLKOUT at 1Hz
void RTC()
{ 
  dataCounter++;
  
  if(writedataflag==LOW&&dataCounter>=sampleTime)  // This stops us loosing data if a second is missed
  { 
    // If this interrupt has happened then we want to write data to SD card:
    // Save the pulsecounter value (this will be stored to write to SD card
    pulsecounterold = pulsecounter;
    // Reset the pulse counter
    pulsecounter = 0;
    // Reset the DataCounter
    dataCounter = 0;  
    // Set the writedataflag HIGH
    writedataflag=HIGH;
  }
}


void loop()
{

  // If the RTC has given us an interrupt then we need to write the data
  // Also want to make sure that there is an SD card present
  if(writedataflag==HIGH)
  {
    // We want to get the data from D7,D8,D9
    D3 = digitalRead(3);
    D4 = digitalRead(4); 
    D7 = digitalRead(7);
    D8 = digitalRead(8);    
    D9 = digitalRead(9);    
    // Also get the data from A0,A1,A2,A3
    
    analog0 = analogRead(A0);  
    analog1 = analogRead(A1);
    analog2 = analogRead(A2);
    analog3 = analogRead(A3);
    
//    // Get the temperature readings and store to variables
//    getTemperature();    
    
    // Each day we want to write a new file.
    // Compare date with previous stored date, every second
    newdate = String(rtc.formatDate(RTCC_DATE_WORLD));  
    if(newdate != date)
    {
       // If date has changed then create a new file
       //Serial.println(getString(dateerror)); 
       createfilename();  // Create the corrct filename (from date)
    }
      
    // Here we create a data string to write
    // Subtracting '0' converts from ASCII to real number
    // The headers are: "Reference, Time, Date, Pulses, Temp1, Temp2, Temp3, Temp4, D7,D8,D9,A0,A1,A2,A3"
    dataString =  String(deviceID[0]); 
    dataString += deviceID[1];
    dataString += comma;
    dataString += newdate;
    dataString += comma;
    dataString += String(rtc.formatTime());
    dataString += comma;
//    dataString += String(pulsecounterold);  // Store the number of pulses from the previous second
//    dataString += comma;
//    dataString += String(Temp1);  // Temperature 1
//    dataString += comma;
//    dataString += String(Temp2);  // Temperature 2
//    dataString += comma;
//    dataString += String(Temp3);  // Temperature 3
//    dataString += comma;
//    dataString += String(Temp4);  // Temperature 4
//    dataString += comma;
    dataString += String(D3);  // D7 digital value (0 or 1)
    dataString += comma;
    dataString += String(D4);  // D8 digital value (0 or 1)
    dataString += comma;
    dataString += String(D7);  // D7 digital value (0 or 1)
    dataString += comma;
    dataString += String(D8);  // D8 digital value (0 or 1)
    dataString += comma;
    dataString += String(D9);  // D9 digital value (0 or 1)
    dataString += comma;
    dataString += String(analog0);  // A0 analogue value (0 to 1024)
    dataString += comma;
    dataString += String(analog1);  // A1 analogue value (0 to 1024)
    dataString += comma;
    dataString += String(analog2);  // A2 analogue value (0 to 1024)
    dataString += comma;
    dataString += String(analog3);  // A3 analogue value (0 to 1024)
    
    if(digitalRead(cardDetect)==LOW&&cardDetectOld==HIGH)
    {
      // There was no card previously so re-initialise and re-check the filename
      initialiseSD();
      createfilename();
    }
    
    if(digitalRead(cardDetect)==LOW&&cardDetectOld==LOW)
    {
      //Ensure that there is a card present)
      digitalWrite(LEDred, HIGH);   // set the LED ON
      // We then write the data to the SD card here:
      writetoSD();
      digitalWrite(LEDred, LOW);   // set the LED ON
    }
    else
    {
       Serial.println(getString(noSD));
      // print to the serial port too:
      Serial.println(dataString);
    }
    
    if(digitalRead(cardDetect)==HIGH)
    {
      // This meands there is no card present so set he LED high
       digitalWrite(LEDred, HIGH);   // set the LED ON
    }     

    cardDetectOld = digitalRead(cardDetect);  // Store the old value of the card detect
    writedataflag=LOW;    // Reset the write data flag
  }
  
  //*********Here we check to see if any serial data has been seen***********
  getData();  // Check the serial port for data
  
}

//*********** FUNCTION TO INITIALISE THE SD CARD***************
void initialiseSD()
{
  Serial.println(getString(initialisesd));
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(chipSelect, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    //Serial.println("FAIL");
    // don't do anything more:
    // Want to turn on an ERROR LED here
    return;
  }
}


// *********FUNCTION TO SORT OUT THE FILENAME**************
void createfilename()
{
  // Check there is a file created with the date in the title
  // If it does then create a new one with the new name
  // The name is created from:
  // RXXDMMDD.CSV, where XX is the reference, MM is the month, DD is the day
  // You must add on the '0' to convert to ASCII
  
  filename[1]=deviceID[0];  // Convert from int to ascii
  filename[2]=deviceID[1];  // Convert from int to ascii  
  date = String(rtc.formatDate());
  day_int = rtc.getDay();  // Get the actual day from the RTC
  month_int = rtc.getMonth();  // Get the month
  day_int1 = day_int/10;    // Find the first part of the integer
  day_int2 = day_int%10;    // Find the second part of the integer
  filename[4]=day_int1 + '0';  // Convert from int to ascii
  filename[5]=day_int2 + '0';  // Convert from int to ascii 
  month_int1 = month_int/10;    // Find the first part of the integer
  month_int2 = month_int%10;    // Find the second part of the integer
  filename[6]=month_int1 + '0';  // Convert from int to ascii
  filename[7]=month_int2 + '0';  // Convert from int to ascii   

  // Create the file and put a row of headers at the top
  if (! SD.exists(filename)) {
    // only open a new file if it doesn't exist
    datafile = SD.open(filename, FILE_WRITE);    // Open the correct file
    // if the file is available, write to it:
    if (datafile) {
      datafile.println(getString(headers));
      datafile.close();
      Serial.println(getString(headersOK));
    }  
    // if the file isn't open, pop up an error:
    else {
      Serial.println(getString(erroropen));
    }  
  }
}


// This routine writes the dataString to the SD card
void writetoSD()
{
  datafile = SD.open(filename, FILE_WRITE);    // Open the correct file
  // if the file is available, write to it:
  if (datafile) {
    datafile.println(dataString);
    datafile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println(getString(erroropen));
  }
}


////*************FUNCTION TO INITIALISE THE TEMP SENSORS*********
//void initialisetemp()
//{
//  // ***********Setup the Dallas DS18D20 temp sensors************************
//  // Start up the library
//  sensors.begin();
//  // Grab a count of devices on the wire
//  numberOfDevices = sensors.getDeviceCount();
//  // locate devices on the bus
//  Serial.print(getString(locate));
//  Serial.print(numberOfDevices, DEC);
//  // report parasite power requirements
//  Serial.print(getString(parasitic)); 
//  if (sensors.isParasitePowerMode()) Serial.println("ON");
//  else Serial.println("OFF");  
//  // Loop through each device, print out address
//  for(int i=0;i<numberOfDevices; i++)
//  {
//    // Search the wire for address
//    if(sensors.getAddress(tempDeviceAddress, i))
//    {
//      Serial.print("Device ");
//      Serial.print(i, DEC);
//      Serial.print(" add: ");
//      printAddress(tempDeviceAddress);
//      Serial.println();
//
//      //Serial.print("Setting resolution to ");
//      //Serial.println(TEMPERATURE_PRECISION, DEC);
//      // set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
//      sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
////      Serial.print("Resolution set to: ");
////      Serial.print(sensors.getResolution(tempDeviceAddress), DEC); 
////      Serial.println();
//    }
//    else{
//      Serial.print("Ghost:");
//      Serial.print(i, DEC);
//      //Serial.print(" but could not detect address. Check power and cabling");
//    }
//  }  
//}
//
//// ***********Function to print a device address*************
//void printAddress(DeviceAddress deviceAddress)
//{
//  for (uint8_t i = 0; i < 8; i++)
//  {
//    if (deviceAddress[i] < 16) Serial.print("0");
//    Serial.print(deviceAddress[i], HEX);
//  }
//}
//
//// *******Get the temperature for all the devices and store**********
//void getTemperature()
//{
//  // Find the temperatures
//  // call sensors.requestTemperatures() to issue a global temperature 
//  // request to all devices on the bus
//  Serial.print("Temp..");
//  sensors.requestTemperatures(); // Send the command to get temperatures
//  Serial.println("OK");  // Print OK when done
//  
//  // Loop through each temp device and store the data
//  for(int i=0;i<numberOfDevices; i++)
//  {
//    // Search the 1 wire for address
//    if(sensors.getAddress(tempDeviceAddress, i))
//    {
//      temp = sensors.getTempC(tempDeviceAddress); 
//    }
//    switch(i){
//    case 0:
//      dtostrf(temp, 4, 2,Temp1);
//      break;
//    case 1:
//      dtostrf(temp, 4, 2,Temp2);
//      break;
//    case 2:  
//      dtostrf(temp, 4, 2,Temp3);
//      break;  
//    case 3:  
//      dtostrf(temp, 4, 2,Temp4);
//      break;
//    }        
//    //else ghost device! Check your power requirements and cabling	
//  }
//  
//}


// This routine pulls the string stored in program memory so we can use it
// It is temporaily stored in the stringBuffer
char* getString(const char* str) {
	strcpy_P(stringBuffer, (char*)str);
	return stringBuffer;
}


// Converts a decimal to BCD (binary coded decimal)
byte DecToBcd(byte value){
  return (value / 10 * 16 + value % 10);
}

// **********************GET DATA SUBROUTINE*****************************************
// This sub-routine picks up and serial string sent to the device and sorts out a power string if there is one
// All values are global, hence nothing is sent/returned

void getData()
{
    // **********GET DATA*******************************************
  // We want to find the bit of interesting data in the serial data stream
  // If we write H then house number then the code will update the house number in EEPROM
  // **** aslo need to write code to update RTC
  
  for(int i = 0; i<10;i++)  // This helps us just take a 'chunk' of data so does not fill up serial buffer
  {
    // get incoming bytes:
    if (Serial.available() > 0) 
    {
     inByte = Serial.read(); 
     str_buffer+=inByte;
    
     if(inByte=='E')    // We read everything up to the byte 'E' which stands for END
     {
       int buffer_length = str_buffer.length();  // We also find the length of the string so we know how many char to display 
       // Depending upon what came before we update different values
       // To change the reference number we enter R00E, where 00 can be any number up to 99 

        for(int i = buffer_length; i>=0; i--)  // Check the buffer from the end of the data, working backwards
        {
          if(str_buffer[i]=='R')
          {
              // In this case we have changed the house number, so UPDATE and store in EEPROM
              Serial.print(getString(reference));
              Serial.println(str_buffer[i+1]);
              deviceID[0]=str_buffer[i+1];
              deviceID[1]=str_buffer[i+2];
              EEPROM.write(0,deviceID[0]);
              EEPROM.write(1,deviceID[1]);
              initialiseSD();
              createfilename();
          }          
          if(str_buffer[i]=='T')
          {
              // In this case we have changed the TIME, so UPDATE and store to RTC
              // The time is in the format  HHMMSS
              
              String hourstr = str_buffer.substring(i+1,i+3);
              int hour = atoi(&hourstr[0]);

              String minutestr = str_buffer.substring(i+3,i+5);
              int minute = atoi(&minutestr[0]);
              
              String secondstr = str_buffer.substring(i+5,i+7);
              int second = atoi(&secondstr[0]);
                        
              //hr, min, sec
              rtc.setTime(hour, minute, second);

              initialiseSD();
              createfilename();
          }
          if(str_buffer[i]=='D')
          {
              // In this case we have changed the DATE, so UPDATE and store to RTC
              // The time is in the format  DDMMYY
              
              String daystr = str_buffer.substring(i+1,i+3);
              int day = atoi(&daystr[0]);
              String monthstr = str_buffer.substring(i+3,i+5);
              int month = atoi(&monthstr[0]);          
              String yearstr = str_buffer.substring(i+5,i+7);
              int year = atoi(&yearstr[0]);          
           
              //day, weekday, month, century(1=1900, 0=2000), year(0-99)
              rtc.setDate(day, 3, month, 0, year);
              
              initialiseSD();
              createfilename();
          }           
          if(str_buffer[i]=='S')
          {          
              // In this case we have changed the sample time, so UPDATE and store to EEPROM
              // Data will be in the form of 5 x chars, signifying XXXXX, a value from 00001 to 99999 seconds
              
              sampleTime = atol(&str_buffer[i+1]);  // Convert the string to a long int
              
              EEPROM.write(2, sampleTime >> 8);    // Do this seperately
              EEPROM.write(3, sampleTime & 0xff);
              Serial.print("Sample Time:");
              Serial.println(sampleTime);
          }           
        }
        str_buffer="";  // Reset the buffer to be filled again 
      }
    }
  }
}

