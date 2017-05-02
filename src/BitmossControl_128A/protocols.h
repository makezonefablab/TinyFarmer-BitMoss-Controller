
#define HEADER_MANUAL_PROTO   0xF0
#define HEADER_SCHED_PROTO    0xF1

#define MANUAL_PROTOCOL_OK    0
#define MANUAL_PROTOCOL_FAIL  1
#define SCHED_PROTOCOL_OK     2
#define SCHED_PROTOCOL_FAIL   3
#define RECEIVING_PROTOCOL_OK 4
#define TIME_PROTOCOL_OK      5

String resCd = "0000";

boolean stringComplete = false;  // whether the string is complete
String  recivedData = "";
byte    receivedBytes[25];
boolean receivedStart = false;
int     index = 0;
byte xorByte = 0x00;
int receivedIndex = 0;

//manual control protocol
typedef struct protocolSt {
  //start(1Byte)  ptcd(1Byte)  relayHist(8Byte) id(1Byte)  kind(1byte)  delay1(1Byte)  
  //delay2(1Byte)  seq(1Byte)  val1(1Byte)  payload Len(1Byte)  Payload   end(1Byte)

  String ptCd;
  String relayHistoryId;
  String id;
  String kind;
  String operTime;
  String seq;
  String val;
  String resCd;
  
} _bmProtocolST;


_bmProtocolST bmParseProtocol(byte * _proto)
{
  // Manual Protocol
  //
  // start(1Byte)  ptcd(1Byte)  relayHist(8Byte) id(1Byte)  kind(1byte)  delay1(1Byte)  delay2(1Byte)  seq(1Byte)  val(1Byte)  payload Len(1Byte)  Payload   end(1Byte)
  // 0xF0          0x07         [0x01~8ea]       0x08       0x00         0x00           0x00              0x01        0x0F          0x00             0xF1      END       = 19bytes
  //
  // End = ptcd ^ relayHist ^ id ^ opertime1 ^ opertime2 ^ seq ^ val ^ payload Len
  //
  _bmProtocolST _st ;

  _st.ptCd = String((int)_proto[1]);
  long _relayHistoryIdL = (long)(_proto[2] << 56)
                          + (long)(_proto[3] << 48)
                          + (long)(_proto[4] << 40)
                          + (long)(_proto[5] << 32)
                          + (long)(_proto[6] << 24)
                          + (long)(_proto[7] << 16)
                          + (long)(_proto[8] << 8)
                          + (long)_proto[9];

                          
  _st.relayHistoryId = String(_relayHistoryIdL);
  _st.id = String((int)_proto[10]);
  _st.kind = String((int)_proto[11]);
  _st.operTime = String((int)_proto[12]<<8 + (int)_proto[13]) ;
  _st.seq = String((int)_proto[14]);
  _st.val = String((int)_proto[15]>>7 & 0x01) // 11111111
               +String((int)_proto[15]>>6 & 0x01)
               +String((int)_proto[15]>>5 & 0x01)
               +String((int)_proto[15]>>4 & 0x01)
               +String((int)_proto[15]>>3 & 0x01)
               +String((int)_proto[15]>>2 & 0x01)
               +String((int)_proto[15]>>1 & 0x01)
               +String((int)_proto[15] & 0x01);

  return _st;
}


//////////////////////////////////////////////////////////////
void bmFeedback(bool * _relay,bool * _feedBack, _bmProtocolST * _st )
{
  bool flag = false;
  delay(1000);
  for (int i = 0; i < NUMBER_RELAY; i++) {
//    if (analogRead(relayCheckPin[i]) > RELAY_ON_VALUE) {
//      feedBack[i] = true;
//    }
//    else if (analogRead(relayCheckPin[i]) < RELAY_OFF_VALUE) {
//      feedBack[i] = false;
//    }
  }

  /*
   * 연결된 아날로그 센서와 비교하여 값이 릴레이 제어 값이랑 달라야 
   */
  for (int i = 0; i < NUMBER_RELAY; i++) {
    if (_relay[i] != _feedBack[i]) 
    {
      //digitalWrite(relayPin[i], LOW); //현재 피드백센서가 없으므로 일단 주석처리
      _st->resCd[i] = '1';
      // Error message send
      Serial.print("error : ");
      Serial.println(i);

      flag = true;
    }
    else {
      _st->resCd[i] = '0';
    }

  }

  if (flag == false) {
    resCd = "0000";
  }

  /////////////////////////////////////////
  // 피드백 센서가 적용되기전까지는 resCd = status ..
  String _resCd ="";
  for (int i = 0; i < NUMBER_RELAY; i++)
  {
    _resCd += String(_relay[i]);
  }
  resCd = _resCd;
  /////////////////////////////////////////

}


//////////////////////////////////////////////////////////////
// 수동 응답 프로토콜
//////////////////////////////////////////////////////////////
void bmTCPSend(int _nodeID, _bmProtocolST _st, bool * _relay) {
  
  String msg = "";
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  String status_Relay ="";
  String overhead ="";

  if(_relay != NULL)
  {
    for (int i = 0; i < NUMBER_RELAY; i++)
    {
      status_Relay = String(_relay[i]) + status_Relay;
    }
  
    for (int i = 0; i < (MAX_RELAY - NUMBER_RELAY) ; i++)
    {
      overhead += "0";
    }
  }
  else
  {
    status_Relay = "00000000";
    overhead = "00000000";
  }

  root["ptCd"] = "08";
  root["relayHistoryId"] = _st.relayHistoryId;
  root["id"] = String(_nodeID);
  root["val"] = overhead + status_Relay;;
  root["resCd"] = _st.resCd; // 00:정상, 99:패킷수신실패 

  digitalWrite(RS485DIR, HIGH); //Enable low, RS485 shield sending data
  root.printTo(Serial1);
  Serial1.println("");
  digitalWrite(RS485DIR, LOW); //Enable low, RS485 shield waiting to receive data
  
  //DEBUG
  if(DEBUG_ON == true)
  {
    root.printTo(Serial);
    Serial.println("");
  }

}


// 상태 (허트비트)
//////////////////////////////////////////////////////////////
void bmStatusSend(int _nodeID,bool * _relay) {
  
  String msg = "";
  StaticJsonBuffer<JSON_BUFFER_SIZE> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  
  String status_Relay ="";
  String overhead ="";
  
  for (int i = 0; i < NUMBER_RELAY; i++)
  {
    status_Relay = String(_relay[i]) + status_Relay;
  }

  for (int i = 0; i < (MAX_RELAY - NUMBER_RELAY) ; i++)
  {
    overhead += "0";
  }
  
  root["ptCd"] = "02";
  root["dtCd"] = "03";
  root["id"] = String(_nodeID);
  root["status"] = overhead+status_Relay;//status_Relay;

  digitalWrite(RS485DIR, HIGH); //Enable low, RS485 shield sending data
  root.printTo(msg);
  Serial1.println(msg);
  digitalWrite(RS485DIR, LOW); //Enable low, RS485 shield waiting to receive data

  //DEBUG
  if(DEBUG_ON == true)
  {
    //root.printTo(msg);
    Serial.println(msg);
  }

}



//////////////////////////////////////////////////////////////
// 서버에게 스케쥴 리스트 요청
//////////////////////////////////////////////////////////////
void bmRequestSchedule(int _nodeID) {
  
  String msg = "";
  StaticJsonBuffer<JSON_BUFFER_SIZE> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  
  String status_Relay ="";

  root["ptCd"] = "01";
  root["dtCd"] = "03";
  root["id"] = String(_nodeID);

  digitalWrite(RS485DIR, HIGH); //Enable low, RS485 shield sending data
  root.printTo(Serial1);
  Serial1.println("");
  digitalWrite(RS485DIR, LOW); //Enable low, RS485 shield waiting to receive data

  //DEBUG
  if(DEBUG_ON == true)
  {
    root.printTo(Serial);
    Serial.println("");
  }

}


//////////////////////////////////////////////////////////////
void bmFeedback_error(int _nodeID, String error_code, _bmProtocolST _st)
{
  _st.resCd = error_code;
  bmTCPSend(_nodeID,_st, NULL);
}

//////////////////////////////////////////////////////////////
// 스케쥴 실행 후 결과 리턴 : 릴레이 현재 상태  반영
//////////////////////////////////////////////////////////////
//void bmFeedbackSend_After_Sched(int _nodeID,_bmProtocolST _st) {
//  
//  String msg = "";
//  StaticJsonBuffer<200> jsonBuffer;
//  JsonObject& root = jsonBuffer.createObject();
//
//  root["ptCd"] = "08";
//  root["id"] = String(_nodeID);
//  root["seq"] = _st.val;
//  root["status"] = _st.resCd;
//
//  digitalWrite(RS485DIR, HIGH); //Enable low, RS485 shield sending data
//  root.printTo(Serial1);
//  Serial1.println("");
//  digitalWrite(RS485DIR, LOW); //Enable low, RS485 shield waiting to receive data
//  
//  //DEBUG
//  root.printTo(Serial);
//  Serial.println("");
//
//}

//////////////////////////////////////////////////////////////
// 스케쥴 프로토콜 수신 후 응답 
//////////////////////////////////////////////////////////////
void bmResponseSend_After_Sched(int _nodeID, long _shId ,String _code) {
  
  String msg = "";
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  root["ptCd"] = "14";
  root["shid"] = String(_shId);
  root["id"] = String(_nodeID);
  root["cd"] = _code; //00(정상), 98(재전송), 99(에러)
  
  digitalWrite(RS485DIR, HIGH); //Enable low, RS485 shield sending data
  root.printTo(Serial1);
  Serial1.println("");
  digitalWrite(RS485DIR, LOW); //Enable low, RS485 shield waiting to receive data
  
  //DEBUG
  if(DEBUG_ON == true)
  {
    root.printTo(Serial);
    Serial.println("");
  }

}

//////////////////////////////////////////////////////////////
// 스케쥴 실행 후 결과 전송 (실행 및 실행 완료)
//////////////////////////////////////////////////////////////
void bmFeedbackSend_On_Sched(int _nodeID, String _status) {
  
  String msg = "";
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  String overhead ="";
  
  for (int i = 0; i < (MAX_RELAY - NUMBER_RELAY) ; i++)
  {
    overhead += "0";
  }
  

  root["ptCd"] = "08";
  root["id"] = String(_nodeID);
  root["status"] = overhead+_status; // 16bit relay result
  
  digitalWrite(RS485DIR, HIGH); //Enable low, RS485 shield sending data
  root.printTo(Serial1);
  Serial1.println("");
  digitalWrite(RS485DIR, LOW); //Enable low, RS485 shield waiting to receive data
  
  //DEBUG
  if(DEBUG_ON == true)
  {
    root.printTo(Serial);
    Serial.println("");
  }

}


int bmParse_Receive_Parse(byte _p)
{
    int ret = RECEIVING_PROTOCOL_OK;
    
    // 수신된 패킷이 수동제어 헤더 바이트인지 검사
    if(_p == HEADER_MANUAL_PROTO && receivedStart == false)
    {
      memset(receivedBytes,0x00,19);
      receivedBytes[0] = HEADER_MANUAL_PROTO;

      xorByte = 0x00;
      stringComplete = false;
      receivedIndex = 0;
      receivedStart = true;
    }
    // 수신된 패킷이 스케쥴 헤더 바이트인지 검사 
    else if(_p == HEADER_SCHED_PROTO && receivedStart == false)
    {
      memset(receivedBytes,0x00,12);
      receivedBytes[0] = HEADER_SCHED_PROTO;

      xorByte = 0x00;
      stringComplete = false;
      receivedIndex = 0;
      receivedStart = true;
    }
//    else if((char)_p == '{' && receivedStart == false)
//    {
//      memset(receivedBytes,0x00,sizeof(receivedBytes));
//      receivedBytes[0] = (byte)'{';
//      
//      
//      stringComplete = false;
//      receivedIndex = 0;
//      receivedStart = true;
//    }

    // HEADER 수신 성공 시
    if(receivedStart == true)
    {
      
      receivedBytes[receivedIndex] = _p;

      // Manual Protocol
      //
      // start(1Byte)  ptcd(1Byte)  relayHist(8Byte) id(1Byte)  kind(1byte)  delay1(1Byte)  delay2(1Byte)  seq(1Byte)  val(1Byte)  payload Len(1Byte)  Payload   end(1Byte)
      // 0xF0          0x07         [0x01~8ea]       0x08       0x00         0x00           0x00              0x01        0x0F         0x00             0xF1      END       = 19bytes
      //
      // End = ptcd ^ relayHist ^ id ^ opertime1 ^ opertime2 ^ seq ^ val ^ payload Len
      //
      if(receivedBytes[0] == HEADER_MANUAL_PROTO)
      {
          // Payload Len 까지만 Xor 처리
          if(receivedIndex >= 1 && receivedIndex <= 16)
          {
            xorByte = xorByte^_p;
          }
    
          //////////////////////////////////////////////////////////
          // 정상적으로 프토토콜이 받아졌는지 검사
          //////////////////////////////////////////////////////////
          if(xorByte == receivedBytes[18] && receivedIndex == 18)
          {
            stringComplete = true;
            receivedStart = false;
            receivedIndex = 0;

            //bmResponseSend_After_REQ(nodeID, "00"); // 패킷수신정상

            ret = MANUAL_PROTOCOL_OK;
          }
          else if(xorByte != receivedBytes[18] && receivedIndex == 18) // 깨진 프로토콜 또는 쓰레기 문자 수신 18byte 시
          {
            receivedStart = false;
            receivedIndex = 0;

            if(DEBUG_ON == true)
              Serial.println("NEW COMMAND RECEIVED !!!!");
            
            ret = MANUAL_PROTOCOL_FAIL;
            
          }
      }
      // Schedule Protocol
      //
      // OxF1 total shid(8byte) count(index) id kind date hour min value(4byte) delay1 delay2 END   =  23bytes
      // 
      //
      // END = total ^ shid(8byte) ^ count(index) ^ id ^ kind ^ date ^ hour ^ min ^ value(4byte) ^ delay1 ^ delay2 
      //
      else if(receivedBytes[0] == HEADER_SCHED_PROTO)
      {
          // delay2 까지만 Xor 처리
          if(receivedIndex >= 1 && receivedIndex <= 21)
          {
            xorByte = xorByte^_p;
          }

          //////////////////////////////////////////////////////////
          // 정상적으로 프토토콜이 받아졌는지 검사
          //////////////////////////////////////////////////////////
          if(xorByte == receivedBytes[22] && receivedIndex == 22)
          {
            stringComplete = true;
            receivedStart = false;
            receivedIndex = 0;
            
            //bmResponseSend_After_REQ(nodeID, "00"); // 패킷수신정상

            ret = SCHED_PROTOCOL_OK;
            
          }
          else if(xorByte != receivedBytes[22] && receivedIndex == 22) // 깨진 프로토콜 또는 쓰레기 문자 수신 18byte 시
          {
            receivedStart = false;
            receivedIndex = 0;
            
            ret = SCHED_PROTOCOL_FAIL;
            
            bmResponseSend_After_Sched(receivedBytes[11], 1,"98"); // 패킷전송 다시 요구, shid ????
          }
      }
      else if(receivedBytes[0] == HEADER_SCHED_PROTO)
      {
          // delay2 까지만 Xor 처리
          if(receivedIndex >= 1 && receivedIndex <= 21)
          {
            xorByte = xorByte^_p;
          }

          //////////////////////////////////////////////////////////
          // 정상적으로 프토토콜이 받아졌는지 검사
          //////////////////////////////////////////////////////////
          if(xorByte == receivedBytes[22] && receivedIndex == 22)
          {
            stringComplete = true;
            receivedStart = false;
            receivedIndex = 0;
            
            //bmResponseSend_After_REQ(nodeID, "00"); // 패킷수신정상

            ret = SCHED_PROTOCOL_OK;
            
          }
          else if(xorByte != receivedBytes[22] && receivedIndex == 22) // 깨진 프로토콜 또는 쓰레기 문자 수신 18byte 시
          {
            receivedStart = false;
            receivedIndex = 0;
            
            ret = SCHED_PROTOCOL_FAIL;
            
            bmResponseSend_After_Sched(receivedBytes[11], 1,"98"); // 패킷전송 다시 요구, shid ????
          }
      }
//      else if((char)receivedBytes[0] == '{')
//      {
//          if((char)_p == '}')
//          {
//            stringComplete = true;
//            receivedStart = false;
//            receivedIndex = 0;
//
//            
//            ret = TIME_PROTOCOL_OK;
//          }
//      }
 
      receivedIndex ++;
      
    }

    return ret;
}

