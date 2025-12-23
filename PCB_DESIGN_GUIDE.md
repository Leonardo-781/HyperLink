# HyperLink TX - Guia de Design PCB

## 1. Lista de Materiais (BOM)

### Microcontrolador
| Componente | Quantidade | Especificação | Notas |
|-----------|------------|---------------|-------|
| ESP32-WROOM-32 | 1 | 38 pinos, 4MB Flash | DevKit Module ou chip direto |
| Capacitor cerâmico | 2 | 100nF / 50V | Desacoplamento ESP32 |
| Capacitor eletrolítico | 1 | 10µF / 16V | Filtro VCC ESP32 |

### Regulação de Energia
| Componente | Quantidade | Especificação | Notas |
|-----------|------------|---------------|-------|
| Regulador LDO 3.3V | 1 | AMS1117-3.3, LD1117V33, ou similar (1A+) | Dropout < 1.2V |
| Capacitor entrada | 1 | 100µF / 16V | Eletrolitico, filtro bateria |
| Capacitor saída | 2 | 10µF / 16V (x2) | Paralelo na saída 3.3V |
| Diodo Schottky | 1 | 1N5819 ou SS14 (1A, 40V) | Proteção polaridade reversa |
| Conector JST-XH | 1 | 2 pinos | Entrada bateria 7.4V |

### Módulos RF
| Componente | Quantidade | Especificação | Notas |
|-----------|------------|---------------|-------|
| NRF24L01+ | 1 | 2.4GHz, +PA+LNA opcional | SMD ou módulo |
| CC1101 | 1 | 915MHz (ou 433MHz) | SMD ou módulo |
| Capacitor RF24 | 2 | 10µF + 100nF | Próximo ao módulo |
| Capacitor CC1101 | 2 | 10µF + 100nF | Próximo ao módulo |
| Antena RF24 | 1 | 2.4GHz PCB ou externa | Casamento 50Ω |
| Antena CC1101 | 1 | 915MHz PCB ou conector SMA | Casamento 50Ω |

### Display
| Componente | Quantidade | Especificação | Notas |
|-----------|------------|---------------|-------|
| Display ST7789 | 1 | 240x240, SPI, 1.3-1.54" | Com ou sem touchscreen |
| Capacitor | 1 | 10µF / 16V | Próximo ao display |
| Resistor backlight | 1 | 100Ω (opcional) | Se PWM controle |

### Entradas Analógicas
| Componente | Quantidade | Especificação | Notas |
|-----------|------------|---------------|-------|
| Joystick analógico 2-axis | 2 | 10kΩ, saída 0-3.3V | Com botão central (opcional) |
| Potenciômetro linear | 3 | 10kΩ | Montagem painel |
| Divisor tensão bateria | 2 | R1=22kΩ, R2=10kΩ (1%) | Precisão importante |
| Capacitor filtro ADC | 7 | 100nF | Um por entrada analógica |

### Entradas Digitais
| Componente | Quantidade | Especificação | Notas |
|-----------|------------|---------------|-------|
| Switch ON-OFF | 2 | SPST ou toggle | SW1, SW2 |
| Switch 4 posições | 1 | Rotativo ou DIP 2 bits | SW3 (modo) |
| Switch SPDT | 1 | Toggle 2 posições | SW4 (seletor rádio) |
| Encoder rotativo | 1 | Com botão integrado | 20-30 pulsos/rotação |
| Push button | 1 | Tátil 6x6mm | Calibração (GPIO 27) |
| Buzzer piezo | 1 | Passivo 3.3V | Feedback sonoro (GPIO 25) |

### Proteção e Diversos
| Componente | Quantidade | Especificação | Notas |
|-----------|------------|---------------|-------|
| LED indicador | 2-3 | 3mm vermelho/verde/azul | Power, TX, status |
| Resistor LED | 2-3 | 220Ω ou 330Ω | Limitação corrente |
| Conector programação | 1 | Header 6 pinos (FTDI) ou micro-USB | Se usar chip direto |
| Resistor pull-up | 2 | 10kΩ | EN e BOOT (se chip direto) |
| Botão RESET | 1 | Tátil 6x6mm | Se chip direto |
| Botão BOOT | 1 | Tátil 6x6mm | Programação (se chip direto) |

---

## 2. Considerações de Layout

### Camadas da PCB
**Recomendado: 4 camadas**
- Camada 1 (TOP): Componentes + sinais
- Camada 2: GND (plano de terra sólido)
- Camada 3: 3.3V (plano de alimentação)
- Camada 4 (BOTTOM): Sinais complementares

**Mínimo: 2 camadas**
- TOP: Componentes + sinais principais
- BOTTOM: GND fill + sinais secundários

### Larguras de Trilha
| Sinal | Largura | Motivo |
|-------|---------|--------|
| VCC 3.3V (principal) | 0.8-1.0mm | Corrente ~800mA total |
| GND | Plano ou 1.0mm+ | Retorno baixa impedância |
| VBAT 7.4V | 0.5-0.8mm | Corrente <300mA |
| SPI (CLK, MOSI, MISO) | 0.25-0.3mm | Alta frequência, controlada |
| Sinais digitais | 0.2-0.25mm | Padrão |
| ADC inputs | 0.2mm + guard ring | Minimizar ruído |

### Zonas Críticas

#### Zona 1: Alimentação (IMPORTANTE!)
```
BATERIA 7.4V
    ↓
[Diodo Schottky SS14] → [Cap 100µF] → [Regulador LDO]
                                           ↓
                                    [Cap 10µF x2] → VCC 3.3V
```
- Capacitor 100µF o mais próximo possível do regulador
- Dois capacitores 10µF em paralelo na saída
- Vias múltiplas (4-6) conectando planos GND

#### Zona 2: RF24L01 (CRÍTICO!)
- Capacitor 10µF + 100nF a menos de 5mm do módulo
- Trilha de antena: 50Ω impedância controlada (largura ~3mm em FR4 1.6mm)
- Afastar de: display, encoder, ESP32
- Plano de terra contínuo embaixo do módulo
- Sem trilhas cruzando embaixo da antena

#### Zona 3: CC1101 (CRÍTICO!)
- Mesmas considerações do RF24
- GDO0 deve ter trilha curta até GPIO 7
- Antena 915MHz precisa casamento de impedância (filtro LC pode ser necessário)

#### Zona 4: ESP32
- Capacitores 100nF próximos aos pinos VCC (pinos 1 e 2)
- Cristal 40MHz (se chip direto) com capacitores 10-22pF e ground guard
- Manter longe das zonas RF

#### Zona 5: ADC (Joysticks/Potenciômetros)
- Capacitor 100nF em cada entrada (filtro anti-aliasing)
- Trilhas curtas entre pots e ESP32
- Separar digitalmente de sinais SPI
- Routing diferencial se possível (par para cada joystick)

#### Zona 6: Display ST7789
- Cabo flat ou conector perpendicular
- Blindar trilhas SPI se cabo longo (>10cm)
- Capacitor 10µF próximo ao conector

### Pontos de Atenção
⚠️ **RF Crítico**: Nunca cruzar trilhas SPI com trilhas de antena  
⚠️ **ADC Sensível**: Manter distância mínima de 5mm de trilhas digitais rápidas  
⚠️ **SPI Compartilhado**: CLK deve ter impedância controlada (~50Ω)  
⚠️ **GND**: Usar vias de costura (stitching vias) a cada 5-10mm ao redor de zonas RF  

---

## 3. Esquemático Sugerido (Blocos)

### Bloco 1: Alimentação
```
[JST VBAT] → [Diodo] → [Cap 100µF] → [LDO 3.3V] → [Cap 10µF||10µF] → VCC
                 ↓
            [22kΩ] → [GPIO15] ADC (medição bateria)
                 ↓
            [10kΩ] → GND
```

### Bloco 2: ESP32 Core
```
ESP32-WROOM-32 (38 pinos)
    ├─ VCC (pinos 1, 2) → 3.3V + [100nF + 10µF]
    ├─ GND (pinos 15, 38, shield) → GND
    ├─ EN → [10kΩ] → VCC + [Botão RESET]
    └─ GPIO0 (BOOT) → [10kΩ] → VCC + [Botão BOOT]
```

### Bloco 3: SPI Bus
```
ESP32 GPIO18 (CLK) ──┬─→ RF24.SCK
                     ├─→ CC1101.SCK
                     └─→ ST7789.SCK

ESP32 GPIO23 (MOSI) ─┬─→ RF24.MOSI
                     ├─→ CC1101.MOSI
                     └─→ ST7789.MOSI

ESP32 GPIO19 (MISO) ─┬─→ RF24.MISO
                     └─→ CC1101.MISO
```

### Bloco 4: Chip Selects
```
GPIO 16 → RF24.CSN
GPIO 5  → CC1101.CSN
GPIO 12 → ST7789.CS
GPIO 13 → ST7789.DC
GPIO 26 → ST7789.RST
GPIO 17 → RF24.CE
GPIO 7  → CC1101.GDO0
```

### Bloco 5: Entradas Analógicas
```
[Joy1X] → [100nF] → GPIO32 (ADC1_CH4)
[Joy1Y] → [100nF] → GPIO33 (ADC1_CH5)
[Joy2X] → [100nF] → GPIO34 (ADC1_CH6)
[Joy2Y] → [100nF] → GPIO35 (ADC1_CH7)
[POT1]  → [100nF] → GPIO36 (ADC1_CH0)
[POT2]  → [100nF] → GPIO39 (ADC1_CH3)
[POT3]  → [100nF] → GPIO37 (ADC1_CH1)
```

### Bloco 6: Switches
```
[SW1] → [Opcional 10kΩ] → GPIO14 (pullup interno)
[SW2] → [Opcional 10kΩ] → GPIO21 (pullup interno)
[SW3A] → GPIO4  (pullup interno)
[SW3B] → GPIO2  (pullup interno)
[SW4] → GPIO22  (pullup interno)
```

### Bloco 7: Encoder
```
[Encoder CLK] → GPIO11
[Encoder DT]  → GPIO8
[Encoder BTN] → GPIO9 (pullup interno)
```

### Bloco 8: Extras
```
[Buzzer +] → GPIO25
[Buzzer -] → GND
[Cal BTN] → GPIO27 (pullup interno)
[LED TX] → [220Ω] → GPIO de escolha → GND
```

---

## 4. Software Recomendado

### Opção 1: KiCad (Gratuito, Open Source)
✅ Profissional e completo  
✅ Biblioteca ESP32 disponível  
✅ Suporte 3D viewer  
✅ Exporta Gerber para fabricação  
📥 Download: https://www.kicad.org/

### Opção 2: EasyEDA (Online, Gratuito)
✅ Interface simples  
✅ Integrado com JLCPCB/LCSC (componentes SMT)  
✅ Não precisa instalar  
⚠️ Precisa conta online  
📥 Link: https://easyeda.com/

### Opção 3: Altium Designer (Pago, Profissional)
✅ Mais avançado do mercado  
✅ Bibliotecas extensas  
⚠️ Licença cara (~$500+/ano)  

### Opção 4: EAGLE (Pago/Limitado Grátis)
✅ Versão gratuita limitada (100x80mm)  
✅ Adquirido pela Autodesk  
⚠️ Menos usado atualmente  

**Recomendação: KiCad ou EasyEDA**

---

## 5. Processo de Fabricação

### Passo 1: Criar Esquemático
- Usar blocos acima como referência
- Adicionar todos os componentes
- Conectar nets (redes)
- Verificar ERC (Electrical Rule Check)

### Passo 2: Layout PCB
- Importar do esquemático
- Posicionar componentes (ver zonas críticas)
- Rotear trilhas (manual ou auto-router)
- Adicionar planos de terra/VCC
- Verificar DRC (Design Rule Check)

### Passo 3: Gerar Gerber Files
- Exportar camadas: Top, Bottom, Silkscreen, Soldermask, Drill
- Formato Gerber RS-274X ou X2
- Incluir arquivo de furos (.drl)

### Passo 4: Escolher Fabricante

**Opções Econômicas (China - 15-30 dias):**
| Fabricante | Preço 10 PCBs | Qualidade | Site |
|-----------|---------------|-----------|------|
| JLCPCB | ~$2-5 + frete | Boa | jlcpcb.com |
| PCBWay | ~$5-10 + frete | Muito boa | pcbway.com |
| AllPCB | ~$5-10 + frete | Boa | allpcb.com |
| Elecrow | ~$5-10 + frete | Boa | elecrow.com |

**Opções Brasil (7-15 dias):**
| Fabricante | Preço 10 PCBs | Qualidade | Site |
|-----------|---------------|-----------|------|
| PCBWay Brasil | ~$50-100 | Muito boa | pcbway.com.br |
| CI Boards | Sob consulta | Boa | ciboards.com.br |

### Passo 5: Assembly (Montagem)

**Opção A: Montagem Manual**
- Comprar componentes na Mouser, Digikey, AliExpress
- Soldar com ferro de solda (componentes THT)
- Soldar com estação de ar quente (SMD)
- Tempo: 2-4 horas por placa

**Opção B: Assembly Service (PCBA)**
- JLCPCB oferece montagem SMT
- Você envia BOM + CPL (Component Placement List)
- Eles montam os componentes SMD
- Você solda apenas conectores THT
- Custo adicional: ~$20-50 dependendo da quantidade

---

## 6. Dimensões Sugeridas

### PCB Principal
- **Tamanho**: 120mm x 80mm (confortável para segurar)
- **Espessura**: 1.6mm (padrão)
- **Cobre**: 1oz (35µm) ou 2oz para trilhas de potência

### Furos de Montagem
- 4 cantos com furo M3 (3.2mm)
- Distância das bordas: 5mm

### Painéis de Controle
Considerar PCB secundária ou painel frontal para:
- Joysticks (podem ser externos com cabo flat)
- Potenciômetros
- Switches
- Display

---

## 7. Checklist Pré-Fabricação

### Verificações Elétricas
- [ ] Todos os VCC conectados a 3.3V
- [ ] Todos os GND conectados ao plano terra
- [ ] Capacitores de desacoplamento próximos aos ICs
- [ ] Divisor de tensão bateria calculado (22k/10k)
- [ ] Pull-ups nos pinos que precisam (EN, switches)
- [ ] Proteção polaridade reversa (diodo Schottky)

### Verificações Mecânicas
- [ ] Furos de montagem alinhados
- [ ] Conectores acessíveis
- [ ] Display visível
- [ ] Joysticks/pots em posição ergonômica
- [ ] Antenas têm espaço livre (sem obstruções metálicas próximas)

### Verificações de Fabricação
- [ ] Trilhas mínimas: 0.15mm ou mais (dependendo do fabricante)
- [ ] Espaçamentos mínimos: 0.15mm ou mais
- [ ] Furos mínimos: 0.3mm para vias, 0.8mm+ para THT
- [ ] Silkscreen legível (texto >0.8mm altura)
- [ ] Marcas de polaridade em diodos/capacitores

### Verificações RF
- [ ] Trilhas de antena: impedância 50Ω
- [ ] Módulos RF afastados de fontes de ruído
- [ ] Plano de terra contínuo sob módulos RF
- [ ] Comprimento de antena correto (λ/4 = ~31mm para 2.4GHz, ~82mm para 915MHz)

---

## 8. Teste e Debug

### Primeira Energização
1. **Antes de ligar:**
   - Verificar curto-circuitos com multímetro (VCC-GND > 1kΩ)
   - Conferir polaridade da bateria
   - Verificar solda fria em componentes críticos

2. **Ligar sem ESP32:**
   - Medir saída do regulador (deve ser 3.30V ±0.05V)
   - Verificar ripple (< 50mV com osciloscópio)

3. **Com ESP32:**
   - Verificar se ESP32 energiza (LED onboard)
   - Conectar UART e verificar boot messages
   - Testar WiFi AP (deve aparecer "HyperLink_TX")

4. **Teste de periféricos:**
   - Joysticks: ler ADC e verificar range 0-4095
   - Switches: verificar lógica (LOW quando ativado)
   - Display: deve inicializar e mostrar algo
   - RF24: verificar com `radio.isChipConnected()`

### Pontos de Medição
| Ponto | Tensão Esperada | Tolerância |
|-------|----------------|------------|
| Saída LDO | 3.30V | ±0.05V |
| ESP32 VCC | 3.30V | ±0.05V |
| Joystick centro | ~1.65V | ±0.2V |
| VBAT (divisor) | 2.42V @ 7.4V | ±0.1V |

---

## 9. Melhorias Futuras

### Versão 2.0 da PCB
- [ ] Adicionar slot para cartão SD (logs de voo)
- [ ] Conector para módulo GPS
- [ ] Porta micro-USB para programação + bateria USB
- [ ] IMU (MPU6050) para telemetria do controle
- [ ] Vibração háptica (motor vibracall)
- [ ] Proteção ESD em entradas críticas
- [ ] Conector para módulo Bluetooth (como backup)
- [ ] EEPROM externa I2C para calibração persistente

### Design Mecânico
- [ ] Case impresso em 3D com ergonomia gamer
- [ ] Suporte para alças (grips)
- [ ] Tampa traseira com acesso à bateria
- [ ] Proteção de borracha nas quinas

---

## 10. Recursos Úteis

### Datasheets
- ESP32-WROOM-32: https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf
- NRF24L01+: https://www.sparkfun.com/datasheets/Components/SMD/nRF24L01Pluss_Preliminary_Product_Specification_v1_0.pdf
- CC1101: https://www.ti.com/lit/ds/symlink/cc1101.pdf
- ST7789: https://www.displayfuture.com/Display/datasheet/controller/ST7789.pdf

### Tutoriais KiCad
- Documentação oficial: https://docs.kicad.org/
- Getting Started: https://www.youtube.com/watch?v=vaCVh2SAZY4
- PCB Design Tutorial: https://www.youtube.com/watch?v=5fvdxd0QhTw

### Calculadoras
- Impedância de trilha PCB: https://www.eeweb.com/tools/microstrip-impedance/
- Divisor de tensão: https://ohmslawcalculator.com/voltage-divider-calculator
- Comprimento antena: https://www.pasternack.com/t-calculator-quarter-wave-ground-plane.aspx

### Comunidades
- r/PrintedCircuitBoard (Reddit)
- KiCad Forums: https://forum.kicad.info/
- EEVblog Forums: https://www.eevblog.com/forum/

---

**Boa sorte com o projeto! 🚀**

Se tiver dúvidas específicas sobre alguma etapa, me avise!
