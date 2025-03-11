#pragma once

#define FREQ 915.0
#define BW 62.5 
#define SF 12
#define CR 8
#define SYNC_WORD 0x34
#define PREAMBLE 8

// Radio pins
#define RADIO_NSS     (8)
#define RADIO_IRQ     (14)
#define RADIO_RST     (12)
#define RADIO_GPIO    (13)




// SPI pins
#define MOSI      10
#define MISO      11
#define SCK       9

//Important config
#define MAX_TX_POWER 22    // Maximum transmission power (dBm) allowed for your board/regulatory limits

//Misc pins
#define BOARD_BUTTON GPIO_NUM_0     // Pin connected to the board's button for triggering functions
#define LED          GPIO_NUM_35   // Pin connected to the board's LED

//I2C pins
#define SDA_OLED  GPIO_NUM_17
#define SCL_OLED  GPIO_NUM_18
#define RST_OLED  GPIO_NUM_21