//Code Robot final  //main

#include "SerialTransfer.h"
#include "Wire.h"
#include <NewPing.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <Servo.h>

#define SS_PIN 53
#define RST_PIN 5

#define enA 3  // vitesse moteurs gauches
#define in1 39  // direction moteurs gauches
#define in2 37
#define enB 4  // vitesse moteurs droits
#define in3 43  // direction moteurs droits
#define in4 45

int motorSpeedA = 0;
int motorSpeedB = 0;
int start_manu;
int Start_auto;

const int buzzer  = 13;

int Sw = 49; 

const int pinultra = 23; // Trigger Pin of Ultrasonic Sensor
const int pinson = 25; // Echo Pin of Ultrasonic Sensor
int dist;
int distance;

MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal lcd = LiquidCrystal(40, 38, 36, 34, 32, 30);


MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
bool pass = 0;

int vitesse;
bool Start;

Servo monServomoteur;
Servo monServomoteur2;
Servo monServomoteur3;

SerialTransfer myTransfer;

struct STRUCT {
  int Joy1X ;
  int Joy1Y;
  int Joy2X;
  int Joy2Y;
  bool BPBuzzer;
  bool BPJoy1;
  bool BPJoy2;
  byte Mode;
  int Dist_sonar;
  char RFID_State;
} testStruct;


char arr[6];


void setup()
{
  pinMode(buzzer, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(22,OUTPUT);
  digitalWrite(22,HIGH);

  pinMode(pinultra, OUTPUT);
  pinMode(pinson, INPUT);
 pinMode(Sw,INPUT_PULLUP);
 lcd.begin(16, 2);
 lcd.print("Arduino:");
 delay(5000);
 monServomoteur.attach(7);   //servo crémaillère
  monServomoteur2.attach(8);   //servo crémaillère
  monServomoteur3.attach(6);    //servo ultrason
  Serial.begin(9600);
  monServomoteur3.write(90);
  Start_auto = 1;


  Serial.begin(38400);
  Serial2.begin(38400);
  SPI.begin();      // Initiate  SPI bus
 mfrc522.PCD_Init();   // Initiate MFRC522
  myTransfer.begin(Serial2);
  pinMode(22,OUTPUT);
  digitalWrite(22,HIGH);


}

void loop()
{ 

  if(myTransfer.available())
  {
    // use this variable to keep track of how many
    // bytes we've processed from the receive buffer
    uint16_t recSize = 0;

    recSize = myTransfer.rxObj(testStruct, recSize);
  
  }
 
  distance = distance_sonar(); 
  
   
                
                  
  

  int droite,devant,gauche;
   while (pass == 0){
     // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  lcd.setCursor(0,0);
  Serial.print("UID tag :");
  lcd.print("UID:");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     lcd.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     lcd.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  lcd.print("Message : ");
  content.toUpperCase();
 // if (content.substring(1) == "BD 31 15 2B") //change here the UID of the card/cards that you want to give access
  if (content.substring(1) == "47 48 C0 A7")
  {
    lcd.setCursor(0,1);
    Serial.println("Authorized access");
    Serial.println();
    lcd.print("Auth access     ");
    lcd.println();
    testStruct.RFID_State=1;
    pass = 1;
                 
    uint16_t sendSize = 0;

  //////////////////////////////////////// Stuff buffer with struct
    sendSize = myTransfer.txObj(testStruct, sendSize);

  ///////////////////////////////////////// Stuff buffer with array
    sendSize = myTransfer.txObj(arr, sendSize);

  ///////////////////////////////////////// Send buffer
    myTransfer.sendData(sendSize);
  }
  else   {
    
    lcd.setCursor(0,1);
    Serial.println(" Access denied");
    lcd.print(" Access denied  ");
    return;
 }}

while(pass == 1){
  lcd.setCursor(0,0);
  lcd.print("                 ");
  lcd.setCursor(0,0);
  lcd.print(testStruct.Mode);
  if(myTransfer.available())
  {
    // use this variable to keep track of how many
    // bytes we've processed from the receive buffer
    uint16_t recSize = 0;

    recSize = myTransfer.rxObj(testStruct, recSize);
  
  }

while (testStruct.Mode == 2){
   // tone(buzzer, 1000); // Send 1KHz sound signal...
   // delay(1000);        // ...for 1 sec
   // noTone(buzzer);     // Stop sound...
     lcd.setCursor(0,1);
     lcd.print("                 ");
     lcd.setCursor(0,1);
     lcd.print(testStruct.Mode);

     mode_auto();
   }
      
while (testStruct.Mode == 1){
  //tone(buzzer, 150); // Send 1KHz sound signal...
  //delay(1000);        // ...for 1 sec
  //noTone(buzzer);     // Stop sound...
   lcd.setCursor(0,1);
     lcd.print("                 ");
     lcd.setCursor(0,1);
     lcd.print(testStruct.Joy1Y);  
     mode_manu();   

      }
       }
  

  

}
void mode_auto(){
 vitesse = 900;
int    droite = Turn_sonar("droite");
int    gauche = Turn_sonar("gauche");
int    devant = Turn_sonar("devant");
  
    if (droite and gauche and devant){
      lcd.setCursor(0,0);
      lcd.print("                 ");
      lcd.setCursor(0,0);
      lcd.print("deriere");
      motor_auto("deriere");
      delay(700);  
      motor_auto("arret");
        
      }
    else if (droite){
      lcd.setCursor(0,0);
      lcd.print("                 ");
      lcd.setCursor(0,0);
      lcd.print("gauche");
      motor_auto("gauche");
      delay(700);
      motor_auto("arret");
      
    }
    else if (gauche or devant){
      lcd.setCursor(0,0);
      lcd.print("                 ");
      lcd.setCursor(0,0);
      lcd.print("droite");
      motor_auto("droite");
      delay(700);
      motor_auto("arret");
      
    }
    else{
      lcd.setCursor(0,0);
      lcd.print("                 ");
      lcd.setCursor(0,0);
      lcd.print("devant");
      motor_auto("devant");
      delay(700);
      motor_auto("arret");
      }

}
void mode_manu(){

  int xAxis = testStruct.Joy1X; // Read Joysticks X-axis
   int yAxis = testStruct.Joy1Y; // Read Joysticks Y-axis 
   
  if(myTransfer.available())
  {
    // use this variable to keep track of how many
    // bytes we've processed from the receive buffer
    uint16_t recSize = 0;

    recSize = myTransfer.rxObj(testStruct, recSize);
  
  }
  if (yAxis < 470) {
    // Set Motor A backward
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  
    // Set Motor B backward
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    
    // Convert the declining Y-axis readings for going backward from 470 to 0 into 0 to 255 value for the PWM signal for increasing the motor speed
    motorSpeedA = map(yAxis, 470, 0, 0, 255);
    motorSpeedB = map(yAxis, 470, 0, 0, 255);

  }
  else if (yAxis > 550) {
    // Set Motor A forward
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    
    // Set Motor B forward
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
   
    // Convert the increasing Y-axis readings for going forward from 550 to 1023 into 0 to 255 value for the PWM signal for increasing the motor speed
    motorSpeedA = map(yAxis, 550, 1023, 0, 255);
    motorSpeedB = map(yAxis, 550, 1023, 0, 255);

  }
  // If joystick stays in middle the motors are not moving
  else {
    motorSpeedA = 0;
    motorSpeedB = 0;
 
  }

  // X-axis used for left and right control
  if (xAxis < 470) {
    // Convert the declining X-axis readings from 470 to 0 into increasing 0 to 255 value
    int xMapped = map(xAxis, 470, 0, 0, 255);
    // Move to left - decrease left motor speed, increase right motor speed
    motorSpeedA = motorSpeedA - xMapped;
    motorSpeedB = motorSpeedB + xMapped;
    
    // Confine the range from 0 to 255
    if (motorSpeedA < 0) {
      motorSpeedA = 0;
    }
    if (motorSpeedB > 255) {
      motorSpeedB = 255;
    }
    
  }
  if (xAxis > 550) {
    // Convert the increasing X-axis readings from 550 to 1023 into 0 to 255 value
    int xMapped = map(xAxis, 550, 1023, 0, 255);
    // Move right - decrease right motor speed, increase left motor speed
    motorSpeedA = motorSpeedA + xMapped;
    motorSpeedB = motorSpeedB - xMapped;
    
    // Confine the range from 0 to 255
    if (motorSpeedA > 255) {
      motorSpeedA = 255;
    }
    if (motorSpeedB < 0) {
      motorSpeedB = 0;
    }
    
  }
  // Prevent buzzing at low speeds (Adjust according to your motors. My motors couldn't start moving if PWM value was below value of 70)
  if (motorSpeedA < 70) {
    motorSpeedA = 0;
  }
  if (motorSpeedB < 70) {
    motorSpeedB = 0;
  }
   
  analogWrite(enA, motorSpeedA); // Send PWM signal to motor A
  analogWrite(enB, motorSpeedB); // Send PWM signal to motor B


  if (testStruct.BPJoy1 == 0){
                
                  
  uint16_t sendSize = 0;

  ///////////////////////////////////////// Stuff buffer with struct
  sendSize = myTransfer.txObj(testStruct, sendSize);

  ///////////////////////////////////////// Stuff buffer with array
  sendSize = myTransfer.txObj(arr, sendSize);

  ///////////////////////////////////////// Send buffer
  myTransfer.sendData(sendSize);
  Serial.println (distance);
  }
  
  else {
    Serial.print("not");
  }

}

 void motor_auto(char*dir){

  if (dir == "droite"){
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);

    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);

    analogWrite(enA, 200); // vitesse moteurs gauches
    analogWrite(enB, 200);
    
    
  }
  else if (dir == "gauche"){
     // Set Motor A backward
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);

    // Set Motor B backward
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);

    analogWrite(enA, 200); // vitesse moteurs gauches
    analogWrite(enB, 200);
  }
  else if (dir == "arret"){
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);

    // Set Motor B backward
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);

    analogWrite(enA, 0); // vitesse moteurs gauches
    analogWrite(enB, 0);
  }
  else if( dir == "devant"){
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);

    // Set Motor B backward
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);

    analogWrite(enA, 200); // vitesse moteurs gauches
    analogWrite(enB, 200);
    }
   else{
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);

    // Set Motor B backward
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);

    analogWrite(enA, 200); // vitesse moteurs gauches
    analogWrite(enB, 200);
    
    }
  }

bool Turn_sonar(char* dir){
 Cremaillere(0);
 int state;
 if (dir == "droite"){
  monServomoteur3.write(150);
  delay(300);}
 else if (dir == "gauche"){
  monServomoteur3.write(30);
  delay(300);
 }
 else if (dir == "devant"){
  monServomoteur3.write(90);
  delay(300);
 }
 delay(700);

 int Son = distance_sonar();
 if (Son < 30){
    state = 1;
    lcd.setCursor(0,0);
      lcd.print("                 ");
      lcd.setCursor(0,0);
      lcd.print("deriere");
    Serial.println(Son);
    }
   else{
    state = 0;
     lcd.setCursor(0,0);
     lcd.print("                 ");
     lcd.setCursor(0,0);
     lcd.print("deriere");
    Serial.println(Son);
   }
return state;    
    
}


long microsecondsToCentimeters(long microseconds) {
   return microseconds / 29 / 2;
}

int distance_sonar(){
  long duration, inches, cm;
  bool state;
  digitalWrite(pinultra, LOW);
  delayMicroseconds(2);
  digitalWrite(pinultra, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinultra, LOW);

  cm = microsecondsToCentimeters(duration);
  return cm;
  }
 void Cremaillere(int dir){
 //Placer ici le code pour la cremaillere 
 if( dir){ monServomoteur.write(180);
    monServomoteur2.write(0);
    monServomoteur3.write(88);}
 else{monServomoteur.write(40);    //crémaillère basse
    monServomoteur2.write(140);}
    delay(700);}