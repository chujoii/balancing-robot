#include <Wire.h> //Include the Wire library
#include <MMA_7455.h> //Include the MMA_7455 library

MMA_7455 mySensor = MMA_7455(); //Make an instance of MMA_7455 accelerometer


const int dt_us = 100; // time between steps
const int start_dt_s = 5; // time before calculate zero angle

const int led_pin = 13;

const int motor1_pin_pwm = 9;
const int motor1_pin_a   = 10;
const int motor1_pin_b   = 11;

const int min_pwm = 0;
const int max_pwm = 255;

float global_zero_angle = 0.0;

float k_p = 1.0;

float k_i = 0.0;
double sum_of_angle = 0.0;

float k_d = 0.0;
float previous_angle = 0.0;
unsigned long previous_time = 0;

boolean fast_init = false;

boolean debug = true;



float read_inclinometer()
{
	//Variables for the values from the sensor
	//float xVal;
	float yVal;
	float zVal;

	//xVal = (float) mySensor.readAxis('x'); //Read out the 'x' Axis (char)
	yVal = (float) mySensor.readAxis('y'); //Read out the 'y' Axis (char)
	zVal = (float) mySensor.readAxis('z'); //Read out the 'z' Axis (char)

	//yVal = yVal - 128;
	
	float angle_of_hade;

	angle_of_hade = atan(yVal/zVal) - global_zero_angle;
	
	//angle_of_hade = (analogRead(inclinometer_pin) - global_zero_angle);
	//ret = constrain(angle_of_hade, min_angle, max_angle);
	
	return angle_of_hade;
}



void motor_control(int speed)
{
	analogWrite(motor1_pin_pwm, constrain(abs(speed), min_pwm, max_pwm));
	if (speed>0) {
		digitalWrite(motor1_pin_a, HIGH);
		digitalWrite(motor1_pin_b, LOW);
	} else {
		digitalWrite(motor1_pin_a, LOW);
		digitalWrite(motor1_pin_b, HIGH);
	}
}



float PID(float angle)
{
	// Proportional Integral Derivative algorithm

	float control = 0.0;

        // Proportional coefficient
	control = k_p * angle;
	
        // Integral coefficient
	sum_of_angle += angle;
	control += k_i * sum_of_angle;
	
        // Derivative coefficient
	unsigned long current_time = micros();
	control += k_d * (angle - previous_angle)/(current_time - previous_time);
	previous_angle = angle; // save angle
	previous_time = current_time; // save time
	
	return control;
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

	if (debug) {Serial.begin(57600);}


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
	float angle;
	float motor_speed;

	angle = read_inclinometer();
	motor_speed = PID(angle);
	motor_control(motor_speed);

	if (debug){
		Serial.print("\tangle=");
		Serial.print(angle);
		Serial.print("\tmotor=");
		Serial.print(motor_speed);
		
		Serial.println();
	}
	delayMicroseconds(dt_us);
}
