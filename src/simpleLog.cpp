//log.cpp

#include "simpleLog.h"

using namespace SIMPLELOG;

SimpleLog simpleLog;

void SimpleLog::begin()
{
  //! easyWifi.setup(); //Check wifi connection
  
  initLittleFS();

  // getntpTime();
    
  
  // startServer();
}


void SimpleLog::initLittleFS()
{
  if(!LittleFS.begin())
  {
    ESP_LOGE(SIMPLELOG::APP, "LittleFS mount failed");
    return;
  }
  
  if(LittleFS.exists(_fileName) == false)
    createLogFile();
}



void SimpleLog::createLogFile()
{
  ESP_LOGD(SIMPLELOG::APP, "SimpleLog LittleFS Dir not created, creating...");
  LittleFS.open(_fileName,"w", true);

  if(LittleFS.exists(_fileName) )
    ESP_LOGV(SIMPLELOG::APP, "Log file created: %s", _fileName);
  else
    ESP_LOGE(SIMPLELOG::APP, "Error creating: %s", _fileName);
  
}

bool SimpleLog::addLogString(const char* format, ...)
{
  if(_vecLogBuffer.size() >= maxStoredBufferLogs)
  {
    writeBufferToFS();
    _vecLogBuffer.clear();
    _vecLogBuffer.shrink_to_fit();
  }

  if (format == nullptr)
  {
    ESP_LOGE(SIMPLELOG::APP, "Error adding log message to buffer, format is null");
    return false;
  }

  constexpr size_t LOG_BUFFER_SIZE = 512;
  char logMessage[LOG_BUFFER_SIZE];

  va_list args;
  va_start(args, format);
  vsnprintf(logMessage, LOG_BUFFER_SIZE, format, args);
  va_end(args);
  
  if(_usingRealTime)
  {
    std::string bufferString = printTime() + ": " + logMessage;
    _vecLogBuffer.emplace_back(bufferString);
  }
  else
    _vecLogBuffer.emplace_back(logMessage);

  return true;
}

void SimpleLog::writeBufferToFS()
{
  File logFile = LittleFS.open(_fileName,"a");

  if(!logFile)
  {
    ESP_LOGE(SIMPLELOG::APP, "Error opening log file in: %s", _fileName);
    return;
  }

  for(const auto &log : _vecLogBuffer)
    logFile.println(log.c_str());
  
  logFile.close();

  ESP_LOGV(SIMPLELOG::APP,"Log buffer inserted in log.txt");
}

void SimpleLog::deleteFile()
{
  if(!LittleFS.exists(_fileName))
  {
    ESP_LOGW(SIMPLELOG::APP, "File: %s does not exist", _fileName);
    return;
  }
  
  LittleFS.remove(_fileName);
  ESP_LOGV(SIMPLELOG::APP, "File: %s successfully deleted", _fileName);
}

void SimpleLog::printLogFileToSerial()
{
  File logFile = LittleFS.open(_fileName,"r");

  if(!logFile)
  {
    ESP_LOGE(SIMPLELOG::APP, "Error opening log file in: %s", _fileName);
    return;
  }

  if(logFile.size() != 0)
  {
    while (logFile.available()) 
    {
      // Serial.write(logFile.read()) //?Da p usar so isso
      String line = logFile.readStringUntil('\n');
      Serial.println(line); 
    }
  }
  else
    Serial.println("Log File is empty");
  
  logFile.close();
}

void SimpleLog::startServer()
{
  ESP_LOGI(SIMPLELOG::APP, "Starting Server for Log");
  WiFi.mode(WIFI_AP);
  std::string softAPName = "ESP32-"+ std::to_string(ESP.getEfuseMac());
  WiFi.softAP(softAPName.c_str(),"cachorro");

  if(!_server)
    _server = new AsyncWebServer(80);

  _server->on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
    request->send(LittleFS, _fileName, "text/plain");
  });

  _server->begin();
  ESP_LOGI(SIMPLELOG::APP, "Server started at http://%s", WiFi.softAPIP().toString().c_str());
}

void SimpleLog::stopServer()
{
  if(_server)
  {
    _server->end();
    delete _server;
    _server = nullptr;

    Serial.println("Log Server ended");
  }
  else
    ESP_LOGE(SIMPLELOG::APP, "Server is not initializated");
}

void SimpleLog::printLogBuffer()
{
  if(_vecLogBuffer.empty())
    Serial.println("Buffer is empty");

  for(size_t i=0; i<_vecLogBuffer.size();i++)
    Serial.println(_vecLogBuffer.at(i).c_str());
}

bool SimpleLog::getntpTime()
{
  //Connect to wifi to proceed
  std::string ssidName = "ESP-" + std::to_string(ESP.getEfuseMac());
  easyWifi.setup(ssidName.c_str(), "cachorro");
  unsigned long start = millis();                 //2min*60sec*1000ms
  while(!WiFi.isConnected() && (millis() - start) <= 2*60*1000){
    easyWifi.update();
    delay(100);
  }

  if(!WiFi.isConnected()){
    ESP_LOGE(APP, "Wifi could not be connected");
    return false;
  }

  const char* ntpServer = "pool.ntp.org";
  const long  gmtOffset_sec = -3 * 60 * 60; //UTC-3 *60min *60sec
  const int   daylightOffset_sec = 0;

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  //Test to wait until ntp time is found
  struct tm test_timeinfo;
  if(getLocalTime(&test_timeinfo, 10000))
  {
    ESP_LOGV(SIMPLELOG::APP, "Successfully found ntp time");
    _usingRealTime = true;
    return true;
  }

  ESP_LOGE(SIMPLELOG::APP, "Error finding NTP Time");
  return false;
}

std::string SimpleLog::printTime()
{
  struct tm timeInfo; 
  if(!getLocalTime(&timeInfo))
  {
    ESP_LOGE(SIMPLELOG::APP,"Failed to obtain Time");
    return "Time Error";
  }

  char buffer[30];
  strftime(buffer, sizeof(buffer), "%H:%M:%S %d/%m/%Y", &timeInfo);

  return std::string(buffer);
}