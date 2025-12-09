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
int servohori = 90;
int servovert = 0;

int tl, tr, ll, lr;
int dvert, dhoriz;

const int tol = 90;
bool modoAutomatico = true;
bool limitesAtivos = true;

// DEBUG controlado pelo utilizador (ativado quando entra no WebServer)
bool debugAtivo = false;

unsigned long lastServo = 0;
unsigned long lastTFT = 0;
const unsigned long servoInterval = 15;
const unsigned long tftInterval = 350;

// ===============================
//          WIFI
// ===============================
const char* ssid = "SunTracker";
const char* password = "12345678";

WebServer server(80);

// ===============================
//          HTML
// ===============================
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt">
<head>
<meta charset="UTF-8">

<!-- Página responsiva no telemóvel -->
<meta name="viewport" content="width=device-width, initial-scale=1.0">

<title>SunTracker</title>

<style>
  body {
    background:#111;
    color:white;
    text-align:center;
    font-family:Arial;
    margin:0;
    padding:0;
  }

  h1 {
    margin-top: 20px;
    font-size: 30px;
    font-weight: bold;
  }

  h2 {
    margin-top: 18px;
    font-size: 23px;
  }

  /* Botões principais */
  button {
    width: 75%;
    max-width: 260px;
    height: 48px;
    margin: 10px auto;
    font-size: 20px;
    border-radius: 12px;
    border: none;
    color: white;
    display: block;
  }

  .g { background:#28a745; }
  .r { background:#c0392b; }
  .b { background:#2980b9; }

  /* Botões de setas */
  .arrow {
    background:#444;
    width: 70px;
    height: 70px;
    font-size: 32px;
    display: inline-flex;
    justify-content:center;
    align-items:center;
    border-radius: 12px;
    margin: 6px;
  }

  #modoTxt {
    font-size: 20px;
    margin-top: 10px;
  }
</style>
</head>

<body>

<h1>SunTracker</h1>

<h2>Modo de Operação</h2>

<button class="g" onclick="setModo('auto')">Automático</button>
<button class="r" onclick="setModo('manual')">Manual</button>

<p id="modoTxt">---</p>

<h2>Horizontal</h2>

<div>
  <button class="arrow" onclick="move('left')">&#9664;</button>
  <button class="arrow" onclick="move('right')">&#9654;</button>
</div>

<h2>Vertical</h2>

<div>
  <button class="arrow" onclick="move('up')">&#9650;</button>
</div>
<div>
  <button class="arrow" onclick="move('down')">&#9660;</button>
</div>

<h2>Ações</h2>

<button class="b" onclick="resetServos()">Reset</button>

<script>

function setModo(m){
  fetch('/setModo?m='+m);
}

function move(d){
  fetch('/move?dir='+d);
}

function resetServos(){
  fetch('/resetServos');
}

/* Atualiza o texto do modo a cada 200ms */
setInterval(() => {
 fetch('/modoAtual')
   .then(r => r.text())
   .then(t => document.getElementById('modoTxt').innerText = t);
}, 200);

</script>

</body>
</html>

)rawliteral";

// ===============================
//     WEBSERVER HANDLERS
// ===============================
void handleRoot() {
  debugAtivo = true; // ATIVA DEBUG AO ENTRAR NO WEBPAGE
  Serial.println(">>> Cliente abriu o WebServer. Debug ativado.");
  server.send(200, "text/html", htmlPage);
}

void handleSetModo() {
  String m = server.arg("m");
  modoAutomatico = (m == "auto");

  if (debugAtivo) {
    Serial.print(">>> Modo alterado para: ");
    Serial.println(modoAutomatico ? "Automático" : "Manual");
  }

  server.send(200, "text/plain", "OK");
}

void handleMove() {
  if (!modoAutomatico) {
    String d = server.arg("dir");
    int passo = 5;

    if (d == "left")  servohori -= passo;
    if (d == "right") servohori += passo;
    if (d == "up")    servovert -= passo;
    if (d == "down")  servovert += passo;

    if (limitesAtivos) {
      servohori = constrain(servohori, 5, 175);
      servovert = constrain(servovert, 1, 100);
    }

    servoH.write(servohori);
    servoV.write(servovert);

    if (debugAtivo) {
      Serial.print(">>> Movimento Manual: ");
      Serial.print(d);
      Serial.print(" | H=");
      Serial.print(servohori);
      Serial.print(" | V=");
      Serial.println(servovert);
    }
  }

  server.send(200, "text/plain", "OK");
}

void handleReset() {
  servohori = 0;
  servovert = 90;

  servoH.write(servohori);
  servoV.write(servovert);

  if (debugAtivo) Serial.println(">>> RESET executado!");

  server.send(200, "text/plain", "OK");
}

void handleLimites() {
  limitesAtivos = !limitesAtivos;

  if (debugAtivo) {
    Serial.print(">>> Limites agora estão: ");
    Serial.println(limitesAtivos ? "ATIVOS" : "DESATIVADOS");
  }

  server.send(200, "text/plain", limitesAtivos ? "Ativos" : "Desativados");
}

void handleModoAtual() {
  server.send(200, "text/plain", modoAutomatico ? "Automático" : "Manual");
}

// ===============================
//     ATUALIZAÇÃO AUTOMÁTICA
// ===============================
void atualizarServos() {
  if (!modoAutomatico) return;

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

  if (abs(dvert) > tol) {
    if (avT > avB) servovert--;
    else           servovert++;
  }

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

  if (debugAtivo) {
    Serial.print("LDRs: TL=");
    Serial.print(tl);
    Serial.print(" TR=");
    Serial.print(tr);
    Serial.print(" LL=");
    Serial.print(ll);
    Serial.print(" LR=");
    Serial.println(lr);

    Serial.print("Servos -> H:");
    Serial.print(servohori);
    Serial.print(" | V:");
    Serial.println(servovert);
  }
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

  spi.begin(TFT_SCK, -1, TFT_MOSI);
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  servoH.attach(SERVO_H_PIN);
  servoV.attach(SERVO_V_PIN);

  servoH.write(servohori);
  servoV.write(servovert);

  WiFi.softAP(ssid, password);

  // APENAS MOSTRAR INFO DO WIFI
  Serial.println("====================================");
  Serial.println("        SUNTRACKER INICIADO!");
  Serial.println("====================================");
  Serial.print("SSID: "); Serial.println(ssid);
  Serial.print("Password: "); Serial.println(password);
  Serial.print("IP do Servidor: "); Serial.println(WiFi.softAPIP());
  Serial.println("====================================");

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
