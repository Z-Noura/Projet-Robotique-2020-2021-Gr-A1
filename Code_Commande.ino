
#include "SerialTransfer.h" //main

#include <SoftwareSerial.h>
SoftwareSerial mSerial( 0,1);


SerialTransfer myTransfer;

struct STRUCT {
  int Joy1X ;
  int Joy1Y;
  int Joy2X;
  int Joy2Y;
  bool BPBuzzer;
  bool BPJoy1;
  bool BPJoy2;
  char Mode;
  int Dist_sonar;
  bool RFID_State;
} testStruct;

char arr[] = "hello";


void setup()
{
  Serial.begin(38400);
  mSerial.begin(38400);
  myTransfer.begin(mSerial);
  pinMode(21,OUTPUT);
  digitalWrite(21,HIGH);
}


void loop()
{
  testStruct.Joy1X = analogRead(A0);  //Joy vertical
  testStruct.Joy1Y = analogRead(A1);  //Joy horizontal
  
  // use this variable to keep track of how many
  // bytes we're stuffing in the transmit buffer
  uint16_t sendSize = 0;

  ///////////////////////////////////////// Stuff buffer with struct
  sendSize = myTransfer.txObj(testStruct, sendSize);

  ///////////////////////////////////////// Stuff buffer with array
  sendSize = myTransfer.txObj(arr, sendSize);

  ///////////////////////////////////////// Send buffer
  myTransfer.sendData(sendSize);

  if(myTransfer.available())
  {
    // use this variable to keep track of how many
    // bytes we've processed from the receive buffer
    uint16_t recSize = 0;

    recSize = myTransfer.rxObj(testStruct, recSize);
  }
  delay(500);
}