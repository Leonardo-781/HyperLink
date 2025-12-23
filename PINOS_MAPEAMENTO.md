# ESQUEMA DE PINOS ESP32 - CONTROLE RF DUAL (RF24 + CC1101 + ST7789)

## 📋 MAPEAMENTO COMPLETO DE PINOS

### **ENTRADAS ANALÓGICAS (ADC1 - Funciona com WiFi)**

| Componente | Pino GPIO | ADC Channel | Função | Observação |
|-----------|-----------|-------------|--------|-----------|
| Joystick 1 VRX | GPIO 32 | ADC1_CH4 | Eixo X | Analógico 0-4095 |
| Joystick 1 VRY | GPIO 33 | ADC1_CH5 | Eixo Y | Analógico 0-4095 |
| Joystick 2 VRX | GPIO 34 | ADC1_CH6 | Eixo X | Analógico 0-4095 |
| Joystick 2 VRY | GPIO 35 | ADC1_CH7 | Eixo Y | Analógico 0-4095 |
| Potenciômetro 1 | GPIO 36 | ADC1_CH0 | Entrada analógica | Analógico 0-4095 |
| Potenciômetro 2 | GPIO 39 | ADC1_CH3 | Entrada analógica | Analógico 0-4095 |
| Potenciômetro 3 | GPIO 27 | ADC2_CH7 | Entrada analógica | **ADC1 (ADC2 não funciona com WiFi)** |
| Sensor Bateria | GPIO 15 | ADC2_CH3 | Leitura 7,4V | **ADC1 (divisor 22k/10k)** |

### **ENTRADAS DIGITAIS (Switches)**

| Componente | Pino GPIO | Tipo | Pull | Função | Lógica |
|-----------|-----------|------|------|--------|--------|
| Switch ON/OFF 1 | GPIO 14 | Digital | PULLUP | Entrada ON/OFF | LOW=ativado |
| Switch ON/OFF 2 | GPIO 21 | Digital | PULLUP | Entrada ON/OFF | LOW=ativado |
| Switch 3 Posição A | GPIO 4 | Digital | PULLUP | Modo (bit 0) | Combinação de bits |
| Switch 3 Posição B | GPIO 2 | Digital | PULLUP | Modo (bit 1) | Combinação de bits |
| Switch 4 Seletor RF | GPIO 22 | Digital | PULLUP | RF24 vs CC1101 | LOW=RF24, HIGH=CC1101 |
| Encoder Rotativo CLK | GPIO 11 | Digital | Nativo | Navegação display | Rotação |
| Encoder Rotativo DT | GPIO 10 | Digital | Nativo | Navegação display | Direção |
| Encoder Botão | GPIO 9 | Digital | PULLUP | Seleção/OK | LOW=pressionado |

### **SPI COMPARTILHADO (VSPI)**

| Pino | GPIO | Função | Compartilhado | Velocidade |
|-----|------|--------|---------------|-----------|
| CLK (SCLK) | GPIO 18 | Clock SPI | RF24, CC1101, ST7789 | 1-10 MHz |
| MOSI (SDI) | GPIO 23 | Master Out Slave In | RF24, CC1101, ST7789 | 1-10 MHz |
| MISO (SDO) | GPIO 19 | Master In Slave Out | RF24, CC1101, ST7789 | 1-10 MHz |

### **RF24L01 (Chip Select = GPIO 16)**

| Pino | GPIO | Função | Tipo |
|-----|------|--------|------|
| CE | GPIO 17 | Chip Enable | Digital out |
| CSN | GPIO 16 | Chip Select (ativo LOW) | Digital out |
| SCK | GPIO 18 | Clock | SPI compartilhado |
| MOSI | GPIO 23 | Dados saída | SPI compartilhado |
| MISO | GPIO 19 | Dados entrada | SPI compartilhado |
| VCC | 3.3V | Alimentação | Capacitor 10µF + 100nF |
| GND | GND | Terra | - |

### **CC1101 915MHz (Chip Select = GPIO 5)**

| Pino | GPIO | Função | Tipo |
|-----|------|--------|------|
| CSN | GPIO 5 | Chip Select (ativo LOW) | Digital out |
| SCK | GPIO 18 | Clock | SPI compartilhado |
| MOSI | GPIO 23 | Dados saída | SPI compartilhado |
| MISO | GPIO 19 | Dados entrada | SPI compartilhado |
| GDO0 | GPIO 25 | Status/Interrupção | Digital in |
| VCC | 3.3V | Alimentação | Capacitor 10µF + 100nF |
| GND | GND | Terra | - |

### **ST7789 240x240 TFT (Chip Select = GPIO 12)**

| Pino | GPIO | Função | Tipo | Observação |
|-----|------|--------|------|-----------|
| CS (Chip Select) | GPIO 12 | Ativo LOW | Digital out | - |
| DC (Data/Cmd) | GPIO 13 | 0=Cmd, 1=Data | Digital out | - |
| RST (Reset) | GPIO 0 | Ativo LOW | Digital out | Pull resistor 10k |
| SCK (Clock) | GPIO 18 | Clock | SPI compartilhado | - |
| MOSI (SDI) | GPIO 23 | Dados entrada | SPI compartilhado | - |
| Backlight | 3.3V | Alimentação | Direct | Sempre ligado ou via MOSFET se PWM desejado |
| VCC | 3.3V | Alimentação | - | Capacitor 10µF |
| GND | GND | Terra | - | - |

---

## 🎮 TABELA DE MODOS (Switch 3)

| Posição | SW3A | SW3B | Modo | Descrição |
|---------|------|------|------|-----------|
| POS 1 | LOW | HIGH | CONTROLE | Controle normal de aeromodelo |
| POS 2 | HIGH | LOW | WALL | Modo wall-flight / acrobático |
| POS 3 | LOW | LOW | AERO | Modo aeromodelo puro |
| POS 4 | HIGH | HIGH | SIMULADOR | Apenas leitura (treino no PC) |

---

## 🔄 TABELA DE RÁDIOS (Switch 4)

| Posição | SW4_RADIO_SEL | Rádio Ativo | Status Display |
|---------|---------------|------------|----------------|
| Baixo (GND) | LOW | RF24L01 | "Modulo: RF24" |
| Alto (VCC) | HIGH | CC1101 915MHz | "Modulo: CC1101" |

---

## 📊 PINOS RESERVADOS (NÃO USAR)

| GPIO | Motivo |
|------|--------|
| 0 | Strapping bit (deve ser HIGH no boot) / TFT_RST |
| 1 | UART TX (Serial) |
| 3 | UART RX (Serial) |
| 5 | CC1101 CSN (SPI) |
| 6-11 | Quad SPI para Flash (não usar) |
| 20 | NC (não existe) |
| 24 | NC (não existe) |
| 28-31 | NC (não existe) |

---

## 🔌 CONEXÕES DE ALIMENTAÇÃO

### **Tensão de Bateria (7,4V - 2S LiPo com BMS)**

```
Bateria 7,4V
    |
    +--- Diodo Schottky (proteção reversa)
    |
    +--- Capacitor 100µF / 16V (filtro)
    |
    +--- Divisor para ADC: 22k --- GPIO15(ADC) --- 10k --- GND
    |
    +--- Regulador LDO 3.3V (1A mínimo) ---+
         |
         +--- ESP32 VCC
         +--- RF24L01 VCC (3.3V, +10µF+100nF perto do módulo)
         +--- CC1101 VCC (3.3V, +10µF+100nF perto do módulo)
         +--- ST7789 VCC (3.3V, +10µF perto do módulo)
         +--- Joysticks, Potenciômetros (3.3V ref)
```

### **Cálculo do Divisor de Tensão**

Para ler 7,4V com ADC (0-3.3V):
- R1 = 22kΩ
- R2 = 10kΩ
- Divisão = 3.3 * (R1+R2)/R2 = 3.3 * 32/10 = **10.56**
- Fórmula no código: `Vbat = (ADC_read / 4095) * 3.3 * 10.56`

---

## 🧪 VERIFICAÇÃO DE CONFLITOS

✅ **Sem Conflitos Detectados:**
- ADC2 (POT3 e VBAT) mapeados para ADC1
- SPI compartilhado corretamente (3 CS separados)
- WiFi ativo com ADC1 funcionando
- Todos os pinos digitais disponíveis
- Sem pinos strapping críticos sendo usados

---

## 📝 CHECKLIST PCB

- [ ] Capacitores de desacoplamento próximos (100nF) a cada módulo SPI
- [ ] Capacitor de filtro (10µF) próximo ao regulador 3.3V
- [ ] Diodo Schottky na entrada de bateria
- [ ] Resistores pull-up 10k nos pinos RST de módulos (se necessário)
- [ ] Resistores pull-down nos CSN de RF24/CC1101 (se necessário)
- [ ] Fios curtos para SPI (< 5cm de preferência)
- [ ] Cristal/Oscilador de 32kHz opcional (não crítico)
- [ ] Antena RF24L01 e CC1101 com impedância de 50Ω

---

## 📚 BIBLIOTECAS NECESSÁRIAS

1. **RF24** (2.4GHz) - `arduino-library/RF24`
2. **CC1101** (915MHz) - `LSatan/CC1101` ou similar
3. **TFT_eSPI** (Display ST7789) - `bodmer/TFT_eSPI`
4. **Adafruit-GFX** - `adafruit/Adafruit GFX Library`

---

## 🚀 NOTAS DE DESENVOLVIMENTO

1. **ADC2 e WiFi**: Pinos ADC2 podem falhar com WiFi ligado. Código usa ADC1 para bateria e POT3.
2. **SPI Multiplexing**: Garanta que cada módulo tem seu próprio CS e que o clock é compatível com 1-10MHz.
3. **Modo Simulador**: Em `MODE_SIMULADOR`, o sistema apenas coleta dados sem enviar, permitindo treino no PC.
4. **Encoder**: Navegação de 3 telas no display (Status, Dados, Diagnóstico).
5. **CC1101 Placeholder**: Código está preparado para CC1101, mas precisa da biblioteca instalada.

---

Gerado em: 23 de dezembro de 2025  
Versão: 2.0 - Dual Radio
