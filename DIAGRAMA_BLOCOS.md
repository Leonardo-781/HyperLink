# DIAGRAMA DE BLOCOS - ESP32 CONTROLE RF DUAL

```
┌─────────────────────────────────────────────────────────────────┐
│                         ESP32 (30-pin)                          │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  JOYSTICKS & POTENCIÔMETROS (Analógicos ADC1)                 │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ GPIO32 (J1X)   ─ Joystick 1 Eixo X (ADC1_CH4)           │  │
│  │ GPIO33 (J1Y)   ─ Joystick 1 Eixo Y (ADC1_CH5)           │  │
│  │ GPIO34 (J2X)   ─ Joystick 2 Eixo X (ADC1_CH6)           │  │
│  │ GPIO35 (J2Y)   ─ Joystick 2 Eixo Y (ADC1_CH7)           │  │
│  │ GPIO36 (POT1)  ─ Potenciômetro 1 (ADC1_CH0)             │  │
│  │ GPIO39 (POT2)  ─ Potenciômetro 2 (ADC1_CH3)             │  │
│  │ GPIO27 (POT3)  ─ Potenciômetro 3 (ADC1 mudado!)         │  │
│  │ GPIO15 (VBAT)  ─ Sensor Bateria 7,4V (ADC1 mudado!)     │  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                 │
│  SWITCHES DIGITAIS                                             │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ GPIO14 (SW1)        ─ Switch ON/OFF 1                   │  │
│  │ GPIO21 (SW2)        ─ Switch ON/OFF 2 (mudado!)         │  │
│  │ GPIO4  (SW3A)       ─ Switch Modo bit A                 │  │
│  │ GPIO2  (SW3B)       ─ Switch Modo bit B                 │  │
│  │ GPIO22 (SW4)        ─ Seletor RF24 <-> CC1101           │  │
│  │                                                          │  │
│  │ Tabela Modo (SW3):                                       │  │
│  │ ┌─────────────────────────────────────────────────┐     │  │
│  │ │ A=L, B=H → CONTROLE   │ A=H, B=H → SIMULADOR   │     │  │
│  │ │ A=H, B=L → WALL       │ A=L, B=L → AERO        │     │  │
│  │ └─────────────────────────────────────────────────┘     │  │
│  │                                                          │  │
│  │ SW4: L → RF24L01 | H → CC1101                          │  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                 │
│  ENCODER ROTATIVO (Navegação Display)                         │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ GPIO11 (ENCODER_CLK) ─ Rotação                          │  │
│  │ GPIO10 (ENCODER_DT)  ─ Direção                          │  │
│  │ GPIO9  (ENCODER_SW)  ─ Botão/Select                     │  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                 │
│  SPI COMPARTILHADO (VSPI) ← Crítico!                          │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ GPIO18 (SCK)  ─ Clock    (RF24, CC1101, ST7789)         │  │
│  │ GPIO23 (MOSI) ─ Data OUT (RF24, CC1101, ST7789)         │  │
│  │ GPIO19 (MISO) ─ Data IN  (RF24, CC1101, ST7789)         │  │
│  │                                                          │  │
│  │ CHIP SELECT (cada módulo tem o seu):                    │  │
│  │ ├─ GPIO16 → RF24L01 CSN                                │  │
│  │ ├─ GPIO5  → CC1101 CSN                                 │  │
│  │ └─ GPIO12 → ST7789 CS                                  │  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘

┌──────────────┐         ┌──────────────┐         ┌──────────────┐
│   RF24L01    │         │   CC1101     │         │  ST7789 TFT  │
│  2.4 GHz     │         │  915 MHz     │         │  240x240     │
├──────────────┤         ├──────────────┤         ├──────────────┤
│              │         │              │         │              │
│ VCC → 3.3V   │         │ VCC → 3.3V   │         │ VCC → 3.3V   │
│ GND → GND    │         │ GND → GND    │         │ GND → GND    │
│              │         │              │         │              │
│ CSN ← 16     │         │ CSN ← 5      │         │ CS  ← 12     │
│ CE  ← 17     │         │ GDO0 ← 25    │         │ DC  ← 13     │
│              │         │              │         │ RST ← 0      │
│ SCK ← 18 ←──┼─────────┼──→ SCK       │         │ SCK ← 18     │
│ MOSI← 23 ←──┼─────────┼──→ MOSI      │         │ MOSI← 23     │
│ MISO→ 19 ──┼─────────┼──← MISO      │         │ MISO→ 19     │
│              │         │              │         │ (não usa)    │
└──────────────┘         └──────────────┘         └──────────────┘
     │                         │                        │
     └─────────────────────────┼────────────────────────┘
                    SPI COMPARTILHADO
                  Velocidade: 1-10 MHz


┌─────────────────────────────────────────────────────────────┐
│              FLUXO DE FUNCIONAMENTO                         │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  BOOT → Inicializa RF24 + CC1101 + Display ST7789         │
│         Cria WiFi AP "Controle_RF"                         │
│                                                             │
│  LOOP                                                       │
│  ├─ Lê Switch Modo (SW3) → define MODE                    │
│  ├─ Lê Switch Rádio (SW4) → define RADIO ATIVO            │
│  ├─ Coleta dados (joysticks, pots, switches)             │
│  ├─ Lê bateria (GPIO15 ADC)                              │
│  │                                                         │
│  ├─ SE MODE == SIMULADOR                                  │
│  │  └─ Log dados apenas (treino PC)                       │
│  │                                                         │
│  └─ SE MODE == CONTROLE/WALL/AERO                         │
│     ├─ Se RADIO == RF24 → Envia via RF24L01              │
│     │                                                      │
│     └─ Se RADIO == CC1101 → Envia via CC1101 915MHz       │
│                                                             │
│  DISPLAY (3 telas, navegável por encoder):               │
│  ├─ Tela 1: STATUS (modo, rádio, RF24/CC1101 OK/FALHA)  │
│  ├─ Tela 2: DADOS (joysticks, pots, switches)           │
│  └─ Tela 3: DIAGNÓSTICO (logs, taxa ACK, frame time)    │
│                                                             │
│  WEB (WiFi AP):                                            │
│  └─ http://192.168.4.1 → Dashboard com métricas          │
│                                                             │
└─────────────────────────────────────────────────────────────┘


┌──────────────────────────────────────────────────────────────┐
│         ALOCAÇÃO DE MEMÓRIA (Estimada)                       │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│ Struct AppState:        ~50 bytes                           │
│ Struct PacketRF:        ~30 bytes                           │
│ Log Buffer (100 x String): ~3-4 KB                          │
│ RF24 Object:            ~200 bytes                          │
│ TFT_eSPI Object:        ~1 KB                               │
│ WebServer:              ~5 KB                               │
│                                                              │
│ TOTAL APROXIMADO:       < 15 KB (ESP32 tem 520 KB SRAM)    │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

---

## ⚡ SEQUÊNCIA DE INICIALIZAÇÃO

```
1. Serial @ 115200 baud
2. Configura pinos (INPUT_PULLUP para switches, analog resolution)
3. Inicializa ST7789 TFT
   ├─ Mostra "INICIALIZANDO..."
   └─ Preparado para desenhar telas
4. Inicia WiFi AP "Controle_RF"
   ├─ IP: 192.168.4.1
   └─ Password: 12345678
5. Inicializa RF24L01
   ├─ Frequência: 2.4GHz
   ├─ Data Rate: 1 Mbps
   ├─ Retries: 5 vezes
   └─ Status → appState.rf24_ok
6. Inicializa CC1101 (placeholder)
   ├─ Frequência: 915MHz
   ├─ Status → appState.cc1101_ok (false por enquanto)
   └─ Aguardando biblioteca
7. Log "Sistema iniciado!"
8. LOOP começa
```

---

## 🧵 TIMING DO SISTEMA

| Evento | Período | Bloqueante |
|--------|---------|-----------|
| Leitura Analógica | Contínua (5x filtro = ~1ms) | Sim (filtro) |
| Envio de Pacote | 40ms (25 Hz) | Não (non-blocking) |
| Teste de Rádio | 2000ms (0.5 Hz) | Não |
| Atualização Display | 500ms (2 Hz) | Não |
| Web Server | Contínua | Não (assíncrono) |
| Encoder | Contínua | Não |

**Nota**: Sistema não usa `delay()` bloqueante no loop. Tudo é baseado em `millis()`.

---

## 🔌 LISTA DE COMPONENTES (PCB)

```
Microcontrolador:
  ☐ 1x ESP32 30-pin (ou 38-pin)

Rádios RF:
  ☐ 1x NRF24L01+ (2.4GHz)
  ☐ 1x CC1101 (915MHz)
  ☐ 2x Antena 2.4GHz / 915MHz

Entradas:
  ☐ 2x Joystick analógico (5V tolerante)
  ☐ 3x Potenciômetro (10k)
  ☐ 4x Microswitch (ou similar)
  ☐ 1x Encoder rotativo

Display:
  ☐ 1x ST7789 240x240 SPI

Sensores:
  ☐ 1x Divisor de tensão 22k/10k (bateria)

Alimentação:
  ☐ 1x Bateria 2S LiPo (7,4V)
  ☐ 1x BMS 2S
  ☐ 1x Regulador LDO 3.3V (1A)
  ☐ 1x Diodo Schottky (proteção reversa)
  ☐ Capacitores: 100µF, 10µF, 100nF (vários)

Conectores:
  ☐ 1x Conector JST PH 2.0 (bateria)
  ☐ 1x USB-C (programação/debug)
```

---

Diagrama gerado: 23/12/2025 - v2.0
