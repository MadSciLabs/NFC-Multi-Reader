/**
 *  @title:  StrongLink SL018/SL030 RFID multireader
 *  @author: michael@neufuture.com based on code of:
 *  @author: marc@marcboon.com
 *  @see:    http://www.stronglink.cn/english/sl018.htm
 *  @see:    http://www.stronglink.cn/english/sl030.htm
 *
 *  Arduino to SL018/SL030 wiring:
 *  A4/SDA     2     3 // Green
 *  A5/SCL     3     4 // Yellow
 *  5V         4     -
 *  GND        5     6
 *  3V3        -     1
 *  5          1     5 // first reader
 *  4          1     5 // second reader
 */

#include <Wire.h>
#include <SL018.h>

int r[3]; //register values

// this is connected to every S0, S1, S2 pins of all the multiplexers
int regPin[] = {
  2,3,4 };
int activePin = 6; 

// For (de)activating each set of multiplexers
int bank[] = {
  7,8 };

const int numReaders = 12;
int readerID;
int readDelay = 10;

int byteTranslate = 65;

String tag;
String lastTag[numReaders];

SL018 rfid;

void setup(){

  Wire.begin();
  Serial.begin(115200);

  // standard address for readers
  rfid.address = 0x50;

  // registration pins
  pinMode(regPin[0], OUTPUT);
  pinMode(regPin[1], OUTPUT);
  pinMode(regPin[2], OUTPUT);
  // active pin
  pinMode(activePin, INPUT);
  // bank switch
  pinMode(bank[0], OUTPUT);
  pinMode(bank[1], OUTPUT);

  Serial.println("Begin reading");

}

void loop(){
  readTag();
  readerID++;
  if(readerID>=numReaders) readerID = 0;
}


void writeReg(){
  if(readerID<8){
    digitalWrite(bank[0], LOW);
    digitalWrite(bank[1], HIGH);
  }
  else{
    digitalWrite(bank[0], HIGH);
    digitalWrite(bank[1], LOW);
  }
  for (int i =0 ; i<3; i++){
    r[i] = bitRead(readerID%8,i);
    digitalWrite(regPin[i], r[i]);
  }
}


void readTag(){
  writeReg();
  delay(readDelay);

  if(!digitalRead(activePin)){
    rfid.seekTag();

    while(rfid.available()){
      tag = rfid.getTagString();

      if(lastTag[readerID] != tag){
        Serial.print(readerID);
        Serial.print(": ");
        Serial.println(tag);
        lastTag[readerID] = tag;
      }
      if(digitalRead(activePin) || rfid.getTagString()) break;
    }
  }
  else if(lastTag[readerID] != ""){
    lastTag[readerID] = "";
    Serial.print(readerID);
    Serial.println(": tag removed");
  }
}




