// Monitor the ambient conditions in a storage locker and log them to an SD card, to track the safety and health of contents
// This system is designed to run on very low power for a very long time, so lots of considerations made for power saving
// Button for user to manually wake on pin 3
// SHT21 on I2C for humidity and temperature
// Micro SD slot on pin A0 for chip select over SPI
// RTC PCF8563 on I2C and interupt pin 1
// Includes MMA8451 accelerometer with interrupt on Pin 1, for future use 

//Include lots of libraries
#include <SHT21.h>
#define PCF8563address 0x51
#include <SD.h>
#include <Wire.h>
#include <SPI.h>
#include <avr/sleep.h>

//Name objects
SHT21 sht; 
File file;

//Variables
float temp;   // variable to store temperature
float humidity; // variable to store hemidity

//Definitions
const int chipSelect = 0;
const int LED = 2;

byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
byte alarmMinute, alarmHour, alarmDay, alarmDayOfWeek;
String days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

byte bcdToDec(byte value)
{
  return ((value / 16) * 10 + value % 16);
}

byte decToBcd(byte value){
  return (value / 10 * 16 + value % 10);
}


void setup() {
  // put your setup code here, to run once:
  Wire.begin();    // begin Wire(I2C)
  
  alarmMinute = 00;
  alarmHour = 00;
  setPCF8563alarm();
  
  //ADCSRA = 0;  //disable ADC
  
  pinMode(LED,OUTPUT);
  digitalWrite(LED, HIGH);

  if (!SD.begin(chipSelect)){
    while(1){
      digitalWrite(LED, HIGH);
      delay(100);
      digitalWrite(LED, LOW);
      delay(100);
    }
  }
  if (SD.exists("DataLog.csv") == 0){
    file = SD.open("DataLog.csv", FILE_WRITE);
    file.println(F("Storage Locker Data Logger"));
    file.println(F("Designed By: Jim Heaney"));
    file.println(F("Automatic Sampling: Hourly"));
    file.println(F("Manual Sampling: Enabled"));
    file.println(F("Month, Day, Year, Hour, Minute, Temperature, Humidity"));
    file.close();
  }

  
  file = SD.open("DataLog.csv", FILE_WRITE);
  file.println("");
  file.println(F("Powered Up - Initial Measurement:"));
  file.close();
  delay(100);
  digitalWrite(LED, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  PCF8563alarmOff();
  digitalWrite(LED, HIGH);
  readPCF8563();
  temp = sht.getTemperature();
  humidity = sht.getHumidity();
  file = SD.open("DataLog.csv", FILE_WRITE);
  file.print(month);
  file.print(F(","));
  file.print(dayOfMonth);
  file.print(F(","));
  file.print(year);
  file.print(F(","));
  file.print(hour);
  file.print(F(","));
  file.print(minute);
  file.print(F(","));
  file.print(temp);
  file.print(F(","));
  file.println(humidity);
  file.close();
  setPCF8563alarm();
  delay(100);
  digitalWrite(LED, LOW);
  delay(50);
  goToSleep();
}

void goToSleep(){
  digitalWrite(LED, LOW);
  sleep_enable();
  attachInterrupt(3, wakeUp, LOW); //Woken by manual button
  attachInterrupt(1, wakeUp, LOW); //Woken by RTC
  delay(1000);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_cpu();
  digitalWrite(LED,HIGH);
}

void wakeUp(){
  sleep_disable();
  detachInterrupt(4);
  detachInterrupt(2);
}

void setPCF8563alarm()
// this sets the alarm data to the PCF8563
{
  byte am, ah, ad, adow;
  am = decToBcd(alarmMinute);
  am = am | 100000000; // set minute enable bit to on
  ah = decToBcd(alarmHour);
  ah = 10000000; // set hour enable bit to off
  ad = decToBcd(alarmDay);
  ad = ad | 000000000; // set day of week alarm enable bit off
  adow = decToBcd(alarmDayOfWeek);
  adow = ad | 000000000; // set day of week alarm enable bit off

  // write alarm data to PCF8563
  Wire.beginTransmission(PCF8563address);
  Wire.write(0x09);
  Wire.write(am);  
  Wire.write(ah);

  // optional day of month and day of week (0~6 Sunday - Saturday)
  /*
  Wire.write(ad);
  Wire.write(adow);  
  */
  Wire.endTransmission();

  // optional - turns on INT_ pin when alarm activated  
  // will turn off once you run void PCF8563alarmOff()
  Wire.beginTransmission(PCF8563address);
  Wire.write(0x01);
  Wire.write(B00000010);
  Wire.endTransmission();
}

void PCF8563alarmOff()
// turns off alarm enable bits and wipes alarm registers. 
{
  byte test;
  // first retrieve the value of control register 2
  Wire.beginTransmission(PCF8563address);
  Wire.write(0x01);
  Wire.endTransmission();
  Wire.requestFrom(PCF8563address, 1);
  test = Wire.read();

  // set bit 3 "alarm flag" to 0
  test = test - B00001000;

  // now write new control register 2  
  Wire.beginTransmission(PCF8563address);
  Wire.write(0x01);
  Wire.write(test);
  Wire.endTransmission();
}

void readPCF8563()
// this gets the time and date from the PCF8563
{
  Wire.beginTransmission(PCF8563address);
  Wire.write(0x02);
  Wire.endTransmission();
  Wire.requestFrom(PCF8563address, 7);
  second     = bcdToDec(Wire.read() & B01111111); // remove VL error bit
  minute     = bcdToDec(Wire.read() & B01111111); // remove unwanted bits from MSB
  hour       = bcdToDec(Wire.read() & B00111111); 
  dayOfMonth = bcdToDec(Wire.read() & B00111111);
  dayOfWeek  = bcdToDec(Wire.read() & B00000111);  
  month      = bcdToDec(Wire.read() & B00011111);  // remove century bit, 1999 is over
  year       = bcdToDec(Wire.read());
}
