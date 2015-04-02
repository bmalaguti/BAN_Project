/*********************************************************************
This is an example for our nRF8001 Bluetooth Low Energy Breakout

Written by Kevin Townsend/KTOWN  for Adafruit Industries.
MIT license, check LICENSE for more information
All text above, and the splash screen below must be included in any redistribution
*********************************************************************/

// This version uses the internal data queing so you can treat it like Serial (kinda)!

#include <SPI.h>
#include "Adafruit_BLE_UART.h"

// Connect CLK/MISO/MOSI to hardware SPI : CLK = 13, MISO = 12, MOSI = 11

#define ADAFRUITBLE_REQ 10
#define ADAFRUITBLE_RDY 2     // This should be an interrupt pin, on Uno thats #2 or #3
#define ADAFRUITBLE_RST 9

Adafruit_BLE_UART BTLEserial = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);

const int xInput = A0;
const int yInput = A1;
const int zInput = A2;

int xRaw = 0;
int yRaw = 0;
int zRaw = 0;

int sampleSize = 10;
/**************************************************************************/
/*!
    Configure the Arduino and start advertising with the radio
*/
/**************************************************************************/
void setup(void)
{ 
  Serial.begin(9600);
  while(!Serial); // Leonardo/Micro should wait for serial init
  Serial.println(F("Adafruit Bluefruit Low Energy nRF8001 Print echo demo"));

  BTLEserial.setDeviceName("_BAN"); /* 7 characters max! */

  BTLEserial.begin();
}

/**************************************************************************/
/*!
    Constantly checks for new events on the nRF8001
*/
/**************************************************************************/
aci_evt_opcode_t laststatus = ACI_EVT_DISCONNECTED;

void loop()
{
  // Tell the nRF8001 to do whatever it should be working on.
  BTLEserial.pollACI();

  // Ask what is our current status
  aci_evt_opcode_t status = BTLEserial.getState();
  // If the status changed....
  if (status != laststatus) {
    // print it out!
    if (status == ACI_EVT_DEVICE_STARTED) {
        Serial.println(F("* Advertising started"));
    }
    if (status == ACI_EVT_CONNECTED) {
        Serial.println(F("* Connected!"));
    }
    if (status == ACI_EVT_DISCONNECTED) {
        Serial.println(F("* Disconnected or advertising timed out"));
    }
    // OK set the last status change to this one
    laststatus = status;
  }

  if (status == ACI_EVT_CONNECTED) {
    // Lets see if there's any data for us!
    if (BTLEserial.available()) {
      Serial.print("* "); Serial.print(BTLEserial.available()); Serial.println(F(" bytes available from BTLE"));
    }
    // OK while we still have something to read, get a character and print it out
    String s_read = "";
    while (BTLEserial.available()) {
      char c = BTLEserial.read();
      s_read.concat(c);
      Serial.print(c);
    }
    if(s_read != "")
    {
      //Serial.print(s_read);
    }
    
    if(s_read == "accel")
    {
      uint8_t sendbuffer[20];
      String s_send;
      
      Serial.println(); 
      Serial.print("Accel Command Received\r\n");
      xRaw = ReadAxis(xInput);
      yRaw = ReadAxis(xInput);
      zRaw = ReadAxis(xInput);
      Serial.print(xRaw);
      s_send += String(xRaw) + ' ';
      Serial.print("-");
      Serial.print(yRaw);
      s_send += String(yRaw) + ' ';
      Serial.print("-");
      Serial.print(zRaw);
      s_send += String(zRaw) + '\n';
      Serial.println();
     
      Serial.print(s_send); 
      s_send.getBytes(sendbuffer, 20);
      char sendbuffersize = min(20, s_send.length());
      BTLEserial.write(sendbuffer, sendbuffersize);

    }

    // Next up, see if we have any data to get from the Serial console

    if (Serial.available()) {
      // Read a line from Serial
      Serial.setTimeout(100); // 100 millisecond timeout
      String s = Serial.readString();

      // We need to convert the line to bytes, no more than 20 at this time
      uint8_t sendbuffer[20];
      s.getBytes(sendbuffer, 20);
      char sendbuffersize = min(20, s.length());

      Serial.print(F("\n* Sending -> \"")); Serial.print((char *)sendbuffer); Serial.println("\"");

      // write the data
      BTLEserial.write(sendbuffer, sendbuffersize);
      
      if(s == "accel")
      {
        xRaw = ReadAxis(xInput);
        yRaw = ReadAxis(xInput);
        zRaw = ReadAxis(xInput);
        Serial.print(xRaw);
        Serial.print("-");
        Serial.print(yRaw);
        Serial.print("-");
        Serial.print(zRaw);
        Serial.println();
      }
    }
  }
}

//
// Read "sampleSize" samples and report the average
//
int ReadAxis(int axisPin)
{
  long reading = 0;
  analogRead(axisPin);
  delay(1);
  for (int i = 0; i < sampleSize; i++)
  {
    reading += analogRead(axisPin);
  }
  return reading/sampleSize;
}
