#ifndef CARDOS_CONFIG_H
#define CARDOS_CONFIG_H

#define CARDOS_VERSION "0.1.0"
#define CARDOS_DEVICE "M5Stack Cardputer"
#define CARDOS_MCU "ESP32-S3"

#define SD_CS_PIN 4
#define SD_MOSI_PIN 37
#define SD_MISO_PIN 35
#define SD_SCK_PIN 36

#define TFT_CS 7
#define TFT_DC 8
#define TFT_RST 9
#define TFT_MOSI 37
#define TFT_MISO 35
#define TFT_SCK 36

#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240

#define MAX_RAM_USAGE 300 * 1024
#define AUTO_SAVE_INTERVAL 5000
#define DEBOUNCE_TIME 50

#define MODULE_COUNT 6

#endif
