/*
Version 1.2
Date: 3/9/2015
Description: For controlling mechatronic arm with joysticks. It features 6 servos controlled by PWM.
Uses 2 Sainsmart Joysticks. Joysticks have 2 trimpots each and one button each. Each trimpot is used for
one servo. The two buttons are used for one servo. Rotation of the base is handled by a fifth trimpot.
Implemented mode switch between a direct analog link and an incremental, memory link. Wireless RF 2.4 GHz
control added using the nRF24L01
Equipment: Arduino Mega, 2 sainsmart joystick breakout boards, various LEDs, 3 buttons, wires, wood, screws,
spacers
*/

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
int BUTTONS[] = {36,37,38,39,40}; //Button inputs, currently not all used.
int pos[6]; //Positions of each servo
float curval; //Value being read in analog port
int rightbutton = 0; //right joystick button
int leftbutton = 0; //left joystick button
int gripval = 90; //default gripper angle value
int gripvalconstant = 2; //value by which the gripper position is incremented
float upperthreshold = 475; //dead zone threshold
float lowerthreshold = 520; //dead zome threshold
int incrementconstant = 1; //Value by which servo angles of bot are incremented
int mode = 0; //default mode(Direct Analog Control) - 0; Incremental Control - 1

// #### Hardware configuration ####
// Set up nRF24L01 radio on SPI bus plus pins 9 & 53 (normally 9 & 10 on regualr Arduinos)
RF24 radio(9,53);

const uint64_t pipe = 0xE8E8F0F0E1LL;

// #### Structures ####
// In the future, verification characters need to be added to ensure
// that, even with packet loss, the right numbers are being sent to 
// the right channels.
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
  
  Serial.begin(57600); //Begin serial comm
  
  //Wireless Setup
  radio.begin();
  radio.setDataRate(RF24_2MBPS); // Both endpoints must have this set the same
  radio.setAutoAck(false);
  radio.setPALevel(RF24_PA_HIGH);
  //radio.printDetails();
  radio.openWritingPipe(pipe);
  
  for (int p = 0; p < 11; p++){pinMode(LEDS[p],OUTPUT);} //Activate LED Pins
  for (int pp = 0; pp < 5; pp++){pinMode(BUTTONS[pp],INPUT);} //Activate Button Pins
  digitalWrite(LEDS[10],HIGH); //Power on indicator
  }

///////////////////////////////////////////////////////////////////////////////

void loop()
{
  switch(mode){
  
  case 0: //default mode - Direct analog control. Position changes with joystick
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
        
        if (curval < lowerthreshold)
        {
          analogWrite(LEDS[(k-1)],map(curval, 0, lowerthreshold, 255, 0));
          analogWrite(LEDS[(k+3)],0);
        }
        else if (curval > upperthreshold)
        {
          analogWrite(LEDS[(k-1)],0);
          analogWrite(LEDS[(k+3)],map(curval, upperthreshold, 1023, 0, 255));
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
  break;
  
  case 1: //Incremental position control
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
        //pos[k] = map(curval, 0, 1023, 179, 0); //servo positioning
        //tilt left/down
        if (curval <= lowerthreshold/3)
        {
          pos[k] = pos[k] - incrementconstant*3;
          analogWrite(LEDS[(k-1)],map(curval, 0, lowerthreshold, 255, 0));
          analogWrite(LEDS[(k+3)],0);
        }
        else if(curval <= lowerthreshold*2/3 && curval > lowerthreshold/3)
        {
          pos[k] = pos[k] - incrementconstant*2;
          analogWrite(LEDS[(k-1)],map(curval, 0, lowerthreshold, 255, 0));
          analogWrite(LEDS[(k+3)],0);
        }
        else if(curval <= lowerthreshold && curval > lowerthreshold*2/3)
        {
          pos[k] = pos[k] + 1;
          analogWrite(LEDS[(k-1)],map(curval, 0, lowerthreshold, 255, 0));
          analogWrite(LEDS[(k+3)],0);
        }
        //Tilt right/up
        else if (curval >= upperthreshold && curval < upperthreshold*4/3)
        {
          pos[k] = pos[k] + 1;
          analogWrite(LEDS[(k-1)],0);
          analogWrite(LEDS[(k+3)],map(curval, upperthreshold, 1023, 0, 255));
        }
        else if(curval >= lowerthreshold*4/3 && curval < lowerthreshold*5/3)
        {
          pos[k] = pos[k] + incrementconstant*2;
          analogWrite(LEDS[(k-1)],0);
          analogWrite(LEDS[(k+3)],map(curval, upperthreshold, 1023, 0, 255));
        }
        else if(curval > lowerthreshold*5/3)
        {
          pos[k] = pos[k] + incrementconstant*3;
          analogWrite(LEDS[(k-1)],0);
          analogWrite(LEDS[(k+3)],map(curval, upperthreshold, 1023, 0, 255));
        }
        else
        {
          analogWrite(LEDS[(k-1)],0);
          analogWrite(LEDS[(k+3)],0);
        }
      }
    }
    
    break;
  }
  
    ////// Digital Read Joystick - for base ////////
  if (rightbutton == LOW && leftbutton == HIGH)
  {
    digitalWrite(LEDS[8],HIGH);
    digitalWrite(LEDS[9],LOW);
    gripval = gripval + gripvalconstant;
  }
  else if(leftbutton == LOW && rightbutton == HIGH)
  {
    digitalWrite(LEDS[9],HIGH);
    digitalWrite(LEDS[8],LOW);
    gripval = gripval - gripvalconstant;
  }
  else if(leftbutton == LOW && rightbutton == LOW)
  {
    digitalWrite(LEDS[8],LOW);
    digitalWrite(LEDS[9],LOW);
  }
   else if(leftbutton == HIGH && rightbutton == HIGH)
  {
    digitalWrite(LEDS[8],HIGH);
    digitalWrite(LEDS[9],HIGH);
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
  //Serial.println(positions.elbow);
  
  delay(10);
}
////////////////////////////////////
