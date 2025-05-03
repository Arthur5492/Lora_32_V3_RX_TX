#include "lora.h"

String inputMessage = "";
boolean messageComplete = false;

void checkSerial();
void handleCommand(const String& cmd);
void handleMessage(const String& cmd);

void setup()
{
  Serial.begin(115200);
  
  lora.setup();
  Serial.println("LoRa Sender started. Type a message and press enter to send.");
}

void loop()
{
  lora.update();
  
  checkSerial();
  
  if(lora.button.isSingleClick())
  {
    easyWifi.NVS_Clear();
    ESP.restart();
  }
  // if (lora.button.isSingleClick()) {
  //   lora.sendMessage("Hi there");
  // }
}

// Função refinada de leitura e processamento
void checkSerial() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();  
    if (line.length() == 0) return;  

    if (line.startsWith("/")) {
      handleCommand(line);
    } else {
      handleMessage(line);
    }
  }
}

// Processa comandos iniciados com “/”
void handleCommand(const String& cmd) {
  if (cmd.equalsIgnoreCase("/logserver")) 
    simpleLog.startServer();
  else if (cmd.equalsIgnoreCase("/endserver")) 
    simpleLog.stopServer();   
  else if(cmd.equalsIgnoreCase("/buffer")) 
    simpleLog.printLogBuffer();
  else if(cmd.equalsIgnoreCase("/logfile"))
    simpleLog.printLogFileToSerial();
  else if(cmd.equalsIgnoreCase("/deleteFile"))
    simpleLog.deleteFile();
  else if(cmd.equalsIgnoreCase("/set_time")){
    simpleLog.getntpTime(); //Colocar como parametro ssid
  }
    

  else if (cmd.equalsIgnoreCase("/help") || cmd.equalsIgnoreCase("/h")) {
    Serial.println("📜 Comandos disponíveis:");
    Serial.println("  /logserver   - Inicia o servidor de logs");
    Serial.println("  /endserver   - Para o servidor de logs");
    Serial.println("  /buffer      - Para mostrar os logs guardados no buffer de logs");
    Serial.println("  /logfile     - Para mostrar o arquivo de logs no Serial");
    Serial.println("  /deleteFile  - Para apagar arquivo de logs");
    Serial.println("  /set_time    - Seta horario atual para colocar no log");
    Serial.println("  /help        - Mostra esta ajuda");
  }
  else {
    Serial.printf("Comando não reconhecido: %s, digite /help\n", cmd.c_str());
  }
}

// Envia mensagens LoRa
void handleMessage(const String& msg) {
  Serial.print("Sending: ");
  Serial.println(msg);
  lora.sendMessage(msg.c_str());
}