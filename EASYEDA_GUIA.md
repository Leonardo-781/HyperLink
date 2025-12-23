# HyperLink TX - Guia Rápido EasyEDA

## 🎯 Passo a Passo Completo

### 1. Criar Projeto no EasyEDA

1. Acesse: https://easyeda.com/
2. Faça login ou crie conta
3. Clique em **"New Project"**
4. Nome: `HyperLink-TX`
5. Descrição: `Transmissor dual-radio ESP32 para drones`

---

### 2. Criar Esquemático

#### 2.1 Adicionar Componentes

Clique em **"Place" → "Component"** ou pressione `Shift+F`

**Biblioteca LCSC (já integrada):**

| Componente | Buscar por | Part Number LCSC |
|-----------|------------|------------------|
| ESP32-WROOM-32 | "ESP32-WROOM-32" | C82899 |
| AMS1117-3.3 | "AMS1117-3.3" | C6186 |
| Capacitor 100µF | "100uf 16v electrolytic" | C327499 |
| Capacitor 10µF | "10uf 16v electrolytic" | C19702 |
| Capacitor 100nF | "0.1uf 50v ceramic" | C49678 |
| Resistor 10kΩ | "10k 0805" ou 1/4W THT | C17414 (SMD) |
| Resistor 22kΩ | "22k 0805 1%" | C17935 |
| Diodo 1N5819 | "1N5819" | C8598 |
| LED 0805 vermelho | "LED red 0805" | C84256 |

**Componentes externos (símbolo genérico):**

| Componente | Símbolo | Pinos |
|-----------|---------|-------|
| NRF24L01 módulo | Generic IC 8-pin | 8 (VCC, GND, CE, CSN, SCK, MOSI, MISO, IRQ) |
| CC1101 módulo | Generic IC 8-pin | 8 (VCC, GND, CSN, SCK, MOSI, MISO, GDO0, GDO2) |
| ST7789 display | Generic IC 8-pin | 8 (VCC, GND, CS, DC, RST, SCK, MOSI, BL) |
| Joystick analógico | Potentiometer x2 | VCC, GND, VRx, VRy |
| Encoder rotativo | Generic 5-pin | CLK, DT, SW, VCC, GND |

#### 2.2 Conectar Componentes (Netlist)

Copie esta lista para fazer as conexões no esquemático:

```
ALIMENTAÇÃO:
- VBAT+ → Diodo D1 (anodo)
- Diodo D1 (catodo) → C1+ (100µF) → AMS1117 VIN
- AMS1117 GND → GND
- AMS1117 VOUT → C2+ (10µF) → VCC_3V3
- C1- → GND
- C2- → GND

DIVISOR BATERIA:
- VBAT+ → R1 (22kΩ) → GPIO15 → R2 (10kΩ) → GND
- GPIO15 → C3 (100nF) → GND

ESP32 CORE:
- ESP32 VCC (pino 1, 2) → VCC_3V3
- ESP32 VCC (pino 1, 2) → C4 (100nF) → GND
- ESP32 VCC (pino 1, 2) → C5 (10µF) → GND
- ESP32 GND (pino 15, 38) → GND
- ESP32 EN → R3 (10kΩ) → VCC_3V3
- ESP32 EN → SW_RESET → GND

SPI BUS:
- ESP32 GPIO18 → RF24.SCK, CC1101.SCK, ST7789.SCK
- ESP32 GPIO23 → RF24.MOSI, CC1101.MOSI, ST7789.MOSI
- ESP32 GPIO19 → RF24.MISO, CC1101.MISO

CHIP SELECTS:
- ESP32 GPIO17 → RF24.CE
- ESP32 GPIO16 → RF24.CSN
- ESP32 GPIO5 → CC1101.CSN
- ESP32 GPIO7 → CC1101.GDO0
- ESP32 GPIO12 → ST7789.CS
- ESP32 GPIO13 → ST7789.DC
- ESP32 GPIO26 → ST7789.RST

JOYSTICKS:
- Joy1.VCC → VCC_3V3
- Joy1.GND → GND
- Joy1.VRx → C6 (100nF) → GPIO32
- Joy1.VRy → C7 (100nF) → GPIO33
- Joy2.VCC → VCC_3V3
- Joy2.GND → GND
- Joy2.VRx → C8 (100nF) → GPIO34
- Joy2.VRy → C9 (100nF) → GPIO35

POTENCIÔMETROS:
- POT1.VCC → VCC_3V3
- POT1.GND → GND
- POT1.OUT → C10 (100nF) → GPIO36
- POT2.VCC → VCC_3V3
- POT2.GND → GND
- POT2.OUT → C11 (100nF) → GPIO39
- POT3.VCC → VCC_3V3
- POT3.GND → GND
- POT3.OUT → C12 (100nF) → GPIO37

SWITCHES:
- SW1 → GPIO14 (pullup interno)
- SW2 → GPIO21 (pullup interno)
- SW3A → GPIO4 (pullup interno)
- SW3B → GPIO2 (pullup interno)
- SW4_RADIO → GPIO22 (pullup interno)

ENCODER:
- Encoder.CLK → GPIO11
- Encoder.DT → GPIO8
- Encoder.SW → GPIO9 (pullup interno)
- Encoder.VCC → VCC_3V3
- Encoder.GND → GND

EXTRAS:
- Buzzer+ → GPIO25
- Buzzer- → GND
- BTN_CAL → GPIO27 (pullup interno)
- LED_TX+ → R4 (220Ω) → VCC_3V3
- LED_TX- → GPIO de escolha
```

---

### 3. Converter para PCB

1. Clique em **"Design" → "Convert Schematic to PCB"**
2. EasyEDA criará o layout automaticamente
3. Todos os componentes aparecerão desorganizados - é normal!

---

### 4. Layout da PCB

#### 4.1 Definir Dimensões

1. Clique em **"PCB Tools" → "Board Outline"**
2. Desenhe retângulo: **120mm x 80mm**
3. Adicione furos de montagem M3 nos 4 cantos (diâmetro 3.2mm)

#### 4.2 Posicionar Componentes

**Dica:** Use as camadas (layers) para organizar

**Zona Superior Esquerda - RF24:**
```
[RF24 Module]
[C desacoplamento 10µF + 100nF]
```

**Zona Superior Direita - CC1101:**
```
[CC1101 Module]
[C desacoplamento 10µF + 100nF]
```

**Centro - ESP32:**
```
    [Regulador AMS1117]
    [C entrada 100µF] [C saída 10µF]
         ↓
    [ESP32-WROOM-32]
    [C 100nF] [C 10µF]
```

**Inferior - Display:**
```
[Conector Display ST7789]
[C 10µF próximo]
```

**Laterais - Conectores:**
```
Esquerda:         Direita:
[Joy1 header]     [Joy2 header]
[POT1/2/3]        [Switches]
                  [Encoder]
```

#### 4.3 Roteamento (Routing)

**Larguras de trilha recomendadas:**
- VCC_3V3: 0.8mm (ou use plano de cobre - "Copper Area")
- GND: Plano de cobre (layer bottom)
- SPI (CLK, MOSI, MISO): 0.3mm
- Sinais ADC: 0.25mm
- Sinais digitais: 0.2mm

**Passo a passo:**

1. **GND primeiro:**
   - Layer Bottom: **"Copper Area" → "GND"**
   - Desenhe cobrindo toda a PCB (deixe margem 2mm)

2. **VCC_3V3:**
   - Layer Top: **"Copper Area" → "VCC_3V3"**
   - Desenhe em áreas onde há concentração de componentes

3. **Rotear sinais críticos manualmente:**
   - SPI CLK, MOSI, MISO (evitar ângulos retos, usar 45°)
   - ADC inputs (distantes de trilhas digitais rápidas)

4. **Auto-router para o resto:**
   - **"Route" → "Auto Router"**
   - Configurar:
     - Layer 1 (Top): Horizontal
     - Layer 2 (Bottom): Vertical
     - Track Width: 0.25mm
     - Clearance: 0.2mm
   - Clicar **"Run"**

5. **Revisar e ajustar:**
   - Verificar trilhas muito longas
   - Encurtar conexões críticas (RF, ADC)
   - Adicionar vias de GND perto de módulos RF

#### 4.4 Design Rules Check (DRC)

1. **"Tools" → "Design Rule Check"**
2. Configurar:
   - Track Width Min: 0.15mm
   - Clearance Min: 0.15mm
   - Hole Size Min: 0.3mm
3. Corrigir todos os erros (vermelho)

---

### 5. Adicionar Silkscreen (Textos)

1. **Layer: TopSilkLayer**
2. **"Text" → "Text"**
3. Adicionar:
   - `HyperLink TX v1.0`
   - `7.4V Battery Input`
   - `RF24` (próximo ao módulo)
   - `CC1101` (próximo ao módulo)
   - `Display`
   - Pinagem dos conectores (VCC, GND, etc)
   - Seu nome/logo

---

### 6. Fabricação (JLCPCB Integrado)

#### 6.1 Gerar Gerber

1. **"Fabrication" → "PCB Fabrication File (Gerber)"**
2. EasyEDA gera automaticamente
3. Download ZIP

#### 6.2 Encomendar (Integração JLCPCB)

1. **"Fabrication" → "Order at JLCPCB"**
2. Configurações recomendadas:
   - **PCB Qty:** 5 ou 10 peças
   - **Layers:** 2 (ou 4 se quiser profissional)
   - **Thickness:** 1.6mm
   - **Surface Finish:** HASL (mais barato) ou ENIG (melhor)
   - **Copper Weight:** 1 oz
   - **Color:** Verde (grátis) ou outra cor (+$5)
3. **Preço:** ~$2-5 (+ frete ~$15-25 Brasil)
4. **Prazo:** 15-30 dias

#### 6.3 SMT Assembly (Opcional - Avançado)

Se quiser que a JLCPCB monte os componentes SMD:

1. **"Fabrication" → "SMT Assembly"**
2. Selecionar lado (Top ou Bottom)
3. Fazer upload do BOM e CPL (Position file)
4. Custo adicional: ~$20-50 dependendo componentes

---

## 🎨 Dicas EasyEDA Específicas

### Atalhos Úteis
- **Shift+F**: Procurar componente
- **W**: Roteamento (wire/track)
- **C**: Copiar
- **M**: Mover
- **R**: Rotacionar
- **Del**: Deletar
- **Ctrl+Z**: Desfazer
- **Ctrl+S**: Salvar

### Biblioteca LCSC
- EasyEDA tem integração com LCSC (loja de componentes)
- Componentes da LCSC têm footprints prontos
- Muito útil para SMT Assembly

### Importar Componentes
Se não encontrar um componente:
1. **"Libraries" → "Search Parts"**
2. Buscar no banco público do EasyEDA
3. Importar para seu projeto

### Criar Footprint Customizado
Para módulos (RF24, CC1101, Display):
1. **"New" → "PCB Lib"**
2. Criar pads manualmente
3. Salvar e usar no projeto

---

## 📐 Checklist Visual Antes de Fabricar

### Esquemático
- [ ] Todos os componentes conectados
- [ ] VCC e GND corretos
- [ ] Sem avisos ou erros (ERC)
- [ ] Valores de componentes corretos (resistores, caps)

### PCB
- [ ] Dimensões 120x80mm (ou menor se conseguir)
- [ ] Furos de montagem M3 nos cantos
- [ ] Todos os pads conectados (sem "rat's nest")
- [ ] Plano de GND contínuo
- [ ] Capacitores próximos aos chips
- [ ] Trilhas SPI curtas e diretas
- [ ] Módulos RF afastados de fontes ruído
- [ ] Silkscreen legível
- [ ] DRC sem erros

### Fabricação
- [ ] Gerber gerado e verificado
- [ ] BOM conferida (se SMT)
- [ ] Preço e prazo aceitáveis
- [ ] Endereço de entrega correto

---

## 🚨 Erros Comuns no EasyEDA

### 1. "Unconnected Pad"
**Problema:** Pad não conectado  
**Solução:** Usar ferramenta Wire (W) para conectar

### 2. "Clearance Error"
**Problema:** Trilhas muito próximas  
**Solução:** Afastar trilhas ou diminuir largura

### 3. "Short Circuit"
**Problema:** VCC tocando GND  
**Solução:** Revisar planos de cobre, adicionar clearance

### 4. "Track Width Too Small"
**Problema:** Trilha muito fina  
**Solução:** Aumentar largura (mínimo 0.15mm)

### 5. "Hole Size Too Small"
**Problema:** Furo menor que 0.3mm  
**Solução:** Aumentar diâmetro dos furos

---

## 📹 Tutorial em Vídeo Recomendado

Procure no YouTube:
- "EasyEDA PCB tutorial for beginners"
- "EasyEDA auto router tutorial"
- "How to order PCB from JLCPCB EasyEDA"

Canal recomendado: **"Robert Feranec"** ou **"Phil's Lab"**

---

## 💡 Próximos Passos

1. ✅ Criar conta no EasyEDA
2. ⬜ Criar projeto "HyperLink-TX"
3. ⬜ Adicionar componentes no esquemático
4. ⬜ Conectar conforme netlist acima
5. ⬜ Converter para PCB
6. ⬜ Posicionar componentes
7. ⬜ Rotear trilhas (manual + auto)
8. ⬜ Verificar DRC
9. ⬜ Adicionar silkscreen
10. ⬜ Gerar Gerber
11. ⬜ Encomendar na JLCPCB

Tempo estimado: 3-5 horas (primeira vez)

---

## 🆘 Precisa de Ajuda?

Se ficar travado em alguma etapa:
1. Print screen do problema
2. Me mande a imagem
3. Posso guiar passo a passo!

Boa sorte! 🚀
