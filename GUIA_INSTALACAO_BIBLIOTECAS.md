# HyperLink TX - Instalação das Bibliotecas

**Projeto:** HyperLink Dual-Radio Controller

## BIBLIOTECAS NECESSÁRIAS

Abra o gerenciador de bibliotecas: Tools → Manage Libraries (Ctrl+Shift+I)

Instale:
1. RF24 (autor TMRh20) - versão 1.4.5 ou superior
2. TFT_eSPI (autor Bodmer) - versão 2.5.0 ou superior
3. WiFi, WebServer e SPI já vêm com o suporte ESP32

---

## CONFIGURAR TFT_eSPI

Importante: copie o arquivo `User_Setup.h` do projeto para:

```UARIO]\Documents\Arduino\libraries\TFT_eSPI\User_Setup.h
macOS:   ~/Documents/Arduino/libraries/TFT_eSPI/User_Setup.h
Linux:   ~/Arduino/libraries/TFT_eSPI/User_Setup.h
```

Faça backup do original antes de substituir.

---

## CONFIGURAR A
## ✅ CONFIGURAR PLACA

No Arduino IDE: Tools → Board → "ESP32 Dev Module"

Configurações recomendadas:
- Upload Speed: 921600
- CPU Frequency: 240MHz
- Flash Size: 4MB
- Partition Scheme: Default 4MB

---

## COMPILAR O CÓDIGO

1. Abra o arquivo `Controle-RF.ino`
2. Clique em Sketch → Verify (ou Ctrl+R)
3. Espere aparecer "Compilation complete"

Se der erro "TFT_eSPI.h not found", o User_Setup.h não foi copiado corretamente ou a IDE precisa ser reiniciada.

---

## PROBLEMAS COMUNS

| Problema | Solução |
|----------|---------|
**Board 'esp32' not found**
- Precisa instalar o suporte ESP32 no Boards Manager
- Adicione esta URL em Files → Preferences → Additional Boards Manager URLs:
  `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`

**Unknown symbol ou erro de compilação**
- Reinicie o Arduino IDE completamente
- Verifique se todas as bibliotecas foram instaladas

**Display fica em branco no hardware**
- Confira se os pinos no User_Setup.h estão corretos
- Verifique conexões físicas e alimentação 3.3V

---

Próximos passos: fazer upload para o ESP32, testar no hardware e partir para o design da PCB.