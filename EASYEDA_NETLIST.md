# HyperLink TX - Netlist Completa para EasyEDA

## 📋 Lista de Componentes (BOM para EasyEDA)

### Copie esta tabela no EasyEDA

| Designator | Component | Value | Package | LCSC Part # |
|-----------|-----------|-------|---------|-------------|
| U1 | ESP32-WROOM-32 | ESP32 | MODULE-38P | C82899 |
| U2 | AMS1117-3.3 | 3.3V 1A | SOT-223 | C6186 |
| D1 | Diodo Schottky | 1N5819 | DO-41 | C8598 |
| C1 | Cap Eletrolítico | 100µF 16V | 6.3x5mm | C327499 |
| C2, C3 | Cap Eletrolítico | 10µF 16V | 4x5mm | C19702 |
| C4-C14 | Cap Cerâmico | 100nF 50V | 0805 | C49678 |
| R1 | Resistor | 22kΩ 1% | 0805 | C17935 |
| R2, R3 | Resistor | 10kΩ | 0805 | C17414 |
| R4, R5, R6 | Resistor | 220Ω | 0805 | C17557 |
| LED1, LED2 | LED | Vermelho | 0805 | C84256 |
| J1 | Conector Bateria | JST-XH 2P | THT | C2316 |
| J2-J7 | Header Macho | 1x4 ou 1x5 | 2.54mm | C2337 |
| SW1 | Push Button | RESET | 6x6mm | C318884 |
| MOD1 | Módulo RF24 | NRF24L01 | Externo | - |
| MOD2 | Módulo CC1101 | 915MHz | Externo | - |
| MOD3 | Display | ST7789 | Externo | - |

---

## 🔌 Netlist Detalhada (Conexões)

### Rede 1: VBAT (Entrada Bateria 7.4V)

```
Net: VBAT
  J1.1 (Bateria +)
  D1.Anode
  R1.1
```

### Rede 2: VBAT_FILT (Após Diodo)

```
Net: VBAT_FILT
  D1.Cathode
  C1.+
  U2.VIN (AMS1117 pino 3)
```

### Rede 3: VCC_3V3 (Saída Regulador 3.3V)

```
Net: VCC_3V3
  U2.VOUT (AMS1117 pino 2)
  C2.+
  C3.+
  U1.VCC (ESP32 pino 1)
  U1.VCC (ESP32 pino 2)
  C4.1
  C5.1
  R3.1 (Pull-up EN)
  MOD1.VCC (RF24)
  MOD2.VCC (CC1101)
  MOD3.VCC (ST7789)
  J2.1 (Joy1 VCC)
  J3.1 (Joy2 VCC)
  J4.1 (POT1 VCC)
  J5.1 (POT2 VCC)
  J6.1 (POT3 VCC)
  J7.1 (Encoder VCC)
  R4.1, R5.1 (LEDs anodo)
```

### Rede 4: GND (Terra Global)

```
Net: GND
  J1.2 (Bateria -)
  C1.-
  U2.GND (AMS1117 pino 1)
  C2.-
  C3.-
  U1.GND (ESP32 pino 15)
  U1.GND (ESP32 pino 38)
  C4.2
  C5.2
  C6.2, C7.2, C8.2, C9.2 (ADC filters)
  C10.2, C11.2, C12.2, C13.2, C14.2
  R2.2
  MOD1.GND (RF24)
  MOD2.GND (CC1101)
  MOD3.GND (ST7789)
  J2.2 (Joy1 GND)
  J3.2 (Joy2 GND)
  J4.2 (POT1 GND)
  J5.2 (POT2 GND)
  J6.2 (POT3 GND)
  J7.2 (Encoder GND)
  LED1.Cathode
  LED2.Cathode
  SW1.2 (RESET button)
```

### Rede 5: EN (Enable ESP32)

```
Net: EN
  U1.EN (ESP32 pino 3)
  R3.2
  SW1.1
```

### Rede 6: GPIO15 (ADC Bateria)

```
Net: GPIO15_VBAT
  R1.2
  R2.1
  C13.1
  U1.GPIO15 (ESP32 pino 23)
```

### Rede 7-14: SPI Bus

```
Net: SPI_CLK
  U1.GPIO18 (ESP32 pino 30)
  MOD1.SCK (RF24)
  MOD2.SCK (CC1101)
  MOD3.SCK (ST7789)

Net: SPI_MOSI
  U1.GPIO23 (ESP32 pino 37)
  MOD1.MOSI (RF24)
  MOD2.MOSI (CC1101)
  MOD3.MOSI (ST7789)

Net: SPI_MISO
  U1.GPIO19 (ESP32 pino 31)
  MOD1.MISO (RF24)
  MOD2.MISO (CC1101)

Net: RF24_CE
  U1.GPIO17 (ESP32 pino 28)
  MOD1.CE

Net: RF24_CSN
  U1.GPIO16 (ESP32 pino 27)
  MOD1.CSN

Net: CC1101_CSN
  U1.GPIO5 (ESP32 pino 29)
  MOD2.CSN

Net: CC1101_GDO0
  U1.GPIO7 (ESP32 pino 8)
  MOD2.GDO0

Net: ST7789_CS
  U1.GPIO12 (ESP32 pino 14)
  MOD3.CS

Net: ST7789_DC
  U1.GPIO13 (ESP32 pino 16)
  MOD3.DC

Net: ST7789_RST
  U1.GPIO26 (ESP32 pino 11)
  MOD3.RST
```

### Rede 15-21: ADC Inputs (Joysticks)

```
Net: J1X
  J2.3 (Joy1 VRx)
  C6.1
  U1.GPIO32 (ESP32 pino 8)

Net: J1Y
  J2.4 (Joy1 VRy)
  C7.1
  U1.GPIO33 (ESP32 pino 9)

Net: J2X
  J3.3 (Joy2 VRx)
  C8.1
  U1.GPIO34 (ESP32 pino 6)

Net: J2Y
  J3.4 (Joy2 VRy)
  C9.1
  U1.GPIO35 (ESP32 pino 7)
```

### Rede 22-24: ADC Pots

```
Net: POT1
  J4.3 (POT1 wiper)
  C10.1
  U1.GPIO36 (ESP32 pino 4)

Net: POT2
  J5.3 (POT2 wiper)
  C11.1
  U1.GPIO39 (ESP32 pino 5)

Net: POT3
  J6.3 (POT3 wiper)
  C12.1
  U1.GPIO37 (ESP32 pino 1)
```

### Rede 25-30: Switches

```
Net: SW1_SIG
  U1.GPIO14 (ESP32 pino 13)
  [Switch externo]

Net: SW2_SIG
  U1.GPIO21 (ESP32 pino 33)
  [Switch externo]

Net: SW3A_SIG
  U1.GPIO4 (ESP32 pino 26)
  [Switch modo bit A]

Net: SW3B_SIG
  U1.GPIO2 (ESP32 pino 24)
  [Switch modo bit B]

Net: SW4_RADIO
  U1.GPIO22 (ESP32 pino 36)
  [Switch seletor rádio]
```

### Rede 31-33: Encoder

```
Net: ENC_CLK
  J7.3 (Encoder CLK)
  U1.GPIO11 (ESP32 pino 16)

Net: ENC_DT
  J7.4 (Encoder DT)
  U1.GPIO8 (ESP32 pino 12)

Net: ENC_SW
  J7.5 (Encoder SW)
  U1.GPIO9 (ESP32 pino 17)
```

### Rede 34-35: Extras

```
Net: BUZZER
  U1.GPIO25 (ESP32 pino 10)
  [Buzzer +]

Net: BTN_CAL
  U1.GPIO27 (ESP32 pino 12)
  [Botão calibração]

Net: LED_TX
  U1.GPIO (escolher livre)
  R4.2
  LED1.Anode

Net: LED_STATUS
  U1.GPIO (escolher livre)
  R5.2
  LED2.Anode
```

---

## 📊 Resumo de Conexões por GPIO

| GPIO | Função | Conecta a | Tipo |
|------|--------|-----------|------|
| GPIO32 | J1X | Joystick 1 VRx | ADC1_CH4 |
| GPIO33 | J1Y | Joystick 1 VRy | ADC1_CH5 |
| GPIO34 | J2X | Joystick 2 VRx | ADC1_CH6 |
| GPIO35 | J2Y | Joystick 2 VRy | ADC1_CH7 |
| GPIO36 | POT1 | Potenciômetro 1 | ADC1_CH0 |
| GPIO39 | POT2 | Potenciômetro 2 | ADC1_CH3 |
| GPIO37 | POT3 | Potenciômetro 3 | ADC1_CH1 |
| GPIO15 | VBAT | Divisor tensão | ADC2_CH3 |
| GPIO18 | SPI CLK | RF24, CC1101, ST7789 | SPI |
| GPIO23 | SPI MOSI | RF24, CC1101, ST7789 | SPI |
| GPIO19 | SPI MISO | RF24, CC1101 | SPI |
| GPIO17 | RF24 CE | NRF24L01 | Digital |
| GPIO16 | RF24 CSN | NRF24L01 | SPI CS |
| GPIO5 | CC1101 CSN | CC1101 | SPI CS |
| GPIO7 | CC1101 GDO0 | CC1101 | Digital |
| GPIO12 | ST7789 CS | Display | SPI CS |
| GPIO13 | ST7789 DC | Display | Digital |
| GPIO26 | ST7789 RST | Display | Digital |
| GPIO14 | SW1 | Switch 1 | Digital Input |
| GPIO21 | SW2 | Switch 2 | Digital Input |
| GPIO4 | SW3A | Modo bit A | Digital Input |
| GPIO2 | SW3B | Modo bit B | Digital Input |
| GPIO22 | SW4 | Seletor Rádio | Digital Input |
| GPIO11 | ENC CLK | Encoder | Digital Input |
| GPIO8 | ENC DT | Encoder | Digital Input |
| GPIO9 | ENC SW | Encoder botão | Digital Input |
| GPIO25 | BUZZER | Buzzer piezo | PWM Output |
| GPIO27 | BTN_CAL | Botão calibração | Digital Input |

---

## 🎨 Cores Sugeridas para Trilhas (EasyEDA)

No EasyEDA você pode colorir redes para facilitar:

1. Selecione uma net (ex: VCC_3V3)
2. Clique direito → Properties
3. Escolha cor:
   - **VCC_3V3**: Vermelho
   - **GND**: Preto
   - **SPI CLK/MOSI/MISO**: Azul
   - **ADC signals**: Verde
   - **Digital I/O**: Amarelo

---

## 📁 Salvar Este Arquivo

Use esta netlist como referência ao fazer as conexões no EasyEDA.  
Recomendo imprimir ou ter em segunda tela.

---

## ✅ Checklist de Verificação

Após conectar tudo no esquemático:

- [ ] VCC_3V3 conectado em todos os VCC dos módulos
- [ ] GND conectado em todos os GND
- [ ] SPI bus compartilhado (CLK, MOSI, MISO) entre 3 módulos
- [ ] Cada módulo SPI tem CS único (RF24=GPIO16, CC1101=GPIO5, ST7789=GPIO12)
- [ ] Todos os 7 ADC inputs têm capacitor 100nF para GND
- [ ] Divisor de tensão bateria (22k + 10k) conectado
- [ ] Diodo de proteção no VBAT
- [ ] Capacitores de desacoplamento (100nF + 10µF) no ESP32
- [ ] Encoder tem 3 sinais (CLK, DT, SW)
- [ ] Buzzer conectado a GPIO25

Bom trabalho! 🚀
