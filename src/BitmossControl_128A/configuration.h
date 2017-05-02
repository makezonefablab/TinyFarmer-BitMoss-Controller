#include <EEPROM.h>
#include <ArduinoJson.h>
#include <avr/eeprom.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>


#define  DEBUG_ON true

#define USE_RTC     

#define DEVICE_ID    3

//led pin
#define LED_B       14
#define LED_G       13
#define LED_R       12
#define MODE_LED     3

// DIAL ID pin
#define DIAL1_8   35
#define DIAL1_4   34
#define DIAL1_2   33
#define DIAL1_1   32
#define DIAL2_8   39
#define DIAL2_4   38
#define DIAL2_2   37
#define DIAL2_1   36


// Clock Setting 
#define SET          26
#define STORE        27
#define INCREASE     29
#define DECREASE     31


//relay pin
#define MAX_RELAY    16
#define NUMBER_RELAY  4
#define RELAY1 25
#define RELAY2 24
#define RELAY3 23
#define RELAY4 22

#define LED_RELAY1     43
#define LED_RELAY2     42
#define LED_RELAY3     40
#define LED_RELAY4     41


//relay feedback pin
#define RELAY1FEEDBACK A0
#define RELAY2FEEDBACK A1
#define RELAY3FEEDBACK A3
#define RELAY4FEEDBACK A6


// RS485 PIN
#define RS485DIR   7

//heartbeat select
#define CONNECTION 1
#define HEARTBEAT  2

//heartbeat setting
#define HEARTBEAT_TIMER 30000
unsigned long heartbeatTimer;


//Relay value
#define RELAY_ON_VALUE 850
#define RELAY_OFF_VALUE 10


////////////////////////////////////////////////////////////////////////////////////////////
//스케쥴 저장을 위한 메모리 설정 
//https://projectgus.com/2010/07/eeprom-access-with-arduino/

#define MAX_SCHED  60

//operation setting
#define OPERAT_TIMER 60000
unsigned long operStartTime[MAX_SCHED];



// 기타 설정
#define SERIAL_SPEED                  9600 //115200 /* 115200 /* 시리얼 속도 */
#define JSON_BUFFER_SIZE              300 /* JSON 버퍼 사이즈 */
#define NUM_OF_RETRY                  5 /* 주기 정보 전송 재시도 횟수 */
#define DELAY_BETWEEN_MESSAGE         100 /* 메세지 사이의 최소 시간 */



int bmGetID()
{
   int upVal = 0;
   int lowVal = 0;
   int id = 0;

   upVal =  8 * digitalRead(DIAL1_8) + 4 * digitalRead(DIAL1_4) + 2 * digitalRead(DIAL1_2) + digitalRead(DIAL1_1) ;
   lowVal =  8 * digitalRead(DIAL2_8) + 4 * digitalRead(DIAL2_4) + 2 * digitalRead(DIAL2_2) + digitalRead(DIAL2_1) ;

   id = upVal * 10 + lowVal;

   return id;
}


//////////////////////////////////////////////////////////////
void bmSetLED(char color, int on)
{
  if (color == 'R')
  {
    digitalWrite(LED_R, !on);
  }
  else if (color == 'G')
  {
    digitalWrite(LED_G, !on);
  }
  else if (color == 'B')
  {
    digitalWrite(LED_B, !on);
  }
}

//////////////////////////////////////////////////////////////
void bmInitLED()
{
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, HIGH);
}

//////////////////////////////////////////////////////////////
void bmSetRelayLED(int _index, boolean on)
{
  int _led = 0;
  
  if (_index == 0 )
  {
    _led = LED_RELAY1;
  }
  else if (_index == 1 )
  {
    _led = LED_RELAY2;
  }
  else if (_index == 2 )
  {
    _led = LED_RELAY3;
  }
  else if (_index == 3 )
  {
    _led = LED_RELAY4;
  }

  digitalWrite(_led, !on);
}


void bmInitializePIO()
{
  // put your setup code here, to run once:
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  pinMode(SET, INPUT);
  pinMode(STORE, INPUT);
  pinMode(INCREASE, INPUT);
  pinMode(DECREASE, INPUT);

  // 다이얼 ID 핀 설정
  pinMode(DIAL1_8, INPUT);
  pinMode(DIAL1_4, INPUT);
  pinMode(DIAL1_2, INPUT);
  pinMode(DIAL1_1, INPUT);
  pinMode(DIAL2_8, INPUT);
  pinMode(DIAL2_4, INPUT);
  pinMode(DIAL2_2, INPUT);
  pinMode(DIAL2_1, INPUT);

  // LELAY STATUS LED
  pinMode(LED_RELAY1, OUTPUT);
  pinMode(LED_RELAY2, OUTPUT);
  pinMode(LED_RELAY3, OUTPUT);
  pinMode(LED_RELAY4, OUTPUT);
  digitalWrite(LED_RELAY1,HIGH);
  digitalWrite(LED_RELAY2,HIGH);
  digitalWrite(LED_RELAY3,HIGH);
  digitalWrite(LED_RELAY4,HIGH);

  //RS485 핀 설정 
  pinMode(RS485DIR, OUTPUT);
  digitalWrite(RS485DIR, LOW); //Enable low, RS485 shield waiting to receive data
  
}

