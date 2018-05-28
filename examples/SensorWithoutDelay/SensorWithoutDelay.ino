#include "IOTMonitor.h"
#if defined(ARDUINO_AVR_YUN)
#include "YunClient.h"
YunClient client;
#else
#if defined(USE_WIFI101_SHIELD) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_ARCH_ESP8266)
// Use WiFi
#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#else
#include <SPI.h>
#include <WiFi101.h>
#endif
char ssid[] = "<YOURNETWORK>";    //  your network SSID (name)
char pass[] = "<YOURPASSWORD>";   // your network password
int status = WL_IDLE_STATUS;
WiFiClient  client;
#elif defined(USE_ETHERNET_SHIELD)
// Use wired ethernet shield
#include <SPI.h>
#include <Ethernet.h>
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
EthernetClient client;
#endif
#endif

#ifdef ARDUINO_ARCH_AVR
// On Arduino:  0 - 1023 maps to 0 - 5 volts
#define VOLTAGE_MAX 5.0
#define VOLTAGE_MAXCOUNTS 1023.0
#elif ARDUINO_SAMD_MKR1000
// On MKR1000:  0 - 1023 maps to 0 - 3.3 volts
#define VOLTAGE_MAX 3.3
#define VOLTAGE_MAXCOUNTS 1023.0
#elif ARDUINO_SAM_DUE
// On Due:  0 - 1023 maps to 0 - 3.3 volts
#define VOLTAGE_MAX 3.3
#define VOLTAGE_MAXCOUNTS 1023.0
#elif ARDUINO_ARCH_ESP8266
// On ESP8266:  0 - 1023 maps to 0 - 1 volts
#define VOLTAGE_MAX 1.0
#define VOLTAGE_MAXCOUNTS 1023.0
#endif
int ChannelNumber = My Channel ID;
String WriteApiKey = "My API Key";
const long interval = 60000;
unsigned long previousMillis = 0;
void setup() {
#ifdef ARDUINO_AVR_YUN
  Bridge.begin();
#else
#if defined(ARDUINO_ARCH_ESP8266) || defined(USE_WIFI101_SHIELD) || defined(ARDUINO_SAMD_MKR1000)
  WiFi.begin(ssid, pass);
#else
  Ethernet.begin(mac);
#endif
#endif

  IOTMonitor.begin(client);

}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    int sensorValue = analogRead(A0);
    // Convert the analog reading
    // On Uno,Mega,YunArduino:  0 - 1023 maps to 0 - 5 volts
    // On ESP8266:  0 - 1023 maps to 0 - 1 volts
    // On MKR1000,Due: 0 - 4095 maps to 0 - 3.3 volts
    float voltage = sensorValue * (VOLTAGE_MAX / VOLTAGE_MAXCOUNTS);
    int FieldID = 1;
    IOTMonitor.SendData(ChannelNumber, FieldID, voltage, WriteApiKey);
  }
}