#include "lora.h"

Lora lora;

ICACHE_RAM_ATTR void setFlag(void)
{
  lora.operationDone = true;
}

Lora::Lora() : button(BOARD_BUTTON)
{
  _package.id = _id;
}

void Lora::setup()
{
  initRadio();
}

void Lora::initRadio()
{
  radio.begin();
  initRadioParameters();
  radio.setPacketReceivedAction(setFlag);
  radio.startReceive();
  ESP_LOGI(APP, "SX1262 Initialized, waiting for reception");
}

void Lora::update()
{
  button.update();

  lora.checkPackageReception();
}

void Lora::checkPackageReception()
{
  if(!operationDone)
    return;

  operationDone = false;

  if(_packageTransmitted) //Last operation was transmission, listen for response
  {
    radio.startReceive(); //Go back to receiving mode
    _packageTransmitted = false; //Reset flag
  }
  else //Last operation was receiving, process data and transmit ID back
  {
    processDataReceived();
    delay(5000);  
    // sendMessage();
  }
}

void Lora::sendMessage(const char *message)
{ 
  if(strlen(message) > sizeof(_package.message))
  {
    ESP_LOGE(APP, "Mensagem muito longa, envie uma mensagem menor. %d", sizeof(_package.message));
    return;
  }

  strlcpy(_package.message, message, sizeof(_package.message));
  
  transmitPackage();

  ESP_LOGI(APP, "Mensagem enviada: %s", _package.message);
};

void Lora::transmitPackage()
{
  ESP_LOGI(APP, "Transmitindo, mensagem: %s, Aguarde...", _package.message);

  int state = radio.transmit((uint8_t*)&_package, sizeof(_package));

  if(noErrorFound(state))
  {
    _packageTransmitted = true;
    radio.startReceive(); //Go back to receiving mode, later will be deep sleep
    ESP_LOGI(APP, "Mensagem enviada, aguardando resposta");
  }
}

void Lora::processDataReceived()
{
  lora_package_t package;
  
  int state = radio.readData((uint8_t*)&package, sizeof(package));

  if(noErrorFound(state))
  {
    ESP_LOGD(APP, "Data received - Id: %d RSSI: %0.2fdBm SNR: %0.2fdB", package.id, radio.getRSSI(), radio.getSNR());
    ESP_LOGI(APP, "Message: %s", package.message);
    Serial.printf("Data received - Id: %d RSSI: %0.2fdBm SNR: %0.2fdB", package.id, radio.getRSSI(), radio.getSNR());

  }
  else
    ESP_LOGE(APP, "Error receiving data");
}

/// @brief //Check if there is any error in the RADIOLIB state given
/// @param state RADIOLIB state returned from a function
/// @return false if there is an error, true if there is no error
bool Lora::noErrorFound(int state)
{
  if(state != RADIOLIB_ERR_NONE)
  {
    ESP_LOGE(APP, "Error on RADIOLIB, status code: %d", state);
    ESP_LOGE(APP, "Find error status code at: https://jgromes.github.io/RadioLib/group__status__codes.html");
    return false;
  }
  
  return true;
}

void Lora::initRadioParameters()
{ 
  ESP_LOGD(APP, "Setting LoRa Parameters: Freq=%f, BW=%f, SF=%d, CR=%d, SW=%d, Preamble=%d, TxPower=%d",
            FREQ, BW, SF, CR, SYNC_WORD, PREAMBLE, MAX_TX_POWER);
  // Serial.printf("Setting LoRa Parameters: Freq=%f, BW=%f, SF=%d, CR=%d, SW=%d, Preamble=%d, TxPower=%d",
            // FREQ, BW, SF, CR, SYNC_WORD, PREAMBLE, MAX_TX_POWER);

  radio.setFrequency(FREQ);
  radio.setBandwidth(BW);
  radio.setSpreadingFactor(SF);
  radio.setCodingRate(CR);
  radio.setSyncWord(SYNC_WORD);
  radio.setPreambleLength(PREAMBLE);
  radio.setOutputPower(MAX_TX_POWER);

  radio.setCRC(true);
}