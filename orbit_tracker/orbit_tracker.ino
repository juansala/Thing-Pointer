/* Juan Salazar 6/23/2019
 * Teensy 3.2 firmware for the Thing Pointer v1.
 * Receives commands via Serial/UART from w/ RPi.
 * Tracks the International Space Station (ISS).
 */

#include <Servo.h>
#define coil_A_1_pin  23
#define coil_A_2_pin  22
#define coil_B_1_pin  21
#define coil_B_2_pin  20


Servo arrow;
int servo_pin = 9;
int enable_pin = 18;
int stepDelay = 10;
int sprockRatio = 0;
float deg2step = 360/200;
int dt = 50;


// Ephem variables
float az; // reported azimuth
float alt; //reported altitude
float oldAz;
float steps;

void setup() {
  Serial.begin(9600);
  arrow.attach(servo_pin);  
  pinMode(23, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(20, OUTPUT);

  // Stepper check 
  forward(50, 20);
  delay(500);
  backwards(50, 20);

  //Servo check
  for (int pos = 0; pos <= 120; pos += 1) { 
    // in steps of 1 degree
    arrow.write(pos);              
    delay(100);                       
  }
  for (int pos = 180; pos >= 0; pos -= 1) { 
    arrow.write(pos);              
    delay(100);                       
  }
}

void loop() {
  while (Serial.available())  {
    alt = Serial.parseFloat();
    az = Serial.parseFloat();
    Serial.flush();
    Serial.print(alt);
    Serial.print(' ');
    Serial.println(az);

    if (az < 0){
      az = (360 - abs(az));
      steps = sprockRatio * deg2step * (az - oldAz);
    }
    else {
      steps = sprockRatio * deg2step * (az - oldAz);
    }
      
    if (steps < 0){
      backwards(dt, abs(steps));
    }
    else {
      forward(dt, abs(steps));
    }
    
    oldAz = az;
    arrow.write(alt + 90); //set -90 from reported altitude to 0 servo pos
    
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
  for (int i  = 0; i <= steps; i++){
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
  for (int i = 0; i <= steps; i++){
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
