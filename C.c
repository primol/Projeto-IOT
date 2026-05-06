

#include <LiquidCrystal.h>  // LCD  pro thinker
#include <Servo.h>

// ------------------------------------------------------------
// Definição de pinos
// ------------------------------------------------------------
#define BTN_1       2   // Botão 1 da sequência
#define BTN_2       3   // Botão 2 da sequência
#define BTN_3       4   // Botão 3 da sequência
#define POT_PIN     A0  // Potenciômetro
#define LED_VERDE   8   // LED verde  (acesso liberado)
#define LED_VERM    9   // LED vermelho (neutro / erro)
#define BUZZER      10  // Buzzer
#define SERVO_PIN   11  // Servo motor

// LCD: RS, EN, D4, D5, D6, D7
LiquidCrystal lcd(12, 13, 5, 6, 7, A1); //pro lcd

Servo servoMotor;

// ------------------------------------------------------------
// Configurações da senha
// ------------------------------------------------------------

// Sequência correta dos botões (use 1, 2 ou 3)
const int SEQUENCIA_CORRETA[] = {1, 3, 2};
const int TAMANHO_SEQUENCIA   = 3;

// Ângulo do potenciômetro aceito como correto (0–1023)
// ~512 = posição do meio. Ajuste conforme necessário.
const int POT_ALVO   = 512;
const int POT_MARGEM = 50;   // tolerância: ±50 ///////////////////////////////////Sem tolerancia era impossivel acertar

// ------------------------------------------------------------
// Variáveis de estado
// ------------------------------------------------------------
int  sequenciaDigitada[3];
int  posicaoAtual    = 0;   // quantos botões já foram pressionados
bool sequenciaOk     = false;
bool sistemaAberto   = false;

// Controle de debounce
unsigned long ultimoTempoBotao = 0;
const unsigned long DEBOUNCE_MS = 200;

// ------------------------------------------------------------
// Protótipos
// ------------------------------------------------------------
void estadoNeutro();
void verificarBotoes();
void verificarPotenciometro();
void abrirCofre();
void erroSenha();
void piscarLedVermelho(int vezes);
void buzzerErro();
void buzzerSucesso();
void fecharCofre();

// ============================================================
// SETUP
// ============================================================
void setup() {
  pinMode(BTN_1,     INPUT);
  pinMode(BTN_2,     INPUT);
  pinMode(BTN_3,     INPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERM,  OUTPUT);
  pinMode(BUZZER,    OUTPUT);

  servoMotor.attach(SERVO_PIN);
  servoMotor.write(0);  // posição fechada

  lcd.begin(16, 2);
  Serial.begin(9600);

  estadoNeutro();
}

// ============================================================
// LOOP
// ============================================================
void loop() {
  if (sistemaAberto) {
    // Cofre aberto: aguarda qualquer botão para fechar
    if (digitalRead(BTN_1) || digitalRead(BTN_2) || digitalRead(BTN_3)) {
      delay(DEBOUNCE_MS);
      fecharCofre();
    }
    return;
  }

  if (!sequenciaOk) {
    verificarBotoes();
  } else {
    verificarPotenciometro();
  }
}

// ============================================================
// FUNÇÕES
// ============================================================

// Tela e LED iniciais (estado neutro / trancado)
void estadoNeutro() {
  posicaoAtual  = 0;
  sequenciaOk   = false;
  sistemaAberto = false;

  digitalWrite(LED_VERM,  HIGH);
  digitalWrite(LED_VERDE, LOW);
  servoMotor.write(0);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("== COFRE ==");
  lcd.setCursor(0, 1);
  lcd.print("Digite a senha");

  Serial.println("[SISTEMA] Estado: TRANCADO");
}

// Lê os botões e monta a sequência digitada
void verificarBotoes() {
  int botaoPressionado = 0;

  if (digitalRead(BTN_1) == HIGH) botaoPressionado = 1;
  else if (digitalRead(BTN_2) == HIGH) botaoPressionado = 2;
  else if (digitalRead(BTN_3) == HIGH) botaoPressionado = 3;

  // Debounce: ignora se pressionado muito rápido
  if (botaoPressionado == 0) return;
  if (millis() - ultimoTempoBotao < DEBOUNCE_MS) return;
  ultimoTempoBotao = millis();

  // Registra o botão pressionado
  sequenciaDigitada[posicaoAtual] = botaoPressionado;

  // Feedback visual no LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Seq: ");
  for (int i = 0; i <= posicaoAtual; i++) {
    lcd.print(sequenciaDigitada[i]);
    lcd.print(" ");
  }

  Serial.print("[BOTAO] Pressionado: ");
  Serial.println(botaoPressionado);

  // Verifica se o botão pressionado está certo até agora
  if (botaoPressionado != SEQUENCIA_CORRETA[posicaoAtual]) {
    erroSenha();
    return;
  }

  posicaoAtual++;

  // Sequência completa e correta?
  if (posicaoAtual == TAMANHO_SEQUENCIA) {
    sequenciaOk = true;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Seq. correta!");
    lcd.setCursor(0, 1);
    lcd.print("Ajuste o angulo");
    Serial.println("[SISTEMA] Sequência correta! Aguardando potenciômetro...");
  }
}

// Lê o potenciômetro e verifica se está no ângulo certo
void verificarPotenciometro() {
  int valorPot = analogRead(POT_PIN);

  // Mostra o valor atual no LCD para facilitar o ajuste
  lcd.setCursor(0, 1);
  lcd.print("Val:");
  lcd.print(valorPot);
  lcd.print("   ");  // limpa dígitos antigos

  Serial.print("[POT] Valor: ");
  Serial.println(valorPot);

  if (valorPot >= POT_ALVO - POT_MARGEM && valorPot <= POT_ALVO + POT_MARGEM) {
    abrirCofre();
  }

  delay(100); // pequena pausa para não sobrecarregar o LCD
}

// Acesso concedido
void abrirCofre() {
  sistemaAberto = true;

  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(LED_VERM,  LOW);

  servoMotor.write(90);  // gira para posição "aberto"

  buzzerSucesso();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Acesso Liberado!");
  lcd.setCursor(0, 1);
  lcd.print("Pressione p/fechar");

  Serial.println("[SISTEMA] COFRE ABERTO!");
}

// Fecha o cofre após ser aberto
void fecharCofre() {
  servoMotor.write(0);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Cofre trancado.");
  delay(1500);

  Serial.println("[SISTEMA] Cofre fechado.");
  estadoNeutro();
}

// Senha errada: pisca LED e apita buzzer
void erroSenha() {
  Serial.println("[ERRO] Senha incorreta!");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Senha errada!");

  buzzerErro();
  piscarLedVermelho(3);

  delay(1000);
  estadoNeutro();
}

// Pisca o LED vermelho N vezes
void piscarLedVermelho(int vezes) {
  for (int i = 0; i < vezes; i++) {
    digitalWrite(LED_VERM, LOW);
    delay(200);
    digitalWrite(LED_VERM, HIGH);
    delay(200);
  }
}

// Buzzer de sucesso: dois bipes curtos + um longo
void buzzerSucesso() {
  tone(BUZZER, 1000, 100); delay(150);
  tone(BUZZER, 1000, 100); delay(150);
  tone(BUZZER, 1500, 400); delay(450);
  noTone(BUZZER);
}

// Buzzer de erro: três bipes graves
void buzzerErro() {
  tone(BUZZER, 300, 200); delay(250);
  tone(BUZZER, 300, 200); delay(250);
  tone(BUZZER, 300, 200); delay(250);
  noTone(BUZZER);
}
