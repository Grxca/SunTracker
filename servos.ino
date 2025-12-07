#include <ESP32Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// ====== TFT ======
#define TFT_CS   20
#define TFT_DC   19
#define TFT_RST  18
#define TFT_SCK  15
#define TFT_MOSI 9

SPIClass spi = SPIClass(FSPI);
Adafruit_ST7735 tft = Adafruit_ST7735(&spi, TFT_CS, TFT_DC, TFT_RST);

// ====== SERVOS ======
#define SERVO_H_PIN 23
#define SERVO_V_PIN 22

Servo servoH;
Servo servoV;

// ====== LDRs ======
#define LDR_TL 2   // Top Left
#define LDR_TR 3   // Top Right
#define LDR_LL 4   // Bottom Left
#define LDR_LR 5   // Bottom Right

// ====== VARIÁVEIS ======
int servohori = 90;  
int servovert = 90;

int tl, tr, ll, lr;
int dvert, dhoriz;

unsigned long lastServo = 0;
unsigned long lastTFT = 0;
const unsigned long servoInterval = 10;
const unsigned long tftInterval   = 300;

int tol = 90;  // tolerância

// ====== SETUP ======
void setup() {

  spi.begin(TFT_SCK, -1, TFT_MOSI);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_GREEN);
  tft.setCursor(10,10);
  tft.println("SUN TRACKER");
  delay(1000);
  tft.fillScreen(ST77XX_BLACK);

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

  dvert  = avT - avB;
  dhoriz = avL - avR;

  // ===== VERTICAL =====
  if (abs(dvert) > tol) {
    if (avT > avB) servovert--;    // luz em cima → subir
    else           servovert++;    // luz em baixo → descer
  }
  servovert = constrain(servovert, 0, 180);
  servoV.write(servovert);

  // ===== HORIZONTAL =====
  if (abs(dhoriz) > tol) {
    if (avL > avR) servohori--;    // luz à esquerda
    else           servohori++;    // luz à direita
  }
  servohori = constrain(servohori, 0, 180);
  servoH.write(servohori);
}

// ====== TFT ======
void atualizarTFT() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(1);

  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(1, 1);
  tft.println("LDRs");

  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(1, 12);  tft.printf("TL:%4d", tl);
  tft.setCursor(1, 22);  tft.printf("TR:%4d", tr);
  tft.setCursor(1, 32);  tft.printf("LL:%4d", ll);
  tft.setCursor(1, 42);  tft.printf("LR:%4d", lr);

  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(1, 58);
  tft.println("DV/DH");

  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(1, 68);  tft.printf("V:%4d", dvert);
  tft.setCursor(1, 78);  tft.printf("H:%4d", dhoriz);

  tft.setTextColor(ST77XX_GREEN);
  tft.setCursor(1, 95);
  tft.println("SERVOS");

  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(1, 105);  tft.printf("H:%3d", servohori);
  tft.setCursor(1, 115);  tft.printf("V:%3d", servovert);
}
