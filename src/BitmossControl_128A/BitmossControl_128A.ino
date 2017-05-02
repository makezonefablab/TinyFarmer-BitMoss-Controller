/*
 * 
 *  Project : Tiny Farmer 
 *  SubProject : Bitmoss Controller
 *  
 *  Since : 2015.11.01
 *  Author : Jang Sun yeon (Mediaflow)
 *  URL : www.tinyfarmer.com  / www.mediaflow.kr
 *  e-mail : iot@mediaflow.kr
 * 
 *  - 4 relays
 *  - LCD 16 X 2
 *  - RTC for Schedule from Cloud
 *  - EEPROM for storage of Schedules
 *  - RS485 communication  
 * 
 *  modification (2016.12.26)
 *  - schedule protocol modified 
 *  - manual protocol following schdule protocol 
 *  - feedbackSend_After_Sched(), responseSend_After_REQ(String _code) added
 *  - relay LED added, feedbackSend_On_Sched(String _seq, String _status) 
 * 
 *  modification (2016.12.29)
 *  - received schedules stored sequeancially 
 *  - control success following manual protocol
 *  
 *  modification (2017.01.04)
 *  - seperated file to each functions
 *  
 *  modification (2017.03.20)
 *  - System Time Protocol between Hub and Node
 * 
 *  modification (2017.03.28)
 *  - protocol add 1byte more to "value"
 *  - protocol add new 4byte more,"shid", in schedule
 *  
 *  modification (2017.03.31)
 *  - manual control protocol : feedback value 16bits
 *  
 *  modification (2017.04.10)
 *  - fix LED (R,G,B)pin 
 *  
 *  modification (2017.04.24)
 *  - add manual time setting (buttons)
 *  
 */

#include "configuration.h"
#include "LCD.h"
#include "protocols.h"
#include "schedule.h"




int  nodeID = DEVICE_ID;                                /* 메시 네트워크 Node 아이디, 스위치로부터 해당 값을 새로 설정 함 */
bool isSendFail = false;                                /* 메시 네트워크 전송 실패 여부 확인 Flag */


//relay state check
bool relay[NUMBER_RELAY] = {0, 0, 0, 0};
unsigned long relayDelayedTime[NUMBER_RELAY] = {0, 0, 0, 0};
bool check = false;
int  relayPin[NUMBER_RELAY] = {RELAY1, RELAY2, RELAY3, RELAY4};
int  relayCheckPin[NUMBER_RELAY] = {A0, A1, A3, A6};
bool feedBack[NUMBER_RELAY] = {0, 0, 0, 0};
int  sequenceID = 0;
unsigned long preHeartBeatTime = 0;
unsigned long preRefreshTime = 0;

////////////////////count_mac////////
int     mac_count = 0;
String  inputString = "";         // a string to hold incoming data
String  Data = "";

_bmProtocolST _stProto;





bool timeSetMODE = false;


void setTimeClock()
{
    if(digitalRead(SET) == LOW)
    {
      
      timeSetMODE = true;
      
      lcd.clear();
      lcd.setCursor(0, 0);

      delay(500);
      
      now = rtc.now();
  
      uint16_t _year = now.year();
      int _month = now.month();
      int _day = now.day();
      
      int _date = now.dayOfTheWeek();
  
      int _hour = now.hour();
      int _min = now.minute();
  
      int _mode = 0;
  
         
      while(timeSetMODE == true)
      {
         if(_mode == 0)      // year
         {
           lcd.clear();
           lcd.print("YEAR :");
           lcd.setCursor(0, 1);
           lcd.print(_year);
  
           if(digitalRead(INCREASE) == LOW)
           {
             _year = _year + 1;
             delay(200);
           }
  
           if(digitalRead(DECREASE) == LOW)
           {
             _year = _year - 1;
             delay(200);
           }
         }
         else if(_mode == 1) // month
         {
           lcd.clear();
           lcd.print("MONTH :");
           lcd.setCursor(0, 1);
           lcd.print(_month);
  
           if(digitalRead(INCREASE) == LOW)
           {
             _month = _month + 1;
  
             if(_month > 12) _month = 1;
             delay(200);
           }
  
           if(digitalRead(DECREASE) == LOW)
           {
             _month = _month - 1;
  
             if(_month < 1) _month = 12;
             delay(200);
           }
           
         }
         else if(_mode == 2) // day
         {
           lcd.clear();
           lcd.print("DAY :");
           lcd.setCursor(0, 1);
           lcd.print(_day);
  
           if(digitalRead(INCREASE) == LOW)
           {
             _day = _day + 1;
  
             if(_day > 31) _day = 1;
             delay(200);
           }
  
           if(digitalRead(DECREASE) == LOW)
           {
             _day = _day - 1;
  
             if(_day < 1) _day = 31;
             delay(200);
           }
         }
         else if(_mode == 3) // date
         {
           lcd.clear();
           lcd.print("DATE :");
           lcd.setCursor(0, 1);
           lcd.print(_date);
  
           if(digitalRead(INCREASE) == LOW)
           {
             _date = _date + 1;
  
             if(_date > 6) _date = 0;
             delay(200);
           }
  
           if(digitalRead(DECREASE) == LOW)
           {
             _date = _date - 1;
  
             if(_date < 0) _date = 6;
             delay(200);
           }
           
         }
         else if(_mode == 4) // hour
         {
           lcd.clear();
           lcd.print("HOUR :");
           lcd.setCursor(0, 1);
           lcd.print(_hour);
  
           if(digitalRead(INCREASE) == LOW)
           {
             _hour = _hour + 1;
  
             if(_hour > 23) _hour = 0;
             delay(200);
           }
  
           if(digitalRead(DECREASE) == LOW)
           {
             _hour = _hour - 1;
  
             if(_hour < 0) _hour = 24;
             delay(200);
           }
         }
         else if(_mode == 5) // min
         {
           lcd.clear();
           lcd.print("MINUTE :");
           lcd.setCursor(0, 1);
           lcd.print(_min);

  
           if(digitalRead(INCREASE) == LOW)
           {
             _min = _min + 1;
  
             if(_hour > 59) _min = 0;
             delay(200);
           }
  
           if(digitalRead(DECREASE) == LOW)
           {
             _min = _min - 1;
  
             if(_min < 0) _min = 59;
             delay(200);
           }
         }



         // 다음셋팅으로 이동
         if(digitalRead(STORE) == LOW)
         {
           if(_mode == 0)      // year
           {
             _mode = 1;
           }
           else if(_mode == 1) // month
           {
             _mode = 2;
           }
           else if(_mode == 2) // day
           {
             _mode = 4;
           }
           /*else if(_mode == 3) // date
           {
             _mode = 4;
           }*/
           else if(_mode == 4) // hour
           {
             _mode = 5;
           }
           else if(_mode == 5) // min
           {
             _mode = 0;
           }
  
           delay(500);
         }




         // 현재 설정값 시계에 저장
         if(digitalRead(SET) == LOW)
         {
            rtc.adjust(DateTime(_year, _month, _day,_hour, _min,0));
            timeSetMODE = false;
            _bmPrintoutLCD("Time Setting");
            delay(1000);
         }

         delay(200);
      }
    }
  
}




void setup() {

  bmInitializePIO();

  // 릴레이 핀 설정
  for (int i = 0; i < NUMBER_RELAY; i++) {
    pinMode(relayPin[i], OUTPUT);
    digitalWrite(relayPin[i], LOW);
    relay[i] = LOW; 
    _stProto.val[i] = false;
  }
  
  // 노드 아이디 세팅 (다이얼 스위치로부터 입력 받음)
  nodeID = bmGetID();

  
  Serial1.begin(9600);
  Serial1.setTimeout(5000);
  
  bmInitLED();
  
  Serial.begin(9600);  //can't be faster than 19200 for softserial

  if(DEBUG_ON == true)
  {
    Serial.print("node ID : ");
    Serial.println(nodeID);
    Serial.println("Bitmoss Debugging");
  }
  
  /////////////////////////////////////////
  // RTC 설정
  /////////////////////////////////////////
  
#ifdef USE_RTC
  delay(3000); // wait for console opening
  
  if (! rtc.begin()) {
    if(DEBUG_ON == true) Serial.println("Couldn't find RTC");
    while (1);
  }

  //if (rtc.lostPower()) 
  {
    if(DEBUG_ON == true) Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  
#endif

  ///////////////////////////////////////////
  // LCD 설정
  ///////////////////////////////////////////
  bmInitLCD();
  
  // 스케쥴 꺼내기
  bmUpdateSchedule();
  
  delay(1000);
  
  //bmStatusSend(nodeID, relay);
  delay(1000);
  
  digitalWrite(MODE_LED, HIGH);

  _bmPrintoutLCD("SYSTEM OK");


  bmRequestSchedule(nodeID); // 스케쥴 요청


  digitalWrite(LED_B, LOW);

}


void loop() {


  digitalWrite(LED_R, LOW);

  setTimeClock();

  
  if (stringComplete == true) 
  {
    /////////////////////////////////////////
    // Manual Protocol
    /////////////////////////////////////////
    if(receivedBytes[0] == HEADER_MANUAL_PROTO)
    {
        // start(1Byte)  ptcd(1Byte)  relayHist(8Byte) id(1Byte)  kind(1byte)  delay1(1Byte)  delay2(1Byte)  
        // seq(1Byte)  val1(1Byte)  payload Len(1Byte)  Payload   end(1Byte)start(1Byte)  ptcd(1Byte)
       
        digitalWrite(LED_G, LOW);
        
        _stProto = bmParseProtocol(receivedBytes);     
         
        if(DEBUG_ON == true)
        {
          Serial.println("============ Manual Protocol parsing completed ==================");
          Serial.println(_stProto.ptCd);
          Serial.println(_stProto.relayHistoryId);
          Serial.println(_stProto.id);
          Serial.println(_stProto.operTime);
          Serial.println(_stProto.seq);
          Serial.println(_stProto.val);
        }

        /*
         * {
         *  "ptCd":"07",
         *  "relayHistoryId":"1",
         *  "id":"1",
         *  "operTime":"1",
         *  "val":"1001",
         *  "seq":"3"
         *  } 
         */
         
        if(_stProto.ptCd == "7" && _stProto.id == String(nodeID))
        {
          /*
           *  seq값은 제어할 릴레이 번호, 제어값은 val에 해당 변환 상태 값 (1 아니면 0) 
           */
          sequenceID = _stProto.seq.toInt();
          sequenceID = sequenceID - 1;
          relay[sequenceID] = (_stProto.val[7] == '1' ? true : false);
          check = true;
        }

        digitalWrite(LED_G, HIGH);
    }
    /////////////////////////////////////////
    // Schedule Protocol
    /////////////////////////////////////////
    else if(receivedBytes[0] == HEADER_SCHED_PROTO) // OxF1 count id kind date hour min seq value delay1 delay2 END   =  12bytes
    {
        digitalWrite(LED_G, LOW);
        if(receivedBytes[11] == nodeID)
        {
            bmUpdateSchedule(); // 스케쥴 꺼내기
        }

        digitalWrite(LED_G, HIGH);
    }

    stringComplete = false;
  }


  /////////////////////////////////////////////
  // 스케쥴 체크
  ////////////////////////////////////////////
  bmCheck_run_schedule(nodeID, relayDelayedTime, relay);

  digitalWrite(LED_R, HIGH);

  if (check == true) 
  {
    
#ifdef USE_RTC    
    //startTime = rtc.now();  // 명령이 시작된 시간 저장
#endif

    for(int i = 0 ; i < NUMBER_RELAY ; i++)
    {
      digitalWrite(relayPin[i], relay[i]);
      bmSetRelayLED(i, relay[i]);
    }

    _bmPrintoutRelayStatus(relay);
    
    bmFeedback(relay, feedBack,&_stProto);
    bmTCPSend(nodeID,_stProto,relay);
    check = false;
  }
  else
  {
    for(int i = 0 ; i < NUMBER_RELAY ; i ++)
    {
      digitalWrite(relayPin[i], relay[i]);
      bmSetRelayLED(i, relay[i]);
    }

    
  }

  // "0"은 수동으로 제어된 신호이므로, 긴 시간 제어되게 임시 변경
  if(_stProto.operTime == "0")
  {
    //operSpenTime[sequenceID] = 999;
  }
  else
  {
    //operSpenTime[sequenceID] = _stProto.operTime.toInt();
  }





  //  LCD refresh
  if ((unsigned long)(millis() - preRefreshTime) > 1000)
  {
    preRefreshTime = millis();
    _bmPrintoutLCD("");
    _bmPrintoutRelayStatus(relay);
  }
  




  //  heartbeatTimer
  if ((unsigned long)(millis() - preHeartBeatTime) > HEARTBEAT_TIMER)
  {
    
    
    preHeartBeatTime = millis();
    Serial.println("heartbeatTimer ===== ");
    bmStatusSend(nodeID,relay);

    
    

#ifdef USE_RTC

    DateTime _now = rtc.now();
    
    if(DEBUG_ON == true)
    {
      Serial.print(_now.year(), DEC);
      Serial.print('/');
      Serial.print(_now.month(), DEC);
      Serial.print('/');
      Serial.print(_now.day(), DEC);
      Serial.print(" (");
      Serial.print(daysOfTheWeek[_now.dayOfTheWeek()]);// "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
      Serial.print(") ");
      Serial.print(_now.hour(), DEC);
      Serial.print(':');
      Serial.print(_now.minute(), DEC);
      Serial.print(':');
      Serial.print(_now.second(), DEC);
      Serial.println();
      Serial.print(" since midnight 1/1/1970 = ");
      Serial.print(_now.unixtime());
      Serial.print("s = ");
      Serial.print(_now.unixtime() / 86400L);
      Serial.println("d");
    }
#else

#endif
  }
  

  delay(100);

}




//////////////////////////////////////////////////////////////
void serialEvent1() 
{

  while(Serial1.available()) 
  {
    digitalWrite(LED_G, LOW);
    
    byte inChar = (byte)Serial1.read();
    //Serial.println (inChar);

    int ret = bmParse_Receive_Parse(inChar);

    if (ret == RECEIVING_PROTOCOL_OK)
    {
      
    }
    else if (ret == MANUAL_PROTOCOL_OK)
    {
      
    }
    else if (ret == MANUAL_PROTOCOL_FAIL)
    {
      bmFeedback_error(nodeID,"99",_stProto); // 패킷수신 실패 
    }
    else if (ret == SCHED_PROTOCOL_OK)
    {
      bmStoreSchedule(nodeID, receivedBytes); // add schedule into EEPROM
    }
    else if (ret == SCHED_PROTOCOL_FAIL)
    {
      
    }
    else if (ret == TIME_PROTOCOL_OK)
    {
      
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject((char*)receivedBytes);
      
      Serial.println("(parsing Jason)");
      
      if (root.success())
      {
        String _time = root["time"];
        String _year = root["year"];
        String _month = root["month"];
        String _day = root["day"];
        String _hour = root["hour"];
        String _min = root["min"];
        String _sec = root["sec"];

        root.printTo(Serial);

        if(_time == "NOW")
            rtc.adjust(DateTime(_year.toFloat(), _month.toInt(), _day.toInt(), _hour.toInt(), _min.toInt(), _sec.toInt()));

      }
    }

    digitalWrite(LED_G, HIGH);
        

  }//while
}

