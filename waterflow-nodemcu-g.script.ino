#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>


//Wifi
const char* ssid		= "16650036";
const char* password	= "azisalvriyanto";
int loading				= 0;
WiFiClientSecure client;


//Script
String readString;
const char* host		= "script.google.com";
const int httpsPort		= 443;
String script_id		= "AKfycbzR8Qdg5prLrU1uTkEHpnOzUsDDG_b27dP53btK34g2MNp6PovF";//Script ID


//Water Flow
const int pin_waterflow			= D2;
byte sensorInterrupt			= 0;
float calibrationFactor			= 4.5;
volatile byte pulseCount		= 0;
float flowRate					= 0.0;
unsigned long flowLitres		= 0;
unsigned long totalLitres		= 0;
unsigned long oldTime			= 0;


//perulangan
int perulangan = 0;


void setup() {
	Serial.begin(115200);
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(ssid);

	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {//loading to connecting
		delay(750);
		if (loading == 1)
		{
			Serial.println("Please wait .");
		}
		else if (loading == 2)
		{
			Serial.println("Please wait ..");
		}
		else if (loading == 3)
		{
			Serial.println("Please wait ...");
			loading = 0;
		}

		loading = loading + 1;
	}


	digitalWrite(pin_waterflow, HIGH);
	attachInterrupt(digitalPinToInterrupt(pin_waterflow), pulseCounter, RISING);
}


void loop() {
	detachInterrupt(sensorInterrupt);
	flowRate			= ((1000.0/(millis()-oldTime))*pulseCount)/calibrationFactor;
	oldTime				= millis();
	flowLitres			= (flowRate/60);//L/Sec
	totalLitres		   += flowLitres;//L

	perulangan = perulangan + 1;
	if (perulangan >= 10)//Setip 10 detik sekali data dikirim ke Spreadsheet
	{
		if(WiFi.status() == WL_CONNECTED){
			Serial.println();
			Serial.println("ESP is connected!");
			Serial.print("IP address: ");
			Serial.println(WiFi.localIP());

			sendData(totalLitres);
			perulangan = 0;
		}
		else{
			Serial.println();
			Serial.println("ESP not connected");
		}
	}


	pulseCount = 0;
	attachInterrupt(sensorInterrupt, pulseCounter, FALLING);

	delay(1000);
}


void pulseCounter() {
	pulseCount++;
}


void sendData(int jumlah) {
	Serial.print("Connecting to ");
	Serial.println(host);
	if (client.connect(host, httpsPort)) {
		String string_jumlah	= String(jumlah, DEC);
		String url = "/macros/s/"+script_id+"/exec?jumlah="+string_jumlah;
		Serial.print("Requesting URL: ");
		Serial.println(url);

		client.print(String("GET ") + url + " HTTP/1.1\r\n" +
			"Host: " + host + "\r\n" +
			"User-Agent: BuildFailureDetectorESP8266\r\n" +
			"Connection: close\r\n\r\n");
	}
	else {
		Serial.println("=[ CONNECTION FAILED ]=");
	}

	Serial.println();
}