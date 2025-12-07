#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "SunTracker";
const char* password = "12345678";

WebServer server(80);

bool modoAutomatico = true;
bool limitesAtivos = true;

// ============================================
//  PÁGINA HTML COM ACENTOS (UTF-8)
// ============================================
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>SunTracker</title>

<style>
  body {
    font-family: Arial, sans-serif;
    background:#0d0d0d;
    color:#ffffff;
    text-align:center;
    padding:10px;
  }

  h1 {
    margin-top:20px;
    color:#ffdd55;
    text-shadow:0 0 10px #ffaa00;
  }

  .box {
    background:#1a1a1a;
    padding:20px;
    margin:15px auto;
    border-radius:12px;
    width:320px;
    border:2px solid #333;
    box-shadow:0 0 15px rgba(255, 200, 0, 0.2);
  }

  .box h2 {
    color:#66c2ff;
    margin-bottom:10px;
    text-shadow:0 0 8px #0066cc;
  }

  button {
    width:150px;
    height:50px;
    margin:8px;
    font-size:17px;
    font-weight:bold;
    border:none;
    border-radius:10px;
    cursor:pointer;
    transition:0.2s;
    color:white;
  }

  .btnBlue { background:#007bff; }
  .btnBlue:hover { background:#3399ff; }

  .btnGreen { background:#28a745; }
  .btnGreen:hover { background:#45c766; }

  .btnRed { background:#ff4444; }
  .btnRed:hover { background:#ff6666; }

  .btnOrange { background:#ff8800; }
  .btnOrange:hover { background:#ffaa33; }

  .arrow {
    width:100px;
    height:50px;
    margin:5px;
    font-size:28px;
    background:#444;
    border-radius:12px;
    color:white;
  }

  .arrow:hover {
    background:#666;
  }

</style>
</head>

<body>

<h1>SunTracker</h1>

<!-- ================= MODO ================= -->
<div class="box">
  <h2>Modo de Operação</h2>
  <button class="btnGreen" onclick="setModo('auto')">Automático</button>
  <button class="btnRed" onclick="setModo('manual')">Manual</button>
  <p id="modoTxt">---</p>
</div>

<!-- ================= SERVO HORIZONTAL ================= -->
<div class="box">
  <h2>Controlo Manual Horizontal</h2>
  <button class="arrow" onclick="move('left')">&#9664;</button>
  <button class="arrow" onclick="move('right')">&#9654;</button>
</div>

<!-- ================= SERVO VERTICAL ================= -->
<div class="box">
  <h2>Controlo Manual Vertical</h2>
  <button class="arrow" onclick="move('up')">&#9650;</button><br>
  <button class="arrow" onclick="move('down')">&#9660;</button>
</div>

<!-- ================= AÇÕES ================= -->
<div class="box">
  <h2>Ações</h2>
  <button class="btnBlue" onclick="resetServos()">Reset Servos</button><br>
  <button class="btnOrange" onclick="limites()">Limites Mecânicos</button>
</div>

<script>
function setModo(m) { fetch(`/setModo?m=${m}`); }
function move(dir) { fetch(`/move?dir=${dir}`); }
function resetServos() { fetch("/resetServos"); }
function limites() { fetch("/limites"); }

setInterval(() => {
  fetch("/modoAtual")
    .then(r => r.text())
    .then(txt => document.getElementById("modoTxt").innerText = txt);
}, 200);
</script>

</body>
</html>
)rawliteral";

// ============================================
//  ROTAS DO WEBSERVER
// ============================================

void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

// Mudar entre automático / manual
void handleSetModo() {
  String m = server.arg("m");
  modoAutomatico = (m == "auto");
  Serial.println(modoAutomatico ? "Modo Automático" : "Modo Manual");
  server.send(200, "text/plain", "OK");
}

// Movimento (left/right/up/down)
void handleMove() {
  String dir = server.arg("dir");
  Serial.println("Mover: " + dir);
  server.send(200, "text/plain", "OK");
}

// Reset dos servos
void handleReset() {
  Serial.println("RESET dos servos");
  server.send(200, "text/plain", "OK");
}

// Ativar/desativar limites mecânicos
void handleLimites() {
  limitesAtivos = !limitesAtivos;
  Serial.println(limitesAtivos ? "Limites Mecânicos Ativos" : "Limites Mecânicos Desativados");
  server.send(200, "text/plain", "OK");
}

// Enviar texto do modo atual
void handleModoAtual() {
  server.send(200, "text/plain", modoAutomatico ? "Automático" : "Manual");
}

// ============================================
//  SETUP
// ============================================
void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  Serial.println("Hotspot criado:");
  Serial.print("SSID: "); Serial.println(ssid);
  Serial.print("IP: "); Serial.println(WiFi.softAPIP());

  // Rotas
  server.on("/", handleRoot);
  server.on("/setModo", handleSetModo);
  server.on("/move", handleMove);
  server.on("/resetServos", handleReset);
  server.on("/limites", handleLimites);
  server.on("/modoAtual", handleModoAtual);

  server.begin();
  Serial.println("WebServer iniciado!");
}

// ============================================
//  LOOP
// ============================================
void loop() {
  server.handleClient();
}
