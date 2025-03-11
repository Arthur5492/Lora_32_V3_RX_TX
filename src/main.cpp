#include "lora.h"

String inputMessage = "";
boolean messageComplete = false;

void setup()
{
  Serial.begin(115200);
  Serial.println("LoRa Sender iniciado. Digite uma mensagem e pressione Enter para enviar.");
  
  lora.setup();
}

void loop()
{
  lora.update();
  
  while (Serial.available() > 0) {
    char inChar = (char)Serial.read();
    
    if (inChar == '\n') {
      messageComplete = true;
    } else {
      inputMessage += inChar;
    }
  }
  
  if (messageComplete) {
    Serial.print("Enviando: ");
    Serial.println(inputMessage);
    
    lora.sendMessage(inputMessage.c_str());
    
    inputMessage = "";
    messageComplete = false;
  }
  

  // if (lora.button.isSingleClick()) {
  //   lora.sendMessage("Hi there");
  // }
}