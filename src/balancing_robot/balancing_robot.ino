int led_pin = 13;      // select the pin for the LED

int motor1_pin_e = 9;
int motor1_pin_a = 10;
int motor1_pin_b = 11;

int inclinometer_pin = A0;


Ssdrcs ssegment(7);
int gnd_pin = 5; // fixme addition gng
int vcc_pin = 6; // fixme addition power


int min_adc = 0; // === min_inclinometer
int max_adc = 1023; // === max_inclinometer

int min_pwm = 0;
int max_pwm = 255;


int zero_angle = 0; // fixme: bad global variable
int right_angle = 0; // fixme: bad global variable
int left_angle = 0; // fixme: bad global variable

float k_p = 0.01;

float k_i = 0.0;
float sum_i =0.0;
float fade_i =0.9;

float k_d = 0.0;
float prev_d = 0.0;

void setup()
{
	
	// declare the ledPin as an OUTPUT:
	pinMode(led_pin, OUTPUT);
	
	pinMode(motor1_pin_e, OUTPUT);
	pinMode(motor1_pin_a, OUTPUT);
	pinMode(motor1_pin_b, OUTPUT);
	
	digitalWrite(motor1_pin_e, LOW);
	digitalWrite(motor1_pin_a, LOW);
	digitalWrite(motor1_pin_b, LOW);


	pinMode(gnd_pin, OUTPUT);
	pinMode(vcc_pin, OUTPUT);
	digitalWrite(gnd_pin, LOW);
	digitalWrite(vcc_pin, HIGH);



	Serial.begin(57600);
	
	
	// calculate zero angle
	digitalWrite(led_pin, HIGH);
	delay(3000); // 3sec
	zero_angle = analogRead(inclinometer_pin);
	digitalWrite(led_pin, LOW);

	// calculate zero angle
	digitalWrite(led_pin, HIGH);
	delay(3000); // 3sec
	zero_angle = analogRead(inclinometer_pin);
	digitalWrite(led_pin, LOW);

	// calculate zero angle
	digitalWrite(led_pin, HIGH);
	delay(3000); // 3sec
	zero_angle = analogRead(inclinometer_pin);
	digitalWrite(led_pin, LOW);
}


float read_inclinometer()
{
	float angle_of_hade = 0.0;
	angle_of_hade = (float)(analogRead(inclinometer_pin) - zero_angle);
	return angle_of_hade;
}

void motor_control(int speed, boolean direction)
{
	analogWrite(motor1_pin_e, speed);
	if (direction) {
		digitalWrite(motor1_pin_a, HIGH);
		digitalWrite(motor1_pin_b, LOW);
	} else {
		digitalWrite(motor1_pin_a, LOW);
		digitalWrite(motor1_pin_b, HIGH);
	}
}

int mymap(int x, int in_min, int in_max, int out_min, int out_max)
{
	// because strange problem with: map - not work :(
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int PID(float angle)
{
	float control;
	// p
	control = angle*k_p;

	// i
	//sum_i = 
	//control = control + sum_i*

	// d
	//control = control +

	control = angle*k_p + k_i + k_d;

	//Serial.println((int)control);	
	//Serial.println((int)mymap(control, -max_adc/2, max_adc/2, -max_pwm, max_pwm));		

	//return mymap((int)control, -max_adc/2, max_adc/2, -max_pwm, max_pwm);
	return control;
}

void loop()
{


	float angle;
	int control_value;
	int direction;
	int speed;



/*
	int tmp;
	for(int i=-50; i<50;i++){
		tmp = mymap(i, -max_adc/2, max_adc/2, -max_pwm, max_pwm);
		Serial.println(tmp);
		//delay(200);
	}
*/

	angle = read_inclinometer();
	//Serial.println((int)angle);


	control_value = PID(angle);
	//Serial.println(control_value);

	if (control_value>0) {direction=true;} else {direction=false;}
	speed = abs(control_value);
	
	

	motor_control(speed, direction);

	delay(10);
}
