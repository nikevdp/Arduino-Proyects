#define led_red 16
#define led_green 26
#define led_yelow 27
#define sensor 25

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(led_red, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(led_yelow, OUTPUT);
  pinMode(sensor, INPUT);
  Serial.println("=== Water Level Sensor (ESP32) ===");
}

void loop() {
  int raw = analogRead(sensor);
  Serial.println(raw);
  digitalWrite(led_green, LOW);
  digitalWrite(led_yelow, LOW);
  digitalWrite(led_red, LOW);

  if (raw < 2000) {
    digitalWrite(led_green, HIGH);
  } else if (raw < 3000) {
    digitalWrite(led_yelow, HIGH);
  } else {
    digitalWrite(led_red, HIGH);
  }

  delay(1000);
}
