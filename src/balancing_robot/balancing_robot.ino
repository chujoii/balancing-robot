int led_pin = 13;

int motor1_pin_e = 9;
int motor1_pin_a = 10;
int motor1_pin_b = 11;

int inclinometer_pin = A0;


// fixme: many global variables

int min_adc = 0; // === min_inclinometer
int max_adc = 1023; // === max_inclinometer

int min_pwm = 0;
int max_pwm = 255;


int zero_angle = 512;
int min_angle = 0; // without zero_angle
int max_angle = 1023; // without zero_angle

float k_p = 1.0;

float k_i = 0.0;
float sum_i =0.0;
float fade_i =0.9;

float k_d = 0.0;
float prev_d = 0.0;


boolean fast_init = false;

void setup()
{
	
	pinMode(motor1_pin_e, OUTPUT);
	pinMode(motor1_pin_a, OUTPUT);
	pinMode(motor1_pin_b, OUTPUT);
	
	digitalWrite(motor1_pin_e, LOW);
	digitalWrite(motor1_pin_a, LOW);
	digitalWrite(motor1_pin_b, LOW);


	Serial.begin(57600);
	
	


	if (fast_init) {
		zero_angle = 934;
		min_angle = -145;
		max_angle = 77;
	} else {
		// calculate zero angle
		delay(3000); // 3sec
		digitalWrite(led_pin, HIGH);
		delay(10000); // 10sec
		zero_angle = analogRead(inclinometer_pin);
		digitalWrite(led_pin, LOW);
		
		// calculate right angle
		delay(3000); // 3sec
		digitalWrite(led_pin, HIGH);
		delay(10000); // 10sec
		min_angle = analogRead(inclinometer_pin) - zero_angle;
		digitalWrite(led_pin, LOW);

		// calculate zero angle
		delay(3000); // 3sec
		digitalWrite(led_pin, HIGH);
		delay(10000); // 10sec
		max_angle = analogRead(inclinometer_pin) - zero_angle;
		digitalWrite(led_pin, LOW);
	}
	






	if (min_angle>max_angle){
		int tmp; 
		tmp = max_angle;
		max_angle = min_angle;
		min_angle = tmp;
	}

	//Serial.println(zero_angle);
	//Serial.println(min_angle);
	//Serial.println(max_angle);
}

int read_inclinometer()
{
	int angle_of_hade;
	//int ret;
	angle_of_hade = constrain((analogRead(inclinometer_pin) - zero_angle), min_angle, max_angle);
	
	//angle_of_hade = (analogRead(inclinometer_pin) - zero_angle);
	//ret = constrain(angle_of_hade, min_angle, max_angle);
	
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
	long tmp;
	tmp = ((long)x - (long)in_min) * ((long)out_max - (long)out_min) / ((long)in_max - (long)in_min) + (long)out_min;
	// because strange problem with: map - not work :(
	return (int)tmp;
}

int PID(int iangle)
{
	float fangle = (float) iangle;
	float fcontrol = 0.0;
	int icontrol =0;
	// ------------------------ p
	fcontrol = fangle*k_p;
	
	// ------------------------ i
	//sum_i = 
	//fcontrol = fcontrol + sum_i*
	
	// ------------------------ d
	//fcontrol = fcontrol +
	
	//fcontrol = fangle*k_p + k_i + k_d;
	
	
	icontrol = (int)fcontrol;
	icontrol = constrain(icontrol, min_angle, max_angle);
	icontrol = map(icontrol, min_angle, max_angle, -max_pwm, max_pwm); //fixme min_angle -> min_control ; max_angle -> max_control
	//Serial.println(icontrol);

	return icontrol;
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

	if (control_value>0) {direction=true;} else {direction=false;}
	speed = abs(control_value);
	
	

	motor_control(speed, direction);

	delay(10);
}
