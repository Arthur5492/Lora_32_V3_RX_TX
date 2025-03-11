#pragma once

//Parameters
#define FREQ 915.0 //915Mhz

#define BANDWIDTH 62.5 // 62.5, 125, 200, 250, 400, 500 e 800, 
//Larguras abaixo de 62.5 kHz sao instaveis, (maior BW = mais throughput/imunidade, porém menor alcance).

#define SPREADING_FACTOR 12 //SF alto: Data rate menor, mas sensibilidade maior (mais alcance).
                            //SF baixo: Data rate maior, menos tempo no ar, consumo menor.
                            //Valores possíveis: 6, 7, 8, 9, 10, 11, 12

//Check https://www.semtech.com/design-support/lora-calculator for mote details, insert SX1262 on device
#define CODING_RATE 8 
#define SYNC_WORD 0x34
#define PREAMBLE_LENGTH 8
#define MAX_TX_POWER 22    // Maximum transmission power (dBm) allowed for your board/regulatory limits (SX1262)

// Radio pins
#define RADIO_NSS     (8)
#define RADIO_IRQ     (14)
#define RADIO_RST     (12)
#define RADIO_GPIO    (13)

// SPI pins
#define MOSI      10
#define MISO      11
#define SCK       9

//Misc pins
#define BOARD_BUTTON GPIO_NUM_0     // Pin connected to the board's button for triggering functions
#define LED          GPIO_NUM_35   // Pin connected to the board's LED

//I2C pins
#define SDA_OLED  GPIO_NUM_17
#define SCL_OLED  GPIO_NUM_18
#define RST_OLED  GPIO_NUM_21