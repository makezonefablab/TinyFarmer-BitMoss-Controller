# TinyFarmer-BitMoss-Controller
  
  
 ![TinyFarmer-Bitmoss-Controller Intro](https://github.com/makezonefablab/TinyFarmer-BitMoss-Controller/blob/master/img/bitmossController.png)      
 
 [*비트모스 컨트롤러 - 사이트 바로가기*](http://106.240.234.10/mediafarmHome/?page_id=13816)
 
 [*타이니파머 개발관련 카페 - 사이트 바로가기*](http://cafe.naver.com/makezone#)
 
 소개
 --------------
농장도 직접 제어가 가능합니다.
비트모스에 4개의 접점을 통해 기존에 수동으로 관리되던 전기 제어 장치를 자동 또는 원거리 제어가 가능하도록 확장합니다.
부가 기능의 액세서리 보드를 연결하여 확장 가능하므로 어떠한 제어 대상의 장치도 비트모스 하나로 똑똑해질 수 있습니다.

### Latest version 
[*라즈베리 3 unstable (2017.04.12)*](http://106.240.234.12/tinyfarmer/atth/2017-04-12-raspbian-jessie-Raspberry3_tinyfarmerHUB_8G.img)
```
2017.04.12 
- 통신보안(security)기능 적용
- 비트모스콘트롤러 프로토콜 변경
- my.tinyfarmer.com과 호환되지 않음 (클라우드 업데이트 후 사용가능)
- TinyFarmer-Arduino-Shield의 라이브러리 1.5버전과 호환됨 (*)

* 알려진 버그
- 클라우드 상에 허브가 정상적으로 연결이지만, 표시는 "끊김" 표시가 종종 나타남
```

[*2017.04.04]
```
2017.04.04 
- 신규 클라우드로 주소(my.tinyfarmer.com) 변경 
- TinyFarmer-Arduino-Shield의 라이브러리 1.5버전과 호환됨 (*)

* 알려진 버그
- 클라우드 상에 허브가 정상적으로 연결이지만, 표시는 "끊김" 표시 상태임
```


[*라즈베리 2 (2016.11.30)*](http://my.tinyfarmer.com/tinyfarmer/atth/TinyFarmer-Hub-Raspberry2.img)

설치 
--------------
설치를 위해서는 위의 OS 이미지파일을 바로 사용하거나 기존의 라즈베리파이 OS를 설치 후 아래 지시사항에 따라 개별설치하여도 됩니다.

> 1. 소스 다운로드 
```
~ $ cd /home/mediaflow
~ $ wget https://github.com/makezonefablab/TinyFarmer-HUB/tree/master/src/TinyfarmerHubWeb.zip
~ $ wget https://github.com/makezonefablab/TinyFarmer-HUB/tree/master/src/TinyfarmerHub.zip
~ $ tar xvf TinyfarmerHub.zip
~ $ tar xvf TinyfarmerHubWeb.zip
```
> 2.자바 메인모듈 
```
~ $ cd TinyfarmerHub/bin
~ $ sudo chown root:root TinyfarmerHub.sh
~ $ sudo chmod 744 TinyfarmerHub.sh
```
> 3.Web Application GUI 
```
~ $ sudo vi /usr/local/tomcat-8.0.36/conf/Catalina/localhost/ROOT.xml    (아래 XML 내용 추가)
~ $ sudo service tomcat restart
```


~~~ xml
<?xml version='1.0' encoding='utf-8'?>
<Context crossContext="true" path="" docBase="/home/mediaflow/TinyfarmerHubWeb" >
</Context >
~~~




준비물
--------------

![TinyFarmer-HUB App](https://github.com/makezonefablab/TinyFarmer-HUB/blob/master/img/rasp.jpg) 

