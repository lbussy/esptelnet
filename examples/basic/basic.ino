/* Copyright (C) 2019-2020 Lee C. Bussy (@LBussy)

This file is part of Lee Bussy's Keg Cop (keg-cop).

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#endif
#ifdef ESP32
#include <WiFi.h>
#include <ESPmDNS.h>
#endif

#include "ESPTelnet.h"
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Arduino.h>

const char *ssid = "NETGEAR_11N";
const char *password = "idoxlr8!";

ESPTelnet SerialAndTelnet;

#undef SERIAL
//#define SERIAL Serial // Do not use Telnet
#define SERIAL SerialAndTelnet // Use Telnet

void waitForConnection()
{
    SERIAL.print(".");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        SERIAL.print(".");
    }
    SERIAL.println(" Connected.");
}

void waitForDisconnection()
{
    SERIAL.print(".");
    while (WiFi.status() == WL_CONNECTED)
    {
        delay(500);
        SERIAL.print(".");
    }
    SERIAL.println(" Disconnected.");
}

void telnetConnected()
{
    SERIAL.println("Telnet connection established.");
}

void telnetDisconnected()
{
    SERIAL.println("Telnet connection closed.");
}

void handleTelnet()
{
    if (SERIAL.available() > 0)
    {
        char c = SERIAL.read();
        switch (c)
        {
        case '\r':
            SERIAL.println();
            break;
        case 'C':
            SERIAL.print("\nConnecting ..");
            WiFi.begin(ssid, password);
            waitForConnection();
            break;
        case 'D':
            SERIAL.print("\nDisconnecting ..");
            WiFi.disconnect();
            waitForDisconnection();
            break;
        case 'R':
            SERIAL.print("\nReconnecting ..");
            WiFi.reconnect();
            waitForDisconnection();
            waitForConnection();
            break;
        default:
            SERIAL.print(c);
            break;
        }
    }
}

void setup()
{
    SERIAL.setWelcomeMsg("Welcome to the ESPTelnet example.\n\n");
    SERIAL.setCallbackOnConnect(telnetConnected);
    SERIAL.setCallbackOnDisconnect(telnetDisconnected);
    SERIAL.begin(BAUD); // Set this if not using the PlatformIO project
    while (!Serial)
    {
        ; // Wait for serial port to connect
    }
    SERIAL.setDebugOutput(false);
    SERIAL.flush();
    SERIAL.println("\n");
    SERIAL.print("Connecting to WiFi ");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    waitForConnection();

    // During updates "over the air" the telnet session will be closed.
    // So the operations of ArduinoOTA cannot be seen via telnet.
    // So we only use the standard "Serial" for logging.
    ArduinoOTA.onStart([]() {
        Serial.println("[OTA] Start.");
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("[OTA] End.");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("[OTA] Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("[OTA] Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
            Serial.println("[OTA] Auth Failed.");
        else if (error == OTA_BEGIN_ERROR)
            Serial.println("[OTA] Begin Failed.");
        else if (error == OTA_CONNECT_ERROR)
            Serial.println("[OTA] Connect Failed.");
        else if (error == OTA_RECEIVE_ERROR)
            Serial.println("[OTA] Receive Failed.");
        else if (error == OTA_END_ERROR)
            Serial.println("[OTA] End Failed.");
    });
    ArduinoOTA.begin();

    SERIAL.println("Ready");
    SERIAL.print("IP address: ");
    SERIAL.println(WiFi.localIP());

    SERIAL.println("\nType 'C' for WiFi connect.");
    SERIAL.println("Type 'D' for WiFi disconnect.");
    SERIAL.println("Type 'R' for WiFi reconnect.");
    SERIAL.println("All other chars will be echoed.\n");
}

void loop()
{
    SERIAL.handle();
    ArduinoOTA.handle();
    handleTelnet();
}
