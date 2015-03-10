//Version 1.0
//Date: 11/25/2014
//
//Description: For controlling mechatronic arm with joysticks. It features 6 servos controlled by PWM.
//Uses 2 Sainsmart Joysticks. Joysticks have 2 trimpots each and one button each. Each trimpot is used for
//one servo. The two buttons are used for one servo. Rotation of the base is handled by a fifth trimpot.
//Desired Future Updates: Optimization. Mode switch.
//
//Equipment: Arduino Mega

///////////////////////////////////////////////////////////////////////////////
// Servos array in order from bottom of the arm up
// Instantiate array
// shoulderRotate : Servo 1 - motor[0] | No Indicator | Trimpot Rotate | A0
// shoulderBend   : Servo 2 - motor[1] | LEDs 2 & 3 | R. Joystick RL | A1
// elbow          : Servo 3 - motor[2] | LEDs 4 & 5 | R. Joystick UD | A2
// wristBend      : Servo 4 - motor[3] | LEDs 6 & 7 | L. Joystick UD | A3
// wristRotate    : Servo 5 - motor[4] | LEDs 8 & 9 | L. Joystick RL | A4
// gripper        : Servo 6 - motor[5] | LEDs 22 & 23| RL. Joystick Press | 36,37
// gripper        : Servo 6 - motor[5] | LEDs 22 & 23 | A5,A6 (Digital Button Not Working)
//
#include <SPI.h>
#include <EEPROM.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

int LEDS[] = {2,4,6,8,3,5,7,10,22,23,24};  // the number of the LED pin
int BUTTONS[] = {36,37,38,39,40};
long fade[10];
long pos[6];
int p = 0;
int pp = 0;
long curval;
int rightbutton = 0;
int leftbutton = 0;
long gripval = 90;
int i = 0;

// #### Hardware configuration ####
// Set up nRF24L01 radio on SPI bus plus pins 9 & 53 (normally 9 & 10 on regualr Arduinos)
RF24 radio(9,53);

const uint64_t pipe = 0xE8E8F0F0E1LL;

// #### Structures ####
// In the future, verification characters need to be added to ensure
// that, even with packet loss, the right numbers are being sent to 
// the right channels.

// Wireless Packet Structure - quaternion and flex sensor
// Since the biceps do not have flex sensors, the will send a 
// specified null value
typedef struct{
  long shoulderRotate;
  long shoulderBend;
  long elbow;
  long wristBend;
  long wristRotate;
  long grippr;
}
A_t;
A_t positions;

///////////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(57600);
  
  //Wireless Setup
  radio.begin();
  radio.setDataRate(RF24_2MBPS); // Both endpoints must have this set the same
  radio.setAutoAck(false);
  //radio.setPALevel(RF24_PA_MIN)
  radio.printDetails();
  
  radio.openWritingPipe(pipe);
  
  for (int p = 0; p < 11; p++)
  {
    pinMode(LEDS[p],OUTPUT);
  }
  
  for (int pp = 0; pp < 5; pp++)
  {
    pinMode(BUTTONS[pp],INPUT);
  }
     
   digitalWrite(LEDS[10],HIGH);
  }

///////////////////////////////////////////////////////////////////////////////

void loop()
{
  for (int k = 0; k < 5; k++)
  {
    curval = analogRead(k);
    if (k == 0)
    {
      //Serial.println(curval);
      pos[k] = map(curval, 0, 1023, 0, 179); //servo positioning
    }
    else
    {
      pos[k] = map(curval, 0, 1023, 179, 0); //servo positioning
      
      if (curval < 475)
      {
        analogWrite(LEDS[(k-1)],map(curval, 0, 509, 255, 0));
        analogWrite(LEDS[(k+3)],0);
      }
      else if (curval > 520)
      {
        analogWrite(LEDS[(k-1)],0);
        analogWrite(LEDS[(k+3)],map(curval, 513, 1023, 0, 255));
      }
      else
      {
        analogWrite(LEDS[(k-1)],0);
        analogWrite(LEDS[(k+3)],0);
      }
    }
  }
 
  delay(10);
  
  /*
  ////// Analog Read Joystick Button ////////
  rightbutton = analogRead(5);
  leftbutton = analogRead(6);

  if (rightbutton < 0.01)
  {
    digitalWrite(LEDS[8],HIGH);
    if (gripval < 179)
    {
      gripval = gripval + 2;
      //Serial.println(gripval);
    }
    rightbutton = analogRead(5);
  }
  else
  {
    digitalWrite(LEDS[8],LOW);
  }
  
   if (leftbutton < 0.01)
  {
    digitalWrite(LEDS[9],HIGH);
    if (gripval > 0)
    {
      gripval = gripval - 2;
      //Serial.println(gripval);
    }
    leftbutton = analogRead(6);
  }
  else
  {
    digitalWrite(LEDS[9],LOW);
  }
  pos[5] = gripval;
  */
  
////// Digital Read Joystick Button - NOT WORKING ////////
  if (button0 == LOW)
  {
    digitalWrite(LEDS[8],HIGH);
    //Serial.println("Button 0 Low");
  }
  else
  {
    digitalWrite(LEDS[8],LOW);
    //Serial.println("Button 0 High");
  }
  
  if (button1 == LOW)
  {
    digitalWrite(LEDS[9],HIGH);
    //Serial.println("Button 1 Low");
  }
  else
  {  
    digitalWrite(LEDS[9],LOW);
    /////Serial.println("Button 1 High");
  }
  
  //Write the positions to the structure being sent
  positions.shoulderRotate = pos[0];
  positions.shoulderBend = pos[1];
  positions.elbow = pos[2];
  positions.wristBend = pos[3];
  positions.wristRotate = pos[4];
  positions.grippr = pos[5];
  
  /* Serial Debug
  //Serial.print(positions.shoulderRotate); Serial.print(".");
  //Serial.print(positions.shoulderBend);  
  //Serial.println(pos[1]);
  //Serial.println(millis());
  */
  
  radio.write( &positions ,sizeof(positions));
  Serial.println(positions.elbow);
  
  delay(10);
}
////////////////////////////////////
