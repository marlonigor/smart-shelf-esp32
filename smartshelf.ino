#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Sensor ultrassônico
#define TRIG_PIN 5
#define ECHO_PIN 18

// LED RGB (ânodo comum)
#define LED_VERMELHO 21
#define LED_VERDE 22

void setup() {
  Serial.begin(9600);
  Serial.println(F("Monitoramento de ambiente e proximidade"));

  dht.begin();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);

  // Apaga tudo inicialmente
  digitalWrite(LED_VERMELHO, HIGH);
  digitalWrite(LED_VERDE, HIGH);
}

void loop() {
  delay(1000);

  // ==== LEITURA DO DHT ====
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // Celsius
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Falha ao ler o sensor DHT!"));
    return;
  }
  // float hic = dht.computeHeatIndex(t, h, false);

  // ==== LEITURA COM MÉDIA DO ULTRASSÔNICO ====
  const int numLeituras = 5;
  float somaDist = 0;

  for (int i = 0; i < numLeituras; i++) {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duracao = pulseIn(ECHO_PIN, HIGH, 30000); // timeout de 30 ms
    float dist = duracao * 0.034 / 2;

    if (dist > 0 && dist < 400) {
      somaDist += dist;
    } else {
      i--; // repete leitura inválida
    }
    delay(50);
  }

  float mediaDistancia = somaDist / numLeituras;
  int distanciaInt = (int)mediaDistancia;

  // ==== EXIBIÇÃO ====
  Serial.print("Umidade: ");
  Serial.print(h);
  Serial.print("%  Temperatura: ");
  Serial.print((int)t);
  // Serial.print("°C  Sensação térmica: ");
  // Serial.print((int)hic);
  Serial.print("°C  Nível: ");

  // ==== AVALIAÇÃO DE NÍVEL + LED RGB ====
  if (distanciaInt <= 5) {
    Serial.println("Cheio");
    digitalWrite(LED_VERDE, HIGH);     // Desliga verde
    digitalWrite(LED_VERMELHO, LOW);   // Liga vermelho
  } else if (distanciaInt <= 15) {
    Serial.println("Médio");
    digitalWrite(LED_VERDE, LOW);      // Liga verde
    digitalWrite(LED_VERMELHO, LOW);   // Liga vermelho → combinação = "amarelo"
  } else {
    Serial.println("Vazio");
    digitalWrite(LED_VERDE, LOW);      // Liga verde
    digitalWrite(LED_VERMELHO, HIGH);  // Desliga vermelho
  }
}