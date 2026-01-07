#include <Arduino.h>
#include <AccelStepper.h>

// Tus GPIO hacia IN1..IN4 del ULN2003
static const int IN1 = 16;
static const int IN2 = 17;
static const int IN3 = 18;
static const int IN4 = 23;

// OJO: este orden es CLAVE y suele ser el correcto para 28BYJ-48 con ULN2003
AccelStepper stepper(AccelStepper::HALF4WIRE, IN1, IN3, IN2, IN4);

void setup() {
  Serial.begin(115200);

  stepper.setMaxSpeed(800);      // probá 400..1000
  stepper.setAcceleration(400);  // suave
  Serial.println("Listo. Envia: cw 512 / ccw 512 / rev");
}

String readLine() {
  static String line;
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\n' || c == '\r') {
      if (line.length()) { String out = line; line = ""; return out; }
    } else line += c;
  }
  return "";
}

void loop() {
  String cmd = readLine();
  if (cmd.length()) {
    cmd.trim(); cmd.toLowerCase();

    if (cmd == "rev") {
      stepper.move(4096); // 1 vuelta aprox
    } else if (cmd.startsWith("cw")) {
      long n = cmd.length() > 2 ? cmd.substring(2).toInt() : 512;
      if (n == 0) n = 512;
      stepper.move(abs(n));
    } else if (cmd.startsWith("ccw")) {
      long n = cmd.length() > 3 ? cmd.substring(3).toInt() : 512;
      if (n == 0) n = 512;
      stepper.move(-abs(n));
    } else {
      Serial.println("Comandos: cw 512 | ccw 512 | rev");
    }
  }

  stepper.run();
}
