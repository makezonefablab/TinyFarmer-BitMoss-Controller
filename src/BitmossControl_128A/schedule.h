
/*
 * __eeprom_data is the magic name that maps all of the data we are 
 * storing in our EEPROM
 */
struct __eeprom_data {
  long shid;
  int count;
  int kind;
  int date;
  int hour;
  int min;
  //int seq;
  uint16_t value;
  unsigned long delayedTime;
} ;
 
typedef struct __eeprom_data _schedule;



int count_of_Schedule = 0;
_schedule mySchedule[MAX_SCHED];





//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//////////////////////////////////////////////////////////////
// Convert normal decimal numbers to binary coded decimal
//////////////////////////////////////////////////////////////
byte _decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}

//////////////////////////////////////////////////////////////
// Convert binary coded decimal to normal decimal numbers
//////////////////////////////////////////////////////////////
byte _bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}


//////////////////////////////////////////////////////////////
boolean _isToday(int _today, int _scheduleDay)
{
   int date[7] = {0x40,0x20,0x10,0x08,0x04,0x02,0x01}; // sun, mon, tues, wed, thur, fri, sat
   boolean ret = false;

   if((date[_today]&_scheduleDay) > 0)
   {
     ret = true;
   }

   return ret;
}


//////////////////////////////////////////////////////////////
void _bmPut_Schedule(int _eeAddress , _schedule sched)
{
   int count = 0;
   
   EEPROM.put(_eeAddress*sizeof(_schedule)+2, sched); // offset +2
   
   EEPROM.get(0, count);
   count ++;
   EEPROM.put(0, count);
   
}



//////////////////////////////////////////////////////////////
_schedule _bmGet_Schedule(int _eeAddress)
{
   _schedule sched;

   EEPROM.get( _eeAddress*sizeof(_schedule)+2, sched ); // offset +2

   return sched;
}

//////////////////////////////////////////////////////////////
void _bmClear_Schedule()
{
   EEPROM.put(0, 0);
}


//////////////////////////////////////////////////////////////
void bmUpdateSchedule()
{
  /////////////////////////
  // 스케쥴 꺼내기
  /////////////////////////
  EEPROM.get( 0, count_of_Schedule);

  for (int i = 0 ; i < count_of_Schedule ; i++)
  {
    mySchedule[i] = _bmGet_Schedule(i);
    
    if(DEBUG_ON == true)
    {
      Serial.println("============ Schedule ==================");
      Serial.print("count : " + String(mySchedule[i].count) +" / ");
      Serial.print("date : " + String(mySchedule[i].date)+" / ");
      Serial.print("hour : " + String(mySchedule[i].hour)+" : ");
      Serial.print("min : " + String(mySchedule[i].min)+" / ");
      Serial.print("value : " + String(mySchedule[i].value)+" / ");
      Serial.println("delayedTime : " + String(mySchedule[i].delayedTime));
    }
  }

  
  if(DEBUG_ON == true)
  {
    Serial.print("count_of_Schedule = ");
    Serial.println(count_of_Schedule);
  }
  

  _bmPrintoutLCD("New Sched ["+String(count_of_Schedule)+"]");
  delay(500);
}

//////////////////////////////////////////////////////////////
void bmCheck_run_schedule(int _nodeID,unsigned long * _relayDelayedTime, bool * _relay)
{
#ifdef USE_RTC
  DateTime _now = rtc.now();
  String status_Relay;

  for (int i = 0 ; i < count_of_Schedule ; i++)
  {
    int _hour = _now.hour();
    int _min = _now.minute();
      
    if(_isToday(_now.dayOfTheWeek(), mySchedule[i].date) == true)
    {
      //int _seq = (int)mySchedule[i].seq-1;
      uint16_t _value = mySchedule[i].value; // 넘어온 값은 32비트이므로 value가 16비트이므로 상위 16비트 제거 

      /*
       * operStartTime[i]가 0보다 크면 이미 스케쥴 중이라고 판단
       * 0인것만 스케쥴 한번 시작
       */

      /////////////////////////////////////////////
      // 스케쥴 시간되면 ON
      /////////////////////////////////////////////
      if((mySchedule[i].hour == _hour) && (mySchedule[i].min == _min) && operStartTime[i] == 0)
      {
        if(DEBUG_ON == true)
          Serial.println(String(i+1)+"st SCHE OK :" + String(_now.hour())+":"+String(_now.minute()));

        // delay time 
        _relayDelayedTime[i] = mySchedule[i].delayedTime;
        operStartTime[i] = now.unixtime();

        status_Relay = "";

        for (int i = 0; i < NUMBER_RELAY; i++)
        {
            _relay[i] = ((_value>>i)&0x01 == 1 ? true : false);      // 각 릴레이 비트 설정
            status_Relay = String(_relay[i]) + status_Relay;
        }

        bmFeedbackSend_On_Sched(_nodeID, status_Relay);
      }
    }


    if(operStartTime[i] > 0)
    {
      if(DEBUG_ON == true)
      {
        Serial.println("========== check schedule time ===========");
        Serial.println("sched time : "+String(mySchedule[i].hour)+":"+String(mySchedule[i].min));
        Serial.println("system time : "+String(_hour)+":"+String(_min) +" ["+String(operStartTime[i])+"] "+status_Relay);
        
        if(i == (count_of_Schedule-1)){Serial.println("");Serial.println("");Serial.println("");}
      }
    }
  }//for


  /////////////////////////////////////////////
  // 시간이 경과되면 릴레이 OFF
  /////////////////////////////////////////////
  for(int i = 0 ; i < count_of_Schedule; i ++)
  {
    if ((operStartTime[i] > 0) 
       && (((unsigned long)_now.unixtime()-(unsigned long)operStartTime[i]) >= (unsigned long)_relayDelayedTime[i]*60))
    {
      uint16_t _value = mySchedule[i].value; // 넘어온 값은 32비트이므로 value가 16비트이므로 상위 16비트 제거

      status_Relay = "";
      
      for (int i = 0; i < NUMBER_RELAY; i++)
      {
          if (((_value>>i)&0x01) == 1) // 스케쥴 상 "1"로 되어있는 릴레이만 false로 셋팅
          {
              _relay[i] = false;      // false
          }
          status_Relay = String(_relay[i]) + status_Relay;
      }

      if(DEBUG_ON == true)
        Serial.println(String(i+1)+"st Sched stop : "+ String(_now.hour())+":"+String(_now.minute()));
      
      operStartTime[i] = 0;
      
      bmFeedbackSend_On_Sched(_nodeID, status_Relay);
    }
  }

  
#endif
}

//////////////////////////////////////////////////////////////
void bmStoreSchedule(int _nodeID, byte * _packet)
{
    if(_packet[0] == 0xF1) // OxF1 total shid(8byte) count(index) id kind date hour min value(4byte) delay1 delay2 END   =  23bytes
    {
        if((int)_packet[1] > 0)
        {
          _schedule _sched;

          _sched.shid =     (long)(_packet[2] << 56)
                          + (long)(_packet[3] << 48)
                          + (long)(_packet[4] << 40)
                          + (long)_packet[5] << 32
                          + (long)(_packet[6] << 24)
                          + (long)(_packet[7] << 16)
                          + (long)(_packet[8] << 8)
                          + (long)_packet[9];
  
          _sched.count = (int)_packet[10]; // count
          _sched.kind = (int)_packet[12];  // id_sched.id = (int)recivedBytes[2];    // id
          _sched.date = (int)_packet[13] ; // mon :64 / tues :32 / wed :16 / thur :8 / Fri :4 / Sat :2 / Sun :1
          _sched.hour = (int)_packet[14];
          _sched.min  = (int)_packet[15];
          _sched.value= (uint16_t)(_packet[18]<<8) + (uint16_t)_packet[19]; // _packet[16], _packet[17] 부분 스킵
          _sched.delayedTime = (unsigned long)(_packet[20] << 8) + (unsigned long)_packet[21];

  
//          Serial.println("============ Schedule Protocol parsing completed ==================");
//          Serial.println((int)_packet[1]);
//          Serial.println(_sched.count);
//          Serial.println(_sched.date);
//          Serial.println(_sched.hour);
//          Serial.println(_sched.min);
//          Serial.println(_sched.seq);
//          Serial.println(_sched.value);
//          Serial.println(_sched.delayedTime);
  
          if(String((int)_packet[11]) == String(_nodeID))
          {
            // 첫번째 스케쥴 수신되면 메모리에서 카운트 0으로 수정 
            if(_sched.count == 1)
            {
              _bmClear_Schedule();
            }
            
            _bmPut_Schedule( _sched.count-1, _sched);
            bmResponseSend_After_Sched(_nodeID, _sched.shid, "00"); // 스케쥴 정상 수신
          }
        }
        else
        {
          _bmClear_Schedule(); // total 0 , clear all
          Serial.println("============ Schedule Clear !!! ==================");
        }

    }
}


