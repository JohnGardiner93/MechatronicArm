//Version 1.0
//Date: 6/29/2014
//Description: This is the first version of the controls code for my 
//mechatronic arm. It features 6 servos controlled by PWM via 6 potentiometers.
//There are no feedback sensors in this version (i.e. it is an Open Feedback 
//system).
//Desired Future Updates: Different controls to enable use of analog pins for
//sensors. Custom Function for each servo reading. Ability to remember patterns.
//Ability to add delays.
//Equipent: Arduino Uno

///////////////////////////////////////////////////////////////////////////////
#include <Servo.h>    // Use Servo library

// Servos in order from bottom of the arm up
Servo  shoulderRotate;  //Servo 1 - A0
Servo  shoulderBend;    //Servo 2 - A1
Servo  elbow;           //Servo 3 - A2
Servo  wristBend;       //Servo 4 - A3
Servo  wristRotate;     //Servo 5 - A4
Servo  gripper;         //Servo 6 - A5

int val0 = 0;  //A0
int val1 = 0;  //A1
int val2 = 0;  //A2
int val3 = 0;  //A3
int val4 = 0;  //A4
int val5 = 0;  //A5

///////////////////////////////////////////////////////////////////////////////

void setup()
{
  //Connect servos to pwm pins
  shoulderRotate.attach(11);
  shoulderBend.attach(10);
  elbow.attach(9);
  wristBend.attach(6);
  wristRotate.attach(5);
  gripper.attach(3);
}

///////////////////////////////////////////////////////////////////////////////

void loop()
{
  val0 = analogRead(0);
  val0 = map(val0, 0, 1023, 0, 179);
  shoulderRotate.write(val0);
  delay(10);

  val1 = analogRead(1);
  val1 = map(val1, 0, 1023, 0, 179);
  shoulderBend.write(val1);
  delay(10);
  
  val2 = analogRead(2);
  val2 = map(val2, 0, 1023, 0, 179);
  elbow.write(val2);
  delay(10);
  
  val3 = analogRead(3);
  val3 = map(val3, 0, 1023, 0, 179);
  wristBend.write(val3);
  delay(10);
  
  val4 = analogRead(4);
  val4 = map(val4, 0, 1023, 0, 179);
  wristRotate.write(val4);
  delay(10);
  
  val5 = analogRead(5);
  val5 = map(val5, 0, 1023, 0, 179);
  gripper.write(val5);
  delay(10);
}

///////////////////////////////////////////////////////////////////////////////
