/*-------------------------INCLUDES-------------------*/

/*-----RTC_MODULE------*/
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

/*----SD_CARD_MODULE---*/
#include <SPI.h>
#include <SD.h>

/*---TEMO_PROP_MODULE--*/
#include <OneWire.h>
#include <DallasTemperature.h>


/*-----------------------DEFINES---------------------*/
/*---TEMP_PROBE_DATA_WIRE_PIN---*/
#define ONE_WIRE_BUS 2


/*---Voltage Measurement Pin----*/
#define VOLTAGE_READING_PIN A0

/*------SD CARD PIN----*/
#define SDCARD_CHIP_SELECT_PIN 4
/*--------------------GLOBAL_VARIABLES-----------------*/

/* array for month names */
const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};


/*---used by Temp Sensor module---*/
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);


/* structure for timestamp members */
tmElements_t tm;






/*--------------------------------------------------------------*
                      FUNCTIONS DEFINITIONS
  --------------------------------------------------------------*/


void setup() {

  System_init();

}


void loop() {
  System_logData();
  delay(2000);
}


void System_init(void) {
  /* begin the serial connection */
  Serial.begin(9600);

  /* initialize RTC */
  RTC_init();

  /* intialize temp sensor */
  tempSensor.begin();

}


void System_logData(void)
{

  /* log function */
  /*
  float currentVoltage = 0;
  RTC_getTimeStamp();
  currentVoltage = getBatteryVoltage();

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
  
  File dataLoggerFile = SD.open("TestFile.txt", FILE_WRITE);

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
  float dailyCharge = getBatteryVoltage();

  return dailyCharge;
  
}

//adjust fridge power draw according to battery charge (may fully or partly discharge battery)
float decideFridgeTemp(void){
  float dailyCharge = getDailyCharge();
  /*
   * 
   * 
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
   */
}

void system_On_Off(void)
{
  /*
   * if button is pressed, night system is on,
   * if not, night system is off
   */
}
/*------------------------Temperature Measurement Section--------------------*/

//Filter acurate fridge temperature sensor reading
float getFridgeTemp(void)
{
  float tempReading = 0;
  tempSensor.requestTemperatures();
  tempReading = tempSensor.getTempCByIndex(0);
  return tempReading;
}


/*-------------------------Voltage Measurement Section-----------------------*/

/*
    Function Name: VOLT_getVoltageReading
    Args         : None
    Outs         : None
    Description  : get the voltage reading

*/

//Filter acurate battery voltage sensor reading
float getBatteryVoltage(void)
{
  int adcReading = 0;
  float voltageReading = 0;

  adcReading = analogRead(VOLTAGE_READING_PIN);
  voltageReading = adcReading * 1.0 * 5.0 / 1023.0;
  voltageReading = voltageReading * 12.8 * 1.0 / 5.0;
  Serial.println(voltageReading, 2);

  return voltageReading;
}

/*--------------------------------SD CARD Section----------------------------*/
/*
    Function Name: SD_CARD_init
    Args         : None
    Outs         : SD_CARD_state
    Description  : intializied SD Card

*/

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

/*--------------------------------RTC Functions Section----------------------*/

/*
    Function Name: RTC_init
    Args         : None
    Outs         : RTC_state
    Description  : intializied RTC with intializied time and date
*/


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

  /* return the state of RTC */
  return RTC_state;
}


/*
    Function Name: RTC_getTimeStamp
    Args         : None
    Outs         : RTC_state
    Description  : get the date and time from RTC
*/

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


/*
    Function Name: getTime
    Args         : None
    Outs         : operaion state
    Description  : get the compiler time
*/
bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}


/*
    Function Name: getDate
    Args         : None
    Outs         : operaion state
    Description  : get the compiler data
*/

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
