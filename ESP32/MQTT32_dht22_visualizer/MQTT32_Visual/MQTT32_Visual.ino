/***********************************************************************
    
  Using Adafruit MQTT Library on ESP32. Using SSL/TLS
  https://learn.adafruit.com/mqtt-adafruit-io-and-you/intro-to-adafruit-mqtt
  https://io.adafruit.com/api/docs/mqtt.html

  Test on Lolin C3 Super Mini. SDA/SCL on 8/10 as debug output shows
  [esp32-hal-i2c.c:112] i2cInit(): Initializing I2C Master: sda=8 scl=10 freq=100000

 **********************************************************************/

#include <WiFi.h>
#include "WiFiClientSecure.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#include "Screen.h"

#define LOGO "MQTT32_Visual"

// Setup: Adafruit.io 
#include "PrivateCredentials.h" // WiFi, AIO_SERVER, AIO_SERVERPORT, AIO_, AIO_USERNAME, AIO_KEY, adafruitio_root_ca

// WiFiFlient, Secure for SSL/TLS support
WiFiClientSecure client;

// MQTT client
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Setup a feed for Subscribeing, one for each value
#define SUB "3"  //Mine is called Temp3, Fugt3, ...
Adafruit_MQTT_Subscribe Temp   = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Temp" SUB);
Adafruit_MQTT_Subscribe Humi   = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Fugt" SUB);

// Connect and reconnect as necessary to the MQTT server.
void MQTT_connect() {
  if (mqtt.connected()) return; // Stop if already connected.
  scrBooting();
  
  Serial.print("Connecting to MQTT...  Reading " AIO_USERNAME "/feeds/Temp" SUB);

  int8_t ret, retries = 1;
  while (ret = mqtt.connect() ) { // connect will return 0 for connected
    scrBooting();
    if (retries++ == 5) while(1); //Reboot 5 min
    Serial.println(mqtt.connectErrorString(ret));
    Serial.printf("Retrying MQTT connection in 5 seconds..., %d/5",retries);
    mqtt.disconnect();
    for (byte n=0; n<6; n++,delay(10000) ) scrBooting(); // 1 min 
  }
  scrBooting();
  Serial.println(", MQTT Connected!");
}

int32_t TempVal, HumiVal;
bool gotData;
byte noInfoSecs=15;

void doDataStuff()
{ 
  scrBooting();
  if (noInfoSecs++ >=24) {  //Each step = 10 secs
    noInfoSecs = 24;
    scrCLS();
    static byte pos01=0;
    scrTextBig((pos01++) & 1,"   Offline",0);
  }
  // Get some data, when ready!
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    gotData = true;
    if (subscription == &Temp) TempVal = atoi( (char *) Temp.lastread);
    if (subscription == &Humi) HumiVal = atoi( (char *) Humi.lastread);
    delay(1000); //Slow down
  }
  if (gotData) {
    noInfoSecs = 0;
    Serial.printf(F("Read Temp:%d, Humidity:%d\n"), TempVal, HumiVal);
    scrCLS();
    if (TempVal) scrTextBig(1,"Temperatur",TempVal);
    if (HumiVal) scrTextBig(0,"Fugt",HumiVal);
  }
  gotData = false;
}

void loop() { 
  scrBooting();
  delay(5000);
  MQTT_connect(); //Ensure the connection to the MQTT server is alive. 
  
  if (bootVar) {
    bootVar = 0;
    scrCLS();
    scrTextBig(1,"   Online",0);
  }
  
  doDataStuff();
  if(! mqtt.ping()) {
    Serial.println("Disconnecting...");
    mqtt.disconnect();
    noInfoSecs = 200;
  }
} 

void setup() {
  delay(1000);
  Serial.begin(115200);
  delay(2000);
  Serial.println(F("\n" LOGO ".ino"));
  scrTextSetup();
  scrBooting();
  delay(500);

  // Connect to WiFi access point.
  WiFi.begin(WLAN_SSID, WLAN_PASS); //Status later..
  Serial.printf(F("Connecting to %s"),WLAN_SSID);
  byte retry;
  retry=0;
  while (WiFi.status() != WL_CONNECTED) {
    scrBooting();
    Serial.print(F(" .."));
    delay(5000);
    if (retry++ == 100) while(1); //Reboot
  }
  Serial.println();
  client.setCACert(adafruitio_root_ca); //Adafruit IO's root CA
  Serial.printf("WiFi connected, IP address: %s\n",WiFi.localIP() .toString().c_str() );
  scrBooting();
  delay(1000);
  scrBooting();
  
  mqtt.subscribe(&Temp);
  mqtt.subscribe(&Humi); 
}
