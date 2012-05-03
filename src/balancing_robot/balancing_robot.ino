int led_pin = 13;      // select the pin for the LED

int motor1_pin_e = 9;
int motor1_pin_a = 10;
int motor1_pin_b = 11;


void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(led_pin, OUTPUT);

  pinMode(motor1_pin_e, OUTPUT);
  pinMode(motor1_pin_a, OUTPUT);
  pinMode(motor1_pin_b, OUTPUT);

}


int sensor


void loop() {

  digitalWrite(led_pin, HIGH);
  digitalWrite(motor1_pin_a, HIGH);
  digitalWrite(motor1_pin_b, HIGH);
  analogWrite(motor1_pin_e, 256);
  delay(2000);
  digitalWrite(led_pin, LOW);
  delay(2000);
  
}


