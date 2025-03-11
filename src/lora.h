#ifndef LORA_H
#define LORA_H

#include <Arduino.h>
#include "heltec_config.h" //Heltec Lora 32 Parameters
#include <RadioLib.h> //Radio library for SX1262
#include <HotButton.h> //HotButton library for button handling

#include <esp_log.h> //ESP32 logging library ESP_LOGX(LORA, "This is %s log", variable); //Where X could be: E,W,I,D,V

static const char *APP = "Heltec32";

struct __attribute__((packed)) lora_package_t
{
  uint64_t id; //Device unique ID
  char message[100] = {0};
};

ICACHE_RAM_ATTR void setFlag(void);

class Lora
{
  public:
    Lora();
  
    HotButton button;
    volatile bool operationDone = false;

    void setup();
    void update();
    void initRadio();
    void checkPackageReception();
    void sendMessage(const char* message);
    void transmitPackage();
    void processDataReceived();
 
    bool noErrorFound(int state);

  private:
    bool _packageTransmitted = false;
    //Private functions
    void initRadioParameters();
    SX1262 radio = new Module(RADIO_NSS, RADIO_IRQ, RADIO_RST, RADIO_GPIO);

    lora_package_t _package;
    //METADATA
    uint64_t _id = ESP.getEfuseMac();   //Device unique ID
    const char* _deviceName;            //Device name for identification */
};

extern Lora lora;


#endif