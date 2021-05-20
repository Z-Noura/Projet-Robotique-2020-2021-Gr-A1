int enA =3;                          // Assign Pin 9 for ENA (Enable motor A)
int in1 = 39;                           // Assign Pin 8 for in1 (Input 1 for Motor A)
int in2 = 37;                           // Assign Pin 7 for in2 (Input for Motor A)
 
int enB = 4;                         // Assign Pin 3 for ENA (Enable motor B)
int in3 = 43;                           // Assign Pin 5 for in3 ( Input for Motor B)
int in4 = 45;                           // Assign Pin 4 for in4 (Input for Motor B)
 
int joyVert = A8;                   // Vertical Movement
int joyHorz = A9;                  // Horizontal Movement
 
// Motor Speed Values - Start at zero
int MotorSpeed1 = 0;         
int MotorSpeed2 = 0;
 
// Joystick Values - Start at 512 (middle position)
int joyposVert = 512;
int joyposHorz = 512; 
 
void setup()
{
 
  // Set all the motor control pins to outputs
  pinMode(enA, OUTPUT);                          // Pin 9 acts as Output Pin for Motor Enable A
  pinMode(enB, OUTPUT);                          // Pin 3 acts as Output Pin for Motor Enable B
  pinMode(in1, OUTPUT);                           // Pin 8 acts as Output Pin for Motor Input 1 (Motor A)
  pinMode(in2, OUTPUT);                           // Pin 7 acts as Output Pin for Motor Input 2 (Motor A)
  pinMode(in3, OUTPUT);                           // Pin 5 acts as Output Pin for Motor Input 3 (Motor B)
  pinMode(in4, OUTPUT);                           // Pin 4 acts as Output Pin for Motor Input 4 (Motor B)
  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH); 
  // Start with motors disabled and direction as forward
 
  // Motor A
  digitalWrite(enA, LOW);                            // Pin 9 is Low for Motor A
  digitalWrite(in1, HIGH);                            // Pin 8 is High for Motor Input1
  digitalWrite(in2, LOW);                             // Pin 7 is Low for Motor Input2
 
  // Motor B
  digitalWrite(enB, LOW);                           // Pin 3 is Low for Motor B
  digitalWrite(in3, HIGH);                           // Pin 5 is High for Motor Input 3
  digitalWrite(in4, LOW);                            // Pin 4 is Low for Motor Input 4
 
}
  
void loop()
{
  // Read the Joystick X and Y positions
  joyposVert = analogRead(joyVert);
  joyposHorz = analogRead(joyHorz);
 
  // Determine if this is a forward or backward motion
  // Do this by reading the Vertical Value
  // Apply results to MotorSpeed and to Direction
 
  if (joyposVert < 460)
  {
    // This is Backward Movement
 
    // Set Motor A backward
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
 
    // Set Motor B backward
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
 
    // Determine Motor Speeds
 
    // As we are going backwards we need to reverse readings
    joyposVert = joyposVert - 460;                       // This produces a negative number
    joyposVert = joyposVert * -1;                         // Make the number positive
 
    MotorSpeed1 = map(joyposVert, 0, 460, 0, 255);
    MotorSpeed2 = map(joyposVert, 0, 460, 0, 255);
  }
 
  else if (joyposVert > 564)
  {
    // This is Forward Movement
 
    // Set Motor A forward
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
 
    // Set Motor B forward
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
 
    // Determine Motor Speeds
    MotorSpeed1 = map(joyposVert, 564, 1023, 0, 255);
    MotorSpeed2 = map(joyposVert, 564, 1023, 0, 255);
  }
 
  else
  {
    // This (Motor) is Stopped
    MotorSpeed1 = 0;
    MotorSpeed2 = 0;
  }
 
  /* Now do the steering
     The Horizontal position will "weigh" the motor speed
     Values for each motor */
 
  if (joyposHorz < 460)
  {
    // Move Left
 
    /* As we are going left we need to reverse readings */
    joyposHorz = joyposHorz - 460;                       // This produces a negative number
    joyposHorz = joyposHorz * -1;                          // Make the number positive
 
    /* Map the number to a value of 255 maximum */
    joyposHorz = map(joyposHorz, 0, 460, 0, 255);
    MotorSpeed1 = MotorSpeed1 - joyposHorz;
    MotorSpeed2 = MotorSpeed2 + joyposHorz;
 
    /* Don't exceed range of 0-255 for motor speeds */
    if (MotorSpeed1 < 0)MotorSpeed1 = 0;
    if (MotorSpeed2 > 255)MotorSpeed2 = 255;
 
  }
  else if (joyposHorz > 564)
  {
    // Move Right
    // Map the number to a value of 255 maximum
    joyposHorz = map(joyposHorz, 564, 1023, 0, 255);       
    MotorSpeed1 = MotorSpeed1 + joyposHorz;
    MotorSpeed2 = MotorSpeed2 - joyposHorz;
 
   
    if (MotorSpeed1 > 255)MotorSpeed1 = 255;          // Don't exceed range of 0-255 for motor
                                                                                      
    if (MotorSpeed2 < 0)MotorSpeed2 = 0;                  // When range is less than 0 for motor
                                                                                     
 }
 
  // Adjust to prevent "buzzing" at very low speed
  if (MotorSpeed1 < 8)MotorSpeed1 = 0;
  if (MotorSpeed2 < 8)MotorSpeed2 = 0;
 
  // Set the motor speeds
  analogWrite(enA, MotorSpeed1);
  analogWrite(enB, MotorSpeed2);
}
