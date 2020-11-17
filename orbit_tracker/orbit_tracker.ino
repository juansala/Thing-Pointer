/* Juan Salazar 6/23/2019
 * Teensy 3.2 firmware for the Thing Pointer v1.
 * Receives commands via Serial/UART from w/ RPi.
 * Tracks the International Space Station (ISS).
 */

#include <Servo.h>
#define HWSERIAL Serial1 //UART from pins 1 and 0 on Teensy 3.2

// Stepper motor pins
#define coil_A_1_pin  23
#define coil_A_2_pin  22
#define coil_B_1_pin  21
#define coil_B_2_pin  20

// Stepper motor states
#define FIRST 1
#define SECOND 2
#define THIRD 3
#define FOURTH 4

// Variables for incoming serial messages
double values[2];
char buf[50];
String incoming; 

// Servo Teensy pins
Servo arrow;
int servo_pin = 9;
int enable_pin = 18;

// Stepper & power transmission parameters
int stepDelay = 10;
int sprockRatio = 1;
float degPerstep = 360.0/200.0;
int dt = 10;
int state = 1;
bool motorDir = NULL;

// Ephem variables
double az; // reported azimuth
double alt; //reported altitude
double oldAz;
double steps;

void setup() {
  Serial.begin(9600);
  HWSERIAL.begin(9600);
  arrow.attach(servo_pin);  
  pinMode(23, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(20, OUTPUT);

  // Stepper check 
  stepForward(50, 200);
  delay(500);
  stepBackward(50, 200);

  //Servo check
  for (int pos = 0; pos <= 120; pos += 1) { 
    // in steps of 1 degree
    arrow.write(pos);              
    delay(100);                       
  }
  for (int pos = 120; pos >= 0; pos -= 1) { 
    arrow.write(pos);              
    delay(100);                       
  }
}

void loop() {
  while (HWSERIAL.available())  {
    incoming = HWSERIAL.readStringUntil('\n');
    HWSERIAL.flush();
//    Serial.println(incoming);
    incoming.toCharArray(buf, 50);
    num_extractor(buf, values, ',');
//    dispValues();
    alt = values[0];
    az = values[1];

    if (az < 0){
      az = (360 - abs(az)); // convert TLE reported azimuth to 0-360 range
      steps = int(sprockRatio * 1/degPerstep * (az - oldAz));
    }
    else {
      steps = int(sprockRatio * 1/degPerstep * (az - oldAz));
    }
    Serial.println(steps);
        
    if (steps < 0.0){
      stepBackward(dt, abs(steps));
    }
    else if (steps > 0.0){
      stepForward(dt, abs(steps));
    }
    else{
      break;
    }

    
    oldAz = az;
    arrow.write(map(alt, -90, 90, 0, 120));
    //arrow.write(alt + 90); //set -90 from reported altitude to 0 servo pos
    } 
    
}


/*
 * Stepper Motor Helper functions
 */
 
void setStep(boolean first, boolean sec, boolean third, boolean fourth){
  digitalWrite(coil_A_1_pin, first);
  digitalWrite(coil_A_2_pin, sec);
  digitalWrite(coil_B_1_pin, third);
  digitalWrite(coil_B_2_pin, fourth);
}

void forward(int dt, int steps){
  for (int i  = 0; i < steps; i++){
    setStep(1, 0, 0, 0);
    delay(dt);
    setStep(0, 1, 0, 0);
    delay(dt);
    setStep(0, 0, 1, 0);
    delay(dt);
    setStep(0, 0, 0, 1);
    delay(dt);
  }
}

void backwards(int dt, int steps){
  for (int i = 0; i < steps; i++){
    setStep(0, 0, 0, 1);
    delay(dt);
    setStep(0, 0, 1, 0);
    delay(dt);
    setStep(0, 1, 0, 0);
    delay(dt);
    setStep(1, 0, 0, 0);
    delay(dt);
  }
}

// TESTING: Microstepping below

void stepperFSM(bool dir){
  if (motorDir != NULL && motorDir != dir)
    state = FIRST;
  motorDir = dir;
  switch(state){
    case FIRST:
      if (motorDir)
        setStep(1, 0, 0, 0);
      else 
        setStep(0, 0, 0, 1);
      state = SECOND;
      break;
    case SECOND:
      if (motorDir)
        setStep(0, 1, 0, 0);
      else
        setStep(0, 0, 1, 0);
      state = THIRD;
      break;
    case THIRD:
      if (motorDir)
        setStep(0, 0, 1, 0);
      else
        setStep(0, 1, 0, 0);
      state = FOURTH;
      break;
    case FOURTH:
      if (motorDir)
        setStep(0, 0, 0, 1);
      else
        setStep(1, 0, 0, 0);
      state = FIRST;
      break;
  }
}

void stepForward(int dt, int steps){
  for (int i  = 0; i < steps; i++){
    stepperFSM(true);
    delay(dt);
  }
}

void stepBackward(int dt, int steps){
  for (int i  = 0; i < steps; i++){
    stepperFSM(false);
    delay(dt);
  }
}
/*
 * Serial message/String parser
 */

// num_extractor parses incoming string for comma-separated values (alt and az)
void num_extractor(char* data_array, double* output_values, char delimiter) {
    char* ptr;
    char* delPointer = &delimiter;
    ptr = strtok(data_array, delPointer);
    int i = 0;
    while (ptr != NULL){
    output_values[i] = atof(ptr);
    ptr = strtok(NULL, delPointer);
    i++;
    }
}

// For debugging serial messages
void dispValues(){
  for (int i = 0; i < 2; i++){
    Serial.print(values[i]);
    Serial.print(",");
  }
  Serial.println();
}
