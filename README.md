# 🔐 Locker Inteligente com Feedback Multissensorial

Projeto final da disciplina **CCM520 – Internet das Coisas**  
Centro Universitário FEI — 1º Semestre de 2026

---

## 📋 Descrição

Sistema de controle de acesso para um compartimento seguro implementado com Arduino Uno. O sistema exige dois fatores de autenticação: pressionar **3 botões na sequência correta** e posicionar o **potenciômetro no ângulo esperado**. O estado do cofre é comunicado por LCD, LEDs (verde/vermelho) e buzzer. A trava física é simulada por um servo motor.

---

## 🎯 Funcionalidades

- **Autenticação dupla:** sequência de 3 botões + posição correta do potenciômetro
- **Display LCD 16x2:** guia o usuário em cada etapa (aguardando senha, ajuste do ângulo, acesso liberado, erro)
- **Estado neutro:** LED vermelho aceso continuamente enquanto o cofre está trancado
- **Erro de senha:** LED vermelho pisca 3 vezes + buzzer apita (3 bipes graves) → sistema reinicia
- **Acesso concedido:** LED verde acende, servo gira para 90°, buzzer emite bipes de confirmação
- **Fechar cofre:** qualquer botão pressionado com o cofre aberto o trava novamente

---

## 🔄 Máquina de Estados (FSM)

```
        [TRANCADO]
            |
     Pressiona botões
            ↓
   [VALIDANDO SEQUÊNCIA]
            |
    Sequência correta?
        /        \
      Sim         Não
       |            |
[VALIDANDO POT]  [ERRO] → pisca LED + buzzer → TRANCADO
       |
 Ângulo correto?
     /      \
   Sim       Não (continua aguardando)
    |
 [ABERTO]
    |
 Pressiona botão
    |
 [TRANCADO]
```

| Estado              | LCD                        | LED Vermelho     | LED Verde | Servo | Buzzer          |
|---------------------|----------------------------|------------------|-----------|-------|-----------------|
| TRANCADO            | "Digite a senha"           | Aceso            | Apagado   | 0°    | —               |
| VALIDANDO SEQUÊNCIA | "Seq: X X X"               | Aceso            | Apagado   | 0°    | —               |
| ERRO DE SENHA       | "Senha errada!"            | Piscando (3x)    | Apagado   | 0°    | 3 bipes graves  |
| VALIDANDO POT       | "Ajuste o angulo / Val:XX" | Aceso            | Apagado   | 0°    | —               |
| ABERTO              | "Acesso Liberado!"         | Apagado          | Aceso     | 90°   | 2 bipes + longo |

---

## 🔧 Hardware utilizado

| Componente           | Quantidade | Função                                   |
|----------------------|------------|------------------------------------------|
| Arduino Uno          | 1          | Microcontrolador principal               |
| Display LCD 16x2     | 1          | Interface visual com o usuário           |
| Push buttons         | 3          | Entrada da sequência de senha            |
| Potenciômetro        | 1          | Segundo fator de autenticação (ângulo)   |
| Servo motor          | 1          | Trava física do compartimento            |
| LED verde            | 1          | Indicação de acesso concedido            |
| LED vermelho         | 1          | Estado neutro / indicação de erro        |
| Buzzer               | 1          | Feedback sonoro (sucesso e erro)         |
| Resistores 220Ω      | 2          | Proteção dos LEDs                        |
| Protoboard + jumpers | —          | Montagem do circuito                     |

---

## 🔌 Diagrama de Conexões

```
Arduino Uno
│
├── D2   → Botão 1 (pino + → D2, pino - → GND)
├── D3   → Botão 2
├── D4   → Botão 3
│
├── A0   → Potenciômetro (pino central; extremos em 5V e GND)
│
├── D5   → LCD D4
├── D6   → LCD D5
├── D7   → LCD D6
├── A1   → LCD D7
├── D12  → LCD RS
├── D13  → LCD EN
│         (LCD VSS → GND, VDD → 5V, V0 → potenciômetro de contraste, RW → GND)
│
├── D8   → LED Verde  (+ resistor 220Ω → GND)
├── D9   → LED Vermelho (+ resistor 220Ω → GND)
├── D10  → Buzzer (+ → D10, - → GND)
├── D11  → Servo motor (sinal PWM; VCC → 5V, GND → GND)
│
├── 5V   → VCC dos componentes
└── GND  → GND comum
```


---


## 🚀 Como usar

1. Ao ligar, o LCD exibe **"Digite a senha"** e o LED vermelho acende
2. Pressione os **3 botões na sequência correta** (padrão: B1 → B3 → B2)
3. Se errar, o LED pisca e o buzzer apita — o sistema reinicia automaticamente
4. Se acertar, o LCD pede para **ajustar o ângulo do potenciômetro** (o valor atual aparece no display)
5. Ao atingir o valor correto, o **LED verde acende**, o **servo abre a trava** e o **buzzer confirma**
6. Para trancar, pressione qualquer botão

---

## 👥 Autores

| Nome                    | RA     |
|-------------------------|--------|
| Vinicius Trivellato  | 22.223.022-9   |
| Gustavo Matias  |  22.2.22.22.2.  |

---
