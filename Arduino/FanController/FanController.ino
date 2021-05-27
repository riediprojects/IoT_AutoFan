/**************************************************
   (c) Manuel Riedi, Patrick Walker
   
   MCU Hardware:
      Feather nRF52840 Express with Grove Shield
   Grove connections:
      A2: Rotary Angle Sensor
      A0: Fan Socket
      D4: 4-Digits-Display TM1637
      D2: Button
   Libraries:
      Grove 4-Digit-Display
***************************************************/

#include <bluefruit.h>
#include <Adafruit_LittleFS.h>
using namespace Adafruit_LittleFS_Namespace;

#include "TM1637.h"
#define CLK 9
#define DIO 10
TM1637 tm1637(CLK, DIO);

BLEService single = BLEService(0x400F);
BLECharacteristic data = BLECharacteristic(0x4B00);

int button = 5;
int on_state = 0;
int switch_state = 0;

unsigned long t0;

uint16_t threshold_temperature = 0;
uint16_t old_threshold_temperature = 0;
int oldFloat;
uint16_t outside_temperatur = 0;
uint16_t old_outside_temperatur = 0;

int fan_state = 0;
int old_fan_state = 0;

int lastval = 0;

int8_t offDisp[] = {0x7d, 0, 15, 15}; //displays "off"

void setup() {
  tm1637.init();
  tm1637.set(BRIGHT_TYPICAL);
  tm1637.point(POINT_ON);

  Bluefruit.begin();
  Bluefruit.setName("Fan_Controller");
  Bluefruit.Periph.setConnectCallback(connectCallback);
  Bluefruit.Periph.setDisconnectCallback(disconnectCallback);

  // Adverstising
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addService(single);
  Bluefruit.ScanResponse.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);
  Bluefruit.Advertising.setFastTimeout(30);

  // Service
  single.begin();
  data.setProperties(CHR_PROPS_READ | CHR_PROPS_WRITE);
  data.setPermission(SECMODE_OPEN, SECMODE_OPEN);
  data.setUserDescriptor("Outside Temperatur");
  data.setFixedLen(2);
  data.begin();
  data.write16(outside_temperatur);

  //Pins
  pinMode(button, INPUT);
  pinMode(A0, OUTPUT);
  digitalWrite(A0, LOW);
  Serial.begin(115200);
}

void loop() {
  int button_state = digitalRead(button);

  if (on_state == 0 && button_state == HIGH) {
    on_state = 1;

  } else if (on_state == 1 && button_state == LOW) {
    Serial.println("Fan Controller On");
    tm1637.point(POINT_ON);
    convertToDigitsNumber(old_threshold_temperature, 0xd7);
    printFanState();
    digitalWrite(A0, fan_state);
    Bluefruit.Advertising.start(0);
    on_state = 2;

  } else if (on_state == 2) { //Turn device on
    operatingMode();
    if (switch_state == 0 && button_state == LOW) {

    } else if (switch_state == 0 && button_state == HIGH) {
      switch_state = 2;
      t0 = millis();

    } else if ( (switch_state == 2 || switch_state == 4)  && button_state == HIGH && (millis() - t0) >= 1000) {
      switch_state = 5;
      turnDeviceOff();

    } else if (switch_state == 2 && button_state == LOW) {
      outdorTempMode();
      switch_state = 3;

    } else if (switch_state == 3 && button_state == HIGH) {
      switch_state = 4;
      t0 = millis();

    } else if (switch_state == 4 && button_state == LOW) {
      Serial.println("Threshold Temperatur Mode");
      convertToDigitsNumber(old_threshold_temperature, 0xd7);
      switch_state = 0;

    } else if (switch_state == 5 && button_state == LOW) { // Turn device of
      switch_state = 0;
      on_state = 0;
    }
  }
  delay(1);
}

void outdorTempMode() {
  Serial.println("Outdor_Temperatur Mode");
  convertToDigitsNumber(outside_temperatur, 0x8c);
}

void convertToDigitsNumber(uint16_t intNumber, uint8_t mod) {

  //Calc int
  uint16_t integer = ( (intNumber - (intNumber % 100) ) / 100) ;
  uint16_t tensInt = ((integer - (integer % 10)) / 10);
  uint16_t onesInt = integer % 10;

  //Calc float
  uint16_t tensFloat = ((intNumber % 100) - (intNumber % 10)) / 10;

  int8_t tempDisp[] = {tensInt, onesInt, tensFloat , mod};
  tm1637.display(tempDisp);
}

void turnDeviceOff() {
  Serial.println("Fan Controller Off");
  tm1637.point(POINT_OFF);
  digitalWrite(A0, LOW);
  tm1637.display(offDisp);
  Bluefruit.Advertising.stop();
  delay(1500);
  tm1637.clearDisplay();
}

void operatingMode() {
  setThreshold_Temperatur();
  setOutside_Temperatur();
}

void setThreshold_Temperatur() {
  int analogTemp = analogRead(A2);

  if((analogRead(A2)> (lastval+6)) || (analogRead(A2)<(lastval-6))){
  lastval =analogRead(A2);
  }
  else{
  analogTemp = lastval;
  }
 
  threshold_temperature = map(analogTemp, 0, 1024, 100, 500);
  int floatNumber = (threshold_temperature % 10); //get float number of threshold_temperature

  //Converts threshold_temperature values to fifth step values
  if ( !( (floatNumber >= (oldFloat - 2)) && (floatNumber <= (oldFloat + 2)) ) ) { //Prevents flicker on 4-Digits-Display due to analog values jumping back and forth
    if (floatNumber > 5) {
      threshold_temperature = ( (threshold_temperature - floatNumber) + 5);
    } else {
      threshold_temperature = (threshold_temperature - floatNumber);
    }
    threshold_temperature = (threshold_temperature * 10);
    old_threshold_temperature = threshold_temperature;
    convertToDigitsNumber(threshold_temperature, 0xd7);
    oldFloat = floatNumber;
    
    //Jumb back to "Threshold Temperatur Mode" when Rotary Angle Sensor is applied
    if(switch_state == 3) {
      switch_state = 4;
    }
  }
}

void setOutside_Temperatur() {
  outside_temperatur = data.read16();

  if (outside_temperatur != old_outside_temperatur) {
    Serial.print("Outside Temperatur has changed: ");
    Serial.print(old_outside_temperatur);
    Serial.print(" => ");
    Serial.print(outside_temperatur);
    Serial.println("°");

    convertToDigitsNumber(outside_temperatur, 0x8c);
    old_outside_temperatur = outside_temperatur;
  }

  fan_state = (outside_temperatur >= (old_threshold_temperature)) ? 1 : 0;
  if (fan_state != old_fan_state) {
    digitalWrite(A0, fan_state);
    old_fan_state = fan_state;
    printFanState();
  }
}

void printFanState() {
  Serial.print("Fan is: ");
  Serial.println(fan_state ? "On" : "Off");
}

void connectCallback(uint16_t handle) {
  BLEConnection* connection = Bluefruit.Connection(handle);

  // handle checks for running sessions and other reconnection details here
  char central[32] = { 0 };
  connection->getPeerName(central, sizeof(central));
}

void disconnectCallback(uint16_t handle, uint8_t reason) {
  Serial.println("Disconnected, advertising...");
}
