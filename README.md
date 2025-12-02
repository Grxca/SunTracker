# 🌞 SunTracker – Rastreamento Solar Automático

**Autores:** Martim Batista & Gonçalo Graça  
**Versão:** 1.0  
**Duração:** 1 Novembro 2025 → 15 Dezembro 2025  
**Plataforma:** ESP32-C6 DevKit  

---

## 📖 Descrição

O **SunTracker** é um sistema de rastreamento solar em dois eixos que utiliza um ESP32-C6, 4 LDRs e dois servos para orientar um mini painel solar automaticamente.  
Inclui modo Automático, modo Manual via WebServer, interface TFT e máquinas de estado não bloqueantes.

---

## 📁 Proposta

Este repositório contém:

- Código-fonte do sistema  
- Documentação técnica  
- Esquemas do circuito  
- Planeamento e milestones  
- Manual de instalação e utilização  

Objetivo: desenvolver um sistema eficiente, modular e replicável para estudo de sensores, eletrónica e controlo.

---

## ⚙️ Características

- Rastreamento solar horizontal + vertical  
- Leitura de 4 LDRs para diferenças de luz  
- Servomotores com limites de segurança  
- WebServer Wi-Fi para controlo Manual  
- TFT ST7735 (160×128) para interface gráfica  
- Máquinas de estado não bloqueantes  
- Estrutura mecânica em 3D (impressão ou acrílico)

---

## 🧰 Pré-Requisitos

### 🔧 Hardware
- ESP32-C6  
- 4 × LDR  
- 4 × resistores 10 kΩ  
- 2 × servos X/Y  
- TFT ST7735  
- Fonte externa 5 V para servos  
- Breadboard + jumpers  
- Suporte mecânico 3D  

### 💻 Software
- Arduino IDE  
- Suporte ESP32 (Espressif)  
- Bibliotecas:
  - WiFi.h  
  - WebServer.h  
  - ESP32Servo.h  
  - Adafruit_GFX.h  
  - Adafruit_ST7735.h  
  - SPI.h  

---

## 🔌 Ligações Principais (ESP32-C6)

### LDRs
- LDR1 → 2  
- LDR2 → 3  
- LDR3 → 4  
- LDR4 → 5  

### Servos
- Servo Horizontal → D6  
- Servo Vertical → D7  

### TFT ST7735
- CS → D20  
- DC → D19  
- RST → D18  
- SCK → D15  
- MOSI → D9  

---

## 🛠️ 5. Instalação e Execução

### 5.1 Instalar suporte ESP32
Arduino IDE → Ferramentas → Gestor de Placas → ESP32 → Instalar

### 5.2 Selecionar placa
Ferramentas → Placa → ESP32 → ESP32-C6 Dev Module  
Ferramentas → USB CDC On Boot → Enabled

### 5.3 Instalar bibliotecas
- Adafruit GFX  
- Adafruit ST7735  
- ESP32Servo  
- WiFi  
- WebServer  
- SPI  

### 5.4 Fazer Upload
Ligar ESP32-C6 → Selecionar Porta → Upload

### 5.5 Aceder ao WebServer
O ESP32 cria um hotspot:

SSID: SunTracker  
Password: 12345678  
IP: 192.168.4.1  

Abrir no browser:  
**http://192.168.4.1**

---

## 🔄 6. Modos de Funcionamento

### 🟢 Modo Automático
- Leitura contínua dos 4 LDR  
- Cálculo das diferenças de luz  
- Movimento dos servos dentro dos limites  
- Alinhamento automático com a luz máxima  

### 🟠 Modo Manual
- Controlo via WebServer  
- Ajustes incrementais dos servos  

### 🔵 TFT
Mostra:
- LDRs  
- Ângulos  
- Estado atual  
- Informações operacionais

---

## 🧩 7. Máquinas de Estado

O sistema possui quatro máquinas de estado independentes:

### SM_Luz
- Leitura dos LDR  
- Processamento das diferenças  
- Cálculo da direção a mover  

### SM_Servos
- Movimento seguro dos servos  
- Aplicação de limites  
- Passo-a-passo não bloqueante  

### SM_Interface
- Atualização do TFT  
- Dados em tempo real  

### SM_WebServer
- Processamento de pedidos HTTP  
- Alteração de modo  
- Controlo manual

---

## 📂 8. Estrutura do Repositório

```
SunTracker/
├── src/
│   ├── main.ino
│   ├── sm_luz.cpp
│   ├── sm_servos.cpp
│   ├── sm_web.cpp
│   └── sm_tft.cpp
│
├── docs/
├── hardware/
├── images/
├── README.md
└── LICENSE
```

---

## 🧭 9. Desenvolvimento (Git)

### 🔹 Branch principal
`main` → versão estável

### 🔹 Criar alterações
```
git checkout -b feature/xxx
```

### 🔹 Commits
- Pequenos  
- Objetivos  
- Exemplo:
```
git commit -m "Atualiza leitura dos LDR"
```

### 🔹 Regras básicas
- Evitar commits diretos na main  
- PR recomendado  
- Sem ficheiros pesados  
- Código limpo e comentado


## 🔗 10. Repositório Git

Repositório oficial:  
**https://github.com/Grxca/SunTracker**

---

## 📜 11. Licença

Projeto distribuído sob **MIT License**.  
Consultar ficheiro `LICENSE`.

