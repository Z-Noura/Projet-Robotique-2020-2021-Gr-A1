//Code Robot final  //main

#include "SerialTransfer.h"
#include "Wire.h"
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <Servo.h>


//Declaration sonar
#define echoPin 25 
#define trigPin 23 
int distance;

//Declaration rfid
#define SS_PIN 53
#define RST_PIN 5
MFRC522 mfrc522(SS_PIN, RST_PIN);
byte pass = 2;

//Declaration pins moteurs
#define enA 3  // vitesse moteurs gauches

#define in1 39  // direction moteurs gauches
#define in2 37

#define enB 4  // vitesse moteurs droits

#define in3 43  // direction moteurs droits
#define in4 45

int motorSpeedA = 0; //Declaration vitesse moteurs
int motorSpeedB = 0;

//Declaration buzzer
const int buzzer  = 13;

//Declaration pins LED RGB
int LED_R = 42;
int LED_G = 44;
int LED_B = 46;

//Declaration LCD
LiquidCrystal lcd = LiquidCrystal(40, 38, 36, 34, 32, 30);

//Declaration Servos
Servo monServomoteur;
Servo monServomoteur2;
Servo monServomoteur3;

//Declaration classe SerialTransfert
SerialTransfer myTransfer;

//Declaration struct
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
  byte RFID_State;
} testStruct;

void setup()
{
  //Reglages entres-sorties
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT); 

  pinMode(buzzer, OUTPUT);

  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode(22,OUTPUT);
  digitalWrite(22,HIGH);
  
  //Affichage Ecran de base
  lcd.begin(16, 2);
  lcd.print("Arduino:");
  delay(5000);

  //Attach pin servo
  monServomoteur.attach(7);   //servos cremailleres
  monServomoteur2.attach(8);

  monServomoteur3.attach(6);    //servo ultrason

  monServomoteur3.write(90);  //Centrer Servo sonar pour eviter les blocages

  //Ouverture ports serie
  Serial.begin(38400);
  Serial2.begin(38400);

  SPI.begin();      // Init bus SPI
  mfrc522.PCD_Init();   // Initiate MFRC522

  myTransfer.begin(Serial2);

  pinMode(22,OUTPUT);   //Reglages relais
  digitalWrite(22,HIGH);
}

void loop()
{
    if(myTransfer.available()) //Si buffer disponible
    {
      
      uint16_t recSize = 0; //Declaration delavariable qui suis la taille du buffer recu

      recSize = myTransfer.rxObj(testStruct, recSize); //Reception buffer
    
    }

    
    while (pass == 2)  //Tant de le rfid ne detecte pas la bonne carte
    {  
        RGB_color(LED_R,LED_G,LED_B,0,0,255);
        if ( ! mfrc522.PICC_IsNewCardPresent()) //Detection de cartes
        {
            return;
        }
        
        if ( ! mfrc522.PICC_ReadCardSerial())  //Lecture carte
        {
            return;
        }

        
        lcd.setCursor(0,0); //Affichage ID lu
        Serial.print("UID tag :");
        lcd.print("UID:");

        //Lecture Carte
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

        if (content.substring(1) == "47 48 C0 A7") //Si la carte presentee est la bonne
        {                                          //Acces autorise     
            lcd.setCursor(0,1);
            Serial.println("Authorized access");
            Serial.println();
            lcd.print("Auth access     ");
            lcd.println();
            testStruct.RFID_State=1;
            pass = 1;

            //Envoi resultat dans buffer                      
            uint16_t sendSize = 0;
            sendSize = myTransfer.txObj(testStruct, sendSize);
            myTransfer.sendData(sendSize);
            RGB_color(LED_R,LED_G,LED_B,0,255,0);
            delay(300);
        }
        else                                   //Mauvaise carte
        {  
          lcd.setCursor(0,1);                  //Acces refuse
          Serial.println(" Access denied");
          lcd.print(" Access denied  ");
          RGB_color(LED_R,LED_G,LED_B,255,0,0);
          delay(300);
          pass = 2;
          return;
        }
    }

    while(pass == 1)   //Quand l'acces est autorise
    {
        lcd.setCursor(0,0);
        lcd.print("                 ");
        lcd.setCursor(0,0);
        lcd.print(testStruct.Mode);


        if(myTransfer.available())  //Reception buffers
        {
            uint16_t recSize = 0;
            recSize = myTransfer.rxObj(testStruct, recSize);
        }

        while (testStruct.Mode == 2) //activation mode auto si mode auto active
        {
              lcd.setCursor(0,1);
              lcd.print("                 ");
              lcd.setCursor(0,1);
              lcd.print(testStruct.Mode);

              mode_auto();
        }
            
        while (testStruct.Mode == 1) //activation mode manu si mode manu active
        {            
            lcd.setCursor(0,1);
            lcd.print("                 ");
            lcd.setCursor(0,1);
            lcd.print(testStruct.Joy1Y); 

            mode_manu();  
        }
    }
}


void mode_auto()
//Mode auto
{
    int droite = Turn_sonar("droite"); //Detection d'obstacles
    int gauche = Turn_sonar("gauche");
    int devant = Turn_sonar("devant");
  
    if (droite and gauche and devant)  //si detection d'obstacles de tous les cotes
    {
        lcd.setCursor(0,0);
        lcd.print("                 ");
        lcd.setCursor(0,0);
        lcd.print("deriere"); 
        motor_auto("deriere"); //Reculer
        delay(700); 
        motor_auto("droite");  //Tourner a droite
        delay(700); 
        motor_auto("arret");   //arret     
    }
    else if (droite)                  //si detection d'obstacles a droite
    {
        lcd.setCursor(0,0);
        lcd.print("                 ");
        lcd.setCursor(0,0);
        lcd.print("gauche");   
        motor_auto("gauche");  //Tourner a gauche
        delay(700);
        motor_auto("arret");   //arret 
      
    }
    else if (gauche or devant)       //si detection d'obstacles a gauche ou devant
    {  
        lcd.setCursor(0,0);
        lcd.print("                 ");
        lcd.setCursor(0,0);
        lcd.print("droite");
        motor_auto("droite");  //Tourner a droite
        delay(700);
        motor_auto("arret");   //arret 
      
    }
    else                           //Sinon
    {
        lcd.setCursor(0,0);
        lcd.print("                 ");
        lcd.setCursor(0,0);
        lcd.print("devant");
        motor_auto("devant");  //Avancer
        delay(700);
        motor_auto("arret");   //arret 
    }

    if(myTransfer.available())  //Reception buffer
    {
         uint16_t recSize = 0;
        recSize = myTransfer.rxObj(testStruct, recSize); 
    }

if (testStruct.Mode != 2){Cremaillere(1);}
}
void mode_manu()
//Mode manu
{    
    if(myTransfer.available())  //Reception buffer
    {
      uint16_t recSize = 0;

      recSize = myTransfer.rxObj(testStruct, recSize);
    
    }

    int xAxis = testStruct.Joy1X; // Read Joysticks X-axis
    int yAxis = testStruct.Joy1Y; // Read Joysticks Y-axis 

    if (yAxis > 550)  //Joystick en bas
    {
        //Regler les moteurs pour tourner en arriere
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
      
        digitalWrite(in3, HIGH);
        digitalWrite(in4, LOW);
        
        //Calcul vitesses
        motorSpeedA = map(yAxis, 550, 1023, 0, 255);
        motorSpeedB = map(yAxis, 550, 1023, 0, 255);
    }
    else if (yAxis < 470) //Joystick en haut
    {
        //Regler les moteurs pour tourner en avant
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        
        digitalWrite(in3, LOW);
        digitalWrite(in4, HIGH);
      
        //Calcul vitesses      
        motorSpeedA = map(yAxis, 470, 0, 0, 255);
        motorSpeedB = map(yAxis, 470, 0, 0, 255);
    }  
    else //Joystick au milieu
    {
        motorSpeedA = 0;  //Arret moteurs
        motorSpeedB = 0;
    }

    if (xAxis > 550) //Joystick a gauche
    {
        int xMapped = map(xAxis, 550, 1023, 0, 255); //Calcul delta vitesse a appliquer

        // deplacement a gauche
        motorSpeedA = motorSpeedA - xMapped;
        motorSpeedB = motorSpeedB + xMapped;
        
        // Garder des vitesses de 0 a 255
        if (motorSpeedA < 0) {
            motorSpeedA = 0;
        }
        if (motorSpeedB > 255) {
            motorSpeedB = 255;
        }
    }
    if (xAxis < 470) //Joystick a gauche
    {
        int xMapped = map(xAxis, 470, 0, 0, 255); //Calcul delta vitesse a appliquer
        
        // deplacement a gauche
        motorSpeedA = motorSpeedA + xMapped;
        motorSpeedB = motorSpeedB - xMapped;
        
        // Garder des vitesses de 0 a 255
        if (motorSpeedA > 255) {
          motorSpeedA = 255;
        }
        if (motorSpeedB < 0) {
          motorSpeedB = 0;
        }    
    }

    // Prevent buzzing at low speeds
    if (motorSpeedA < 70) {
      motorSpeedA = 0;
    }
    if (motorSpeedB < 70) {
      motorSpeedB = 0;
    }
    
    //Ecriture vitesses moteurs
    analogWrite(enA, motorSpeedA); // Send PWM signal to motor A
    analogWrite(enB, motorSpeedB); // Send PWM signal to motor B

    if (testStruct.BPJoy1 == 0) //Releve et envoir de la distance des obstacles si BP joystick appuie
    {
        testStruct.Dist_sonar  = distance_sonar();   
           
        uint16_t sendSize = 0;
        sendSize = myTransfer.txObj(testStruct, sendSize);
        myTransfer.sendData(sendSize);
        Serial.println (distance);
    }

}

void motor_auto(char*dir){
/*Fonction qui fait tourner les moteurs
parametres: dir indique la direction desiree du robot:
                "devant" : avance le robot
                "derriere" : recule le robot
                "gauche" : tourne le robot a gauche
                "droite" : tourne le robot a droite
Vitesse (PWM) par defaut : 200
*/  
  if (dir == "droite"){
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);

    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);

    analogWrite(enA, 200); 
    analogWrite(enB, 200);
  
  }
  else if (dir == "gauche"){
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);

    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);

    analogWrite(enA, 200); 
    analogWrite(enB, 200);
  }
  else if (dir == "arret"){
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);

    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);

    analogWrite(enA, 0); 
    analogWrite(enB, 0);
  }
  else if( dir == "devant"){
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);

    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);

    analogWrite(enA, 200); 
    analogWrite(enB, 200);
    }
   else{
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);

    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);

    analogWrite(enA, 200); 
    analogWrite(enB, 200);
    
    }
  }

bool Turn_sonar(char* dir)
/* Fonction qui tourne le sonar dans la direction dir souhaitee
et qui retourne si un obstacle est detecte par celui ci
Parametres possibles:
        "droite" : torune le sonar de 60 degres a droite
        "gauche" : torune le sonar de 60 degres a gauche
        "devant" : mets le sonar devant le robot
*/
{
  Cremaillere(0); // descente cremaillere
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

  int Son = distance_sonar(); //detection obstacles
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


int distance_sonar()
/* Fonction qui retourne un entier exprimant la distance en cm
d'un obstacle detecte par le sonar. Dist max 200 cm 
*/
{
    long duration, inches;
    int cm;
    bool state;
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
  
    digitalWrite(trigPin, HIGH); //Emmision d'un signal ultrason
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    duration = pulseIn(echoPin, HIGH); //mesure du temps pris pour recevoir l'echo du signal
    
    cm = duration * 0.034 / 2; // calcul de la distance
    return cm;
}
 void Cremaillere(int dir)
 /*Fonction qui monte ou descent la cremaillere
  dir = 1 : cremaillere haute
  dir = 0 : cremaillere basse 
  */
 {
    if( dir)
    { 
        monServomoteur.write(180);
        monServomoteur2.write(0);
        monServomoteur3.write(88);
    }
    else
    {
        monServomoteur.write(40);    
        monServomoteur2.write(140);
    }
    delay(700);
}
void RGB_color(int red_light_pin,int green_light_pin,int blue_light_pin,int red_light_value, int green_light_value, int blue_light_value)
/*Fonction permetant de choir les couleur d'une led reb
Parametres :
        int red_light_pin    : Pin Led rouge
        int green_light_pin  : Pin Led verte
        int blue_light_pin   : Pin Led Bleue

        int red_light_value  : Valeur (PWM) lumiere Rouge
        int green_light_value: Valeur (PWM) lumiere Verte
        int blue_light_value : Valeur (PWM) lumiere Bleure

*/
{
  analogWrite(red_light_pin, red_light_value);
  analogWrite(green_light_pin, green_light_value);
  analogWrite(blue_light_pin, blue_light_value);
}