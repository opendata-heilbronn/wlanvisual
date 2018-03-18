/**
 * WLAN RSSI Visualisation Firmware | wlanvisual.ino
 *
 * @package     ch.mikroelek.arduino.wlanvisual
 * @author      Tim Oldenbuerger <t.o@mikroelek.ch>
 * @author		Franz Imschweiler
 * @author		Leon Hellmich
 * @author		Leandro Späth
 * @author		Patrick Krämer
 * @version     v 1.2.0 (2018-03-17)
 * @copyright   Copyright (c) 2018
 */

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include "FastLED.h"
#include <SPI.h>
#include <WiFiClient.h>
#include <Wire.h>

#define DEBUG 1
#define NUM_LEDS 32
#define BRIGHTNESS 25

const char* ssid = "***";
const char* password = "***";

#define OLED_RESET LED_BUILTIN  //4
Adafruit_SSD1306 display(OLED_RESET);

long rssi;
String state = "UNKNOWN";

CRGB leds[NUM_LEDS];
CHSV color;
CHSV black;

const uint16_t numValues = 20;
uint8_t integration[numValues] = {0};
uint8_t intZaehler = 0;

unsigned long lastHistory = 0;
int history[128];
byte histIdx = 0;

void setup()
{
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
	display.clearDisplay();
 	display.display();

	display.setTextSize(1);
	display.setTextColor(WHITE);
	display.setCursor(0,0);
	display.println("Franz&Leon&Tim&");
  	display.println("Leandro&Patrick");
  	display.println("");
	display.println("WLAN to LED Matrix");
	display.display();

  	pinMode(D0, OUTPUT);	// Internal LED

	FastLED.addLeds<WS2812B, 5, GRB>(leds, NUM_LEDS);
	for (int x=0; x<32; x++)
	{
		leds[x] = CRGB::Black;
	}
	FastLED.setBrightness(BRIGHTNESS);
	FastLED.show();

	if (DEBUG == 1)
	{
		delay(500);				// Wait for Arduino IDE Serial Console
		Serial.begin(115200);
		Serial.print("Connecting to ");
		Serial.println(ssid);
	}

	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		if (DEBUG == 1)
		{
			Serial.print(".");
		}
	}
	if (DEBUG == 1)
	{
		Serial.println("WiFi connected");
		Serial.print("IP address: ");
		Serial.println(WiFi.localIP());
	}
}



void loop() {
	rssi = WiFi.RSSI();
  	if (WiFi.status() == WL_CONNECTED)
 	{
 		state = "CONNECTED";
  	} else
  	if (WiFi.status() == WL_DISCONNECTED)
 	{
    	state = "DISCONNECTED";
  	} else
  	if (WiFi.status() == WL_CONNECTION_LOST)
  	{
    	state = "CONNECTION LOST";
  	} else
  	if (WiFi.status() == WL_NO_SSID_AVAIL)
 	{
    	state = "NO SSID AVAILABLE";
  	} else
  	if (WiFi.status() == WL_IDLE_STATUS)
 	{
    	state = "IDLE STATUS";
  	} else {
    	state = "OTHER STATE";
	}

	display.clearDisplay();
	display.setTextColor(WHITE);
	display.setCursor(0,0);
	display.print("Status: ");
	display.println(state);
	display.print("RSSI  : ");
	display.print(rssi);
 	display.println(" dBm");
	display.print("SSID  : ");
	display.println(WiFi.SSID());
	display.print("IP: ");
	display.println(WiFi.localIP());
	drawGraph();
	display.display();

	//int x = map(rssi, -85, -30, 0, 8);

	int strength = map(rssi, -85, -30, 0, 255);

	integration[intZaehler++] = strength;
	if (intZaehler >= numValues)
	{
    	intZaehler = 0;
  	}

  	uint16_t intSumme = 0;
  	for (int f=0; f<numValues; f++)
  	{
    	intSumme+= integration[f];
  	}
  	uint8_t outputValue = intSumme / numValues;

	if (DEBUG == 1)
	{
		Serial.print("RSSI: ");
		Serial.print(rssi);
		Serial.print(", Value: ");
		Serial.println(outputValue);
	}

	if (lastHistory + 250 < millis())
	{
		lastHistory = millis();
		history[histIdx++] = rssi;
		if(histIdx >= 128) histIdx = 0;
	}

	for(byte i = 0; i < NUM_LEDS; i++)
	{
	    //leds[i] = i % 8 < x ? color : black;
    	leds[i] = CHSV(outputValue, 200, 225);
  	}
	FastLED.show();

  	delay(50);
}

void drawGraph() {
  for(int i = 0; i < 128; i++) {
    int rssi = history[(histIdx + i) % 128];
    int dispY = constrain(map(rssi, -120, -40, 31, 0), 0, 31);
    display.drawPixel(i, 32 + dispY, WHITE);
  }
}
