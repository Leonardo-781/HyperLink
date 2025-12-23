# HyperLink TX

Transmissor dual-radio profissional para controle remoto de drones e aeromodelos.

![Status](https://img.shields.io/badge/status-ready%20for%20PCB-green)
![Version](https://img.shields.io/badge/version-2.1-blue)
![Platform](https://img.shields.io/badge/platform-ESP32-orange)

## Características

- **Dual-Radio:** RF24L01 (2.4GHz) + CC1101 (915MHz) com seleção em tempo real
- **Display:** ST7789 240x240 TFT navegável por encoder
- **Modos:** Controle | Wall | Aeromodelo | Simulador
- **Calibração:** Sistema automático de joysticks com persistência
- **WiFi:** Access Point com dashboard web em 192.168.4.1
- **Telemetria:** Monitoramento de bateria, RSSI, taxa de ACK

## Hardware

- **MCU:** ESP32 Dev Module (38 pinos)
- **Pinos utilizados:** 27/30 GPIOs
- **ADC:** 8/8 canais ADC1 (compatível com WiFi)
- **Alimentação:** 7.4V 2S LiPo com BMS
- **Entradas:** 2 joysticks, 3 potenciômetros, 4 switches, encoder rotativo, botão calibração
- **Saídas:** Display TFT, buzzer piezo, 2 módulos RF

## Instalação

### 1. Bibliotecas Necessárias

Abra o Arduino IDE e instale via Library Manager (Ctrl+Shift+I):

- **RF24** (TMRh20) - versão 1.4.5+
- **TFT_eSPI** (Bodmer) - versão 2.5.0+
- WiFi, WebServer, SPI (built-in ESP32)

### 2. Configurar TFT_eSPI

Copie o arquivo `User_Setup.h` do projeto para:
```
Arduino/libraries/TFT_eSPI/User_Setup.h
```

### 3. Configurar Placa

No Arduino IDE:
- Board: "ESP32 Dev Module"
- Upload Speed: 921600
- CPU Frequency: 240MHz
- Flash Size: 4MB

### 4. Compilar e Upload

```bash
# Abra Controle-RF.ino no Arduino IDE
# Sketch → Verify (Ctrl+R)
# Sketch → Upload (Ctrl+U)
```

## Uso

### Calibração de Joysticks

1. Pressione o botão de calibração (GPIO 27)
2. Mova os joysticks para todos os extremos
3. Aguarde 10 segundos ou solte o botão
4. Dois beeps confirmam calibração concluída

### Navegação no Display

- Gire o encoder para trocar de página (3 páginas)
- Página 1: Status geral
- Página 2: Dados dos sensores
- Página 3: Diagnóstico e logs

### Dashboard Web

Conecte-se ao WiFi "HyperLink_TX" (senha: 12345678) e acesse:
```
http://192.168.4.1
```

## Mapeamento de Pinos

### Joysticks (ADC1)
- J1X: GPIO 32 (ADC1_CH4)
- J1Y: GPIO 33 (ADC1_CH5)
- J2X: GPIO 34 (ADC1_CH6)
- J2Y: GPIO 35 (ADC1_CH7)

### Potenciômetros (ADC1)
- POT1: GPIO 36 (ADC1_CH0)
- POT2: GPIO 39 (ADC1_CH3)
- POT3: GPIO 37 (ADC1_CH1)

### SPI (Compartilhado)
- SCLK: GPIO 18
- MOSI: GPIO 23
- MISO: GPIO 19

### Módulos RF
- **RF24:** CE=17, CSN=16
- **CC1101:** CSN=5, GDO0=7

### Display ST7789
- CS=12, DC=13, RST=26

Veja [PINOS_MAPEAMENTO_38P.md](PINOS_MAPEAMENTO_38P.md) para detalhes completos.

## Estrutura do Projeto

```
HyperLink/
├── Controle-RF.ino              # Código principal
├── User_Setup.h                 # Config TFT_eSPI
├── README.md                    # Este arquivo
├── RELATORIO_VALIDACAO_COMPLETO.md
├── SUMARIO_EXECUTIVO.md
├── GUIA_INSTALACAO_BIBLIOTECAS.md
├── PINOS_MAPEAMENTO_38P.md
└── VERIFICACAO_FINAL.md
```

## Especificações Técnicas

| Item | Especificação |
|------|---------------|
| Linhas de código | 820 |
| Funções | 22 |
| Taxa de atualização | 25Hz (40ms) |
| Update display | 2Hz (500ms) |
| Teste de rádios | 0.5Hz (2s) |
| Tamanho pacote RF | ~30 bytes |
| Consumo estimado | ~300mA @ 3.3V |

## Roadmap

- [x] Código base funcional
- [x] Calibração de joysticks
- [x] Sistema dual-radio
- [x] Interface display 3 páginas
- [x] WiFi dashboard
- [ ] Implementação CC1101 completa
- [ ] Calibração persistente (EEPROM)
- [ ] Design PCB
- [ ] Case 3D impresso

## Contribuindo

Pull requests são bem-vindos. Para mudanças maiores, abra uma issue primeiro.

## Licença

[MIT](https://choosealicense.com/licenses/mit/)

## Autor

Leonardo - (https://github.com/Leonardo-781)

## Agradecimentos

- TMRh20 pela biblioteca RF24
- Bodmer pela biblioteca TFT_eSPI
- Comunidade ESP32
