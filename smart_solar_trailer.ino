/*-------------------------TASK LIST-------------------

* Replace all delay commands with single thread "multi-tasking" if statements
* Single line comments = //
* In the future, the system might be activated by detecting if people are in the house (door opens, motion detected) so when people use the house, fridge is active, otherwise appliance is disabled
* Implement timelord library to track sunrise/sunset
* Compensate time from sunrise sunset to sun disapearing behind mountains (house is in a valley) 
*/

/*-------------------------INCLUDES-------------------*/

//RTC_MODULES
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

//SD_CARD_MODULES
#include <SPI.h>
#include <SD.h>

//TEMO_PROP_MODULE
#include <OneWire.h>
#include <DallasTemperature.h>

/*-----------------------DEFINES---------------------*/
//TEMP_PROBE_DATA_WIRE_PIN
#define ONE_WIRE_BUS 2


//Voltage Measurement Pin
#define VOLTAGE_READING_PIN A0

//SD CARD PIN
#define SDCARD_CHIP_SELECT_PIN 4

/*--------------------GLOBAL_VARIABLES-----------------*/

//array for month names
const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};


//used by Temp Sensor module
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);


//structure for timestamp members
tmElements_t tm;

/*--------------------------------Main block------------------------------*/


void setup() {
  System_init();
}


void loop() {
  System_logData();
  /* 
  What else should be looping?
  - Check if it's time to activate/deactivate a power consumer (at each loop is probably too much so after time passd) (currenty just fridge)
  - optional - Ensure voltage is within normal discharge range (or being drained for unexpected reason, I.E. charging extra device(s))
  - 
  */
  delay(2000);
}


void System_init(void) {
  //begin the serial connection
  Serial.begin(9600); //remove once logging to SD

  //initialize RTC
  RTC_init();

  //intialize temp sensor
  tempSensor.begin();
}

/*

void System_logData(void)
{
  // - Data logging doesn't need an independant function unless logging is needed that doesn't fit into the rest of the program. Otherwise logging should be done at appropriate action
  //log function
  
  float currentVoltage = 0;
  RTC_getTimeStamp();
  
  currentVoltage = getBatteryVoltage(); // - should be moved to getBatteryVoltage
 function. Makes no sense to get battery voltage in logging function since it's outside of scope of function name and app becomes spaggeti
  String record = "";

  record += String(tm.Hour);
  record += ":";
  record += String(tm.Minute);
  record += ":";
  record += String(tm.Second);
  record += ",";
  record += String(tm.Day);
  record += "/";
  record += String(tm.Month);
  record += "/";
  record += String(tmYearToCalendar(tm.Year));
  record += ",";
  record += "Voltage Reading: ";
  record += String(currentVoltage);
  
  File dataLoggerFile = SD.open("TestFile.txt", FILE_WRITE); // - as disscussed, later switch to recording directly in .csv file

  if (dataLoggerFile) {
    dataLoggerFile.println(record);
    dataLoggerFile.close();
    Serial.println(record);
  }
  else
  {
    Serial.println("Error while writing in SD");
  }

  delay(1000);

  */
}

//Get battery charge level at end of each day for use at night
float getDailyCharge(void)
{
  /* 
  * - find a sunny day - compare last n days to find day with best charging which indicates likely clear (not cloudy) day. Data from this day gives exact indication of when charging happens at that time of year (mountains blocking sun in morning and evening making the charging day shorter)
  * - Save table of sun times calibration for later years
  * - 
  */
  float dailyCharge = getBatteryVoltage(); // - might be average of daily readings so one call a day might not be enough but for now ok

  return dailyCharge; // Save to SD. This data is stored log term
  
}

//adjust fridge power draw according to battery charge (may fully or partly discharge battery)
float decideFridgeTemp(void){
  float dailyCharge = getDailyCharge();
  /*
   * - Compressor consumption - Fridge compressor reduces approx 10mv charge level every 2 minutes (less while charging increses voltage during the day while fridge runs)
   * - Compressor on time - for each minute the compressor runs, the fridge stays off for about 2 minutes on average (1 part on, 2 parts off).
   * - Battery loses about 0.1v an hour (approx 20 minutes on time)
   * - Battery range 9.6v (empty) - 12.6v (full), so full battery should (in theory) run fridge for about max 30 hours of compressor time. It probably runs a lot less
   * - The higher the fridge temperature, the less energy it uses (the less cold it losses and needs to produce).
   * - Fridge temperature should depend on available charge so battery is not depleted during the night (or on a cloudy day after)
   * - 
   *
   * some code to decide the fridge temp
   * 
   * 
   * 
   */
}

//Keep fridge within temperature range that was set by 'decideFridgeTemp' func
void maintaingFridgeTemp(void){

  /*
   * if current temp is higher than the max limit, turn on the compresseor
   * and if it is lower, turn off the compresser
   * if it is between, do nothing
   * on-time temperature range maybe the same at all temperature ranges or different, depending on testing
   * Temp range should be saved long term (with time stamp) so we have a history of what the fridge was doing
   */
}

// Optional function to override cooling policy and make fridge extra cold (for what period of time?)
void chargeOverride(void)
{
  /*
   * if button is pressed, night system is on,
   * if not, night system is off
   */
}
/*------------------------Measurements--------------------*/

//Get filtered emperature measurment
float getFridgeTemp(void)
{
  float tempReading = 0;
  tempSensor.requestTemperatures();
  tempReading = tempSensor.getTempCByIndex(0);
  return tempReading;
}

//get filtered voltage reading
float getBatteryVoltage(void){
  int adcReading = 0;
  float voltageReading = 0;

  adcReading = analogRead(VOLTAGE_READING_PIN);
  voltageReading = adcReading * 1.0 * 5.0 / 1023.0;
  voltageReading = voltageReading * 12.8 * 1.0 / 5.0;
  Serial.println(voltageReading, 2);

  return voltageReading;
}

/*--------------------------------SD CARD----------------------------*/

//intialise SD Card
void SD_CARD_init(void)
{
  bool SD_CARD_state = false;
  if (!SD.begin(SDCARD_CHIP_SELECT_PIN))
  {
    Serial.println("Card not found");
    Serial.println("Please check your connection, and reset the arduino");
    SD_CARD_state = false;
  }
  else {
    SD_CARD_state = true;
    Serial.println("CARD is initialized");
  }

  return SD_CARD_state;
}

/*--------------------------------RTC Functions----------------------*/

//intialise RTC with computers time and date
bool RTC_init(void)
{
  /* RTC state variables */
  bool parse  = false;
  bool config = false;
  bool RTC_state = false;

  // get the date and time the compiler was run
  if (getDate(__DATE__) && getTime(__TIME__)) {
    parse = true;
    // and configure the RTC with this info
    if (RTC.write(tm)) {
      config = true;
    }
  }

  while (!Serial) ; // wait for Arduino Serial Monitor
  delay(200);
  if (parse && config) {
    Serial.print("DS1307 configured Time=");
    Serial.print(__TIME__);
    Serial.print(", Date=");
    Serial.println(__DATE__);
  } else if (parse) {
    Serial.println("DS1307 Communication Error :-{");
    Serial.println("Please check your circuitry");
  } else {
    Serial.print("Could not parse info from the compiler, Time=\"");
    Serial.print(__TIME__);
    Serial.print("\", Date=\"");
    Serial.print(__DATE__);
    Serial.println("\"");
  }

  RTC_state = parse && config;

  //return RTC state
  return RTC_state;
}

//print date and time from RTC to serial monitor

void RTC_getTimeStamp(void)
{
  if (RTC.read(tm)) {
    Serial.print("Ok, Time = ");
    print2digits(tm.Hour);
    Serial.write(':');
    print2digits(tm.Minute);
    Serial.write(':');
    print2digits(tm.Second);
    Serial.print(", Date (D/M/Y) = ");
    Serial.print(tm.Day);
    Serial.write('/');
    Serial.print(tm.Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.println();
  } else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    delay(9000);
  }
  delay(1000);
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}
