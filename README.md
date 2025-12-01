SunTracker

Autor: Martim Batista & Gonçalo Graça  
Versão:  1.0  
Duração do Projeto: 1 Novembro 2025 ate 15 Dezembro 2025  
Plataforma: ESP32-C6
Descrição
O SunTracker é um protótipo de rastreamento solar automático em dois eixos que utiliza sensores LDR, servomotores e um microcontrolador Arduino UNO. O sistema identifica a direção de máxima luminosidade e orienta um painel fotovoltaico de forma dinâmica, maximizando a captação de energia. Inclui ainda interface TFT, controlo remoto via WI-FI e máquinas de estados não bloqueantes.

---

1. Proposta

Este repositório contém:
- Código-fonte do sistema SunTracker  
- Documentação técnica  
- Plano de trabalho, entregáveis e milestones  
- Esquemas e materiais utilizados  
- Manual de instalação e utilização  
O objetivo do projeto é construir um sistema eficiente, replicável e de baixo custo para demonstrar conceitos de eletrónica, sensores, controlo automático e programação modular.

---

2. Características

- Rastreamento solar em dois eixos (horizontal + vertical)
- Sistema de leitura com 4 LDRs dispostos em cruz
- Processamento diferencial de luz para ajuste de orientação
- Controlo com servomotores de posicionamento
- Bluetooth para ativar/desativar o modo automático
- Interface:
  - TFT 160×128 → representação gráfica
- Alimentação externa 9–12 V para funcionamento autónomo
- Implementação com máquinas de estados não bloqueantes
- Estrutura mecânica ajustável (3D print / acrílico / madeira)

---

3. Pre requisitos

Hardware
- ESP32-C6
- 4 LDR  
- 4 resistores (10kΩ)  
- 2 servomotores (X/Y)  
- Módulo Bluetooth serial  
- TFT 160×128 
- Fonte externa 5V para os servos 
- Breadboard e cabos jumpers  
- Estrutura mecânica (molde)

Software
- Arduino IDE
- Drivers ESP32
- Bibliotecas:**WiFi.h**
  - WebServer.h
  - ESP32Servo.h
  - Adafruit_GFX.h
  - Adafruit_ST7735.h
  - SPI.h
 
4. Ligações Principais (ESP32-C6)

LDRs (entradas analógicas)
- LDR1 → A0  
- LDR2 → A1  
- LDR3 → A2  
- LDR4 → A3  

Servos
- Servo Horizontal  → D6  
- Servo Vertical    → D7  
(5V externos recomendados)

TFT ST7735
- CS  → D20  
- DC  → D19  
- RST → D18  
- SCK → D15 (FSPI)  
- MOSI→ D9 (FSPI)

Alimentação
- ESP32-C6 → USB-C  
- Servos → Fonte 5V dedicada
