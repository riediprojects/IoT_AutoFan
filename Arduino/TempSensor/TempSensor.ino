#include <DHTesp.h>
#include <cmath>
#include <ESP8266WiFi.h>
#include <TM1637Display.h>


// Temp sensor
DHTesp dht;
const int interval = 15000; // Thingspeak free license can only push every 15 seconds

// WiFi
const char *ssid = "MY_SSID"; // TODO add your ssid here
const char *password = "MY_PASSWORD"; // TODO add your password here

// Thingspeak
const char* host = "api.thingspeak.com";
const char* key = "BIMSSP1NENLK9HS9" ; // write api key for thingsspeak

// Display
#define CLK 5
#define DIO 4
TM1637Display display(CLK, DIO);
const uint8_t SEG_WIFI[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,  // W
  SEG_E | SEG_F,                          // I
  SEG_A | SEG_E | SEG_F | SEG_G,          // F
  SEG_E | SEG_F,                          // I
};
const uint8_t SEG_ERTS[] = {
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,  // E
  SEG_E | SEG_G,                          // r
  SEG_D | SEG_E | SEG_F | SEG_G,          // t
  SEG_A | SEG_C | SEG_D | SEG_F | SEG_G   // S
};
const uint8_t SEG_ERTP[] = {
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,  // E
  SEG_E | SEG_G,                          // r
  SEG_D | SEG_E | SEG_F | SEG_G,          // t
  SEG_A | SEG_B | SEG_E | SEG_F | SEG_G   // P
};


void setup() {
  Serial.begin(115200);
  setupDisplay();
  setupWifi();
  setupSensor();
}

void loop() {
  delay(dht.getMinimumSamplingPeriod());

  float temperature = dht.getTemperature();
  
  if(!isnan(temperature)) {
    Serial.print("Temp: ");
    Serial.println(temperature, 1);
    sendTemp(temperature);
    displayTemp(temperature);
  } else {
    Serial.println(dht.getStatusString());
    display.setSegments(SEG_ERTP);
  }
  
  delay(interval);
}

void setupSensor() {
  dht.setup(2, DHTesp::DHT11);
  Serial.println();
  Serial.println("Start measuring temperature");
}

void setupDisplay() {
  display.setBrightness(7);
  uint8_t data[] = {0xff, 0xff, 0xff, 0xff};
  display.setSegments(data);
}

void setupWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting ...");
  display.setSegments(SEG_WIFI);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100); // keeps watchdog happy
  }
  Serial.println("Connection successfull");
}

void sendTemp(float temperature) {
  BearSSL::WiFiClientSecure client;
  client.setInsecure();
  if (client.connect(host, 443)) {
    client.print("GET /update?api_key=");
    client.print(key);
    client.print("&field1=");
    client.print(temperature);
    client.print(" HTTP/1.1\r\n");
    client.print("Host: ");
    client.print(host);
    client.print("\r\n");
    client.print("Connection: close\r\n\r\n");
  } else {
    Serial.println("Could not connect to thingspeak!");
    display.setSegments(SEG_ERTS);
  }
}

void displayTemp(float temperature) {
  int data = 100 * temperature; // display 2 deciaml places
  display.showNumberDecEx(data, 64, true);
}
