#include "ESP32Encoder.h"

#define ENCODER_PERIOD 100

long encoder_timer = 0;

ESP32Encoder Encoder1;
ESP32Encoder Encoder2;

//Usado para as velocidades dos motores
struct Vel {
  struct Linear {
    float x;
    float y;
    float z;
  } linear;
  struct Angular {
    float x;
    float y;
    float z;
  } angular;
};

// Cria uma variável do tipo Vel
Vel vel;

// Motor FL
int FL_enable1Pin = 13; 
int FL_motor1Pin1 = 12;
int FL_motor1Pin2 = 14; 

// Motor FR
int FR_enable1Pin = 26; 
int FR_motor1Pin1 = 25;
int FR_motor1Pin2 = 33; 

// Setting PWM properties
const int freq = 30000;
const int pwmChannelFL = 0;
const int pwmChannelFR = 1;


const int resolution = 8;

//PID
// Definições para controle PID
const float Kp = 0.9; // Ganho proporcional
const int sampleTime = 100; // Tempo de amostragem em milissegundos

float output = 0;

float FL_PWM;
float FR_PWM;

void clearEncoders() {
  // clear the encoder's raw count and set the tracked count to zero
  Encoder1.clearCount();
  Encoder2.clearCount();

}

void ControleMotores() {
  // Model parameters for the robot
  float wheel_radius = 0.032; // radius of the wheels
  float robot_radius = 0.178; // distance from the center to the wheel

  // Calculate desired wheel speeds based on linear and angular velocity
  float desiredSpeedFL = (vel.linear.x - vel.angular.z * robot_radius) / wheel_radius;
  float desiredSpeedFR = (vel.linear.x + vel.angular.z * robot_radius) / wheel_radius;

  // Measure the actual speed of the wheels from the encoders
  float FL_WheelSpeed = Encoder1.getCount() / (ENCODER_PERIOD / 1000.0); // Speed in encoder units per second
  float FR_WheelSpeed = Encoder2.getCount() / (ENCODER_PERIOD / 1000.0); // Speed in encoder units per second


  // Calculate the error between desired and actual speeds
  float FL_Error = desiredSpeedFL - FL_WheelSpeed;
  float FR_Error = desiredSpeedFR - FR_WheelSpeed;


  // Apply PID control for each motor
  FL_PWM = Kp * FL_Error / (sampleTime / 1000.0);
  FR_PWM = Kp * FR_Error / (sampleTime / 1000.0);


  // Limitar valores de PWM entre -255 e 255
  FL_PWM = constrain(FL_PWM, -255, 255);
  FR_PWM = constrain(FR_PWM, -255, 255);


  // Aplicar valores de PWM aos motores
  analogWrite(FL_enable1Pin, abs(FL_PWM));
  analogWrite(FR_enable1Pin, abs(FR_PWM));

  // Set motor directions based on PWM sign
  digitalWrite(FL_motor1Pin1, FL_PWM > 0 ? HIGH : LOW);
  digitalWrite(FL_motor1Pin2, FL_PWM > 0 ? LOW : HIGH);

  digitalWrite(FR_motor1Pin1, FR_PWM > 0 ? HIGH : LOW);
  digitalWrite(FR_motor1Pin2, FR_PWM > 0 ? LOW : HIGH);


}


void setup() {
  Serial.begin(115200);
  
  // sets the pins as outputs:
  pinMode(FL_motor1Pin1, OUTPUT);
  pinMode(FL_motor1Pin2, OUTPUT);
  pinMode(FL_enable1Pin, OUTPUT);
  pinMode(FR_motor1Pin1, OUTPUT);
  pinMode(FR_motor1Pin2, OUTPUT);
  pinMode(FR_enable1Pin, OUTPUT);

  // configure LED PWM functionalitites
  ledcSetup(pwmChannelFL, freq, resolution);
  ledcSetup(pwmChannelFR, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(FL_enable1Pin, pwmChannelFL);
  ledcAttachPin(FR_enable1Pin, pwmChannelFR);
  
  //define enconders
  Encoder1.attachHalfQuad(5, 18);
  Encoder2.attachHalfQuad(19, 21);

  clearEncoders();

  // Define os valores
  vel.linear.x = 0.8;
  vel.linear.y = 0.0;
  vel.linear.z = 0.0;
  vel.angular.x = 0.0;
  vel.angular.y = 0.0;
  vel.angular.z = 0.0;

}

void loop() {


  if(millis() - encoder_timer >=  ENCODER_PERIOD){

    ControleMotores();

    encoder_timer = millis();

    //print cmd_vel 
    Serial.println(vel.linear.x);

    //print valor enconder 1 e 2
    Serial.println(Encoder1.getCount()  );
    Serial.println(Encoder2.getCount());

    //print PWM FL e FR
    Serial.println(FL_PWM);
    Serial.println(FR_PWM);

  }
  
  //algoritomo de controle de motor.

}

