#ifndef IOTMonitor_h
#define IOTMonitor_h

//#define PRINT_DEBUG_MESSAGES
//#define PRINT_HTTP


#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_SAM)
#include "Arduino.h"
#include <Client.h>
#else
#error Only Arduino MKR1000, Yun, Uno/Mega/Due with either WiFi101 or Ethernet shield. ESP8266 also supported.
#endif

#define IOTMonitor_URL "IOTMonitor.ir"
#define PORT_NUMBER 80

#define OK_SUCCESS              200     // OK / Success
#define ERR_BADAPIKEY           400     // Incorrect API key (or invalid ThingSpeak server address)
#define ERR_BADURL              404     // Incorrect API key (or invalid ThingSpeak server address)
#define ERR_OUT_OF_RANGE        -101    // Value is out of range or string is too long (> 255 bytes)
#define ERR_INVALID_FIELD_NUM   -201    // Invalid field number specified
#define ERR_SETFIELD_NOT_CALLED -210    // setField() was not called before writeFields()
#define ERR_CONNECT_FAILED      -301    // Failed to connect to ThingSpeak
#define ERR_UNEXPECTED_FAIL     -302    // Unexpected failure during write to ThingSpeak
#define ERR_BAD_RESPONSE        -303    // Unable to parse response
#define ERR_TIMEOUT             -304    // Timeout waiting for server to respond
#define ERR_NOT_INSERTED        -401    // Point was not inserted (most probable cause is the rate limit of once every 15 seconds)


class IOTMonitorClass
{
  public:

    IOTMonitorClass()
    {

    };
    bool begin(Client & client)
    {
      this->setClient(&client);
      return true;
    };

    int writeField(unsigned long channelNumber, unsigned int field, int value, const char * writeAPIKey)
    {
      char valueString[10];  // int range is -32768 to 32768, so 7 bytes including terminator, plus a little extra
      itoa(value, valueString, 10);
      return writeField(channelNumber, field, valueString, writeAPIKey);
    };
    int writeField(unsigned long channelNumber, unsigned int field, long value, const char * writeAPIKey)
    {
      char valueString[15];  // long range is -2147483648 to 2147483647, so 12 bytes including terminator
      ltoa(value, valueString, 10);
      return writeField(channelNumber, field, valueString, writeAPIKey);
    };
    int writeField(unsigned long channelNumber, unsigned int field, float value, const char * writeAPIKey)
    {
#ifdef PRINT_DEBUG_MESSAGES
      Serial.print("ts::writeField (channelNumber: "); Serial.print(channelNumber); Serial.print(" writeAPIKey: "); Serial.print(writeAPIKey); Serial.print(" field: "); Serial.print(field); Serial.print(" value: "); Serial.print(value, 5); Serial.println(")");
#endif
      char valueString[20]; // range is -999999000000.00000 to 999999000000.00000, so 19 + 1 for the terminator
      int status = convertFloatToChar(value, valueString);
      if (status != OK_SUCCESS) return status;

      return writeField(channelNumber, field, valueString, writeAPIKey);
    };
    int writeField(unsigned long channelNumber, unsigned int field, const char * value, const char * writeAPIKey)
    {
      return writeField(channelNumber, field, String(value), writeAPIKey);
    };
    int writeField(unsigned long channelNumber, unsigned int field, String value, const char * writeAPIKey)
    {
      return  SendData(channelNumber, field, value, String(writeAPIKey));

    };
    int convertFloatToChar(float value, char *valueString)
    {
      // Supported range is -999999000000 to 999999000000
      if (0 == isinf(value) && (value > 999999000000 || value < -999999000000))
      {
        // Out of range
        return ERR_OUT_OF_RANGE;
      }
      // assume that 5 places right of decimal should be sufficient for most applications

#if defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_SAM)
      sprintf(valueString, "%.5f", value);
#else
      dtostrf(value, 1, 5, valueString);
#endif
      return OK_SUCCESS;
    };

    bool connectIOTMonitor()
    {
      bool connectSuccess = false;

#ifdef PRINT_DEBUG_MESSAGES
      Serial.print("               Connect to default IOTMonitor URL...");
#endif
      connectSuccess = client->connect(IOTMonitor_URL, PORT_NUMBER);


#ifdef PRINT_DEBUG_MESSAGES
      if (connectSuccess)
      {
        Serial.println("Success.");
      }
      else
      {
        Serial.println("Failed.");
      }
#endif
      return connectSuccess;
    };
    void SendData(unsigned long channelNumber, unsigned int field, float value, const char * writeAPIKey)
    {
      SendData(channelNumber, field, String( value), String(writeAPIKey));
    }
    void SendData(unsigned long channelNumber, unsigned int field, String value, const char * writeAPIKey)
    {
      SendData(channelNumber, field, value, String(writeAPIKey));
    }
    int SendData(unsigned long channelNumber, unsigned int field, String value, String writeAPIKey)
    {


      // exit if unable to connect
      if (!client->connect(IOTMonitor_URL, PORT_NUMBER)) {
        return 0;
      }
      String Data = "/api/Client/?channelNumber=";
      Data += String(channelNumber);
      Data += "&FieldId=";
      Data += String(field);
      Data += "&data=";
      Data += value;
      Data += "&WriteApiKey=";
      Data += String(writeAPIKey);
      //Serial.println(Data);
      this->client->print(String("GET ") + Data + " HTTP/1.1\r\n" +
                          "Host: " + IOTMonitor_URL + "\r\n" +
                          "Connection: close\r\n\r\n");
      return 1;
    }


    void setClient(Client * client)
    {
      this->client = client;
    }
    Client * client = NULL;
    const char * customHostName = NULL;
    IPAddress customIP = INADDR_NONE;
    unsigned int port = PORT_NUMBER;
    int lastStatus;
};
extern IOTMonitorClass IOTMonitor;

#endif