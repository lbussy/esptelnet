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

#ifndef _ESPTELNET_H
#define _ESPTELNET_H

#define ESPTELNET_BUFFER_LEN 3000
#define ESPTELNET_MIN_BLOCK_SIZE 64
#define ESPTELNET_COLLECTING_TIME 100
#define ESPTELNET_MAX_BLOCK_SIZE 512
#define ESPTELNET_PING_TIME 1500
#define ESPTELNET_PORT 23
#define ESPTELNET_CAPTURE_OS_PRINT true
#define ESPTELNET_WELCOME_MSG "Connection established via ESPTelnet.\n"
#define ESPTELNET_REJECT_MSG "ESPTelnet: Only one connection at a time is possible.\n"

#ifdef ESP8266
#include <ESP8266WiFi.h>
// Empty defines, so on ESP8266 nothing will be changed
#define CRITCAL_SECTION_MUTEX
#define CRITCAL_SECTION_START
#define CRITCAL_SECTION_END
#endif
#ifdef ESP32
#include <WiFi.h>
// Add spinlock for ESP32
#define CRITCAL_SECTION_MUTEX portMUX_TYPE AtomicMutex = portMUX_INITIALIZER_UNLOCKED;
// Non-static Data Member Initializers, see:
// https://web.archive.org/web/20160316174223/https://blogs.oracle.com/pcarlini/entry/c_11_tidbits_non_static
#define CRITCAL_SECTION_START portENTER_CRITICAL(&AtomicMutex);
#define CRITCAL_SECTION_END portEXIT_CRITICAL(&AtomicMutex);
#endif

#include <WiFiClient.h>

class ESPTelnet : public Stream
{
public:
	ESPTelnet();
	~ESPTelnet();
	void handle(void);
	void setPort(uint16_t portToUse);
	void setWelcomeMsg(const char *msg);
	void setRejectMsg(const char *msg);
	void setMinBlockSize(uint16_t minSize);
	void setCollectingTime(uint16_t colTime);
	void setMaxBlockSize(uint16_t maxSize);
	bool setBufferSize(uint16_t newSize);
	uint16_t getBufferSize();
	void setStoreOffline(bool store);
	bool getStoreOffline();
	void setPingTime(uint16_t pngTime);
	void setSerial(HardwareSerial *usedSerial);
	bool isClientConnected();
	void setCallbackOnConnect(void (*callback)());
	void setCallbackOnDisconnect(void (*callback)());
	// Functions offered by HardwareSerial class:
#ifdef ESP8266
	void begin(unsigned long baud)
	{
		begin(baud, SERIAL_8N1, SERIAL_FULL, 1);
	}
	void begin(unsigned long baud, SerialConfig config) { begin(baud, config, SERIAL_FULL, 1); }
	void begin(unsigned long baud, SerialConfig config, SerialMode mode) { begin(baud, config, mode, 1); }
	void begin(unsigned long baud, SerialConfig config, SerialMode mode, uint8_t tx_pin);
#else // ESP32
	void begin(unsigned long baud, uint32_t config = SERIAL_8N1, int8_t rxPin = -1, int8_t txPin = -1, bool invert = false);
#endif
	void end();
#ifdef ESP8266
	void swap()
	{
		swap(1);
	}
	void swap(uint8_t tx_pin);
	void set_tx(uint8_t tx_pin);
	void pins(uint8_t tx, uint8_t rx);
	bool isTxEnabled(void);
	bool isRxEnabled(void);
#endif
	int available(void) override;
	int peek(void) override;
	int read(void) override;
	int availableForWrite(void);
	void flush(void) override;
	size_t write(uint8_t) override;
	inline size_t write(unsigned long n) { return write((uint8_t)n); }
	inline size_t write(long n) { return write((uint8_t)n); }
	inline size_t write(unsigned int n) { return write((uint8_t)n); }
	inline size_t write(int n) { return write((uint8_t)n); }
	using Print::write;
	operator bool() const;
	void setDebugOutput(bool);
	uint32_t baudRate(void);
	void baudUpdate(unsigned long);
	void disconnectClient();

protected:
	CRITCAL_SECTION_MUTEX
	void sendBlock(void);
	void addTelnetBuf(char c);
	char pullTelnetBuf();
	char peekTelnetBuf();
	int telnetAvailable();
	WiFiServer *telnetServer;
	WiFiClient client;
	uint16_t port;
	HardwareSerial *usedSer;
	bool storeOffline;
	bool started;
	bool listening;
	bool firstMainLoop;
	unsigned long waitRef;
	unsigned long pingRef;
	uint16_t pingTime;
	char *welcomeMsg;
	char *rejectMsg;
	uint16_t minBlockSize;
	uint16_t collectingTime;
	uint16_t maxBlockSize;
	bool debugOutput;
	char *telnetBuf;
	uint16_t bufLen;
	uint16_t bufUsed;
	uint16_t bufRdIdx;
	uint16_t bufWrIdx;
	bool connected;
	void (*callbackConnect)();
	void (*callbackDisconnect)();
};

#endif // _ESPTELNET_H
