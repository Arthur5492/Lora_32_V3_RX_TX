//log.h

#ifndef LOG_H
#define LOG_H

#include <Arduino.h>
#include <LittleFS.h>
#include <vector>
#include <string>
#include <esp_log.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h> //To get log in web
#include <TimeLib.h>

#include <easyWifi.h>

/*
* Implementar easyWifi
* 
*/

namespace SIMPLELOG
{
  constexpr const char* APP = "SIMPLELOG";
  constexpr uint8_t maxStoredBufferLogs = 3;

  class SimpleLog
  {
    
    private: 
      std::vector<std::string> _vecLogBuffer; //Buffer that contains Log Data
      const char* _fileName = "/SimpleLog/log.txt";
      AsyncWebServer *_server;
      
      bool _usingRealTime = false;

      void initLittleFS();
      void createLogFile();
      void writeBufferToFS();
      
      public:
      SimpleLog() = default;
      ~SimpleLog() = default;

      void begin();

      bool addLogString(const char* format, ...);
      void printLogBuffer();
      void printLogFileToSerial();
      void deleteFile();
      
      void startServer();
      void stopServer();
      
      bool getntpTime();
      std::string printTime();
  };
}

extern SIMPLELOG::SimpleLog simpleLog;



#endif