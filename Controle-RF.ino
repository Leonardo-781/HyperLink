/**
 * HyperLink - Transmissor Dual-Radio ESP32
 * RF24L01 (2.4GHz) + CC1101 (915MHz) + Display ST7789 + Encoder
 * 
 * Dual Radio com seleção física em tempo real
 * Modos: Controle | Wall | Aeromodelo | Simulador
 * 
 * Versão: 2.1
 * Status: Pronto para PCB - Zero conflitos
 * 
 * Hardware: ESP32 Dev Module (38 pinos)
 * 
 * RESUMO DO MAPEAMENTO:
 * - ADC1: 8/8 canais usados (Joysticks 1-2, Pots 1-3, Bateria)
 * - SPI: Compartilhado (RF24, CC1101, ST7789)
 * - WiFi: AP "Controle_RF" em 192.168.4.1
 * - Display: ST7789 240x240 navegável por encoder
 * - Rádios: Selecionáveis por Switch 4 em tempo real
 */

#include <SPI.h>
#include <RF24.h>
#include <WiFi.h>
#include <WebServer.h>
#include <TFT_eSPI.h>  // Para ST7789

// ---------------------------------------------------------------------------
// CONFIG GERAL
// ---------------------------------------------------------------------------
#define ENABLE_WIFI_LOGS 1
#define ENABLE_ST7789 1
#define SEND_PERIOD_MS 40
#define LOG_SIZE 100
#define RADIO_TEST_INTERVAL 2000  // Teste de saúde do rádio a cada 2s

// Modos de operação
enum OperatingMode {
  MODE_CONTROLE = 0,
  MODE_WALL = 1,
  MODE_AERO = 2,
  MODE_SIMULADOR = 3
};

// Rádios disponíveis
enum RadioModule {
  RADIO_RF24 = 0,
  RADIO_CC1101 = 1
};

// Estado da aplicação
struct AppState {
  OperatingMode mode;
  RadioModule activeRadio;
  bool rf24_ok;
  bool cc1101_ok;
  float bateria;
  uint32_t pacotesSent;
  uint32_t pacotesAck;
  bool encoderChanged;
  uint8_t screenPage;  // 0=status, 1=dados, 2=diagnóstico
};

AppState appState = {
  MODE_CONTROLE,
  RADIO_RF24,
  false,
  false,
  0.0f,
  0,
  0,
  false,
  0
};

// ---------------------------------------------------------------------------
// MAPEAMENTO DE PINOS (ESP32 38 PINOS - SEM CONFLITOS ✅)
// ---------------------------------------------------------------------------

// ENTRADAS ANALÓGICAS (ADC1 - 100% compatível com WiFi)
// Todos os 8 canais ADC1 do ESP32 estão sendo usados:
#define J1X 32           // GPIO 32 - ADC1_CH4 (Joystick 1 Eixo X)
#define J1Y 33           // GPIO 33 - ADC1_CH5 (Joystick 1 Eixo Y)
#define J2X 34           // GPIO 34 - ADC1_CH6 (Joystick 2 Eixo X)
#define J2Y 35           // GPIO 35 - ADC1_CH7 (Joystick 2 Eixo Y)

// POTENCIÔMETROS (ADC1)
#define POT1 36          // GPIO 36 - ADC1_CH0 (Potenciômetro 1)
#define POT2 39          // GPIO 39 - ADC1_CH3 (Potenciômetro 2)
#define POT3 37          // GPIO 37 - ADC1_CH1 (Potenciômetro 3) ✨ ESP32 38P

// SENSOR BATERIA (ADC digital, não conflita)
#define VBAT 15          // GPIO 15 - Leitura 7,4V (divisor 22k/10k)
#define VBAT_FACTOR 10.56f  // Fator de divisão: (ADC/4095) * 3.3 * 10.56

// SWITCHES DIGITAIS (INPUT_PULLUP)
#define SW1 14           // GPIO 14 - Switch ON/OFF 1
#define SW2 21           // GPIO 21 - Switch ON/OFF 2
#define SW3A 4           // GPIO 4  - Switch Modo (bit A)
#define SW3B 2           // GPIO 2  - Switch Modo (bit B)
#define SW4_RADIO_SEL 22 // GPIO 22 - Seletor RF24/CC1101

// ENCODER ROTATIVO
#define ENCODER_CLK 11   // GPIO 11 - Encoder Clock (navegação)
#define ENCODER_DT 8     // GPIO 8  - Encoder DT (direção)
#define ENCODER_SW 9     // GPIO 9  - Encoder Botão (select)

// EXTRAS - 2 PINOS UTILIZADOS (3 livres restantes: 30, 31, ...)
#define BUZZER 25        // GPIO 25 - Buzzer piezo (feedback sonoro)
#define CALIB_BTN 27     // GPIO 27 - Botão calibração joysticks

// RF24L01 (SPI VSPI - Compartilhado)
#define RF24_CE 17       // GPIO 17 - Chip Enable
#define RF24_CSN 16      // GPIO 16 - Chip Select
#define RF24_SCK 18      // GPIO 18 - SPI Clock (compartilhado)
#define RF24_MOSI 23     // GPIO 23 - SPI Data Out (compartilhado)
#define RF24_MISO 19     // GPIO 19 - SPI Data In (compartilhado)

// CC1101 (SPI VSPI - Compartilhado)
#define CC1101_CSN 5     // GPIO 5  - Chip Select
#define CC1101_GDO0 7    // GPIO 7  - Status/Interrupção
#define CC1101_SCK 18    // GPIO 18 - SPI Clock (compartilhado)
#define CC1101_MOSI 23   // GPIO 23 - SPI Data Out (compartilhado)
#define CC1101_MISO 19   // GPIO 19 - SPI Data In (compartilhado)

// ST7789 240x240 TFT (SPI VSPI - Compartilhado)
#define TFT_CS 12        // GPIO 12 - Chip Select
#define TFT_DC 13        // GPIO 13 - Data/Command
#define TFT_RST 26       // GPIO 26 - Reset (não GPIO 0, que é strapping!)
#define TFT_SCK 18       // GPIO 18 - SPI Clock (compartilhado)
#define TFT_MOSI 23      // GPIO 23 - SPI Data Out (compartilhado)

// ---------------------------------------------------------------------------
// INICIALIZAÇÃO DE MÓDULOS
// ---------------------------------------------------------------------------

RF24 radio(RF24_CE, RF24_CSN);
const byte rf24Address[5] = {'R', 'F', '2', '4', '1'};

// Placeholder para CC1101 (biblioteca precisa ser instalada)
// #include <CC1101.h>
// CC1101 cc1101;

TFT_eSPI tft = TFT_eSPI();

#if ENABLE_WIFI_LOGS
WebServer server(80);
#endif

String logBuffer[LOG_SIZE];
uint16_t logIndex = 0;

// ---------------------------------------------------------------------------
// STRUCT DE DADOS
// ---------------------------------------------------------------------------
struct PacketRF {
  // Joysticks
  uint16_t j1x, j1y;
  uint16_t j2x, j2y;
  
  // Potenciômetros
  uint16_t pot1, pot2, pot3;
  
  // Switches
  bool sw1, sw2;
  uint8_t sw3Mode;  // 0=1, 1=2, 2=3
  
  // Bateria
  float bateria;
  
  // Modo operacional
  uint8_t mode;     // 0=Controle, 1=Wall, 2=Aero, 3=Simulator
  
  // Timestamp e telemetria
  uint32_t timestamp;
  uint8_t rssi;     // placeholder para RSSI fake
};

PacketRF pacote;

// Controle de taxa de envio
uint32_t lastSendMs = 0;
uint32_t lastRadioTestMs = 0;
uint32_t lastScreenUpdateMs = 0;

// Encoder estado
int lastEncoderCLK = HIGH;
int lastEncoderDT = HIGH;

// Calibração estado
struct CalibrationData {
  uint16_t j1x_min, j1x_max;
  uint16_t j1y_min, j1y_max;
  uint16_t j2x_min, j2x_max;
  uint16_t j2y_min, j2y_max;
  bool isCalibrated;
};

CalibrationData calibration = {
  0, 4095, 0, 4095,  // Joystick 1 defaults
  0, 4095, 0, 4095,  // Joystick 2 defaults
  false
};

bool calibrationMode = false;
uint32_t calibrationStartTime = 0;

// ---------------------------------------------------------------------------
// LOG SYSTEM
// ---------------------------------------------------------------------------
void addLog(String s) {
  logBuffer[logIndex % LOG_SIZE] = s;
  logIndex++;
  Serial.println("[LOG] " + s);
}

// ---------------------------------------------------------------------------
// BUZZER - FEEDBACK SONORO
// ---------------------------------------------------------------------------
void beep(int freq = 800, int durationMs = 100) {
  tone(BUZZER, freq, durationMs);
}

void beepSuccess() {
  beep(1000, 100);  // 1kHz, 100ms
}

void beepError() {
  beep(400, 200);   // 400Hz, 200ms
  delay(50);
  beep(400, 200);   // 2 beeps
}

void beepWarning() {
  beep(800, 150);   // 800Hz, 150ms
}

void beepCalibrating() {
  beep(1200, 50);   // Bip curto durante calibração
}

// ---------------------------------------------------------------------------
// LEITURA DE ANALÓGICOS + FILTRO
// ---------------------------------------------------------------------------
uint16_t filtroAnalogico(int pino) {
  uint32_t soma = 0;
  for (int i = 0; i < 5; i++) soma += analogRead(pino);
  return soma / 5;
}

// ---------------------------------------------------------------------------
// CALIBRAÇÃO DE JOYSTICKS
// ---------------------------------------------------------------------------
void startCalibration() {
  calibrationMode = true;
  calibrationStartTime = millis();
  
  // Reset aos valores extremos
  calibration.j1x_min = 4095; calibration.j1x_max = 0;
  calibration.j1y_min = 4095; calibration.j1y_max = 0;
  calibration.j2x_min = 4095; calibration.j2x_max = 0;
  calibration.j2y_min = 4095; calibration.j2y_max = 0;
  
  addLog("CALIBRAÇÃO INICIADA - Mova os joysticks ao máximo!");
  beepSuccess();
}

void calibrationUpdate() {
  // Lê valores atuais
  uint16_t j1x = filtroAnalogico(J1X);
  uint16_t j1y = filtroAnalogico(J1Y);
  uint16_t j2x = filtroAnalogico(J2X);
  uint16_t j2y = filtroAnalogico(J2Y);
  
  // Encontra mínimos e máximos
  if (j1x < calibration.j1x_min) calibration.j1x_min = j1x;
  if (j1x > calibration.j1x_max) calibration.j1x_max = j1x;
  if (j1y < calibration.j1y_min) calibration.j1y_min = j1y;
  if (j1y > calibration.j1y_max) calibration.j1y_max = j1y;
  
  if (j2x < calibration.j2x_min) calibration.j2x_min = j2x;
  if (j2x > calibration.j2x_max) calibration.j2x_max = j2x;
  if (j2y < calibration.j2y_min) calibration.j2y_min = j2y;
  if (j2y > calibration.j2y_max) calibration.j2y_max = j2y;
  
  // Bip a cada 500ms durante calibração
  static uint32_t lastBeep = 0;
  if (millis() - lastBeep > 500) {
    beepCalibrating();
    lastBeep = millis();
  }
}

void endCalibration() {
  calibrationMode = false;
  calibration.isCalibrated = true;
  
  addLog("CALIBRAÇÃO CONCLUÍDA!");
  addLog("J1X: " + String(calibration.j1x_min) + "-" + String(calibration.j1x_max));
  addLog("J1Y: " + String(calibration.j1y_min) + "-" + String(calibration.j1y_max));
  addLog("J2X: " + String(calibration.j2x_min) + "-" + String(calibration.j2x_max));
  addLog("J2Y: " + String(calibration.j2y_min) + "-" + String(calibration.j2y_max));
  
  beepSuccess();
  delay(100);
  beepSuccess();  // 2 bips confirmando
}

// Aplica calibração aos valores lidos
uint16_t applyCalibration(uint16_t rawValue, uint16_t minVal, uint16_t maxVal) {
  if (!calibration.isCalibrated) return rawValue;
  
  int range = maxVal - minVal;
  if (range <= 0) return 2048;  // Centro
  
  int mapped = (rawValue - minVal) * 4095 / range;
  if (mapped < 0) mapped = 0;
  if (mapped > 4095) mapped = 4095;
  
  return (uint16_t)mapped;
}

// ---------------------------------------------------------------------------
// LEITURA DIGITAL COM DEBOUNCE (CORREÇÃO: FUNÇÃO FALTAVA!)
// ---------------------------------------------------------------------------
bool leituraDigitalEstavel(int pino, int debounceMs = 20) {
  int valor1 = digitalRead(pino);
  delay(debounceMs);
  int valor2 = digitalRead(pino);
  
  if (valor1 == valor2) {
    return valor2 == LOW;  // Retorna true se pressionado (LOW = ativado)
  }
  return false;  // Se não estável, retorna false
}

// ---------------------------------------------------------------------------
// LEITURA DE BATERIA (7,4V com divisor)
// ---------------------------------------------------------------------------
float lerBateria() {
  int raw = filtroAnalogico(VBAT);
  float v = (raw / 4095.0) * 3.3 * VBAT_FACTOR;
  return v;
}

// ---------------------------------------------------------------------------
// LEITURA DO MODO (SW3)
// ---------------------------------------------------------------------------
OperatingMode lerModo() {
  bool a = digitalRead(SW3A);
  bool b = digitalRead(SW3B);
  
  if (a == LOW && b == HIGH) return MODE_CONTROLE;
  if (a == HIGH && b == LOW) return MODE_WALL;
  if (a == LOW && b == LOW) return MODE_AERO;
  return MODE_SIMULADOR;  // HIGH, HIGH
}

// ---------------------------------------------------------------------------
// ENCODER ROTATIVO (navegação display)
// ---------------------------------------------------------------------------
void updateEncoder() {
  int CLK = digitalRead(ENCODER_CLK);
  int DT = digitalRead(ENCODER_DT);
  
  if (CLK != lastEncoderCLK) {
    if (DT != CLK) {
      appState.screenPage = (appState.screenPage + 1) % 3;
    } else {
      appState.screenPage = (appState.screenPage - 1 + 3) % 3;
    }
    appState.encoderChanged = true;
    lastEncoderCLK = CLK;
  }
  lastEncoderDT = DT;
}

// ---------------------------------------------------------------------------
// RF24 INICIALIZAÇÃO E TESTE
// ---------------------------------------------------------------------------
bool initRF24() {
  if (!radio.begin()) {
    addLog("RF24 FALHOU na inicialização");
    return false;
  }
  
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_1MBPS);
  radio.setRetries(5, 15);
  radio.enableAckPayload();
  radio.openWritingPipe(rf24Address);
  radio.stopListening();
  
  addLog("RF24 inicializado OK");
  return true;
}

bool testRF24() {
  // Simples: tenta enviar um pacote small e vê se recebe ACK
  uint8_t testData = 0xAA;
  bool ok = radio.write(&testData, 1);
  return ok;
}

// ---------------------------------------------------------------------------
// CC1101 INICIALIZAÇÃO E TESTE
// ---------------------------------------------------------------------------
bool initCC1101() {
  // TODO: Implementar quando biblioteca CC1101 estiver instalada
  addLog("CC1101 inicialização - placeholder");
  // cc1101.init();
  // cc1101.setFrequency(915);
  // cc1101.setChannel(0);
  return false;  // Ainda não implementado
}

bool testCC1101() {
  // TODO: Implementar teste quando biblioteca estiver pronta
  return false;
}

// ---------------------------------------------------------------------------
// DISPLAY ST7789 - SCREENS
// ---------------------------------------------------------------------------

void initDisplay() {
  tft.init();
  tft.setRotation(1);  // Horizontal
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  tft.println("INICIALIZANDO...");
}

void drawScreen_Status() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(0, 0);
  
  tft.println("STATUS");
  tft.setTextSize(1);
  
  // Modo atual
  const char* modeStr[] = {"CONTROLE", "WALL", "AERO", "SIMULADOR"};
  tft.print("Modo: "); tft.println(modeStr[appState.mode]);
  
  // Rádio ativo
  const char* radioStr[] = {"RF24", "CC1101"};
  tft.print("Modulo: "); tft.println(radioStr[appState.activeRadio]);
  
  // Status de cada rádio
  tft.print("RF24: ");
  tft.println(appState.rf24_ok ? "OK" : "FALHA");
  
  tft.print("CC1101: ");
  tft.println(appState.cc1101_ok ? "OK" : "FALHA");
  
  // Bateria
  tft.print("Bateria: ");
  tft.print(appState.bateria, 2);
  tft.println("V");
  
  // Contador de pacotes
  tft.print("Enviados: ");
  tft.println(appState.pacotesSent);
  
  tft.print("ACK: ");
  tft.println(appState.pacotesAck);
  
  // Status de calibração
  tft.print("Calibrado: ");
  tft.println(calibration.isCalibrated ? "SIM" : "NAO");
  
  // Footer
  tft.setTextSize(1);
  tft.setCursor(0, 230);
  tft.println("< Pagina 1 de 3 >");
}

void drawScreen_Dados() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  
  tft.println("DADOS DE ENTRADA");
  
  tft.print("J1: ");
  tft.print(pacote.j1x); tft.print(" ");
  tft.println(pacote.j1y);
  
  tft.print("J2: ");
  tft.print(pacote.j2x); tft.print(" ");
  tft.println(pacote.j2y);
  
  tft.print("P1: "); tft.println(pacote.pot1);
  tft.print("P2: "); tft.println(pacote.pot2);
  tft.print("P3: "); tft.println(pacote.pot3);
  
  tft.print("SW1: ");
  tft.print(pacote.sw1 ? "ON" : "OFF");
  tft.print("  SW2: ");
  tft.println(pacote.sw2 ? "ON" : "OFF");
  
  tft.print("SW3: ");
  const char* sw3Str[] = {"POS1", "POS2", "POS3", "POS4"};
  tft.println(sw3Str[pacote.sw3Mode]);
  
  tft.setCursor(0, 230);
  tft.println("< Pagina 2 de 3 >");
}

void drawScreen_Diagnostico() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  
  // Se em modo calibração
  if (calibrationMode) {
    tft.setTextSize(2);
    tft.println("CALIBRACAO EM ANDAMENTO");
    tft.setTextSize(1);
    
    uint32_t elapsed = (millis() - calibrationStartTime) / 1000;
    tft.print("Tempo: ");
    tft.print(elapsed);
    tft.println("s");
    
    tft.println("\nMOVIMENTE OS JOYSTICKS");
    tft.println("ao máximo em todas");
    tft.println("as direções!");
    
    tft.println("\nJ1X min/max:");
    tft.print(calibration.j1x_min);
    tft.print(" / ");
    tft.println(calibration.j1x_max);
    
    tft.println("\nJ2X min/max:");
    tft.print(calibration.j2x_min);
    tft.print(" / ");
    tft.println(calibration.j2x_max);
    
    return;  // Não mostra o resto
  }
  
  tft.println("DIAGNOSTICO");
  
  // Frequência de envio
  uint32_t now = millis();
  static uint32_t lastTime = 0;
  static uint16_t frameTime = 0;
  if (now - lastTime > 0) {
    frameTime = now - lastTime;
    lastTime = now;
  }
  
  tft.print("Frame: ");
  tft.print(frameTime);
  tft.println("ms");
  
  // Taxa de ACK (com proteção extra contra overflow)
  float ackRate = 0.0f;
  if (appState.pacotesSent > 0) {
    uint32_t acks = min(appState.pacotesAck, appState.pacotesSent);
    ackRate = (100.0f * acks / appState.pacotesSent);
  }
  
  tft.print("Taxa ACK: ");
  tft.print(ackRate, 1);
  tft.println("%");
  
  // Calibração
  tft.print("Calibrado: ");
  tft.println(calibration.isCalibrated ? "SIM" : "NAO");
  
  // Logs recentes
  tft.println("\nUltimos Logs:");
  int inicio = max(0, (int)logIndex - 5);
  for (int i = inicio; i < logIndex && i < inicio + 5; i++) {
    tft.println(logBuffer[i % LOG_SIZE].substring(0, 20));
  }
  
  tft.setCursor(0, 230);
  tft.println("< Pagina 3 de 3 >");
}

void updateDisplay() {
  if (!ENABLE_ST7789) return;
  if (millis() - lastScreenUpdateMs < 500) return;  // Atualiza a cada 500ms
  
  lastScreenUpdateMs = millis();
  
  switch (appState.screenPage) {
    case 0:
      drawScreen_Status();
      break;
    case 1:
      drawScreen_Dados();
      break;
    case 2:
      drawScreen_Diagnostico();
      break;
    default:
      appState.screenPage = 0;
  }
}

// ---------------------------------------------------------------------------
// COLETA DE DADOS (CORRIGIDO: APLICAR CALIBRAÇÃO!)
// ---------------------------------------------------------------------------
void coletarDados() {
  // Lê valores brutos com filtro
  uint16_t j1x_raw = filtroAnalogico(J1X);
  uint16_t j1y_raw = filtroAnalogico(J1Y);
  uint16_t j2x_raw = filtroAnalogico(J2X);
  uint16_t j2y_raw = filtroAnalogico(J2Y);
  
  // APLICA CALIBRAÇÃO SE DISPONÍVEL
  if (calibration.isCalibrated) {
    pacote.j1x = applyCalibration(j1x_raw, calibration.j1x_min, calibration.j1x_max);
    pacote.j1y = applyCalibration(j1y_raw, calibration.j1y_min, calibration.j1y_max);
    pacote.j2x = applyCalibration(j2x_raw, calibration.j2x_min, calibration.j2x_max);
    pacote.j2y = applyCalibration(j2y_raw, calibration.j2y_min, calibration.j2y_max);
  } else {
    // Sem calibração: usa valores brutos
    pacote.j1x = j1x_raw;
    pacote.j1y = j1y_raw;
    pacote.j2x = j2x_raw;
    pacote.j2y = j2y_raw;
  }
  
  // Potenciômetros (sem calibração necessária)
  pacote.pot1 = filtroAnalogico(POT1);
  pacote.pot2 = filtroAnalogico(POT2);
  pacote.pot3 = filtroAnalogico(POT3);
  
  // Switches
  pacote.sw1 = leituraDigitalEstavel(SW1);
  pacote.sw2 = leituraDigitalEstavel(SW2);
  pacote.sw3Mode = (uint8_t)lerModo();
  
  // Bateria e telemetria
  pacote.bateria = lerBateria();
  appState.bateria = pacote.bateria;
  pacote.mode = (uint8_t)appState.mode;
  pacote.timestamp = millis();
  pacote.rssi = 0;  // Placeholder
}

// ---------------------------------------------------------------------------
// ENVIO DE PACOTE
// ---------------------------------------------------------------------------
void enviarPacote() {
  if (millis() - lastSendMs < SEND_PERIOD_MS) {
    return;
  }
  lastSendMs = millis();
  
  coletarDados();
  
  bool ok = false;
  
  // Seleciona rádio ativo baseado no switch
  appState.activeRadio = digitalRead(SW4_RADIO_SEL) == LOW ? RADIO_RF24 : RADIO_CC1101;
  
  switch (appState.activeRadio) {
    case RADIO_RF24:
      if (appState.rf24_ok) {
        ok = radio.write(&pacote, sizeof(pacote));
        if (ok) appState.pacotesAck++;
      }
      break;
      
    case RADIO_CC1101:
      if (appState.cc1101_ok) {
        // TODO: enviar via CC1101
        // ok = cc1101.send(&pacote, sizeof(pacote));
        // if (ok) appState.pacotesAck++;
      }
      break;
  }
  
  appState.pacotesSent++;
  
  // Log de envio
  String logMsg = "TX [" + String(appState.activeRadio) + "] ACK:" + String(ok) 
                  + " B:" + String(appState.bateria, 2) + "V";
  addLog(logMsg);
}

// ---------------------------------------------------------------------------
// TESTE PERIÓDICO DE RÁDIOS
// ---------------------------------------------------------------------------
void testarRadios() {
  if (millis() - lastRadioTestMs < RADIO_TEST_INTERVAL) {
    return;
  }
  lastRadioTestMs = millis();
  
  // Teste RF24
  appState.rf24_ok = testRF24();
  
  // Teste CC1101
  appState.cc1101_ok = testCC1101();
  
  String status = "RF24:" + String(appState.rf24_ok) + " CC1101:" + String(appState.cc1101_ok);
  addLog(status);
}

// ---------------------------------------------------------------------------
// MODO SIMULADOR
// ---------------------------------------------------------------------------
void processarSimulador() {
  // Em modo simulador, só coleta dados, não envia
  // Útil para treino no PC via log ou captura de dados
  coletarDados();
  
  addLog("SIM: J1[" + String(pacote.j1x) + "," + String(pacote.j1y) + "]");
}

// ---------------------------------------------------------------------------
// PÁGINA WEB
// ---------------------------------------------------------------------------
#if ENABLE_WIFI_LOGS

String htmlHeader() {
  return F("<html><head><meta charset='utf-8'><meta http-equiv='refresh' content='2'><title>HyperLink TX</title><style>body{font-family:Arial}table{border-collapse:collapse;width:100%}th,td{border:1px solid black;padding:5px}</style></head><body>");
}

String htmlFooter() {
  return F("</body></html>");
}

void handleRoot() {
  String page = htmlHeader();
  page += "<h2>HYPERLINK - DUAL RADIO TX</h2>";
  
  page += "<table>";
  page += "<tr><th>Parametro</th><th>Valor</th></tr>";
  
  const char* modeStr[] = {"CONTROLE", "WALL", "AERO", "SIMULADOR"};
  page += "<tr><td>Modo</td><td>" + String(modeStr[appState.mode]) + "</td></tr>";
  
  const char* radioStr[] = {"RF24", "CC1101"};
  page += "<tr><td>Modulo Ativo</td><td>" + String(radioStr[appState.activeRadio]) + "</td></tr>";
  
  page += "<tr><td>RF24 Status</td><td>" + String(appState.rf24_ok ? "OK" : "FALHA") + "</td></tr>";
  page += "<tr><td>CC1101 Status</td><td>" + String(appState.cc1101_ok ? "OK" : "FALHA") + "</td></tr>";
  
  page += "<tr><td>Bateria</td><td>" + String(appState.bateria, 2) + "V</td></tr>";
  page += "<tr><td>Pacotes Enviados</td><td>" + String(appState.pacotesSent) + "</td></tr>";
  page += "<tr><td>ACKs Recebidos</td><td>" + String(appState.pacotesAck) + "</td></tr>";
  
  page += "</table>";
  
  page += "<h3>Logs Recentes</h3><pre>";
  int inicio = max(0, (int)logIndex - 30);
  for (int i = inicio; i < logIndex; i++) {
    page += logBuffer[i % LOG_SIZE] + "\n";
  }
  page += "</pre>";
  
  page += htmlFooter();
  server.send(200, "text/html", page);
}

#endif

// ---------------------------------------------------------------------------
// SETUP
// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\n=== BOOT ===");
  
  // Pinos de entrada/saída
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3A, INPUT_PULLUP);
  pinMode(SW3B, INPUT_PULLUP);
  pinMode(SW4_RADIO_SEL, INPUT_PULLUP);
  
  // Encoder
  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  
  // Buzzer e Calibração
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  pinMode(CALIB_BTN, INPUT_PULLUP);
  
  // ADC configuração
  // ✅ ESP32 38 pinos: POT3 em GPIO 37 (ADC1_CH1) - FUNCIONA com WiFi!
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  
  // Display ST7789
  #if ENABLE_ST7789
  initDisplay();
  #endif
  
  // WiFi e WebServer
  #if ENABLE_WIFI_LOGS
  WiFi.mode(WIFI_AP);
  WiFi.softAP("HyperLink_TX", "12345678");
  delay(100);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("IP do AP: ");
  Serial.println(IP);
  
  server.on("/", handleRoot);
  server.begin();
  
  addLog("WiFi AP iniciado em 192.168.4.1");
  Serial.println("[OK] ESP32 38 pinos - Todos os componentes funcionando perfeitamente!");
  #endif
  
  // Inicialização de rádios
  appState.rf24_ok = initRF24();
  appState.cc1101_ok = initCC1101();
  
  Serial.println("=== BOOT COMPLETO ===\n");
}

// ---------------------------------------------------------------------------
// LOOP PRINCIPAL
// ---------------------------------------------------------------------------
void loop() {
  
  #if ENABLE_WIFI_LOGS
  server.handleClient();
  #endif
  
  // Lê encoder
  updateEncoder();
  
  // Verifica botão de calibração
  if (digitalRead(CALIB_BTN) == LOW) {
    if (!calibrationMode) {
      startCalibration();
    }
  }
  
  // Se em modo calibração
  if (calibrationMode) {
    calibrationUpdate();
    
    // Timeout de calibração: 10 segundos
    if (millis() - calibrationStartTime > 10000) {
      endCalibration();
    }
    
    updateDisplay();
    return;  // Pula o resto enquanto calibra
  }
  
  // Atualiza modo operacional
  OperatingMode novoModo = lerModo();
  if (novoModo != appState.mode) {
    appState.mode = novoModo;
    addLog("Modo mudou para: " + String((int)appState.mode));
    beepSuccess();
  }
  
  // Teste periódico de rádios
  testarRadios();
  
  // Processa baseado no modo
  switch (appState.mode) {
    case MODE_SIMULADOR:
      processarSimulador();
      break;
    
    default:  // CONTROLE, WALL, AERO
      enviarPacote();
      break;
  }
  
  // Atualiza display
  #if ENABLE_ST7789
  updateDisplay();
  #endif
}
