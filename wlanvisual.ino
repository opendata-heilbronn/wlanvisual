/**
   WLAN RSSI Visualisation Firmware | wlanvisual.ino

   @package     ch.mikroelek.arduino.wlanvisual
   @author      Tim Oldenbuerger <t.o@mikroelek.ch>
   @author		Franz Imschweiler
   @author		Leon Hellmich
   @author		Leandro Späth
   @author		Patrick Krämer
   @version     v 1.2.0 (2018-03-17)
   @copyright   Copyright (c) 2018
*/

#include <Adafruit_GFX.h>
#include "src/Adafruit_SSD1306/Adafruit_SSD1306.h"
#include "src/MenuLib/MenuLib.h"
#include <RotaryEncoder.h>
#include <ESP8266WiFi.h>
#include <FastLED.h>
#include <SPI.h>
#include <WiFiClient.h>
#include <Wire.h>

//BEGIN Config
#define DEBUG 1
#define NUM_LEDS 32
#define BRIGHTNESS 25

const char* ssid = "***";
const char* password = "***";

#define OLED_RESET LED_BUILTIN  //4

const uint16_t numIntegrationValues = 20;
//END Config

int displayMode = 0;

void gotoMainmenue();
void gotoSettings();
void emptyFunction();
void gotoStatus();

RotaryEncoder rotary(D4, D6); // Button at D3

MenuItem items[] = {
  MenuItem(&gotoStatus, "Status"),
  MenuItem(&gotoSettings, "Settings")
};
Menu defaultMenu = Menu(0, items, sizeof(items) / sizeof(items[0]));

MenuItem items2[] = {
  MenuItem(&gotoMainmenue, "Back"),
  MenuItem(&emptyFunction, "Do nothing")
};
Menu settingsMenue = Menu(0, items2, sizeof(items2) / sizeof(items2[0]));

MenuSystem menuSystem = MenuSystem(&defaultMenu, 6);

void gotoMainmenue() {
  menuSystem.goToMenu(&defaultMenu);
}
void gotoSettings() {
  menuSystem.goToMenu(&settingsMenue);
}

void gotoStatus() {
  displayMode = 1;
}

void emptyFunction() {}

Adafruit_SSD1306 display(OLED_RESET);

long rssi;
String state = "UNKNOWN";

CRGB leds[NUM_LEDS];
CHSV color;
CHSV black;

uint8_t integration[numIntegrationValues] = {0};
uint8_t intZaehler = 0;

unsigned long lastHistory = 0;
int history[128];
byte histIdx = 0;

void updateRotary() {
  rotary.tick();
}

void setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Franz&Leon&Tim&");
  display.println("Leandro&Patrick");
  display.println("");
  display.println("WLAN to LED Matrix");
  display.display();

  pinMode(D0, OUTPUT);	// Internal LED

  FastLED.addLeds<WS2812B, 5, GRB>(leds, NUM_LEDS);
  for (int x = 0; x < 32; x++)
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

  attachInterrupt(digitalPinToInterrupt(D4), &updateRotary, CHANGE);
  attachInterrupt(digitalPinToInterrupt(D6), &updateRotary, CHANGE);
}

void loop() {
  Serial.println(displayMode);
  if (displayMode == 0) {
    if (buttonKeyDown()) {
      menuSystem.forward();
    }
    int rotaryChange = getRotaryChange();
    if (rotaryChange != 0) {
      menuSystem.updown(rotaryChange);
    }
    drawMenu();
  } else if (displayMode == 1) {
    drawStatusPage();
    if (buttonKeyDown())
      displayMode = 0;
  }

  updateRssi();
  updateHistory();
  updateWifiState();
  updateLeds();

  delay(50);
}

void updateRssi() {
  rssi = WiFi.RSSI();

  if (DEBUG == 1)
  {
    Serial.print("RSSI: ");
    Serial.println(rssi);
  }
}

void updateWifiState() {
  int statusNum = WiFi.status();
  if (statusNum == WL_CONNECTED)
  {
    state = "CONNECTED";
  }
  else if (statusNum == WL_DISCONNECTED)
  {
    state = "DISCONNECTED";
  }
  else if (statusNum == WL_CONNECTION_LOST)
  {
    state = "CONNECTION LOST";
  }
  else if (statusNum == WL_NO_SSID_AVAIL)
  {
    state = "NO SSID AVAILABLE";
  }
  else if (statusNum == WL_IDLE_STATUS)
  {
    state = "IDLE STATUS";
  }
  else
  {
    state = "OTHER STATE";
  }
}

void updateHistory() {
  if (lastHistory + 250 < millis())
  {
    lastHistory = millis();
    history[histIdx++] = rssi;
    if (histIdx >= 128) histIdx = 0;
  }
}

void updateLeds() {
  int strength = map(rssi, -85, -30, 0, 255);

  integration[intZaehler++] = strength;
  if (intZaehler >= numIntegrationValues)
  {
    intZaehler = 0;
  }

  uint16_t intSumme = 0;
  for (int f = 0; f < numIntegrationValues; f++)
  {
    intSumme += integration[f];
  }
  uint8_t outputValue = intSumme / numIntegrationValues;

  for (byte i = 0; i < NUM_LEDS; i++)
  {
    //leds[i] = i % 8 < x ? color : black;
    leds[i] = CHSV(outputValue, 200, 225);
  }
  FastLED.show();
}

bool buttonKeyDown() {
  static bool lastButtonState = 0;
  bool buttonState = digitalRead(D3);
  bool toReturn = 0;
  if (lastButtonState && !buttonState) {
    toReturn = 1;
  }
  lastButtonState = buttonState;
  return toReturn;
}

int getRotaryChange() {
  static int pos = 0;
  int newPos = rotary.getPosition();
  int toReturn = 0;
  if (pos != newPos) {
    toReturn = newPos - pos;
    pos = newPos;
  }
  return toReturn;
}

void drawMenu() {
  display.clearDisplay();

  int displaySize = menuSystem.getDisplaySize();
  struct MenuOutputItem itm[displaySize];
  menuSystem.outputMenu(itm);
  for (int i = 0; i < displaySize; i++)
  {
    display.setCursor(0, i * 10);
    display.setTextColor(itm[i].selected ? BLACK : WHITE, itm[i].selected ? WHITE : BLACK);
    display.print(itm[i].name);
  }

  display.display();
}

void drawStatusPage() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
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
}

void drawGraph() {
  for (int i = 0; i < 128; i++) {
    int rssi = history[(histIdx + i) % 128];
    int dispY = constrain(map(rssi, -120, -40, 31, 0), 0, 31);
    display.drawPixel(i, 32 + dispY, WHITE);
  }
}
