# TinyFarmer-BitMoss-Controller - not supported
  
  
 ![TinyFarmer-Bitmoss-Controller Intro](https://github.com/makezonefablab/TinyFarmer-BitMoss-Controller/blob/master/img/bitmossController.png)      
 
 [*비트모스 컨트롤러 - 사이트 바로가기*](http://106.240.234.10/mediafarmHome/?page_id=13816)
 
 [*타이니파머 개발관련 카페 - 사이트 바로가기*](http://cafe.naver.com/makezone#)
 
 소개
 --------------
농장도 직접 제어가 가능합니다.
비트모스에 4개의 접점을 통해 기존에 수동으로 관리되던 전기 제어 장치를 자동 또는 원거리 제어가 가능하도록 확장합니다.
부가 기능의 액세서리 보드를 연결하여 확장 가능하므로 어떠한 제어 대상의 장치도 비트모스 하나로 똑똑해질 수 있습니다.

 [*제품 보러 가기*](https://www.icbanq.com/P007594802)


### Latest version 

> **소스 개정을 더 이상 지원하지 않음 (2018.01.15) - 비트모스 쉴드 사용권장**

2017.05.02
```
- 통신보안(security)기능 적용
- 비트모스콘트롤러 프로토콜 변경
- my.tinyfarmer.com과 호환되지 않음 (클라우드 업데이트 후 사용가능)
- LCD현재 상태 표시
- RTC시간 수정 버튼 기능
- 타이니파머 허브 "라즈베리 3 unstable (2017.04.12)" 이상 버전부터 호환

```

설명
 --------------
TinyFarmer-BitMoss-Controller/src/BitmossControl_128A/BitmossControl_128A.ino

위의 소스 파일을 그대로 사용할 수 있습니다. 단, 몇가지 추가 장착되는 모듈에 대한 수정이 발생될 수 있습니다.

### 1. LCD
i2c를 인터페이스로 사용하는 character LCD를 활용하여 추가적인 신호선 사용을 줄였습니다.

[*상세한 LCD 스팩 보기 - 사이트 바로가기*](https://www.icbanq.com/P007320703)

 ![IIC/I2C 1602 LCD 모듈](https://github.com/makezonefablab/TinyFarmer-BitMoss-Controller/blob/master/img/023304.jpg)
```
+ 5 V를 추가 파란색 백라이트 LCD 디스플레이입니다.
Arduino 컨트롤러의 핀 리소스가 제한되어 있으므로 일정량의 센서 또는 SD 카드를 연결한 후에 프로젝트에서 
일반 LCD 실드를 사용할 수 없습니다.
I2C 인터페이스 LCD 모듈을 사용하면 단 2 개의 와이어를 통해 데이터를 표시 할 수 있습니다.

I2C 주소 : 0x27.백라이트 (파란색 배경의 흰색 문자).
공급 전압 : 5V.

듀폰 라인으로 연결할 수있는 IIC 인터페이스가 있습니다.

크기 : 3.6cm x 8cm - 1.4inch x 3.1inch.
```

TinyFarmer-BitMoss-Controller/src/BitmossControl_128A/LCD.h 의 파일에서 모듈마다 지정된 주소값을 수정하여 줍니다.

현재 적용된 위의 LCD는 0x27을 사용합니다.

```
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

LiquidCrystal_I2C lcd(0x27,16,2);  // 여기서 LCD의 i2c주소를 지정합니다. 현재 0x27

byte schedICON[8] = {
  B01110,
  B10101,
  B10101,
  B10111,
  B10001,
  B01110,
  B00000,
};

......
```

### 2. Time Setting
시간 셋팅은 4개의 버튼으로 제어합니다.

TinyFarmer-BitMoss-Controller/src/BitmossControl_128A/configuration.h

```
...
// Clock Setting 
#define SET          26  //pin 26 - 셋팅모드
#define STORE        27  //pin 27 - 저장
#define INCREASE     29  //pin 29 - Increase (+)
#define DECREASE     31  //pin 31 - Decrease (-)

...


```


설치 
--------------
소스를 다운받아 다운로더 장치로 다운받는다. 다운로더는 USB2SERIAL장치 중 대부분과 호환이 되며, 특히, DFRobot의 USB Serial Light(https://www.dfrobot.com/product-581.html) 와 100% 호환된다. 
다운로드 방법은 Arduino IDE 환경에서 가능하다.

준비물
--------------



