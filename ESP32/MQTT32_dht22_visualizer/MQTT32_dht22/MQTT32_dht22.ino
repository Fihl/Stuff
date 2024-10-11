/***********************************************************************
  
  Using Adafruit MQTT Library on ESP32. Using SSL/TLS

 **********************************************************************/

// DHT 11/22 pins
#define DHT_GND 2
#define DHT_PIN 1
#define DHT_Vcc 0

#include <WiFi.h>
#include "WiFiClientSecure.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h" //Get the one from Adafruit

#define LOGO "MQTT32_dht22"

// Do copy my initial file into same folder. 
// Or better into the library folder, for reuse in other projects 

// Setup: Adafruit.io 
#include "PrivateCredentials.h" // AIO_SERVER, AIO_SERVERPORT, AIO_, AIO_USERNAME, AIO_KEY, AIO_FINGERPRINT, adafruitio_root_ca

// WiFiFlient, Secure for SSL/TLS support
WiFiClientSecure client;

// MQTT client
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Setup a feed for publishing, one for each value
// Remember max rate of 30 packets per minute
#define SUB "3"  //Mine is called Temp3, Fugt3, ...
Adafruit_MQTT_Publish Temp   = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Temp" SUB);
Adafruit_MQTT_Publish Humi   = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Fugt" SUB);
Adafruit_MQTT_Publish Online = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Online" SUB);

DHT dht(DHT_PIN, DHT22);   //Select DHT11 or DHT22

// Connect and reconnect as necessary to the MQTT server.
void MQTT_connect() {
  if (mqtt.connected()) return; // Stop if already connected.
  
  Serial.print("Connecting to MQTT... ");

  int8_t ret, retries;
  retries = 0;
  while (ret = mqtt.connect() ) { // connect will return 0 for connected
    blink(2);
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    blink(2);
    delay(5000);
    if (retries++ == 100) while(1); //Reboot
  }
  Serial.println("MQTT Connected!");
}

void doDataStuff()
{ 
  MQTT_connect(); //Ensure the connection to the MQTT server is alive. 
  
  // Now we can publish!
  Serial.printf(F("Check Temperature & Humidity"));
  int32_t TempVal = dht.readTemperature(false);
  int32_t HumiVal = dht.readHumidity();
  if (TempVal>100) TempVal=0;
  if (HumiVal>101) HumiVal=0;
  Serial.printf(F(", Sending Temp:%d, Humidity:%d"), TempVal, HumiVal);
  bool Ok =  Temp.publish(TempVal);
  Ok = Ok & Humi.publish(HumiVal); 
  static byte MinutCounter = 58;
  static int32_t OnlineVal = 0;
  if (MinutCounter++ == 60) {
    MinutCounter=0; // 1 hour increment
    OnlineVal++;
    Serial.printf(F(", Online:%d"), OnlineVal);
    Ok = Ok & Online.publish(OnlineVal);
  }
  if (Ok) Serial.println(F(", OK!")); else Serial.println(F(", Failed"));
}

void blink(byte cnt)
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 1-digitalRead(LED_BUILTIN) );
  while (cnt-- > 0) {
    digitalWrite(LED_BUILTIN, 1);
    delay(333/2);
    digitalWrite(LED_BUILTIN, 0);
    delay(222/2);  //Total 333mSec per blink
  }
  pinMode(LED_BUILTIN, INPUT);
}

void loop() { 
  doDataStuff();
  delay(60000); 
} 

void setup() {
  delay(5000);  
  Serial.begin(115200);
  blink(3);
  Serial.println(F("\n" LOGO ".ino"));

  // Connect to WiFi access point.
  WiFi.begin(WLAN_SSID, WLAN_PASS); //Status later..
  Serial.printf(F("Connecting to %s"),WLAN_SSID);
  byte retry;
  retry=0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(F(" .."));
    blink(5);
    delay(5000);
    if (retry++ == 100) while(1); //Reboot
  }
  Serial.println();
  client.setCACert(adafruitio_root_ca); //Adafruit IO's root CA
  Serial.printf("WiFi connected, IP address: %s\n",WiFi.localIP() .toString().c_str() );
  pinMode(DHT_Vcc, OUTPUT); digitalWrite(DHT_Vcc,1); //Power up the DHTxx 
  pinMode(DHT_GND, OUTPUT); digitalWrite(DHT_GND,0);
  dht.begin();
  delay(1000);
}
