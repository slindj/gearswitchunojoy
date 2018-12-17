
#include "MegaJoy.h"

#define ENCODERPIN1 2
#define ENCODERPIN2 3
#define ENCODERSW0 4

#define ENCODERPIN3 18
#define ENCODERPIN4 19
#define ENCODERSW1 22

#define BTN0 24
#define BTN1 26
#define BTN2 28
#define BTN3 30

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

void setup(){
  setupPins();
  setupMegaJoy();
  setupInterrupts();
  
}

void loop(){
  // Always be getting fresh data
  megaJoyControllerData_t controllerData = getControllerData();
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
  int MSB = digitalRead(ENCODERPIN1); //MSB = most significant bit
  int LSB = digitalRead(ENCODERPIN2); //LSB = least significant bit
  
  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number 
  int sum = (lastEncoded0 << 2) | encoded; 
  //adding it to the previous encoded value 
  if (!encoder0timeout) {
    if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)
    { 
      encoderValue0 ++; 
      knob0pos = 50;
      knob0neg = 0;
      encoder0timeout = 0;
    }
    if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) 
    {
      encoderValue0 --;  //store this value for next time 
      knob0neg = 50;
      knob0pos = 0;
      encoder0timeout = 0;
    }
  }
 lastEncoded0 = encoded; //store this value for next time 
}

void updateEncoder1(){
  int MSB = digitalRead(ENCODERPIN3); //MSB = most significant bit
  int LSB = digitalRead(ENCODERPIN4); //LSB = least significant bit
  
  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number 
  int sum = (lastEncoded1 << 2) | encoded; 
  //adding it to the previous encoded value 
  if (!encoder1timeout) {
    if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
      encoderValue1 ++; 
      knob1pos = 50;
      knob1neg = 0;
      encoder1timeout = 0;
    }
    if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
      encoderValue1 --;
      knob1neg = 50;
      knob1pos = 0;
      encoder1timeout = 0;
    }
  }
  lastEncoded1 = encoded; //store this value for next time 
}
void setupPins(void){
  // Set all the digital pins as inputs
  // with the pull-up enabled, except for the 
  // two serial line pins
  for (int i = 2; i <= 54; i++){
    pinMode(i, INPUT);
    digitalWrite(i, HIGH);
  }
}

megaJoyControllerData_t getControllerData(void){
  
  // Set up a place for our controller data
  //  Use the getBlankDataForController() function, since
  //  just declaring a fresh dataForController_t tends
  //  to get you one filled with junk from other, random
  //  values that were in those memory locations before
  megaJoyControllerData_t controllerData = getBlankDataForMegaController();
  // Since our buttons are all held high and
  //  pulled low when pressed, we use the "!"
  //  operator to invert the readings from the pins
  int myInts[54];
  for (int i = 0; i < 54; i++){
  myInts[i] = 0;
  }
  
  myInts[2] = !digitalRead(MSW0);
  myInts[3] = !digitalRead(MSW1);
  myInts[4] = !digitalRead(MSW2);
  myInts[5] = !digitalRead(MSW3);
  if (knob0pos)
    myInts[6] = 1;
  else
    myInts[6] = 0;
  if (knob0neg)
    myInts[7] = 1;
  else
    myInts[7] = 0;
  if (knob1pos)
    myInts[8] = 1;
  else
    myInts[8] = 0;
  if (knob1neg) 
    myInts[9] = 1;
  else
    myInts[9] = 0;
  myInts[10] = digitalRead(GEARSW1);
  myInts[11] = digitalRead(GEARSW0);
  myInts[12] = !digitalRead(ENCODERSW0);
  myInts[13] = !digitalRead(ENCODERSW1);
  myInts[14] = digitalRead(MASTER);

  myInts[15] = !digitalRead(BTN3);
  myInts[16] = !digitalRead(BTN2);
  myInts[17] = !digitalRead(BTN1);
  myInts[18] = !digitalRead(BTN0);

  for (int i = 2; i < 54; i++){
    controllerData.buttonArray[(i - 2) / 8] |= (myInts[i]) << ((i - 2) % 8);
  }
  
  // Set the analog sticks
  //  Since analogRead(pin) returns a 10 bit value,
  //  we need to perform a bit shift operation to
  //  lose the 2 least significant bits and get an
  //  8 bit number that we can use 
    if (encoderValue0 > -128 && encoderValue0 < 128)
    controllerData.analogAxisArray[0] = encoderValue0 + 128;
  else {
    if (encoderValue0 < 0)
      controllerData.analogAxisArray[0] = 0;
    else   
      controllerData.analogAxisArray[0] = 255;
  }
  
  if (encoderValue1 > -128 && encoderValue1 < 128)
    controllerData.analogAxisArray[1] = encoderValue1 + 128;
  else {
    if (encoderValue1 < 0)
      controllerData.analogAxisArray[1] = 0;
    else   
      controllerData.analogAxisArray[1] = 255;
  }
  controllerData.analogAxisArray[2] = 127; 
  controllerData.analogAxisArray[3] = 127; 
  controllerData.analogAxisArray[4] = 127; 
  controllerData.analogAxisArray[5] = 127; 
  controllerData.analogAxisArray[6] = 127; 
  controllerData.analogAxisArray[7] = 127; 
  controllerData.analogAxisArray[8] = 127; 
  controllerData.analogAxisArray[9] = 127; 
  controllerData.analogAxisArray[10] = 127; 
  controllerData.analogAxisArray[11] = 127; 
  
  // And return the data!
  return controllerData;
}

void setupInterrupts() {
  attachInterrupt(digitalPinToInterrupt(ENCODERPIN1), updateEncoder0, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODERPIN2), updateEncoder0, CHANGE);
  
  attachInterrupt(digitalPinToInterrupt(ENCODERPIN3), updateEncoder1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODERPIN4), updateEncoder1, CHANGE);
}

