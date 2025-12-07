#include <ESP32Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <WiFi.h>
#include <WebServer.h>

// ===============================
//          DEFINIÇÕES TFT
// ===============================
#define TFT_CS   20
#define TFT_DC   19
#define TFT_RST  18
#define TFT_SCK  15
#define TFT_MOSI 9
SPIClass spi = SPIClass(FSPI);
Adafruit_ST7735 tft = Adafruit_ST7735(&spi, TFT_CS, TFT_DC, TFT_RST);

// ===============================
//          DEFINIÇÕES SERVOS
// ===============================
#define SERVO_H_PIN 23
#define SERVO_V_PIN 22

Servo servoH;
Servo servoV;

// ===============================
//          DEFINIÇÕES LDRs
// ===============================
#define LDR_TL 2
#define LDR_TR 3
#define LDR_LL 4
#define LDR_LR 5

// ===============================
//          VARIÁVEIS
// ===============================
int servohori = 90; // posição inicial
int servovert = 0;  // posição inicial vertical que pediste

int tl, tr, ll, lr;
int dvert, dhoriz;

const int tol = 90;        // tolerância de luz
bool modoAutomatico = true;
bool limitesAtivos = true;

unsigned long lastServo = 0;
unsigned long lastTFT = 0;
const unsigned long servoInterval = 15;
const unsigned long tftInterval = 350;

// ===============================
//          Wi-Fi e WebServer
// ===============================
const char* ssid = "SunTracker";
const char* password = "12345678";

WebServer server(80);

// ===============================
//           HTML PAGE
// ===============================
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt">
<head>
<meta charset="UTF-8">
<title>SunTracker</title>
<style>
  body { background:#111; color:white; text-align:center; font-family:Arial; }
  button { width:140px; height:45px; margin:8px; font-size:18px; border-radius:10px; border:none; color:white; }
  .g { background:#28a745; }
  .r { background:#c0392b; }
  .b { background:#2980b9; }
  .y { background:#f39c12; }
  .arrow { background:#444; width:100px; height:50px; font-size:28px; }
</style>
</head>
<body>

<h1>SunTracker</h1>

<h2>Modo de Operação</h2>
<button class="g" onclick="setModo('auto')">Automático</button>
<button class="r" onclick="setModo('manual')">Manual</button>
<p id="modoTxt">---</p>

<h2>Horizontal</h2>
<button class="arrow" onclick="move('left')">&#9664;</button>
<button class="arrow" onclick="move('right')">&#9654;</button>

<h2>Vertical</h2>
<button class="arrow" onclick="move('up')">&#9650;</button><br>
<button class="arrow" onclick="move('down')">&#9660;</button>

<h2>Ações</h2>
<button class="b" onclick="resetServos()">Reset</button>
<button class="y" onclick="limites()">Limites</button>

<script>
function setModo(m){ fetch('/setModo?m='+m); }
function move(d){ fetch('/move?dir='+d); }
function resetServos(){ fetch('/resetServos'); }
function limites(){ fetch('/limites'); }

setInterval(() => {
 fetch('/modoAtual')
   .then(r=>r.text())
   .then(t=>document.getElementById('modoTxt').innerText=t);
}, 200);
</script>

</body>
</html>
)rawliteral";

// ===============================
//     FUNÇÕES DO WEBSERVER
// ===============================

void handleRoot() { server.send(200, "text/html", htmlPage); }

void handleSetModo() {
  String m = server.arg("m");
  modoAutomatico = (m == "auto");
  server.send(200, "text/plain", "OK");
}

void handleMove() {
  if (!modoAutomatico) {
    String d = server.arg("dir");

    if (d == "left")  servohori--;
    if (d == "right") servohori++;
    if (d == "up")    servovert--;
    if (d == "down")  servovert++;

    if (limitesAtivos) {
      servohori = constrain(servohori, 5, 175);
      servovert = constrain(servovert, 1, 100);
    }

    servoH.write(servohori);
    servoV.write(servovert);
  }

  server.send(200, "text/plain", "OK");
}

void handleReset() {
  servohori = 90;
  servovert = 0;

  servoH.write(servohori);
  servoV.write(servovert);

  server.send(200, "text/plain", "OK");
}

void handleLimites() {
  limitesAtivos = !limitesAtivos;
  server.send(200, "text/plain", limitesAtivos ? "Limites ATIVOS" : "Limites DESATIVADOS");
}

void handleModoAtual() {
  server.send(200, "text/plain", modoAutomatico ? "Automático" : "Manual");
}

// ===============================
//     ATUALIZAÇÃO DOS SERVOS
// ===============================
void atualizarServos() {

  if (!modoAutomatico) return; // manual -> não mexer automaticamente

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

  // VERTICAL
  if (abs(dvert) > tol) {
    if (avT > avB) servovert--;
    else           servovert++;
  }

  // HORIZONTAL
  if (abs(dhoriz) > tol) {
    if (avL > avR) servohori--;
    else           servohori++;
  }

  if (limitesAtivos) {
    servovert = constrain(servovert, 1, 100);
    servohori = constrain(servohori, 5, 175);
  }

  servoH.write(servohori);
  servoV.write(servovert);
}

// ===============================
//     ATUALIZAR TFT
// ===============================
void atualizarTFT() {

  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(1);

  tft.setCursor(2, 2);
  tft.setTextColor(ST77XX_CYAN);
  tft.println("SUN TRACKER");

  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(2, 20);
  tft.println("LDRs");

  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(2, 35); tft.printf("TL:%4d", tl);
  tft.setCursor(2, 45); tft.printf("TR:%4d", tr);
  tft.setCursor(2, 55); tft.printf("LL:%4d", ll);
  tft.setCursor(2, 65); tft.printf("LR:%4d", lr);

  tft.setCursor(2, 85);
  tft.setTextColor(ST77XX_GREEN);
  tft.println("Servos");

  tft.setCursor(2, 100); tft.printf("H:%3d", servohori);
  tft.setCursor(2, 110); tft.printf("V:%3d", servovert);
}

// ===============================
//              SETUP
// ===============================
void setup() {
  Serial.begin(115200);

  // TFT
  spi.begin(TFT_SCK, -1, TFT_MOSI);
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  servoH.attach(SERVO_H_PIN);
  servoV.attach(SERVO_V_PIN);

  servoH.write(servohori);
  servoV.write(servovert);

  // WiFi
  WiFi.softAP(ssid, password);
  Serial.println("WebServer Ativo!");

  // Rotas
  server.on("/", handleRoot);
  server.on("/setModo", handleSetModo);
  server.on("/move", handleMove);
  server.on("/resetServos", handleReset);
  server.on("/limites", handleLimites);
  server.on("/modoAtual", handleModoAtual);

  server.begin();
}

// ===============================
//              LOOP
// ===============================
void loop() {

  server.handleClient();

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
