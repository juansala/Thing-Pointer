#include <Servo.h>
#define coil_A_1_pin  4
#define coil_A_2_pin  17
#define coil_B_1_pin  23
#define coil_B_2_pin  24


Servo arrow;
int servo_pin = 9;
int enable_pin = 18;
int stepDelay = 10;

// Ephem variables
float az; // reported azimuth
float alt; //reported altitude

void setup() {
  Serial.begin(9600);
  digitalWrite(enable_pin, HIGH);
  arrow.attach(servo_pin);  

}

void loop() {
  while (Serial.available())  {
    alt = Serial.parseFloat();
    az = Serial.parseFloat();
    Serial.flush();
    Serial.print(alt);
    Serial.print(' ');
    Serial.println(az);
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
    setStep(1, 0, 1, 0);
    delay(dt);
    setStep(0, 1, 1, 0);
    delay(dt);
    setStep(0, 1, 0, 1);
    delay(dt);
    setStep(1, 0, 0, 1);
    delay(dt);
  }
}

void backwards(int dt, int steps){
  for (int i = 0; i <= steps; i++){
    setStep(1, 0, 0, 1);
    delay(dt);
    setStep(0, 1, 0, 1);
    delay(dt);
    setStep(0, 1, 1, 0);
    delay(dt);
    setStep(1, 0, 1, 0);
    delay(dt); 
  }
}
