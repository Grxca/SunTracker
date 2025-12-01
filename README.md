SunTracker

Autor: Martim Batista & Gonçalo Graça  
Versão:  1.0  
Duração do Projeto: 1 Novembro 2025 ate 15 Dezembro 2025  

Descrição
O SunTracker é um protótipo de rastreamento solar automático em dois eixos que utiliza sensores LDR, servomotores e um microcontrolador Arduino UNO. O sistema identifica a direção de máxima luminosidade e orienta um painel fotovoltaico de forma dinâmica, maximizando a captação de energia. Inclui ainda interface LCD/TFT, controlo remoto via Bluetooth e máquinas de estados não bloqueantes.

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
- Arduino UNO  
- 4 LDR  
- 4 resistores (10kΩ)  
- 2 servomotores (X/Y)  
- Módulo Bluetooth serial  
- TFT 160×128 
- Fonte externa 9–12 V  
- Breadboard e cabos jumpers  
- Estrutura mecânica 

Software
- Arduino IDE
- Drivers para Arduino  
- Bibliotecas:
  - Servo.h (servomotores)
  - TFT_eSPI           (TFT 160x128)?
