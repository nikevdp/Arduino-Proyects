#define LED_PIN 25
#define BUTTON 27

bool estadoLED = false;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=== TEST ESP32 ===");
  Serial.println("Inicializando...");
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  
}

void loop() {
  bool estadoBoton = digitalRead(BUTTON);
  if (estadoBoton == LOW){
    estadoLED = !estadoLED;
    digitalWrite(LED_PIN, estadoLED);
    Serial.println("Boton Apretado");
    Serial.println(estadoLED);
    delay(200); // anti-rebote simple
  }
  delay(300);
}