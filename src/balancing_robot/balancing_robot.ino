/*
  balancing_robot.ino - Program for balancing robot
  Created by Roman V. Prikhodchenko, 2012.04(apr)
  Released into the GPLv3.
 */

#define PROPORTIONAL 0
#define INTEGRAL 1
#define DERIVATIVE 2

#include <math.h>
#include <Wire.h> //Include the Wire library
#include <MMA_7455.h> //Include the MMA_7455 library

MMA_7455 mySensor = MMA_7455(); //Make an instance of MMA_7455 accelerometer


const int dt_us = 100; // time between steps
const int start_dt_s = 5; // time before calculate zero angle

const int led_pin = 13;

const int motor1_pin_pwm = 10;
const int motor1_pin_a   = 11;
const int motor1_pin_b   = 12;

const int min_pwm = 0;
const int max_pwm = 255;

const float epsilon = 1.0e-10;

float global_zero_angle = 0.0;

float global_coefficient[]          = {1.0e5, 0.0, -1.0e10}; //{1.0e5, 0.0, -1.0e9};
float global_previous_coefficient[] = {1.0, 0.0, 0.0};
float global_coefficient_step[]     = {1.0, 1.0, 1.0};
float global_func = 0.0;          // for search extremum (minimum) of function
float global_previous_func = 0.0; // for search extremum (minimum) of function

double global_sum_of_angle = 0.0;
float global_previous_angle = 0.0;
unsigned long global_previous_time = 0;


const unsigned int learn_duration_time_ms = 10000;
unsigned int learn_time_ms = 0;
float learn_max_angle = 0.0;
float learn_min_angle = 0.0;
unsigned int learn_start_turn_time_us = 0;
unsigned int learn_end_turn_time_us = 0;
unsigned int learn_max_turn_time_us = 0;
boolean learn_between_start_end_time = false;
int learn_coeff_num = 0; // 0 Proportional; 1 Integral; 2 Derivative algorithm

boolean fast_init = false;

boolean debug = false;



float read_inclinometer()
{
	//Variables for the values from the sensor
	//char xVal;
        char yVal;
	char zVal;

	//xVal = mySensor.readAxis('x'); //Read out the 'x' Axis (char)
	yVal = mySensor.readAxis('y'); //Read out the 'y' Axis (char)
	zVal = mySensor.readAxis('z'); //Read out the 'z' Axis (char)

	// fixme  strange, but char->float not work (signed char interpreter as unsigned byte)
	//double x = (int) xVal;
	double y = (int) yVal;
	double z = (int) zVal;

	if (debug){
		Serial.print("\ty=");
		Serial.print(y);
		Serial.print("\tz=");
		Serial.print(z);
	}
	
	double angle_of_hade;

	if (abs(z) < epsilon){
		angle_of_hade = 0.0;
	} else {
		angle_of_hade = atan(y/z) - global_zero_angle;
	}

	return angle_of_hade;
}



void motor_control(float speed)
{
	int motor_pwm = (int) constrain(abs(speed), min_pwm, max_pwm);
	analogWrite(motor1_pin_pwm, motor_pwm);
	if (speed<0) {
		digitalWrite(motor1_pin_a, HIGH);
		digitalWrite(motor1_pin_b, LOW);
	} else {
		digitalWrite(motor1_pin_a, LOW);
		digitalWrite(motor1_pin_b, HIGH);
	}
	if (debug){
		Serial.print("\tmotor=");
		Serial.print(motor_pwm);
	}
}



float PID(float previous_angle, unsigned int previous_time, float current_angle, unsigned int current_time)
{
	// Proportional Integral Derivative algorithm

	float control = 0.0;

        // Proportional coefficient
	control = global_coefficient[PROPORTIONAL] * current_angle;
	
        // Integral coefficient
	global_sum_of_angle += current_angle;
	control += global_coefficient[INTEGRAL] * global_sum_of_angle;
	
        // Derivative coefficient
	float dt = (float) (current_time - previous_time);
	control += global_coefficient[DERIVATIVE] * (current_angle - previous_angle)/dt;
	/*
	Serial.print("\tda=");
	Serial.print(current_angle - previous_angle);
	Serial.print("\tdt=");
	Serial.print(dt);
	*/
	return control;
}

void statistic(float previous_angle, unsigned int previous_time, float current_angle, unsigned int current_time)
{
	if (previous_angle<0.0 && current_angle>=0.0 && learn_between_start_end_time==false) {
		learn_start_turn_time_us = micros();
		learn_between_start_end_time = true;
	}
	if (previous_angle>0.0 && current_angle<=0.0 && learn_between_start_end_time==true) {
		learn_end_turn_time_us   = micros();
		learn_between_start_end_time = false;
		unsigned int dtse = learn_end_turn_time_us - learn_start_turn_time_us;
		if (learn_max_turn_time_us < dtse) {learn_max_turn_time_us = dtse;}
	}
	if (learn_max_angle < current_angle) {learn_max_angle = current_angle;}
	if (learn_min_angle > current_angle) {learn_min_angle = current_angle;}
}

void new_learn()
{
	learn_max_angle = 0.0;
	learn_min_angle = 0.0;
	learn_start_turn_time_us = 0;
	learn_end_turn_time_us = 0;
	learn_max_turn_time_us = 0;
	learn_between_start_end_time = false;
	
	learn_coeff_num++;
	if (learn_coeff_num>3){learn_coeff_num = 0;}
}

void learn(int coeff_num)
{
	// Gauss–Seidel method -OR- method of successive displacement -OR- iterative method of coordinate descent
	float result;
	

	// select new coefficient
	
}

void setup()
{
	
	pinMode(motor1_pin_pwm, OUTPUT);
	pinMode(motor1_pin_a,   OUTPUT);
	pinMode(motor1_pin_b,   OUTPUT);
	
	digitalWrite(motor1_pin_pwm, LOW);
	digitalWrite(motor1_pin_a,   LOW);
	digitalWrite(motor1_pin_b,   LOW);


	// Set the sensitivity you want to use
	// 2 = 2g, 4 = 4g, 8 = 8g
	mySensor.initSensitivity(2);
	// Calibrate the Offset, that values corespond in 
	// flat position to: xVal = -30, yVal = -20, zVal = +20
	// !!!Activate this after having the first values read out!!!
	//mySensor.calibrateOffset(0.23, -43.2, 12.43);

	if (debug) {Serial.begin(115200);}


	if (fast_init) { // fixme
		global_zero_angle = 934;
	} else {
		// calculate zero angle
		for (int i=0; i<start_dt_s; i++){
			delay(500);
			digitalWrite(led_pin, HIGH);
			delay(500);
			digitalWrite(led_pin, LOW);
		}
		global_zero_angle = read_inclinometer();
	}

	if (debug){
		Serial.print("zero_angle=");
		Serial.print(global_zero_angle);

		Serial.println();
	}
}



void loop()
{
	float current_angle = read_inclinometer();
	unsigned long current_time = micros();
	
	float motor_speed;

	if (learn_time_ms<millis()){
		learn_time_ms = millis() + learn_duration_time_ms;
		learn(learn_coeff_num);
		new_learn();
	}
	
	motor_speed = PID(global_previous_angle, global_previous_time, current_angle, current_time);
	motor_control(motor_speed);
	statistic(global_previous_angle, global_previous_time, current_angle, current_time);
		
	global_previous_angle = current_angle; // save angle
	global_previous_time = current_time; // save time

	if (debug){
		Serial.print("\tangle=");
		Serial.print(current_angle);
		//Serial.print("\tmotor=");
		//Serial.print(motor_speed);
		
		Serial.println();
	}
	delayMicroseconds(dt_us);
}
