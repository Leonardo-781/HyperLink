// User_Setup.h - Configuração TFT_eSPI para HyperLink TX
// 
// Projeto: HyperLink - Dual Radio Controller
// Copiar este arquivo para: Arduino/libraries/TFT_eSPI/User_Setup.h
// 
// Display: ST7789 240x240
// Microcontrolador: ESP32 38 pinos

#ifndef USER_SETUP_H_
#define USER_SETUP_H_

// Driver e resolução
#define ST7789_DRIVER
#define TFT_WIDTH  240
#define TFT_HEIGHT 240

// Pinos do ESP32 (conforme mapeamento do projeto)

// Pinos SPI (VSPI compartilhado com RF24 e CC1101)
#define TFT_MOSI GPIO_NUM_23  // GPIO 23 - Dados saída (MOSI/SDI)
#define TFT_MISO GPIO_NUM_19  // GPIO 19 - Dados entrada (MISO/SDO)  [Não usado em write-only]
#define TFT_SCLK GPIO_NUM_18  // GPIO 18 - Clock SPI

// Pinos de controle do display
#define TFT_CS   GPIO_NUM_12  // GPIO 12 - Chip Select
#define TFT_DC   GPIO_NUM_13  // GPIO 13 - Data/Command
#deSPI compartilhado com RF24 e CC1101
#define TFT_MOSI GPIO_NUM_23
#define TFT_MISO GPIO_NUM_19
#define TFT_SCLK GPIO_NUM_18

// Controle do display
#define TFT_CS   GPIO_NUM_12
#define TFT_DC   GPIO_NUM_13
#define TFT_RST  GPIO_NUM_26

// Velocidade SPI
#define SPI_FREQUENCY 20000000   // 20MHz (se tiver problemas, tente 10000000)

// Rotação da tela
#define TFT_ROTATION 1  // Landscape

// Backlight (opcional)
#define TFT_BL GPIO_NUM_15

// Fontes
// =============================================
// 9. OTIMIZAÇÕES DE PERFORMANCE
// =============================================
#define SMOOTH_FONT          // Fontes Anti-aliased (melhor qualidade)
// #define SSD1963_DRIVER    // Descomente se usar outro display

// =============================================
// 10. CONFIGURAÇÕES AVANÇADAS
// ==============
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7

#define SMOOTH_FONT

// Delay após reset
#define TFT_RST_DELAY 120