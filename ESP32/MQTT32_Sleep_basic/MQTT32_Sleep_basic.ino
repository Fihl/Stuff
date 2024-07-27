
#include "esp_sleep.h"

#define LOGO "MQTT32_Sleep_basic"

#define LED_BUILTIN 8     //Not 7 
//S2 Mini = 15, correct

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

esp_sleep_wakeup_cause_t print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.print(F("Wakeup caused by external signal using RTC_CNTL")); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.print(F("Wakeup caused by TIMER")); break;
    default : Serial.print(F("Wakeup WAS NOT caused by deep sleep"));
  }
  Serial.printf(F(": %d\n"),wakeup_reason);
  return wakeup_reason;
}

//https://raw.githubusercontent.com/RuiSantosdotme/ESP32-Course/master/code/DeepSleep/TimerWakeUp/TimerWakeUp.ino
void GoToSleep(int Secs)
{
  Serial.printf(F("Going to sleep for %d secs\n"),Secs);
  Serial.flush();
//  blink(2);
  
//  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
//  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
//  esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL,         ESP_PD_OPTION_OFF);

  delay(100);
  esp_sleep_enable_timer_wakeup( (int32_t)Secs * 1000000); // xx Secs
  esp_deep_sleep_start();
  Serial.println(F("Never here....."));
}

void loop() { }

void setup() 
{
  setCpuFrequencyMhz(80); //https://www.espboards.dev/blog/esp32-power-optimisation/
  blink(1);
  delay(1000); //Delay before initilizing USB, or else crash or worse
  Serial.begin(115200);
  delay(1000);
  Serial.printf(F("\n" LOGO ".ino (LED=%d)\n"),LED_BUILTIN); 
  print_wakeup_reason();
  blink(1);
  GoToSleep(15);
  
  Serial.println(F("Never here, unless not DeepSleeping"));  
  blink(1);
}
