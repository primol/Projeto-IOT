#include <LiquidCrystal.h>
#include <Servo.h>

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
Servo servo;

// ---------------- Pinos ----------------
const int b1 = 8;
const int b2 = 9;
const int b3 = 10;

const int servoPin = 11;
const int buzzer = 12;
const int led = 13;
const int pot = A1; 
const int ldr = A0; 

// ---------------- Senha ----------------
int senha[3] = {1, 2, 1};
int entrada[3];
int pos = 0;

// ---------------- Potenciômetro --------
const int POT_ALVO = 700;  
const int POT_MARGEM = 100; 

int tentativas = 0;
bool aberto = false;
bool bloqueado = false;
bool aguardaPot = false; 
int ldrValue = 0;

// FUNCAO BOTAO

bool clicou(int botao) {
  if (digitalRead(botao) == LOW) {
    delay(50);
    while (digitalRead(botao) == LOW);
    return true;
  }
  return false;
}


// BUZZER

void beep(int tempo) {
  digitalWrite(buzzer, HIGH);
  delay(tempo);
  digitalWrite(buzzer, LOW);
}


void abrirCofre() {
  servo.write(90);
  aberto = true;
  aguardaPot = false;

  digitalWrite(led, LOW);

  lcd.clear();
  lcd.print("ACESSO LIBERADO");

  beep(150); delay(100);
  beep(150);
}


void fecharCofre() {
  servo.write(0);
  aberto = false;

  digitalWrite(led, HIGH);

  lcd.clear();
  lcd.print("TRANCADO");
  lcd.setCursor(0, 1);
  lcd.print("Digite senha");

  pos = 0;
}


void setup() {
  lcd.begin(16, 2);

  pinMode(b1, INPUT_PULLUP);
  pinMode(b2, INPUT_PULLUP);
  pinMode(b3, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);

  servo.attach(servoPin);
  servo.write(0);

  digitalWrite(led, HIGH);

  lcd.print("TRANCADO");
  lcd.setCursor(0, 1);
  lcd.print("Digite senha");


}


void loop() {

  // INVASAO 
  if (!bloqueado) {
    ldrValue = analogRead(ldr);
    if (ldrValue < 700) {
      bloqueado = true;
      tentativas = 0;
      pos = 0;
      aguardaPot = false;

      digitalWrite(led, HIGH);

      lcd.clear();
      lcd.print("!! INVASAO !!");
      lcd.setCursor(0, 1);
      lcd.print("Luz detectada");

      for (int i = 0; i < 5; i++) {
        beep(150); delay(100);
      }
      return;
    }
  }

  
  // BLOQUEADO
 
  if (bloqueado) {
    lcd.setCursor(0, 0);
    lcd.print(" BLOQUEADO");
    lcd.setCursor(0, 1);
    lcd.print(" Segure B3   ");

    digitalWrite(led, HIGH);

    if (digitalRead(b3) == LOW) {
      delay(2000);
      if (digitalRead(b3) == LOW) {
        bloqueado = false;
        tentativas = 0;

        lcd.clear();
        lcd.print("RESETADO");
        delay(1000);
        fecharCofre();
      }
    }
    return;
  }

  
  // ABERTO
  
  if (aberto) {
    lcd.setCursor(0, 0);
    lcd.print("ABERTO ");
    lcd.setCursor(0, 1);
    lcd.print("B3=Fechar ");

    digitalWrite(led, LOW);

    if (clicou(b3)) {
      lcd.clear();
      lcd.print("Fechando");
      delay(500);
      fecharCofre();
    }
    return;
  }

  //  POTENCIOMETRO
  // 
  if (aguardaPot) {
    // Verifica invasao tambem durante espera do pot
    {
      ldrValue = analogRead(ldr);
      if (ldrValue < 800) {
        bloqueado = false;
        aguardaPot = false;
        pos = 0;
        digitalWrite(led, HIGH);
        lcd.clear();
        lcd.print("!! INVASAO !!");
        lcd.setCursor(0, 1);
        lcd.print("Luz detectada");
        for (int i = 0; i < 5; i++) {
          beep(150); delay(100);
        }
        return;
      }
    }

    int valorPot = analogRead(pot);

    lcd.setCursor(0, 0);
    lcd.print("Gire o pot");

    if (valorPot >= POT_ALVO - POT_MARGEM &&
        valorPot <= POT_ALVO + POT_MARGEM) {
      abrirCofre();
    }

    delay(100);
    return;
  }

 
  // DIGITACAO DOS BOTOES
  
  if (clicou(b1)) {
    if (pos < 3) {
      entrada[pos++] = 1;
      lcd.setCursor(pos - 1, 1);
      lcd.print("*");
      beep(100);
    }
  }

  if (clicou(b2)) {
    if (pos < 3) {
      entrada[pos++] = 2;
      lcd.setCursor(pos - 1, 1);
      lcd.print("*");
      beep(100);
    }
  }

  
  // CONFIRMAR SENHA 
  
  if (clicou(b3)) {

    if (pos != 3) return;

    bool correta = true;
    for (int i = 0; i < 3; i++) {
      if (entrada[i] != senha[i]) correta = false;
    }

    // SENHA CORRETA 
    if (correta) {
      tentativas = 0;
      aguardaPot = true;
      pos = 0;

      lcd.clear();
      lcd.print("Gire o pot:");
      lcd.setCursor(0, 1);
      lcd.print("Val:---");
    }

    // SENHA ERRADA
    else {
      tentativas++;

      lcd.clear();
      lcd.print("SENHA ERRADA");
      lcd.setCursor(0, 1);
      lcd.print("Tent:");
      lcd.print(tentativas);

      beep(500);
      delay(2000);

      if (tentativas >= 3) {
        bloqueado = true;
        lcd.clear();
        lcd.print("BLOQUEADO");
        return;
      }

      lcd.clear();
      lcd.print("TRANCADO");
      lcd.setCursor(0, 1);
      lcd.print("Digite senha");
    }

    pos = 0;
  }
}
