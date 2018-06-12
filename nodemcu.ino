#include <ESP8266WiFi.h>

const char* ssid		= "aswaswasw";
const char* password	= "qazwsxedc";
int loading = 0;
boolean status = true;

void setup() {
	Serial.begin(115200);
	delay(200);

	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(ssid);

	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
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
}

void loop() {
	if(WiFi.status() == WL_CONNECTED){
		if (status)
		{
			Serial.println();
			Serial.println("ESP is connected!");  
			Serial.print("IP address: ");
			Serial.println(WiFi.localIP());

			status = false;
		}
	}
	else{
		if (!status)
		{
			Serial.println();
			Serial.println("ESP not connected");

			status = true;
		}
	}
}