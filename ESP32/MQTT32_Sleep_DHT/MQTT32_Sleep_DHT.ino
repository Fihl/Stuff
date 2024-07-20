/***********************************************************************
  
  Using Adafruit MQTT Library on ESP32. Using SSL/TLS

  To upload, do reset board manually, as USB prot then is open in 60 seconds

 **********************************************************************/

#include <WiFi.h>
#include "WiFiClientSecure.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h" //Get the one from Adafruit

#define LOGO "MQTT32_Sleep_DHT"
#define DeepSleep 1

#include "esp_sleep.h"
RTC_DATA_ATTR int32_t SavtakValue = 0;

// Adafruit.io Setup
#include "PrivateCredentials.h" // AIO_SERVER, AIO_SERVERPORT, AIO_, AIO_USERNAME, AIO_KEY, AIO_FINGERPRINT, adafruitio_root_ca

// WiFiFlient, Secure for SSL/TLS support
WiFiClientSecure client;

// MQTT client
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Setup a feed for publishing, one for each value
// Remember max rate of 30 packets per minute
#define TEMP "Temp"
Adafruit_MQTT_Publish Temp   = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/" TEMP);
#define HUMI "Humi"
Adafruit_MQTT_Publish Humi   = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/" HUMI);
Adafruit_MQTT_Publish Savtak = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Savtak");

// DHT 11/22 pins
#define DHT_GND 0
#define DHT_PIN 1
#define DHT_Vcc 2

DHT dht(DHT_PIN, DHT11);   //Select DHT11 or DHT22

void blink(byte cnt);
void GoToSleep(int Secs);

void setup() {
  setCpuFrequencyMhz(80); //https://www.espboards.dev/blog/esp32-power-optimisation/
  blink(3); //0=LED_on
  pinMode(DHT_GND, OUTPUT); digitalWrite(DHT_GND,0); //Power up the DHTxx
  pinMode(DHT_Vcc, OUTPUT); digitalWrite(DHT_Vcc,1); 
  dht.begin();
  
  delay(1000); //Delay before initilizing USB, or else crash or worse
  Serial.begin(115200);
  while (!Serial); //delay(1000);
  Serial.println(F(LOGO));
  
  // Connect to WiFi access point.
  Serial.printf(F("Connecting to %s"),WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  byte n=0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(F(" .."));
    blink(3);
    if (n++ == 10) GoToSleep(10*60);
  }
  Serial.println();

  //Serial.println("WiFi connected, IP address: "); Serial.println(WiFi.localIP());
  Serial.printf("WiFi connected, IP address: %s\n",WiFi.localIP() .toString().c_str() );
  // https://arduino.stackexchange.com/questions/30534/formatting-of-ipaddress-via-printf-family-similar-to-serial-object

  // Set Adafruit IO's root CA
  client.setCACert(adafruitio_root_ca);

  if (print_wakeup_reason() == 0) {
    doDataStuff();
    Serial.println(F("Waiting 60 secs, to allow for new compile/upload"));
    delay(60000); //First time around. To give more time for reprograming
  }
}

void loop() {
  doDataStuff();
  if (DeepSleep) GoToSleep(60);
  Serial.println(F("Never here, unless not DeepSleeping"));  
  delay(10000); 
  blink(1);
}

// Connect and reconnect as necessary to the MQTT server.
void MQTT_connect() {
  if (mqtt.connected()) return; // Stop if already connected.
  
  Serial.print("Connecting to MQTT... ");

  int8_t ret, retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    blink(2);
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) GoToSleep(15*60);
    //while (retries == 0) { blink(5); delay(1000); } // basically die and wait for WDT to reset me
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
  if (HumiVal>100) HumiVal=0;
  if (SavtakValue++ == 10) SavtakValue=0; //SavtakValue is saved, even through sleep/boot
  Serial.printf(F(", Sending Temp:%d, Humidity:%d, Saw:%d to feed... "), TempVal, HumiVal, SavtakValue);
  //Serial.printf(F(", Sending Temperature:%dC (to %s),"), TempVal, TEMP);
  //Serial.printf(F(", Sending Humidity:%d%% (to %s),"), HumiVal, HUMI);
  bool Ok =  Temp.publish(TempVal);
  Ok = Ok & Humi.publish(HumiVal); 
  Ok = Ok & Savtak.publish(SavtakValue);
  if (Ok) Serial.println(F(", OK!")); else Serial.println(F(", Failed"));
}

void blink(byte cnt)
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 1-digitalRead(LED_BUILTIN) );
  while (cnt-- > 0) {
    //Serial.printf("%d, ",cnt);
    digitalWrite(LED_BUILTIN, 1);
    delay(100);
    digitalWrite(LED_BUILTIN, 0);
    delay(233);
  }
}

esp_sleep_wakeup_cause_t print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by TIMER"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by TOUCHPAD"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup WAS NOT caused by deep sleep: %d\n",wakeup_reason); break;
  }
  return wakeup_reason;
}

//https://raw.githubusercontent.com/RuiSantosdotme/ESP32-Course/master/code/DeepSleep/TimerWakeUp/TimerWakeUp.ino
void GoToSleep(int Secs)
{
  Serial.println(F("Going to sleep in 3.."));
  digitalWrite(LED_BUILTIN, 1); //Going to sleep
  Serial.flush();
  blink(10);
  if (!DeepSleep) return;
  //esp_sleep_pd_config(ESP_PD_DOMAIN_VDDSDIO, ESP_PD_OPTION_OFF);
  //esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_OFF);

//  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
//  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
//  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);

  esp_sleep_enable_timer_wakeup( (int32_t)Secs * 1000000); // xx Secs
  esp_deep_sleep_start();
  Serial.println(F("Never here"));

  Serial.println(F("Going to sleep in 3........."));
  blink(3);
  // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH,  ESP_PD_OPTION_OFF);
  // esp_deep_sleep_pd_config(ESP_PD_DOMAIN_XTAL,   ESP_PD_OPTION_OFF);
  esp_sleep_enable_timer_wakeup( 40000000); // xx Secs
  esp_deep_sleep_start();
  
}
