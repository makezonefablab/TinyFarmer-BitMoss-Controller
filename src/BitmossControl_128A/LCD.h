
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

// initialize the library with the numbers of the interface pins
LiquidCrystal_I2C lcd(0x3f,16,2);  // set the LCD address to 0x3f for a 16 chars and 2 line display

byte schedICON[8] = {
  B01110,
  B10101,
  B10101,
  B10111,
  B10001,
  B01110,
  B00000,
};

byte relayICON[8] = {
  B01110,
  B00100,
  B00100,
  B11111,
  B11111,
  B11111,
  B11111,
};

byte relayON[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};

byte relayOFF[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
};


uint8_t bellICON[8]  = {0x4,0xe,0xe,0xe,0x1f,0x0,0x4};
uint8_t noteICON[8]  = {0x2,0x3,0x2,0xe,0x1e,0xc,0x0};
uint8_t clockICON[8] = {0x0,0xe,0x15,0x17,0x11,0xe,0x0};
uint8_t heartICON[8] = {0x0,0xa,0x1f,0x1f,0xe,0x4,0x0};
uint8_t duckICON[8]  = {0x0,0xc,0x1d,0xf,0xf,0x6,0x0};
uint8_t checkICON[8] = {0x0,0x1,0x3,0x16,0x1c,0x8,0x0};
uint8_t crossICON[8] = {0x0,0x1b,0xe,0x4,0xe,0x1b,0x0};
uint8_t retarrowICON[8] = {  0x1,0x1,0x5,0x9,0x1f,0x8,0x4};

//RTC
#ifdef USE_RTC
#define DS3231_I2C_ADDRESS 0x68
DateTime now;
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tues", "Wed", "Thurs", "Fri", "Sat"};
DateTime startTime, endTime;
#endif




//void _bmPrintoutLCD(String _arg);
//void _bmPrintoutRelayStatus(bool * _relay);
//void _bmAddCharLCD();
//void bmInitLCD();




//////////////////////////////////////////////////////////////
void _bmPrintoutLCD(String _arg)
{

   lcd.clear();
   lcd.setCursor(0, 0);
   
#ifdef USE_RTC
   now = rtc.now();
   
   String _date = String(daysOfTheWeek[now.dayOfTheWeek()])+" ";
   String _hour = String(now.hour());
   String _min = String(now.minute());
   String _sec = String(now.second());

   if(_hour.length() == 1) _hour="0"+_hour;
   if(_min.length() == 1) _min="0"+_min;
   if(_sec.length() == 1) _sec="0"+_sec;
   
   lcd.print(_date);
   lcd.print(_hour+":"+_min+":"+_sec);
#endif
   lcd.setCursor(15, 0);
   lcd.write(byte(0));
   lcd.setCursor(0, 1);
   lcd.print(_arg);
}

//////////////////////////////////////////////////////////////
void _bmPrintoutRelayStatus(bool * _relay)
{
   //lcd.setCursor(0, 1);
   //lcd.print("                "); // clear

   lcd.setCursor(0, 1);
   lcd.write(byte(3));   // 버튼 아이콘
   lcd.print(":"); 
   
   for(int i = 0 ; i < NUMBER_RELAY ; i++)
    {
      if(_relay[i] == true)
      {
        lcd.setCursor(i+3, 1);
        lcd.write(byte(1)); // 버튼 ON 아이콘
      }
      else
      {
        lcd.setCursor(i+3, 1);
        lcd.write(byte(2)); // 버튼 OFF 아이콘
      }
    }


    // 현재 스케쥴 수 표시 
    int count = 0;
    lcd.setCursor(NUMBER_RELAY+4, 1);
    EEPROM.get(0, count);
    lcd.print("/");
    lcd.setCursor(NUMBER_RELAY+6, 1);
    lcd.write(byte(4)); // bell icon
    lcd.setCursor(NUMBER_RELAY+8, 1);
    lcd.print("["+String(count)+"]");
}

//////////////////////////////////////////////////////////////
void _bmAddCharLCD()
{
   lcd.createChar(0, schedICON);
   lcd.createChar(1, relayON);
   lcd.createChar(2, relayOFF);
   lcd.createChar(3, relayICON);
}


//////////////////////////////////////////////////////////////
void bmInitLCD()
{
   // set up the LCD's number of columns and rows:
  lcd.init();                      // initialize the lcd 
  
  for(int i = 0; i< 3; i++)
  {
    lcd.backlight();
    delay(250);
    lcd.noBacklight();
    delay(250);
  }
  
  lcd.backlight(); // finish with backlight on  

  lcd.createChar(0, clockICON);
  lcd.createChar(1, relayON);
  lcd.createChar(2, relayOFF);
  lcd.createChar(3, relayICON);

  lcd.createChar(4, bellICON);
//  lcd.createChar(5, noteICON);
//  lcd.createChar(6, clockICON);
//  lcd.createChar(7, heartICON);
//  lcd.createChar(8, duckICON);
//  lcd.createChar(9, checkICON);
//  lcd.createChar(10, crossICON);
//  lcd.createChar(11, retarrowICON);
  
  lcd.home();

  
  // Print a message to the LCD.
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("bitMossC V.1.0");
  lcd.setCursor(0, 1);
  lcd.print("booting ......");
}

