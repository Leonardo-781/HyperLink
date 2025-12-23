# 🔧 GUIA DE CALIBRAÇÃO E BUZZER

## ✅ O QUE FOI ADICIONADO

### **1. Buzzer Piezo (GPIO 25)**
- Feedback sonoro para ações importantes
- Diferentes sons para diferentes eventos

### **2. Botão de Calibração (GPIO 27)**
- Ativa/desativa modo de calibração
- Duração automática: 10 segundos

---

## 🎵 **SONS DO BUZZER**

| Situação | Som | Frequência | Duração |
|----------|-----|-----------|---------|
| Sucesso | 1 beep | 1000 Hz | 100ms |
| Erro | 2 beeps | 400 Hz | 200ms c/ pause |
| Aviso | 1 beep | 800 Hz | 150ms |
| Calibrando | Beeps | 1200 Hz | 50ms (repetido) |
| Modo trocado | 1 beep | 1000 Hz | 100ms |

---

## 📱 **COMO CALIBRAR OS JOYSTICKS**

### **Passo 1: Pressionar Botão**
```
1. Pressione o botão de calibração (GPIO 27)
2. Display muda para "CALIBRACAO EM ANDAMENTO"
3. Buzzer faz bip confirmando
4. Timer de 10 segundos começa
```

### **Passo 2: Mover Joysticks**
```
Enquanto o timer conta:

1. Mova o Joystick 1 para:
   - Canto superior esquerdo
   - Canto superior direito
   - Canto inferior esquerdo
   - Canto inferior direito
   - Centro

2. Repita o mesmo com o Joystick 2

3. O display mostra em tempo real:
   - Tempo decorrido
   - Valores mín/máx encontrados
   - Beeps a cada 500ms confirmando leitura
```

### **Passo 3: Calibração Completa**
```
Após 10 segundos (ou quando soltar o botão novamente):

1. Buzzer faz 2 beeps de confirmação
2. Display volta a mostrar diagnóstico
3. Log mostra valores calibrados
4. Joysticks estão prontos para uso

Exemplo de log:
"CALIBRAÇÃO CONCLUÍDA!"
"J1X: 100-3950"
"J1Y: 150-3900"
"J2X: 120-3980"
"J2Y: 140-3920"
```

---

## 📊 **INFORMAÇÕES NO DISPLAY**

### **Tela de Status (Página 1)**
```
STATUS
Modo: CONTROLE
Modulo: RF24
RF24: OK
CC1101: FALHA
Bateria: 7.40V
Enviados: 1234
ACK: 1100
Calibrado: SIM        ← Nova linha
```

### **Tela de Diagnóstico (Página 3) - Durante Calibração**
```
CALIBRACAO EM ANDAMENTO
Tempo: 5s

MOVIMENTE OS JOYSTICKS
ao máximo em todas
as direções!

J1X min/max:
150 / 3950

J2X min/max:
200 / 3980
```

### **Tela de Diagnóstico (Página 3) - Normal**
```
DIAGNOSTICO
Frame: 42ms
Taxa ACK: 95.5%
Calibrado: SIM          ← Nova linha
Ultimos Logs:
LOG: TX [0] ACK:1 B:...
...
```

---

## 🎮 **ESTRUTURA DE DADOS DE CALIBRAÇÃO**

```cpp
struct CalibrationData {
  uint16_t j1x_min, j1x_max;  // Joystick 1 X range
  uint16_t j1y_min, j1y_max;  // Joystick 1 Y range
  uint16_t j2x_min, j2x_max;  // Joystick 2 X range
  uint16_t j2y_min, j2y_max;  // Joystick 2 Y range
  bool isCalibrated;          // Flag de conclusão
};
```

---

## 🔐 **FUNÇÕES DISPONÍVEIS**

### **Buzzer**
```cpp
beep(1000, 100);      // 1kHz durante 100ms
beepSuccess();        // Som de sucesso (1000Hz)
beepError();          // Som de erro (2 beeps)
beepWarning();        // Som de aviso (800Hz)
beepCalibrating();    // Bip curto durante calibração
```

### **Calibração**
```cpp
startCalibration();   // Inicia modo de calibração
calibrationUpdate();  // Atualiza valores (chamado no loop)
endCalibration();     // Finaliza e salva
applyCalibration(...) // Aplica calibração aos valores lidos
```

---

## 💡 **CASOS DE USO**

### **Calibração Inicial (Primeira Vez)**
```
1. Programar ESP32
2. Liga o controle
3. Pressione botão de calibração
4. Mova os joysticks por 10 segundos
5. Calibração salva automaticamente
6. Pronto para voar!
```

### **Recalibragem (Joystick Drift)**
```
1. Display mostrando valores estranhos?
2. Pressione botão de calibração novamente
3. Repita processo
4. Valores antigos são sobrescritos
```

### **Verificar Status**
```
1. Vá para Tela 1 (Status)
2. Veja "Calibrado: SIM ou NAO"
3. Se NAO, execute calibração
```

---

## 🚨 **AVISOS E LIMITAÇÕES**

### **Tempo Limite**
- A calibração automática encerra após **10 segundos**
- Se pressionar o botão novamente, **aborta e salva** imediatamente

### **Valores Padrão**
- Se não calibrado: usa full range (0-4095)
- Joysticks não serão mapeados corretamente
- Recomenda-se calibrar sempre na primeira vez

### **Memória**
- Dados de calibração são mantidos na **RAM**
- **NÃO são salvos em EEPROM** (perdidos ao desligar)
- Para persistência, seria necessário adicionar EEPROM

---

## 📝 **COMPONENTES NECESSÁRIOS**

### **Buzzer Piezo**
- Tipo: Passivo (sem oscilador interno)
- Frequência: 1-20 kHz
- Tensão: 3.3V-5V
- Pinos: 2 (+ e -)
- Custo: ~R$2-5

**Conexão:**
```
GPIO 25 (Buzzzer +) ─┐
                      ├─ Buzzer
      GND (Buzzer -) ─┘
```

### **Botão de Calibração**
- Tipo: Momentâneo (tactile button)
- Contatos: 2 ou 4 (use 2)
- Resistência: até 100mA
- Custo: ~R$1-2

**Conexão:**
```
GPIO 27 ─┐
         ├─ Botão
  GND ──┘
```

**Circuito no Esquemático:**
```
       +3.3V
         │
        10k
         │
    ─────┼──────┬─────── GPIO 27
         │      │
       Botão    ↓
         │     GND
        GND
```

---

## 🧪 **TESTE DE FUNCIONAMENTO**

### **Teste 1: Buzzer**
```cpp
// No loop, adicione para testar:
beepSuccess();   // Deve fazer "bip" de 1000Hz
delay(500);
beepError();     // Deve fazer 2 "beeps" de 400Hz
delay(500);
beepWarning();   // Deve fazer "bip" de 800Hz
```

### **Teste 2: Botão de Calibração**
```cpp
// Serial debug no setup:
Serial.println(digitalRead(CALIB_BTN));  // Deve ser HIGH (não pressionado)

// Pressione botão:
// Serial deve mostrar LOW

// Solte:
// Serial deve voltar a HIGH
```

### **Teste 3: Calibração Completa**
```
1. Serial Monitor aberto
2. Pressione botão de calibração
3. Veja "CALIBRAÇÃO INICIADA"
4. Mova joysticks por 10 segundos
5. Veja "CALIBRAÇÃO CONCLUÍDA"
6. Log mostra valores (J1X: 100-3950, etc)
```

---

## 📋 **CHECKLIST**

- ✅ Buzzer conectado em GPIO 25
- ✅ Botão de calibração conectado em GPIO 27
- ✅ Ambos com GND apropriado
- ✅ Buzzer passivo (não ativo)
- ✅ Botão com pull-up interno ativado
- ✅ Código compilado e programado
- ✅ Serial Monitor mostrando logs
- ✅ Buzzer fazendo sons ao ligar
- ✅ Botão respondendo ao pressionar
- ✅ Calibração funcionando

---

Sucesso! 🎉

