#include <Arduino.h>
#include <WiFi.h>

// =========================================
// CONFIGURAÇÕES DO WIFI
// =========================================
const char* WIFI_SSID = "uaifai-tiradentes";
const char* WIFI_PASSWORD = "bemvindoaocesar";

// =========================================
// PINOS DO SENSOR ULTRASSÔNICO
// =========================================
const int PINO_TRIG = 4;
const int PINO_ECHO = 2;

// =========================================
// PINO DO POTENCIÔMETRO
// =========================================
const int PINO_POT = 32;

// =========================================
// PINOS DO LED RGB
// =========================================
const int PINO_BLUE = 21;
const int PINO_GREEN = 22;
const int PINO_RED = 23;

// =========================================
// LIMITES DE TURBIDEZ
// =========================================
const int TURBIDEZ_LIMPA = 30;
const int TURBIDEZ_ATENCAO = 60;

// =========================================
// FUNÇÕES DO LED RGB
// =========================================
void apagarTodas() {
  digitalWrite(PINO_RED, LOW);
  digitalWrite(PINO_GREEN, LOW);
  digitalWrite(PINO_BLUE, LOW);
}

void acenderVermelho() {
  apagarTodas();
  digitalWrite(PINO_RED, HIGH);
}

void acenderVerde() {
  apagarTodas();
  digitalWrite(PINO_GREEN, HIGH);
}

void acenderAzul() {
  apagarTodas();
  digitalWrite(PINO_BLUE, HIGH);
}

// =========================================
// CONECTAR NO WIFI
// =========================================
void conectarWiFi() {
  Serial.println();
  Serial.println("====================================");
  Serial.println("Conectando ao Wi-Fi...");
  Serial.println("====================================");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Enquanto não conectar
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi conectado com sucesso!");
  Serial.print("IP do ESP32: ");
  Serial.println(WiFi.localIP());

  Serial.println("====================================");
}

// =========================================
// LEITURA DO SENSOR ULTRASSÔNICO
// =========================================
float medirDistancia() {
  digitalWrite(PINO_TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(PINO_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PINO_TRIG, LOW);

  long duracao = pulseIn(PINO_ECHO, HIGH, 30000);

  // Se não houver leitura
  if (duracao == 0) {
    return -1;
  }

  // Fórmula da distância em cm
  float distancia = (duracao * 0.0343) / 2.0;

  return distancia;
}

// =========================================
// LEITURA DA TURBIDEZ SIMULADA
// =========================================
int lerTurbidez() {
  int valorPot = analogRead(PINO_POT);

  // Converte de 0–4095 para 0–100
  int turbidez = map(valorPot, 0, 4095, 0, 100);

  return turbidez;
}

// =========================================
// DEFINE O STATUS DA ÁGUA
// =========================================
String obterStatusAgua(int turbidez) {
  if (turbidez <= TURBIDEZ_LIMPA) {
    acenderVerde();
    return "AGUA LIMPA";
  }

  if (turbidez <= TURBIDEZ_ATENCAO) {
    acenderAzul();
    return "ATENCAO";
  }

  acenderVermelho();
  return "AGUA TURVA / LIMPEZA NECESSARIA";
}

void setup() {
  // Inicializa Serial
  Serial.begin(9600);

  // Configura ultrassônico
  pinMode(PINO_TRIG, OUTPUT);
  pinMode(PINO_ECHO, INPUT);

  // Configura potenciômetro
  pinMode(PINO_POT, INPUT);

  // Configura LED RGB
  pinMode(PINO_RED, OUTPUT);
  pinMode(PINO_GREEN, OUTPUT);
  pinMode(PINO_BLUE, OUTPUT);

  apagarTodas();

  // Conecta no Wi-Fi
  conectarWiFi();
}

void loop() {
  // =========================================
  // MEDIÇÃO DE DISTÂNCIA
  // =========================================
  float distancia = medirDistancia();

  if (distancia >= 0) {
    Serial.print("Distancia: ");
    Serial.print(distancia);
    Serial.println(" cm");
  } else {
    Serial.println("Sem leitura do ultrassonico");
  }

  // =========================================
  // LEITURA DA TURBIDEZ
  // =========================================
  int turbidez = lerTurbidez();

  Serial.print("Turbidez simulada: ");
  Serial.println(turbidez);

  // =========================================
  // DEFINE STATUS DA ÁGUA
  // =========================================
  String statusAgua = obterStatusAgua(turbidez);

  Serial.print("Status: ");
  Serial.println(statusAgua);

  Serial.println("------------------------------------");

  delay(1000);
}