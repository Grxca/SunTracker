#include <ESP32Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// ====== TFT com os PINOS QUE PEDISTE ======
#define TFT_CS   20   // GPIO20
#define TFT_DC   19   // GPIO19
#define TFT_RST  18   // GPIO18

#define TFT_SCK  15   // GPIO15
#define TFT_MOSI  9   // GPIO9

// Criar SPI manual
SPIClass spi = SPIClass(FSPI);

// *** IMPORTANTE: usar este construtor ***
Adafruit_ST7735 tft = Adafruit_ST7735(&spi, TFT_CS, TFT_DC, TFT_RST);

// ====== SERVOS ======
#define SERVO_H_PIN 23
#define SERVO_V_PIN 23

Servo servoH;
Servo servoV;

// ====== LDRs (ADC válidos) ======
#define LDR_TL 2
#define LDR_TR 3
#define LDR_LL 4
#define LDR_LR 5

// ====== VARIÁVEIS ======
int servohori = 180;
int servovert = 45;

int tl, tr, ll, lr;
int dvert, dhoriz;
int tol = 90;

unsigned long lastServo = 0;
unsigned long lastTFT = 0;
const unsigned long servoInterval = 10;
const unsigned long tftInterval = 400;

// ====== SETUP ======
void setup() {

  // INICIAR SPI COM OS TEUS PINOS
  spi.begin(TFT_SCK, -1, TFT_MOSI);

  // TFT
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(1);
  tft.setCursor(10,10);
  tft.println("SUN TRACKER");
  delay(1500);
  tft.fillScreen(ST77XX_BLACK);

  // SERVOS
  servoH.attach(SERVO_H_PIN);
  servoV.attach(SERVO_V_PIN);
  servoH.write(servohori);
  servoV.write(servovert);
}

// ====== LOOP ======
void loop() {
  unsigned long now = millis();

  if (now - lastServo >= servoInterval) {
    lastServo = now;
    atualizarServos();
  }

  if (now - lastTFT >= tftInterval) {
    lastTFT = now;
    atualizarTFT();
  }
}

// ====== SERVOS ======
void atualizarServos() {
  tl = analogRead(LDR_TL);
  tr = analogRead(LDR_TR);
  ll = analogRead(LDR_LL);
  lr = analogRead(LDR_LR);

  int avT = (tl + tr) / 2;
  int avB = (ll + lr) / 2;
  int avL = (tl + ll) / 2;
  int avR = (tr + lr) / 2;

  dvert = avT - avB;
  dhoriz = avL - avR;

  if (abs(dvert) > tol) {
    if (avT > avB) servovert++;
    else servovert--;
    servoV.write(servovert);
  }

  if (abs(dhoriz) > tol) {
    if (avL > avR) servohori--;
    else servohori++;
    servoH.write(servohori);
  }
}

// ====== TFT ======
void atualizarTFT() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(1);

  // ===== LDRs =====
  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(1, 1);
  tft.println("LDRs");

  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(1, 11);   tft.printf("TL:%4d", tl);
  tft.setCursor(1, 21);   tft.printf("TR:%4d", tr);
  tft.setCursor(1, 31);   tft.printf("LL:%4d", ll);
  tft.setCursor(1, 41);   tft.printf("LR:%4d", lr);

  // ===== DIFERENCAS =====
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(1, 55);
  tft.println("DIF");

  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(1, 65);   tft.printf("V:%4d", dvert);
  tft.setCursor(1, 75);   tft.printf("H:%4d", dhoriz);

  // ===== SERVOS =====
  tft.setTextColor(ST77XX_GREEN);
  tft.setCursor(1, 90);
  tft.println("SERVO");

  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(1, 100);   tft.printf("H:%3d", servohori);
  tft.setCursor(1, 110);   tft.printf("V:%3d", servovert);
}