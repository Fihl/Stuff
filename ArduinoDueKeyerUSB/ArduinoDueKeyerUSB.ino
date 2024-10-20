/* ArduinoDueKeyerUSB
 *  
 * Copyrights: Christen Fihl, OZ1AAB
 * https://www.facebook.com/groups/oz1aab 
 * https://www.fihl.net 
 * 
*/

#define CALL "OZ1AAB "
#define PREvvv "vvv "
#define INFO "ArduinoDueKeyerUSB"

#define doDebug true

#undef printf // https://forum.arduino.cc/t/printf-on-due/144797/3
#include <stdio.h>

#include <Cth.h> //CopyThreads, real nice tool

String sendBuf = ""; 
volatile char sendBufKbd; //Set in USBkbd
bool doPrintInfo = true;
byte curSpeed=15;
int speed_ms = 1200 / 15;  // = 1200 / curSpeed;
byte Farnsworth=1; //0..9
int txBits;
int msDIHcounter;
String cw;
String unBuf;
bool ShutUpLED;

#define LED_KEYint      LED_BUILTIN
#define LED_GND         2
#define LED_ON          3
#define LED_KEY         4
#define KEY_NPN_GND     A5  //Emitter on NPN
#define KEY_NPN_BASE    A6  //Base on NPN, throuh few kOhm

void setup()
{ if (doDebug) Serial.begin(115200);
  startTimer(TC1, 0, TC3_IRQn, 1000); //TC1 channel 0, the IRQ for that channel and the desired frequency
  Scheduler.startLoop(LoopKbd);
  delay(2000); //Wait for USB ready, if there
  if (doDebug) Serial.println("Start " INFO ", " CALL);
}

void startTimer(Tc *tc, uint32_t channel, IRQn_Type irq, uint32_t frequency) {
  pmc_set_writeprotect(false);
  pmc_enable_periph_clk((uint32_t)irq);
  TC_Configure(tc, channel, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK4);
  uint32_t rc = VARIANT_MCK/128/frequency; //128 because we selected TIMER_CLOCK4 above
  TC_SetRA(tc, channel, rc/2); //50% high, 50% low
  TC_SetRC(tc, channel, rc);
  TC_Start(tc, channel);
  tc->TC_CHANNEL[channel].TC_IER=TC_IER_CPCS;
  tc->TC_CHANNEL[channel].TC_IDR=~TC_IER_CPCS;
  NVIC_EnableIRQ(irq);
}

void TC3_Handler() //TC1 ch 0
{ TC_GetStatus(TC1, 0);
  LoopKeying1mSec();
}

void doKeyCW() {
  for (byte n=0; n<cw.length(); n++) {
    int len = cw[n]=='-'?3*speed_ms:speed_ms;
    msDIHcounter = len;  // Handled in 1mSec Interrupt controller part
    Scheduler.delay(len+speed_ms); //interdih = +1 dih
  }
  Scheduler.delay( (Farnsworth+2) *speed_ms); //interchar = 3
}

void LoopKbd() 
{ 
  Scheduler.delay(1);
  
  if (sendBuf != "") {
    char ch= sendBuf[0];
    sendBuf.remove(0,1);
    txBits = decode(toupper(ch)); //' '=> 0
    if (!txBits) {
      Scheduler.delay(7*speed_ms);
      return;
    }
    cw="";
    do {
      if (txBits & 1) cw="-"+cw; else cw="."+cw; 
      txBits = txBits/2;
    } while (txBits>1);
    //if (doDebug) Serial.println(cw);
    doKeyCW();
    return;
  }

  if (unBuf!="") {
    cw=unBuf;
    unBuf="";
    //if (doDebug) Serial.println(cw);
    doKeyCW();
    return;
  }
}

void LoopKeying1mSec()
{
  byte curBit = 0;
  if (msDIHcounter) {
    noInterrupts();
      msDIHcounter--;
    interrupts();
    curBit = 1;
  }
  digitalWrite(LED_KEYint, curBit); pinMode(LED_KEYint, OUTPUT); 
  
  digitalWrite(LED_KEY, curBit);
  pinMode(LED_KEY, OUTPUT);
  pinMode(LED_GND, OUTPUT);
  pinMode(LED_ON,  OUTPUT);
  digitalWrite(LED_GND, 0);
  digitalWrite(KEY_NPN_GND, 0);     pinMode(KEY_NPN_GND, OUTPUT); //Emiter on NPN
  
  pinMode(KEY_NPN_BASE, OUTPUT); 
  digitalWrite(KEY_NPN_BASE, curBit); // 0 = Off. 1=ON

  static int decay=2000;  //Delay 2 secs at startup :-) 
  if (ShutUpLED | curBit) decay=1000; //Delay after each TX, and usb input
  ShutUpLED = false;
  if (decay==1) digitalWrite(LED_ON, 1);
  if (decay>1) {
    decay--;
    digitalWrite(LED_ON, 0);  //Delayed ON LED
  }
}

void loop() {
  Scheduler.yield(); 

  if (doDebug) 
    if (Serial.available()) {  //Also TX from serial port
      while (Serial.available()) {
        char ch=Serial.read();
        if (ch>=' ')
          sendBuf += ch;
      }
    }
  
  USBkbdIdle(); //Poll USB hostport
  if (sendBufKbd>=' ') 
    sendBuf += sendBufKbd;
  sendBufKbd = 0;
  
  if (doPrintInfo & doDebug) {
    doPrintInfo = false;
    Serial.print("Speed:"); Serial.print(curSpeed);
    Serial.print(", delayMs:"); Serial.print(speed_ms);
    Serial.print(", Farnsworth:"); Serial.print(Farnsworth);
    Serial.print(", sendBuf:"); Serial.println(sendBuf);
  }
}
