/**
 * @file Lora.h
 * @brief Header para gerenciar comunicação LoRa usando o Heltec Lora 32 e a RadioLib.
 *
 * Este arquivo declara a classe Lora, responsável por configurar, inicializar e operar
 * a comunicação via SX1262 (RadioLib). Também lida com recebimento e envio de mensagens
 * empacotadas, usando um struct para enviar os pacotes.
*/

#ifndef LORA_H
#define LORA_H

#include <Arduino.h>
#include "heltec_config.h" //Heltec Lora 32 Parameters
#include <RadioLib.h> //Radio library for SX1262
#include <HotButton.h> //HotButton library for button handling

#include <esp_log.h> //ESP32 logging library ESP_LOGX(LORA, "This is %s log", variable); //Where X could be: E,W,I,D,V


static const char *APP = "Heltec32"; //Nome do aplicativo para o esp_log

/**
 * @brief Estrutura de dados para envio/recebimento via LoRa.
 *
 * @param id       Identificador único do dispositivo (64 bits).
 * @param message  Buffer para texto (até 100 caracteres).
 */
struct __attribute__((packed)) lora_package_t
{
  uint64_t id; //Device unique ID
  char message[100] = {0};
};

/**
 * @brief Interrupção para sinalizar que a operação do rádio foi concluída.
 *
 * @note ICACHE_RAM_ATTR (otimização em ESP, para tratar interrupção na RAM).
 */
ICACHE_RAM_ATTR void setFlag(void);


/**
 * @class Lora
 * @brief Classe para gerenciar a comunicação LoRa usando SX1262 (RadioLib) no Heltec Lora 32.
 *
 * Responsável por:
 *  - Configurar parâmetros do rádio (frequência, SF, BW, etc.).
 *  - Enviar e receber pacotes.
 *  - Tratar interrupções e sinalizar fim de operação.
 *  - Integrar-se com um botão (via HotButton) para realizar ações específicas.
 */
class Lora
{
  public:
    Lora();
    
    HotButton button;                      // Instância do HotButton para gerenciar cliques no Heltec Lora 32.
    volatile bool operationDone = false;   // Flag volátil sinalizando término de uma operação (TX ou RX).

    void setup();                          // Inicializa o rádio e configura parâmetros iniciais. Chamar na setup() do Arduino.
    void update();                         // Atualiza rotinas do rádio e do botão. Chamar periodicamente no loop().

    void initRadio();                      // Inicializa parâmetros de hardware e software do rádio (SX1262).

    void checkPackageReception();          // Verifica se há pacotes recebidos e processa-os se necessário.

    /// @brief Monta e envia uma mensagem via LoRa.
    /// @param message string que será enviada.
    void sendMessage(const char* message); 
    
    void transmitPackage(); // Efetua a transmissão do pacote _package.

    void processDataReceived(); // Trata os dados recebidos e imprime/loga se necessário.
  
    /// Retorna true se não houve erro na operação do rádio.
    /// @param state Código de estado retornado pela RadioLib (ex.: ERR_NONE).
    bool noErrorFound(int state);

  private:
    bool _packageTransmitted = false; // Indica se o pacote foi efetivamente transmitido.
    
    
    void initRadioParameters();     // Função interna para setar parâmetros específicos (BW, SF, CR, etc.).

    /// @details SX1262, inicializado com os pinos definidos em heltec_config.h.
    SX1262 radio = new Module(RADIO_NSS, RADIO_IRQ, RADIO_RST, RADIO_GPIO);

    // Pacote(Struct) usado para transmitir/receber dados.
    lora_package_t _package;
    //METADATA

    // ID único do dispositivo (obtido via MAC do ESP32).
    uint64_t _id = ESP.getEfuseMac();   //Device unique ID
    const char* _deviceName;            //Device name for identification */
};

extern Lora lora;


#endif