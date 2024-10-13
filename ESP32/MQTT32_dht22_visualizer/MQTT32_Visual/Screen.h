#pragma once

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SSDport 0x3c    // 3c or 3d
#define CRTROTATION 2

Adafruit_SSD1306 display(128, 32, &Wire, -1);

void scrTextSetup()
{
  //Wire.begin(8,10);  //8,10 is default for C3 Super Mini
  if (!display.begin(SSD1306_SWITCHCAPVCC, SSDport) ) { // Address 0x3C for 128x32, 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    while(1);
  }
  display.setRotation(CRTROTATION); // Uncomment to rotate display 180 degrees
  display.setTextColor(SSD1306_WHITE);
  display.cp437(true);
  display.display();
}

void scrCLS()
{
  display.clearDisplay();
}

void scrTextBig(int line, char* prompt, byte value)
{
  display.setTextSize(1);
  display.setCursor(8, line*16+4);
  display.print(prompt);
  display.display();
  if (!value) return;
  display.setTextSize(2);
  display.setCursor(80, line*18+0);
  display.print(value);
  display.display();
}

byte bootVar=1; // Set to zero, to stop "Boot" info
unsigned long bootTm; 
void scrBooting()
{
  if ( bootVar && (millis()-bootTm > 500) ) {
    bootTm = millis();
    scrCLS();
    scrTextBig(1,"   Boot",bootVar);
    if (bootVar++==100) bootVar=1;
  }
}
