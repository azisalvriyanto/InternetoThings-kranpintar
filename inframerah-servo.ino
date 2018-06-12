#include <Servo.h>

Servo myservo;
boolean lockLow = true;
boolean takeLowTime;
int pir_input = 12;
int pir_output = 13;

void setup() {
	myservo.attach(4);
	Serial.begin(9600);
	pinMode(pir_input, INPUT);
	pinMode(pir_output, OUTPUT);
	digitalWrite(pir_output, HIGH);

	Serial.print("\n[ MULAI ]");
}

void loop() {
	if (digitalRead(pir_input) == HIGH) {
		if (lockLow) {
			myservo.write(0);
			lockLow = false;
			
			Serial.print("\n=> Sensor jarak terdeteksi.");
		}

		takeLowTime = true;
	}

	if (digitalRead(pir_input) == LOW) {
		if (takeLowTime) {
			takeLowTime = false;
		}

		if (!lockLow) {
			myservo.write(80);
			lockLow = true;

			Serial.print("\n=> Sensor jarak tidak ada.");
		}
	}
}