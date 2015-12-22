#include <Wire.h> //Include the Wire library
#include <MMA_7455.h> //Include the MMA_7455 library

MMA_7455 mySensor = MMA_7455(); //Make an instance of MMA_7455 accelerometer




const int led_pin = 13;

const int motor1_pin_e = 9;
const int motor1_pin_a = 10;
const int motor1_pin_b = 11;




const int min_adc = 0; // === min_inclinometer
const int max_adc = 1023; // === max_inclinometer

const int min_pwm = 0;
const int max_pwm = 255;


float global_zero_angle = 0.0;
const int min_angle = 0; // without zero_angle
const int max_angle = 1023; // without zero_angle

float k_p = 1.0;

float k_i = 0.0;
double sum_of_angle = 0.0;

float k_d = 0.0;
float previous_angle = 0.0;
unsigned long previous_time = 0;

boolean fast_init = false;

void setup()
{
	
	pinMode(motor1_pin_e, OUTPUT);
	pinMode(motor1_pin_a, OUTPUT);
	pinMode(motor1_pin_b, OUTPUT);
	
	digitalWrite(motor1_pin_e, LOW);
	digitalWrite(motor1_pin_a, LOW);
	digitalWrite(motor1_pin_b, LOW);


	// Set the sensitivity you want to use
	// 2 = 2g, 4 = 4g, 8 = 8g
	mySensor.initSensitivity(2);
	// Calibrate the Offset, that values corespond in 
	// flat position to: xVal = -30, yVal = -20, zVal = +20
	// !!!Activate this after having the first values read out!!!
	//mySensor.calibrateOffset(0.23, -43.2, 12.43);

	Serial.begin(57600);


	if (fast_init) { // fixme
		global_zero_angle = 934;
	} else {
		// calculate zero angle
		digitalWrite(led_pin, HIGH);
		delay(500); // 0.5sec
		digitalWrite(led_pin, LOW);
		delay(500); // 0.5sec
		digitalWrite(led_pin, HIGH);
		delay(500); // 0.5sec
		digitalWrite(led_pin, LOW);
		delay(500); // 0.5sec
		digitalWrite(led_pin, HIGH);
		delay(500); // 0.5sec
		digitalWrite(led_pin, LOW);
		delay(500); // 0.5sec
		global_zero_angle = read_inclinometer();
	}
	
	Serial.println(global_zero_angle);
	//Serial.println(min_angle);
	//Serial.println(max_angle);
}

float read_inclinometer()
{
	//Variables for the values from the sensor
	//float xVal;
	float yVal;
	float zVal;

	//xVal = (float) mySensor.readAxis('x'); //Read out the 'x' Axis (char)
	yVal = (float) mySensor.readAxis('y'); //Read out the 'y' Axis (char)
	zVal = (float) mySensor.readAxis('z'); //Read out the 'z' Axis (char)
	
	float angle_of_hade;

	angle_of_hade = atan(yVal/zVal) - global_zero_angle;
	
	//angle_of_hade = (analogRead(inclinometer_pin) - global_zero_angle);
	//ret = constrain(angle_of_hade, min_angle, max_angle);
	
	return angle_of_hade;
}

void motor_control(int speed)
{
	analogWrite(motor1_pin_e, constrain(abs(speed), min_pwm, max_pwm));
	if (speed>0) {
		digitalWrite(motor1_pin_a, HIGH);
		digitalWrite(motor1_pin_b, LOW);
	} else {
		digitalWrite(motor1_pin_a, LOW);
		digitalWrite(motor1_pin_b, HIGH);
	}
}


int PID(float angle)
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

void loop()
{


	int angle;

	int control_value = 0;
	int direction;
	int speed;



/*
	int tmp;
	for(int i=-50; i<50;i++){
		tmp = mymap(i, -max_adc/2, max_adc/2, -max_pwm, max_pwm);
		//Serial.println(tmp);
		//delay(200);
	}
*/

	angle = read_inclinometer();
	//Serial.println((int)angle);
	
	
	control_value = PID(angle);
	//Serial.println(control_value);

	speed = control_value;
	
	

	motor_control(speed);

	delay(10);
}
