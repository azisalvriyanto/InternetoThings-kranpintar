//library arduino
#include <ArduinoJson.h>

//library WiFi
#include <ESP8266WiFi.h>

//library Firebase
#include <FirebaseArduino.h>

//library waktu
#include <time.h>

//deklarasi Wifi
#define ssid		"16650036"
#define password	"azisalvriyanto"
int loading	= 0;

//deklarasi firebase
#define firebase_host	"wudhu-4fa44.firebaseio.com"
#define firebase_auth	"WZdzDB2OR80v61mhRqxl5wVCXU2nEv5RLhmt4tMg"
String reset;

//deklarasi waterflow
const int pin_waterflow			= D2;
byte sensorInterrupt			= 0;
float calibrationFactor			= 4.5;
volatile byte pulseCount		= 0;
float flowRate					= 0.0;
unsigned long milliliters_flow	= 0;
unsigned long milliliters_total	= 0;
unsigned long oldTime			= 0;
int jumlah, jdata = 0, backup_id = 0;
unsigned long milliliters_total_sebelum	= 0;

//deklarasi waktu
const char* ntpServer			= "us.pool.ntp.org";
const long  gmtOffset_sec		= 3600*7;//GMT+7
const int   daylightOffset_sec	= 0;
char pukul[69], tanggal[69];

//deklarasi perulangan
int perulangan = 0;


void setup() {
	Serial.begin(115200);
	Serial.println();

	//setup wifi
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
		delay(1000);
	}
	Serial.println("WiFi is connected.");
	
	//setup waktu
	configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
	unsigned timeout = 5000;
	unsigned start = millis();
	while (!time(nullptr))
	{
		Serial.print(".");
		delay(1000);
	}

	//setup firebase
	Firebase.begin(firebase_host, firebase_auth);
	
	//setup waterflow
	digitalWrite(pin_waterflow, HIGH);
	attachInterrupt(digitalPinToInterrupt(pin_waterflow), pulseCounter, RISING);
}


void loop() {
	detachInterrupt(sensorInterrupt);

	//set waktu
	time_t rawtime;
	struct tm * timeinfo;
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	strftime (pukul, 80, "%H:%M:%S", timeinfo);
	strftime (tanggal, 80, "%A, %d %B %Y", timeinfo);

	//Reset data ketika status bernilai true
	if (Firebase.getString("status") == "reset") {
		byte sensorInterrupt	= 0;
		calibrationFactor		= 4.5;
		byte pulseCount			= 0;
		flowRate				= 0.0;
		milliliters_flow		= 0;
		milliliters_total		= 0;
		oldTime					= 0;

		//mengembalikan nilai status menjadi false
		if(Firebase.failed()) {
			Serial.println("Status reset gagal diupdate.");
		}
		else {
			Firebase.setString	("backup/"+String(backup_id)+"/pukul"	, pukul						);
			Firebase.setString	("backup/"+String(backup_id)+"/tanggal"	, tanggal					);
			Firebase.setInt		("backup/"+String(backup_id)+"/jumlah"	, milliliters_total_sebelum	);
			Firebase.setString("status", "nothing");
			Firebase.remove("detail");
			backup_id	= backup_id + 1;//id backup
			jdata		= 0;//mengulang penghitungan
			milliliters_total_sebelum = 0;//set ulang total milliliters sebelumnya
			Serial.println("Status reset berhasil diupdate.");
		}
	}

	//menghitung mL
	flowRate			= ((1000.0/(millis()-oldTime))*pulseCount)/calibrationFactor;
	oldTime				= millis();
	milliliters_flow	= (flowRate/60)*1000;//mL/Sec
	milliliters_total  += milliliters_flow;//mL

	//pengiriman data ke firebase
	perulangan = perulangan + 1;
	if (perulangan >= 10) {//Setip 10 detik sekali data akan dikirim
		if (WiFi.status() == WL_CONNECTED) {
			Serial.println();
			Serial.print("IP address: ");
			Serial.println(WiFi.localIP());

			if(Firebase.failed()) {
				Serial.println("Data gagal diunggah.");
			}
			else {
				//pengiriman
				jumlah = milliliters_total - milliliters_total_sebelum;
				Firebase.setString	("pukul"		, pukul				);
				Firebase.setString	("tanggal"		, tanggal			);
				Firebase.setInt		("kecepatan"	, milliliters_flow	);
				Firebase.setInt		("total"		, milliliters_total	);
				Firebase.setString	("detail/" + String(jdata) + "/pukul"	, pukul		);
				Firebase.setString	("detail/" + String(jdata) + "/tanggal"	, tanggal	);
				Firebase.setInt		("detail/" + String(jdata) + "/jumlah"	, jumlah	);
				jdata = jdata + 1;

				Serial.print("Data berhasil diunggah pada ");
				Serial.print(pukul);
				Serial.print(" - ");
				Serial.print(tanggal);
				Serial.println(".");
				
				perulangan = 0;//set ulang perulangan
			}
		}
		else {
			Serial.println();
			Serial.println("WiFi not connected");
		}
	}

	//perulangan data waterflow
	milliliters_total_sebelum = milliliters_total;
	pulseCount = 0;
	attachInterrupt(sensorInterrupt, pulseCounter, FALLING);

	//delay "loop()" (1 detik)
	delay(1000);
}


//fungsi penjumlahan data waterflow
void pulseCounter() {
	pulseCount++;
}