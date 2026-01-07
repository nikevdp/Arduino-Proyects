 #include <ESP32Servo.h>

#define POT_PIN   34   // ADC1 recomendado
#define SERVO_PIN 18   // pin de señal del servo

Servo servo;

void setup() {
  Serial.begin(115200);

  // ADC
  analogReadResolution(12); // 0..4095

  // Servo (50 Hz)
  servo.setPeriodHertz(50);
  servo.attach(SERVO_PIN, 500, 2500); // min/max microseconds (SG90 aprox)
}

void loop() {
  int raw = analogRead(POT_PIN);              // 0..4095
  int angle = map(raw, 0, 4095, 0, 180);      // 0..180 grados

  servo.write(angle);

  Serial.print("raw=");
  Serial.print(raw);
  Serial.print(" angle=");
  Serial.println(angle);

  delay(20); // update suave
}