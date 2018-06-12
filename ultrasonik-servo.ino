//library servo
#include <Servo.h>

//deklarasi sensor ultarsonik
int pin_trig = 6;
int pin_echo = 7;
long duration, jarak=30, jarak1, jarak2=200;

//deklarasi servo
Servo myservo;
int pin_servo = 9;
boolean status = true;


void setup() {
	Serial.begin(9600);

	//setup senesor ultrasonik
	pinMode(pin_trig, OUTPUT);
	pinMode(pin_echo, INPUT);

	//setup servo
	myservo.attach(pin_servo);
}


void loop() {
	//kalibrasi sensor ultrasonik
	digitalWrite(pin_trig, LOW);
	delayMicroseconds(2);
	digitalWrite(pin_trig, HIGH);
	delayMicroseconds(10);
	digitalWrite(pin_trig, LOW);

	//menhitung jarak
	duration = pulseIn(pin_echo, HIGH);
	jarak1 = (duration/29)/2;

	//pengondisian jarak (range yang terdeteksi 5 - 200 cm)
	if ((jarak1 >= 5 && jarak1 <= 200) && (jarak2 >= 5 && jarak2 <= 200)) {
		if (jarak1 > jarak && jarak2 > jarak) {//kondisi jarak melebihi value variabel "jarak"
			if (status) {
				myservo.write(80);//kondisi dimana kran tertutup

				Serial.print("MATI =||= jarak1 = ");
				Serial.print(jarak1);
				Serial.print(" | jarak2 = ");
				Serial.print(jarak2);
				Serial.println();

				status = false;
			}
		}
		else {
			if (!status) {
				myservo.write(0);//kondisi dimana kran terbuka

				Serial.print("HIDUP =||= jarak1 = ");
				Serial.print(jarak1);
				Serial.print(" | jarak2 = ");
				Serial.print(jarak2);
				Serial.println();

				status = true;
			}
		}

		jarak2 = jarak1;
		delay(1000);//delay "loop()" 1 detik
	}
}