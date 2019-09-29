#define _DISABLE_TLS_

#include <ThingerESP8266.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET LED_BUILTIN

#define USERNAME "julesrimet3000"
#define DEVICE_ID "SWM"
#define DEVICE_CREDENTIAL "ABCDEFGHIJ"

#define SSID "Kampret"
#define SSID_PASSWORD "kampret123"
Adafruit_SSD1306 display(OLED_RESET);

byte indikator = 13;
 
byte sensorInt = 0; 
byte flowsensor = D3;
 
float konstanta = 4.5; //konstanta flow meter
 
volatile byte pulseCount;
 
float debit;
float harga;
unsigned int flowmlt;
unsigned long totalmlt;
 
unsigned long oldTime;

ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

void setup()
{
display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
display.clearDisplay();
display.display();
display.setTextSize(1);
display.setTextColor(WHITE);
display.setCursor(0, 0);
 
// Inisialisasi port serial
Serial.begin(9600);
 
pinMode(flowsensor, INPUT);

pulseCount = 0;
debit = 0.0;
flowmlt = 0;
totalmlt = 0;
oldTime = 0;
harga = 0.0;

// digital pin control example (i.e. turning on/off a light, a relay, configuring aparameter, etc)
thing["sensor"] >> [](pson& out){

digitalWrite(flowsensor, HIGH);

attachInterrupt(digitalPinToInterrupt(D3), pulseCounter, FALLING);

out["debit"] = debit;
out["volume"] = totalmlt;
out["harga"] = harga;

};
}

void loop()
{
thing.handle();
display.clearDisplay();
if((millis() - oldTime) > 1000) 
{ 
detachInterrupt(sensorInt);
debit = ((1000.0 / (millis() - oldTime)) * pulseCount) / konstanta;
oldTime = millis();
flowmlt = (debit / 60) * 1000;
totalmlt += flowmlt;
harga = totalmlt*0.002;

unsigned int frac;
 
Serial.print("Debit air: ");
Serial.print(int(debit));
Serial.print("L/min");
Serial.print("\t");
display.setCursor(0, 0);
display.print("Debit air: ");
display.setCursor(60, 0);
display.print(int(debit));
display.setCursor(85, 0);
display.print("L/min");


Serial.print("Volume: "); 
Serial.print(totalmlt);
Serial.print("mL"); 
Serial.print("\t");
display.setCursor(0, 12);
display.print("Volume: ");
display.setCursor(50, 12); 
display.print(totalmlt);
display.setCursor(100, 12);
display.print("mL"); 
display.print("\t");

Serial.print("Harga: "); 
Serial.print("Rp  "); 
Serial.println(harga);
display.setCursor(0, 24);
display.print("Harga: "); 
display.setCursor(45, 24);
display.print("Rp  "); 
display.setCursor(70, 24);
display.println(harga);
display.display();


pulseCount = 0;
 
attachInterrupt(digitalPinToInterrupt(D3), pulseCounter, FALLING);
}
}
 
 
void pulseCounter()
{
// Increment the pulse counter
pulseCount++;
}