git
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// ===== CONFIG WIFI =====
const char* ssid = "MOVISTAR-WIFI6-4B70";
const char* pass = "U7RyAfStv3HCeMGTB6ae";

WebServer server(80);

const char INDEX_HTML[] PROGMEM = R"rawliteral(
  <!doctype html>
<html lang="es">
<head>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=device-width,initial-scale=1" />
  <title>ESP32 Car Control</title>
  <style>
    body { font-family: system-ui, sans-serif; padding: 20px; max-width: 520px; margin: 0 auto; }
    h1 { font-size: 20px; }
    .grid { display: grid; gap: 12px; grid-template-columns: repeat(3, 1fr); }
    button { padding: 16px; font-size: 16px; border-radius: 12px; border: 1px solid #ccc; }
    button:active { transform: scale(0.98); }
    .full { grid-column: 1 / -1; }
    .status { margin-top: 14px; font-size: 14px; opacity: .8; }
  </style>
</head>

<body>
  <h1>Control del auto (LAN)</h1>

  <div class="grid">
    <div></div>
    <button id="btnF">↑ Adelante</button>
    <div></div>

    <button id="btnL">← Izq</button>
    <button id="btnS">■ Stop</button>
    <button id="btnR">Der →</button>

    <div></div>
    <button id="btnB">↓ Atrás</button>
    <div></div>

    <button class="full" id="btnPing">Ping</button>
  </div>

  <div class="status" id="status">Estado: listo</div>

  <script>
    const statusEl = document.getElementById("status");

    async function sendCmd(code) {
      try {
        statusEl.textContent = `Estado: enviando ${code}...`;
        const r = await fetch(`/cmd?m=${encodeURIComponent(code)}`, { cache: "no-store" });
        const t = await r.text();
        statusEl.textContent = `Estado: ${t}`;
      } catch (e) {
        statusEl.textContent = `Estado: error (${e})`;
      }
    }

    // Clicks (modo simple)
    document.getElementById("btnF").onclick = () => sendCmd("F");
    document.getElementById("btnB").onclick = () => sendCmd("B");
    document.getElementById("btnL").onclick = () => sendCmd("L");
    document.getElementById("btnR").onclick = () => sendCmd("R");
    document.getElementById("btnS").onclick = () => sendCmd("S");
    document.getElementById("btnPing").onclick = async () => {
      try {
        const r = await fetch("/ping", { cache: "no-store" });
        statusEl.textContent = `Estado: ${await r.text()}`;
      } catch (e) {
        statusEl.textContent = `Estado: ping error (${e})`;
      }
    };

    // (Opcional) mantener apretado para moverse y soltar = stop
    function holdButton(btnId, code) {
      const el = document.getElementById(btnId);
      el.addEventListener("pointerdown", () => sendCmd(code));
      el.addEventListener("pointerup",   () => sendCmd("S"));
      el.addEventListener("pointercancel", () => sendCmd("S"));
      el.addEventListener("pointerleave",  () => sendCmd("S"));
    }
    holdButton("btnF", "F");
    holdButton("btnB", "B");
    holdButton("btnL", "L");
    holdButton("btnR", "R");
  </script>
</body>
</html>
)rawliteral";

// Tus GPIO hacia IN1..IN4 del ULN2003
static const int IN1 = 25;
static const int IN2 = 26;
static const int IN3 = 27;
static const int IN4 = 14;

void engineB(uint8_t V3, uint8_t V4){
  digitalWrite(IN3, V3);
  digitalWrite(IN4, V4);
}

void engineA(uint8_t V1, uint8_t V2){
  digitalWrite(IN1, V1);
  digitalWrite(IN2, V2);
}

void forward(){
  engineB(LOW, HIGH);
  engineA(LOW, HIGH);
}

void backward(){
  engineB(HIGH, LOW);
  engineA(HIGH, LOW);
}

void left(){
  engineB(LOW, HIGH);
  engineA(HIGH, LOW);
}

void right(){
  engineB(HIGH, LOW);
  engineA(LOW, HIGH);
}

void stopCar() {
  engineB(LOW, LOW);
  engineA(LOW, LOW);
}

void setup() {
  Serial.begin(115200);

  Serial.println("=== TEST ESP32 ===");
  Serial.println("Inicializando...");
  
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopCar();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.print("Conectando WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(250); Serial.print("."); }
  Serial.println();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  // Página principal
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html; charset=utf-8", INDEX_HTML);
  });

  // Endpoint ping
  server.on("/ping", HTTP_GET, []() {
    server.send(200, "text/plain; charset=utf-8", "pong");
  });

  // Endpoint de comandos: /cmd?m=F
  server.on("/cmd", HTTP_GET, []() {
    if (!server.hasArg("m")) {
      server.send(400, "text/plain; charset=utf-8", "ERR: missing m");
      return;
    }
    String m = server.arg("m");
    String resp = handleMoveCode(m);
    server.send(200, "text/plain; charset=utf-8", resp);
  });

  server.begin();
  Serial.println("HTTP server listo.");
}

String handleMoveCode(const String& m) {
  if (m == "F") { forward();  return "OK: forward"; }
  if (m == "B") { backward(); return "OK: backward"; }
  if (m == "L") { left();     return "OK: left"; }
  if (m == "R") { right();    return "OK: right"; }
  if (m == "S") { stopCar();  return "OK: stop"; }
  return "ERR: unknown cmd";
}

void loop() {
  server.handleClient();
}
