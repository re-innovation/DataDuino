/********************************************************
/****** DataDuino - Arduino Serial DAQ UNIT *************
/****** by Matt Little **********************************
/****** Date: 30/10/12 **********************************
/****** info@re-innovation.co.uk ************************
/****** www.re-innovation.co.uk *************************
/********************************************************

  See www.re-innovation.co.uk for information and construction details
  This is the SERIAL data storage version.
  
  This is sample code for the DataDuino PCB.

/*************Details of Code*****************************

  The DataDuino is an Arduino based SD card datalogger.
  A PCF8563 Realt Time Clock is used to timestamp the data.
  Data is written every time there is serial data in the correct format.

  Data is written to a .csv file created on an SD card.
  A new file is created each day. If file alreay present then data is appended.
  The file name is created from the reference number and the date in the format:
  RXXDXXXX.csv, where RXX is the reference number and DXXXX is the date in the format DDMM. 

  SERIAL Commands (at 115200 baud) :
  "aXXCHID??E--"
  This will change the reference number to ??
  "a--ID?----"
  This will return the device ID 


  Updates: 
  30/10/12 Code written - Matt Little
  31/10/12 writing code description and sorting out new sample timings - Matt Little
  19/9/13  Getting code to work on Arduino 1.0.5 - Matt Little
  19/9/13  Updated the Rtc_Pcf8563 to work with Arduino 1.0.5 - Matt Little
  19/9/13  Changed getData and sortData to 'listen' for serial data - Matt Little
  19/9/13  Removed all references to temperature - not needed - Matt Little
  23/9/13  Changed format of data - Matt Little

 
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
int D7 = 0;  // Digital input
int D8 = 0;  // Digital input
int D9 = 0;  // Digital input

/********** Analogue Data Storage ************/
int analog0 = 0;  // Analog input
int analog1 = 0;  // Analog input
int analog2 = 0;  // Analog input
int analog3 = 0;  // Analog input

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

//String dataType;   // This holds the dataType ("P", "V", "I")
String dataVoltage;  // This holds the actual data
String dataCurrent;  // This holds the actual data
String dataPower;  // This holds the actual data

// These are Char Strings - they are stored in program memory to save space in data memory
// These are a mixutre of error messages and serial printed information
const char headers[] PROGMEM = "Ref,Date,Time,Voltage,Current,Power";  // Headers for the top of the file
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
  Serial.begin(115200);    // Set up a serial output for data display and changing parameters
  
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
  deviceID[0] = char(EEPROM.read(0));
  deviceID[1] = char(EEPROM.read(1));
  
 
  initialiseSD();    // Inisitalise the SD card
  createfilename();  // Create the corrct filename (from date)

  pinMode(LEDred,OUTPUT);    // Set D5 to be an output LED
  pinMode(cardDetect,INPUT);  // D6 is the SD card detect on pin 6.
 
  //Set up digital data lines
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

void loop()
{

  // If there has been serial data with power information then we can wr
  // Also want to make sure that there is an SD card present
  if(writedataflag==HIGH)
  {
      
    // Each day we want to write a new file.
    // Compare date with previous stored date, every second
    newdate = String(rtc.formatDate(RTCC_DATE_WORLD));  
    if(newdate != date)
    {
       // If date has changed then create a new file
       createfilename();  // Create the corrct filename (from date)
    }
      
    // Here we create a data string to write
    // Subtracting '0' converts from ASCII to real number
    // The headers are: "Reference, Time, Date, V, Voltage, I, Current, P, Power"
    dataString =  String(deviceID[0]); 
    dataString += deviceID[1];
    dataString += comma;
    dataString += newdate;
    dataString += comma;
    dataString += String(rtc.formatTime());
    dataString += comma;
    dataString += dataVoltage;  // This is the data value
    dataString += comma;
    dataString += dataCurrent;  // This is the data value
    dataString += comma;
    dataString += dataPower;  // This is the data value
    
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
  getData();    // Check the serial port for data
  sortData();   // Sort the data for useful information
  delay(50);
  
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
  
  filename[1]=deviceID[0];  
  filename[2]=deviceID[1];   
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
  // As mentioned above, we are using LLAP for the data.
  // All the data arrives as serial commands via the serial interface.
  // All data is in format aXXDDDDDDDDD where XX is the device ID
  if (Serial.available() > 0) 
  {
    inByte = Serial.read(); // Read whatever is happening on the serial port
  
    if(inByte=='a')    // If we see an 'a' then read in the next 11 chars into a buffer.
    {   
      str_buffer+=inByte;
      for(int i = 0; i<11;i++)  // Read in the next 11 chars - this is the data
      {
        inByte = Serial.read(); 
        str_buffer+=inByte;
      }
      Serial.println(str_buffer);  // TEST - print the str_buffer data (if it has arrived)
      sortData();
      str_buffer="";  // Reset the buffer to be filled again
    }
  }
}

// **********************SORT DATA SUBROUTINE*****************************************
// This sub-routine takes the read-in data string (12 char, starting with a) and does what is required with it
// The str-buffer is global so we do not need to send it to the routine

void sortData()
{ 
  // Here we want to be able to ask the device what its ID is
  // “aXXID?-----“
  // It does not matter what XX are
  if(str_buffer.substring(3,6) == "ID?")
  {
    Serial.print(getString(reference));
    Serial.println(deviceID);
  } 
  
  // For all other commands we first want to check if the device ID matches.
  // If it does not then we disregard the command (as it was not meant for this device      
  if(str_buffer.substring(1,3) == deviceID)
  {
    // If yes then we can do further checks on ths data
    // This is where we do all of the checks on the incomming serial command:
    
    //Serial.println("Matches Device ID");  // TEST - got into this routine
    
    // Change device ID:
    // Device ID
    // “aXXCHIDXXE--“
    // Where the last two values (XX) are the new device ID (from AA to ZZ).
    if(str_buffer.substring(3,7) == "CHID")
    {
      // First check if the NEW device ID is within the allowable range (AA-ZZ)
      // to do this we can convert to an int and check if it is within the OK levels
      // A -> int is 65, Z -. int is 90
      // So our new device ID as an int must be between 65 and 90 for it to be valid
      if(65<=int(str_buffer[7])&&int(str_buffer[7])<=90&&65<=int(str_buffer[8])&&int(str_buffer[8])<=90)   
      { // If is all OK then write the data
        // Change device ID
        Serial.print("Change Device ID to ");
        Serial.println(str_buffer.substring(7,9));  // This will change the device ID
        deviceID[0] = str_buffer[7];
        deviceID[1] = str_buffer[8];
        // Also want to store this into EEPROM
        EEPROM.write(0, deviceID[0]);    // Do this seperately
        EEPROM.write(1, deviceID[1]);
        initialiseSD();
        createfilename();             
      }
      else
      {
        Serial.println("Invalid device ID"); 
     }      
    } 
    
    // Receive Power Data in format “aXXP????-----“ 
    // Where XX is the reference and ???? is the data
    else if(str_buffer.substring(3,4) == "P")
    {
      //Serial.print("Power:");
      //Serial.println(str_buffer.substring(4,8));
      dataPower = str_buffer.substring(4,8);
      writedataflag=HIGH;  // Set the unit to write the data
    } 
 
     // Receive Power Data in format “aXXV???-----“ 
    // Where XX is the reference and ??? is the data
    else if(str_buffer.substring(3,4) == "V")
    {
      //Serial.print("Voltage:");
      //Serial.println(str_buffer.substring(4,7));
      dataVoltage = str_buffer.substring(4,6);
      dataVoltage += '.';      
      dataVoltage += str_buffer.substring(6,7);
      
      //writedataflag=HIGH;  // Set the unit to write the data
    }
        // Receive Current Data in format “aXXI????-----“ 
    // Where XX is the reference and ???? is the data
    else if(str_buffer.substring(3,4) == "I")
    {
      //Serial.print("Current:");
      //Serial.println(str_buffer.substring(4,8));
      dataCurrent = str_buffer.substring(4,8);
      //writedataflag=HIGH;  // Set the unit to write the data
    }
    
  }
  else
  {
    //Serial.println("Device ID does not match");
  }
}

