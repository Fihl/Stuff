
#include <KeyboardController.h>

USBHost usb;
KeyboardController keyboard(usb);

void USBkbdIdle() 
{
  usb.Task();
}

// This function intercepts key pressed
//void keyPressed() {
//  Serial.println(" PRESS");
//}

// This function intercepts key release
void keyReleased() {
  ShutUpLED = true;
  byte key = keyboard.getOemKey();
  char ch = keyboard.getKey();
  int mod = keyboard.getModifiers(); // bits field with the modifiers-keys

  if (doDebug) printf("Key: %c (%.2x), mod:%2x\n",ch,key,mod);
  delay(100);

  ch = toupper(ch);
  if (mod & RightCtrl) {
    doPrintInfo = true;
    byte newval=0;
    if (key==53) newval=6;            //'1/2' => 6
    else if (key==39) newval=30;      //'0' => 30
    else newval = 2*(key-30) + 10;    //'1'..'9' => 2*(0..8) + 10 => 10..26
    if (newval>=6 & newval<=30)
      curSpeed = newval;
    speed_ms = 1200 / curSpeed;
    if (doDebug) { printf("RCtrl, speed: %d\n",curSpeed); }
  }
  if (mod & LeftCtrl) {
    doPrintInfo = true;
    byte newval=0;
    if (key==39) newval=0;            //'0' => 0
    if (key==53) newval=0;            //'1/2' => 0
    else newval = key-30+1;           //'1'..'9' => 1..9
    if (newval<=9)
      Farnsworth = newval;
    if (doDebug) { printf("LCtrl, Farnsworth: %d\n",Farnsworth); }
  }
  if (ch==0) {
    if (doDebug) printf("Fn %d\n",key);
    doPrintInfo = true;
    switch (key) {
      //ESC
      case 41: sendBuf = ""; break;
      //F5..F8
      case 62: sendBuf += PREvvv " CQ TEST CQ TEST DE " CALL CALL; break;
      case 63: sendBuf += PREvvv " CQ TEST TEST DE " CALL CALL; break;
      case 64: sendBuf = "eeeee"; break; ///??????
      case 65: sendBuf = " e e e e e "; break;
      //F9..F12
      case 66: unBuf="--......--"; break; // 7+3 _73_
      case 67: unBuf="-...-.-"; break; // _BK_
      ///case 68: unBuf=".-.-."; break; // _AR_
      case 69: unBuf=".-.-."; break; // _AR_
    }
    if (doDebug) {
      char raw[50]; 
      sendBuf.toCharArray(raw, 50); if (raw[0]) printf("%s\n",raw);
      unBuf.toCharArray(raw, 50);   if (raw[0]) printf("%s\n",raw);
    }
  }
  if ( (!mod) && (ch >= ' ') ) sendBuf += ch;
}

//enum KeyboardModifiers {
//  LeftCtrl = 1,
//  LeftShift = 2,
//  Alt = 4,
//  LeftCmd = 8,
//  RightCtrl = 16,
//  RightShift = 32,
//  AltGr = 64,
//  RightCmd = 128
//};
