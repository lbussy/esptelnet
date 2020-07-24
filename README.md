:exclamation: This is a (disconnected) fork of [yasheena](https://github.com/yasheena) / [telnetspy](https://github.com/yasheena/telnetspy). That project seemed as if the author abandoned it.  And to be honest, ain't nobody got time for that!

# Telnet server for ESP8266 / ESP32

Access the controller's serial port via Telnet.

---

[![GitHub Issues](https://img.shields.io/github/issues/lbussy/esptelnet.svg)](https://github.com/lbussy/esptelnet/issues)
[![GitHub Pull Requests](https://img.shields.io/github/issues-pr/lbussy/esptelnet.svg)](https://github.com/lbussy/esptelnet/pulls)

## Table of Contents

- [Description](#description)
- [Usage](#usage)
  - [Basic](#basic)
  - [Functions](#functions)
- [License](#license)

## Description <a name = "description"></a>

- This library allows you to interact with the controller (in most cases) as if you connected directly to the serial port. Use ```ESPTelnet``` instead of ```Serial``` to send data to the serial port and a Telnet connection simultaneously. 
- There is a circular buffer that stores data when a Telnet connection is not available. In that way, it is possible to accumulate data even while the controller sets up network connections.
- You may choose to create a Telnet session only if it is necessary.
- The controller will process data to the ESP8266 / ESP32 via Telnet as data received from the serial port.
- You may use more than one instance of ESPTelnet. For example: To send control information on the first instance and data dumps on the second instance.
 	 	
## Usage <a name = "usage"></a>

### Basic <a name = "basic"></a>

Add the following line to your sketch:
```
#include <ESPTelnet.h>
ESPTelnet MYSERIAL;
```

Add the following line to your initialization block ```void setup()```:
```
MYSERIAL.begin();
```

Add the following line within your main loop ```void loop()```:
```
MYSERIAL.handle();
```

Also, see the [sample code](https://github.com/lbussy/esptelnet/blob/master/examples/basic/basic.ino) for a fully-functioning example.

### Use the following functions of the ESPTelnet object to modify behavior <a name = "functions"></a>

#. [void begin()](#begin)
#. [void setPort()](#setPort)
#. [void setWelcomeMsg()](#setWelcomeMsg)
#. [void setRejectMsg()](#setRejectMsg)
#. [void setMinBlockSize()](#setMinBlockSize)
#. [void setCollectingTime()](#setCollectingTime)
#. [void setMaxBlockSize()](#setMaxBlockSize)
#. [bool setBufferSize()](#setBufferSize)
#. [uint16_t getBufferSize()](#getBufferSize)
#. [void setStoreOffline()](#setStoreOffline)
#. [bool getStoreOffline()](#getStoreOffline)
#. [void setPingTime()](#setPingTime)
#. [void setSerial()](#setSerial)
#. [bool isClientConnected()](#isClientConnected)
#. [void setCallbackOnConnect()](#setCallbackOnConnect)
#. [void setCallbackOnDisconnect()](#setCallbackOnDisconnect)
#. [void disconnectClient()](#disconnectClient)
#. [void handle()](#handle)
#. [void end()](#end)

---

### void begin(unsigned long baud) <a name = "begin"></a>

The ```begin()``` method has ESP32 and ESP8266 variants.:

#### ESP8266

void begin(unsigned long baud)
void begin(unsigned long baud, SerialConfig config)
void begin(unsigned long baud, SerialConfig config, SerialMode mode)
void begin(unsigned long baud, SerialConfig config, SerialMode mode, uint8_t tx_pin);

#### ESP32

void begin(unsigned long baud, uint32_t config = SERIAL_8N1, int8_t rxPin = -1, int8_t txPin = -1, bool invert = false);

### void setPort(uint16_t portToUse) <a name = "setPort"></a>

Set or change the port number of this Telnet server. If a client is already connected, it will be disconnected.

Default: 64 

### void setWelcomeMsg(const char* msg) <a name = "setWelcomeMsg"></a>

Change the message which displays to the Telnet client after establishing a session.

Default: "Connection established via ESPTelnet."`

### void setRejectMsg(char* msg) <a name = "setRejectMsg"></a>

Change the error message that will be sent to the Telnet client if attempting a second connection to the same server.

Default: "ESPTelnet: Only one connection at a time is possible."

### void setMinBlockSize(uint16_t minSize) <a name = "setMinBlockSize"></a>

Change the number of characters to collect before sending a Telnet block.

Default: 64 

### void setCollectingTime(uint16_t colTime) <a name = "setCollectingTime"></a>

Change the time (in ms) to wait before sending a Telnet block if it's size is less than <minSize> (defined by ```setMinBlockSize```).

Default: 100

### void setMaxBlockSize(uint16_t maxSize) <a name = "setMaxBlockSize"></a>

Change the maximum size of the Telnet packets to send.

Default: 512

### bool setBufferSize(uint16_t newSize) <a name = "setBufferSize"></a>

Change the size of the circular buffer. Set to ```0``` to disable buffering. Changing size tries to preserve the already collected data. If the new buffer size is smaller than the currently buffered data, it will truncate the buffer. Returns ```false``` if the method cannot set the requested buffer size.

Default: 3000

### uint16_t getBufferSize() <a name = "getBufferSize"></a>

This function returns the actual size of the ring buffer.

### void setStoreOffline(bool store) <a name = "setStoreOffline"></a>

Enable/disable storing new data in the ring buffer if there is no Telnet connection established. This function allows you to store essential data only. You can do this by disabling ```storeOffline``` while sending unimportant data.

Default: true

### bool getStoreOffline() <a name = "getStoreOffline"></a>

Get the state of storing data when offline.

### void setPingTime(uint16_t pngTime) <a name = "setPingTime"></a>

Use ```setPingTime``` to define the time (in ms) without traffic, after which the controller sends a ping (```chr(0)```) to the Telnet client. Use ```0``` to disable pings.

Default: 1500  

### void setSerial(HardwareSerial* usedSerial) <a name = "setSerial"></a>

Set the serial port you want to use with this object, or ```NULL``` if no serial port should be used (Telnet only).

Default: Serial

### bool isClientConnected() <a name = "isClientConnected"></a>

This function returns true if a Telnet client is connected.

### void setCallbackOnConnect(void (*callback)()) <a name = "setCallbackOnConnect"></a>

This function installs a callback function, which executes upon every connection (except rejected connect tries). Use ```NULL``` to remove the callback.

Default: NULL

### void setCallbackOnDisconnect(void (*callback)()) <a name = "setCallbackOnDisconnect"></a>

This function installs a callback function, which executes upon every Telnet disconnect (except rejected connect tries). Use ```NULL``` to remove the callback.

Default: NULL

### void disconnectClient() <a name = "disconnectClient"></a>

This method forces a clean client disconnection.

### void handle() <a name = "handle"></a>

Call this method at the beginning of the ```loop()``` to process data in and out of the Telnet connection.

### void end() <a name = "end"></a>

Closes the object and deletes it from memory.
