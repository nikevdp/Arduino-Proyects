#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// ===== CONFIG WIFI =====
const char* ssid = "MOVISTAR-WIFI6-4B70";
const char* password = "U7RyAfStv3HCeMGTB6ae";

// ===== SERVO =====
static const int SERVO_PIN = 18;
Servo servo;
int servoAngle = 90;

// ===== SERVER =====
WebServer server(80);

// ===== HTML =====
const char MAIN_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial; text-align: center; }
    input { width: 80%; }
  </style>
</head>
<body>
  <h2>ESP32 Servo Control</h2>
  <p>Angulo: <span id="val">90</span>°</p>
  <input type="range" min="0" max="180" value="90"
         oninput="update(this.value)">
  <script>
    function update(val) {
      document.getElementById('val').innerText = val;
      fetch('/set?angle=' + val);
    }
  </script>
</body>
</html>
)rawliteral";

// ===== HANDLERS =====
void handleRoot() {
  server.send(200, "text/html", MAIN_page);
}

void handleSet() {
  if (server.hasArg("angle")) {
    servoAngle = server.arg("angle").toInt();
    servoAngle = constrain(servoAngle, 0, 180);
    servo.write(servoAngle);
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);

  // Servo
  servo.setPeriodHertz(50);
  servo.attach(SERVO_PIN, 500, 2400);
  servo.write(servoAngle);

  // WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // Server
  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.begin();
}

void loop() {
  server.handleClient();
}