# 🔐 Locker Inteligente com Feedback Multissensorial

Projeto Final — CCM520 Internet das Coisas  
Centro Universitário FEI — 5º Semestre

---
<img width="1475" height="796" alt="image" src="https://github.com/user-attachments/assets/af91a9eb-f86d-40ec-a35e-51a2bce4911e" />

## 📋 Descrição

Sistema de controle de acesso para um compartimento seguro implementado com Arduino Uno. O projeto integra múltiplos periféricos para oferecer autenticação por senha em dois fatores (sequência de botões + potenciômetro), monitoramento de invasão por sensor de luz (LDR), feedback visual via display LCD 16x2, feedback sonoro via buzzer e controle de abertura via servomotor.

---

## 🎯 Funcionalidades

- **Autenticação em dois fatores:** senha por sequência de botões seguida de confirmação com potenciômetro
- **Bloqueio automático:** após 3 tentativas erradas o sistema trava e exige reset manual
- **Alarme de invasão:** sensor LDR detecta incidência de luz no interior do cofre e dispara alerta sonoro
- **Feedback visual:** display LCD guia o usuário em cada etapa do processo
- **Feedback sonoro:** buzzer emite sinais distintos para acesso liberado, erro de senha e invasão
- **Indicador LED:** LED sinaliza o estado do cofre (aceso = trancado, apagado = aberto)
- **Reset de emergência:** segurar o botão B3 por 2 segundos durante o bloqueio reinicia o sistema

---

## 🔧 Componentes de Hardware

| Componente | Quantidade | Pino Arduino |
|---|---|---|
| Arduino Uno | 1 | — |
| Display LCD 16x2 | 1 | D2, D3, D4, D5, D6, D7 |
| Servomotor | 1 | D11 |
| Buzzer | 1 | D12 |
| LED | 1 | D13 |
| Botão B1 | 1 | D8 |
| Botão B2 | 1 | D9 |
| Botão B3 (confirmar/fechar/reset) | 1 | D10 |
| Potenciômetro | 1 | A1 |
| Sensor LDR | 1 | A0 |
| Resistores, jumpers, protoboard | — | — |

---

## 🗺️ Diagrama de Conexões

```
Arduino Uno
├── D2  → LCD pino 7 (D7)
├── D3  → LCD pino 6 (D6)
├── D4  → LCD pino 5 (D5)
├── D5  → LCD pino 4 (D4)
├── D6  → LCD pino RS (Enable)
├── D7  → LCD pino EN (RS)
├── D8  → Botão B1 (INPUT_PULLUP)
├── D9  → Botão B2 (INPUT_PULLUP)
├── D10 → Botão B3 (INPUT_PULLUP)
├── D11 → Sinal do Servomotor
├── D12 → Buzzer (+)
├── D13 → LED (+ resistor 220Ω)
├── A0  → LDR (divisor de tensão com resistor)
└── A1  → Potenciômetro (pino central)
```

> Veja também a imagem do circuito montado no Tinkercad incluída neste repositório.

---

## 🤖 Máquina de Estados (FSM)

O firmware é estruturado como uma Máquina de Estados Finita com os seguintes estados:

```
                    ┌─────────────────────────────────────┐
                    │                                     │
              LDR detecta luz                     LDR detecta luz
                    │                                     │
                    ▼                                     │
  Inicialização ──► TRANCADO ──── B1/B2 digitados ──► DIGITAÇÃO
                    ▲                                     │
                    │                              B3 confirma
                    │                                     │
              fechar (B3)               ┌────────────┴────────────┐
                    │                   │                         │
                    │              Senha correta           Senha errada
                    │                   │                         │
                    │                   ▼                    (tentativas++)
                    │            AGUARDA POT                      │
                    │                   │                    3 erros?
                    │           Pot na posição                    │
                    │                   │                         ▼
                    └──── ABERTO ◄──────┘                   BLOQUEADO
                                                                  │
                                                        B3 segurado 2s
                                                                  │
                                                             TRANCADO
```

### Estados detalhados

**TRANCADO** — Estado inicial. LCD exibe "TRANCADO / Digite senha". LED aceso. LDR monitorado continuamente.

**DIGITAÇÃO** — Usuário pressiona B1 (dígito 1) ou B2 (dígito 2) para compor uma sequência de 3 dígitos. Cada pressionamento exibe `*` no LCD e emite um beep curto. B3 confirma a senha.

**AGUARDA POT** — Senha correta aceita. LCD solicita que o usuário gire o potenciômetro até o valor-alvo (700 ±100 na escala de 0–1023). Funciona como segundo fator de autenticação.

**ABERTO** — Cofre desbloqueado. Servo vai para 90°. LED apagado. LCD exibe "ABERTO / B3=Fechar". Pressionar B3 retorna ao estado TRANCADO.

**BLOQUEADO** — Ativado após 3 senhas erradas ou detecção de invasão por LDR. LCD exibe "BLOQUEADO / Segure B3". Segurar B3 por 2 segundos reinicia o sistema.

**INVASÃO** — Subestado do BLOQUEADO. Ativado quando LDR lê valor abaixo de 700 (luz detectada no interior). Emite 5 beeps e exibe "!! INVASAO !! / Luz detectada".

---

## 🔑 Lógica de Autenticação

### Fator 1 — Sequência de botões
A senha padrão é `1 2 1`, inserida pelos botões:
- **B1** → dígito `1`
- **B2** → dígito `2`
- **B3** → confirma a sequência de 3 dígitos

### Fator 2 — Potenciômetro
Após a senha correta, o usuário deve girar o potenciômetro até que a leitura analógica fique entre **600 e 800** (alvo: 700). Somente então o servo abre o cofre.

---

## 🚨 Sistema de Segurança

| Evento | Resposta do sistema |
|---|---|
| Senha errada (1ª ou 2ª vez) | Beep longo, exibe tentativas, aguarda nova entrada |
| 3 senhas erradas | Sistema bloqueado, LED aceso, exige reset manual |
| LDR < 700 (luz detectada) | Alarme com 5 beeps, tela de invasão, bloqueio total |
| Reset de emergência | Segurar B3 por 2 s no estado BLOQUEADO reinicia o sistema |

---

## 📦 Bibliotecas Utilizadas

- `LiquidCrystal.h` — Controle do display LCD (built-in Arduino IDE)
- `Servo.h` — Controle do servomotor (built-in Arduino IDE)

Não são necessárias instalações adicionais.

---

## ▶️ Como Reproduzir

1. Monte o circuito conforme o diagrama de conexões e a imagem do Tinkercad.
2. Abra o arquivo `locker.ino` na Arduino IDE (versão 1.8+ ou 2.x).
3. Selecione a placa **Arduino Uno** e a porta COM correspondente.
4. Clique em **Upload**.
5. Após o upload, o LCD exibirá "TRANCADO / Digite senha".

### Uso básico
1. Pressione **B1, B2, B1** (senha padrão `1-2-1`).
2. Gire o potenciômetro até o LCD indicar que o cofre abrirá.
3. O servo rotaciona 90°, indicando abertura.
4. Pressione **B3** para fechar o cofre.

---

## 📁 Estrutura do Repositório

```
/
├── locker.ino          # Código-fonte principal (comentado)
├── README.md           # Esta documentação
└── circuito.png        # Imagem do circuito montado no Tinkercad
```

---

## 📝 Relatório Técnico

### Objetivo
Desenvolver um sistema embarcado de controle de acesso seguro utilizando o Arduino Uno, integrando múltiplos periféricos e implementando uma lógica de estados robusta para simular um cofre eletrônico inteligente.

### Recursos Utilizados

**Hardware:** Arduino Uno, display LCD 16x2 (interface paralela 4 bits), servomotor SG90, buzzer passivo, LED, 3 botões de pressão com pull-up interno, potenciômetro 10kΩ, sensor LDR com divisor resistivo.

**Firmware:** Máquina de Estados Finita (FSM) implementada em C++ para Arduino, com lógica de debounce por software nos botões, leitura analógica com margem de tolerância para o potenciômetro e monitoramento contínuo do LDR.

### Funcionalidades Implementadas

O sistema implementa autenticação em dois fatores: a primeira camada consiste em uma sequência de 3 dígitos inserida via botões; a segunda camada exige o ajuste físico de um potenciômetro para um valor-alvo específico. Essa abordagem combinada eleva a segurança do acesso, pois requer tanto o conhecimento da senha quanto a posse do dispositivo físico calibrado.

O monitoramento de segurança é feito de forma contínua pelo sensor LDR, que detecta incidência de luz no interior do cofre — condição que indica tentativa de abertura forçada. Ao ser detectada, o sistema entra em estado de alarme com alertas visuais e sonoros.

O sistema de bloqueio após três tentativas erradas mitiga ataques de força bruta. O reset de emergência via pressionamento prolongado do B3 permite que um operador autorizado restaure o sistema sem necessidade de reinicialização por hardware.

### Desafios e Decisões de Projeto

A principal decisão de projeto foi a adoção da autenticação em dois fatores, que diferencia este sistema de implementações simples de cofre com senha. O uso do potenciômetro como segundo fator é uma solução criativa dentro das restrições dos componentes disponíveis.

O debounce por software nos botões foi necessário para evitar leituras múltiplas acidentais, implementado com uma verificação de estado LOW seguida de delay e espera pela liberação do botão.

A margem de tolerância de ±100 no potenciômetro (600–800) garante usabilidade sem comprometer excessivamente a segurança, pois o ajuste fino ainda é necessário.



## 👥 Autores

| Nome                    | RA     |
|-------------------------|--------|
| Vinicius Trivellato  | 22.223.022-9   |
| Gustavo Matias  |  22.124.007-0  |

---
