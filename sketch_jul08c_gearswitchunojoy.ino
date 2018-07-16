#include "UnoJoy.h"

//From bildr article: http://bildr.org/2012/08/rotary-encoder-arduino/

//these pins can not be changed 2/3 are special pins
#define encoderPin1 2
#define encoderPin2 3
#define encoderSw0 4

#define encoderPin3 18
#define encoderPin4 19
#define encoderSw1 22

#define GEARSW0 13
#define GEARSW1 12

#define MASTER 11

#define MSW0 10
#define MSW1 9
#define MSW2 8
#define MSW3 7
volatile int lastEncoded0 = 0;
volatile int encoder0timeout = 0;
volatile long encoderValue0 = 0;
long encoderValue0Temp = 0;
int encoderSw0Value = 0;

volatile int lastEncoded1 = 0;
volatile int encoder1timeout = 0;
volatile long encoderValue1 = 0;
long encoderValue1Temp = 0;
int encoderSw1Value = 0;

long lastencoderValue0 = 0;
long lastencoderValue1 = 0;

int lastMSB0 = 0;
int lastLSB0 = 0;
int lastMSB1 = 0;
int lastLSB1 = 0;

int gearStatus = 0;
int masterarm = 0;
int miniswitch0 = 0;
int miniswitch1 = 0;
int miniswitch2 = 0;
int miniswitch3 = 0;

volatile int knob0neg = 0;
volatile int knob0pos = 0;

volatile int knob1neg = 0;
volatile int knob1pos = 0;
void setup() {
  Serial.begin (9600);
  setupPins();
  setupInterrupts();
  setupUnoJoy();
  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3)

}

void loop(){
//Do stuff here

  dataForController_t controllerData = getControllerData();
  setControllerData(controllerData);
  if (knob0neg )
    knob0neg--;
  if (knob0pos)
    knob0pos--;
  if (knob1neg)
    knob1neg--;
  if (knob1pos)
    knob1pos--;
  if (encoder0timeout)
    encoder0timeout--;
  if (encoder1timeout)
    encoder1timeout--;

}

void updateEncoder0(){
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit
  
  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number 
  int sum = (lastEncoded0 << 2) | encoded; 
  //adding it to the previous encoded value 
  if (!encoder0timeout) {
    if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)
    { 
      encoderValue0 ++; 
      knob0pos = 100;
      knob0neg = 0;
      encoder0timeout = 100;
    }
    if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) 
    {
      encoderValue0 --;  //store this value for next time 
      knob0neg = 100;
      knob0pos = 0;
      encoder0timeout = 100;
    }
  }
 lastEncoded0 = encoded; //store this value for next time 
}

void updateEncoder1(){
  int MSB = digitalRead(encoderPin3); //MSB = most significant bit
  int LSB = digitalRead(encoderPin4); //LSB = least significant bit
  
  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number 
  int sum = (lastEncoded1 << 2) | encoded; 
  //adding it to the previous encoded value 
  if (!encoder1timeout) {
    if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
      encoderValue1 ++; 
      knob1pos = 100;
      knob1neg = 0;
      encoder1timeout = 100;
    }
    if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
      encoderValue1 --;
      knob1neg = 100;
      knob1pos = 0;
      encoder1timeout = 100;
    }
  }
  lastEncoded1 = encoded; //store this value for next time 
}

void setupPins() {
  pinMode(encoderPin1, INPUT);
  pinMode(encoderPin2, INPUT);
  
  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on
  
  pinMode(encoderPin3, INPUT);
  pinMode(encoderPin4, INPUT);
  
  digitalWrite(encoderPin3, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin4, HIGH); //turn pullup resistor on
  
  pinMode(encoderSw0, INPUT_PULLUP);
  pinMode(encoderSw1, INPUT_PULLUP);
  
  pinMode(GEARSW0, INPUT_PULLUP);
  pinMode(GEARSW1, INPUT_PULLUP);
  pinMode(MASTER, INPUT_PULLUP);
  pinMode(MSW0, INPUT_PULLUP);
  pinMode(MSW1, INPUT_PULLUP);
  pinMode(MSW2, INPUT_PULLUP);
  pinMode(MSW3, INPUT_PULLUP);

}

void setupInterrupts() {
  attachInterrupt(digitalPinToInterrupt(encoderPin1), updateEncoder0, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPin2), updateEncoder0, CHANGE);
  
  attachInterrupt(digitalPinToInterrupt(encoderPin3), updateEncoder1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPin4), updateEncoder1, CHANGE);
}

dataForController_t getControllerData(void){
  
  // Set up a place for our controller data
  //  Use the getBlankDataForController() function, since
  //  just declaring a fresh dataForController_t tends
  //  to get you one filled with junk from other, random
  //  values that were in those memory locations before
  dataForController_t controllerData = getBlankDataForController();
  // Since our buttons are all held high and
  //  pulled low when pressed, we use the "!"
  //  operator to invert the readings from the pins
  controllerData.triangleOn = !digitalRead(MSW0);
  controllerData.circleOn = !digitalRead(MSW1);
  controllerData.squareOn = !digitalRead(MSW2);
  controllerData.crossOn = !digitalRead(MSW3);
  if (knob0pos)
    controllerData.dpadUpOn = 1;
  else
    controllerData.dpadUpOn = 0;
  if (knob0neg)
    controllerData.dpadDownOn = 1;
  else
    controllerData.dpadDownOn = 0;
  if (knob1pos)
    controllerData.dpadLeftOn = 1;
  else
    controllerData.dpadLeftOn = 0;
  if (knob1neg) 
    controllerData.dpadRightOn = 1;
  else
    controllerData.dpadRightOn = 0;
  controllerData.l1On = digitalRead(GEARSW1);
  controllerData.r1On = digitalRead(GEARSW0);
  controllerData.selectOn = !digitalRead(encoderSw0);
  controllerData.startOn = !digitalRead(encoderSw1);
  controllerData.homeOn = digitalRead(MASTER);
  
  // Set the analog sticks
  //  Since analogRead(pin) returns a 10 bit value,
  //  we need to perform a bit shift operation to
  //  lose the 2 least significant bits and get an
  //  8 bit number that we can use  
  if (encoderValue0 > -128 && encoderValue0 < 128)
    controllerData.leftStickX = encoderValue0 + 128;
  else {
    if (encoderValue0 < 0)
      controllerData.leftStickX = 0;
    else   
      controllerData.leftStickX = 255;
  }
  
  if (encoderValue1 > -128 && encoderValue1 < 128)
    controllerData.leftStickY = encoderValue1 + 128;
  else {
    if (encoderValue1 < 0)
      controllerData.leftStickY = 0;
    else   
      controllerData.leftStickY = 255;
  }

  controllerData.rightStickX = 127;
  controllerData.rightStickY = 127;
  // And return the data!
  return controllerData;
}

