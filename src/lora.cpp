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

/// \brief Inicializa o rádio e configura parâmetros iniciais.
/// Chame este método na setup() do Arduino.
void Lora::setup()
{
  initRadio();
  simpleLog.begin();
}

/// \brief Atualiza rotinas do rádio e do botão.
/// Chame este método periodicamente (por exemplo, no loop()).
void Lora::update()
{
  button.update();

  lora.checkPackageReception(); // Verifica se houve recepção de pacote
}

/// @brief Initialize the SX1262 radio module
void Lora::initRadio()
{
  radio.begin();
  initRadioParameters();
  radio.setPacketReceivedAction(setFlag);
  radio.startReceive();
  ESP_LOGI(APP, "Radio Initialized, waiting for reception");
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
  }
}

/// \brief Monta e envia uma mensagem via LoRa.
/// \param message Ponteiro para string que será enviada.
void Lora::sendMessage(const char *message)
{ 
  if(strlen(message) > sizeof(_package.message))
  {
    ESP_LOGE(APP, "Payload too long(%d), send a tinnier message.", sizeof(_package.message));
    return;
  }

  strlcpy(_package.message, message, sizeof(_package.message));
  
  transmitPackage();
};

/// \brief Efetua a transmissão do pacote _package.
void Lora::transmitPackage()
{
  ESP_LOGI(APP, "Transmitting message, please wait...", _package.message);
  
  int state;

  state = radio.transmit((uint8_t*)&_package, sizeof(_package));

  if(noErrorFound(state))
  {
    _packageTransmitted = true;
    ESP_LOGI(APP, "Message sent");
    
    simpleLog.addLogString("Message Sent: %s", _package.message);
    
    radio.startReceive(); //Go back to receiving mode, later will be deep sleep
  }
}

/// \brief Trata os dados recebidos e imprime/loga se necessário.
void Lora::processDataReceived()
{
  lora_package_t package;
  
  int state = radio.readData((uint8_t*)&package, sizeof(package));

  if(noErrorFound(state))
  {
    ESP_LOGD(APP, "Data received - Id: %d RSSI: %0.2fdBm SNR: %0.2fdB", package.id, radio.getRSSI(), radio.getSNR());
    ESP_LOGI(APP, "Message: %s", package.message);

    simpleLog.addLogString("Data received - Id: %d RSSI: %0.2fdBm SNR: %0.2fdB; Message: %s", 
                          package.id, radio.getRSSI(), radio.getSNR(),package.message);

    if(strcmp(package.message, "pingpong") == 0)
    {
      delay(1000); //Wait a little before sending back
      sendMessage("pingpong");
    }

  }
  else
  {
    ESP_LOGE(APP, "Error receiving data");
    simpleLog.addLogString("Error on packet reception, RadioLib status code: %d", state);
  }
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
  ESP_LOGV(APP, "Setting LoRa Parameters: Freq=%f, BW=%f, SF=%d, CR=%d, SW=%d, Preamble=%d, TxPower=%d",
            FREQ, BANDWIDTH, SPREADING_FACTOR, CODING_RATE, SYNC_WORD, PREAMBLE_LENGTH, MAX_TX_POWER);
  // Serial.printf("Setting LoRa Parameters: Freq=%f, BW=%f, SF=%d, CR=%d, SW=%d, Preamble=%d, TxPower=%d",
            // FREQ, BW, SF, CR, SYNC_WORD, PREAMBLE, MAX_TX_POWER);
  
  radio.setFrequency(FREQ);
  radio.setBandwidth(BANDWIDTH);
  radio.setSpreadingFactor(SPREADING_FACTOR);
  radio.setCodingRate(CODING_RATE);
  radio.setSyncWord(SYNC_WORD);
  radio.setPreambleLength(PREAMBLE_LENGTH);
  radio.setOutputPower(MAX_TX_POWER);

  radio.setCRC(true);
}