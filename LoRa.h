// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license.

// Modifications and additions copyright 2018 by Mark Qvist
// Obviously still under the MIT license.

#ifndef LORA_H
#define LORA_H

#include "Platform.h"

#if LIBRARY_TYPE == LIBRARY_ARDUINO
    #include <Arduino.h>
    #include <SPI.h>
#elif LIBRARY_TYPE == LIBRARY_C
    #include <cstdlib>
    #include <cstdint>
    #include <iostream>
    
    // Arduino Stream is not available, but not actually needed.
    class Stream {};
    
    typedef unsigned char byte;
    
    // Arduino SPI is not available, so make a Linux-ish version of SPISettings
    #define MSBFIRST 0
    #define LSBFIRST 1
    #define SPI_MODE0 SPI_MODE_0
    #define SPI_MODE1 SPI_MODE_1
    #define SPI_MODE2 SPI_MODE_2
    #define SPI_MODE3 SPI_MODE_3
    class SPISettings {
    public:
      inline SPISettings(uint32_t frequency, byte bitness, byte mode) : frequency(frequency), bitness(bitness), mode(mode) {};
      SPISettings& operator=(const SPISettings& other) = default;
      uint32_t frequency;
      byte bitness;
      byte mode;
    };
    
#endif

#define LORA_DEFAULT_SS_PIN    10
#define LORA_DEFAULT_RESET_PIN 9
#define LORA_DEFAULT_DIO0_PIN  2

#define PA_OUTPUT_RFO_PIN      0
#define PA_OUTPUT_PA_BOOST_PIN 1

#define RSSI_OFFSET 157

class LoRaClass : public Stream {
public:
  LoRaClass();

  int begin(long frequency);
  void end();

  int beginPacket(int implicitHeader = false);
  int endPacket();

  int parsePacket(int size = 0);
  int packetRssi();
  uint8_t packetRssiRaw();
  uint8_t packetSnrRaw();
  float packetSnr();
  long packetFrequencyError();

  // from Print
  virtual size_t write(uint8_t byte);
  virtual size_t write(const uint8_t *buffer, size_t size);

  // from Stream
  virtual int available();
  virtual int read();
  virtual int peek();
  virtual void flush();

  void pollReceive();
  void onReceive(void(*callback)(int));

  void receive(int size = 0);
  void idle();
  void sleep();

  void setTxPower(int level, int outputPin = PA_OUTPUT_PA_BOOST_PIN);
  uint32_t getFrequency();
  void setFrequency(long frequency);
  void setSpreadingFactor(int sf);
  long getSignalBandwidth();
  void setSignalBandwidth(long sbw);
  void setCodingRate4(int denominator);
  void setPreambleLength(long length);
  void setSyncWord(int sw);
  uint8_t modemStatus();
  void enableCrc();
  void disableCrc();

  // deprecated
  void crc() { enableCrc(); }
  void noCrc() { disableCrc(); }

  byte random();

  void setPins(int ss = LORA_DEFAULT_SS_PIN, int reset = LORA_DEFAULT_RESET_PIN, int dio0 = LORA_DEFAULT_DIO0_PIN);
  void setSPIFrequency(uint32_t frequency);
  
#if LIBRARY_TYPE == LIBRARY_ARDUINO
  void dumpRegisters(Stream& out);
#elif LIBRARY_TYPE == LIBRARY_C
  void dumpRegisters(std::ostream& out);
#endif

private:
  void explicitHeaderMode();
  void implicitHeaderMode();

  void handleDio0Rise();
  void handleRx();

  uint8_t readRegister(uint8_t address);
  void writeRegister(uint8_t address, uint8_t value);
  uint8_t singleTransfer(uint8_t address, uint8_t value);

  static void onDio0Rise();

  void handleLowDataRate();

private:
  SPISettings _spiSettings;
  int _ss;
  int _reset;
  int _dio0;
  long _frequency;
  int _packetIndex;
  int _implicitHeaderMode;
  void (*_onReceive)(int);
  bool _spiBegun;
  #if LIBRARY_TYPE == LIBRARY_C
    int _fd;
  #endif
};

extern LoRaClass LoRa;

#endif
