#include <SoftwareSerial.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

#define pinGreenLed 16
#define pinRedLed 4
#define pinButton 0

const char* ssid = "WIFI_ALUNOS";
const char* password = "WIFIgps123";
const char* serverIP = "192.168.0.100"; // Substitua pelo IP do seu servidor Flask

SoftwareSerial mySerial(12, 14); // RX, TX

void setup() {
  Serial.begin(9600);

  pinMode(pinRedLed, OUTPUT);
  pinMode(pinGreenLed, OUTPUT);
  pinMode(pinButton, INPUT);

  // Inicialize a comunicação serial
  mySerial.begin(57600);

  // Conecte-se ao WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (digitalRead(pinButton) == LOW) {
    createUser();
  } else {
    openDoor();
  }
}

void createUser() {
  digitalWrite(pinRedLed, LOW);
  digitalWrite(pinGreenLed, HIGH);

  int fingerCode = getFingerprintID();
  while (fingerCode == -1) {
    fingerCode = getFingerprintID();
    digitalWrite(pinGreenLed, digitalRead(pinGreenLed) == HIGH ? LOW : HIGH);
  }

  Serial.println("Fingerprint code: " + String(fingerCode));

  String jsonData = "{\"fingerprint_code\": " + String(fingerCode) + "}";

  WiFiClient client;
  HTTPClient http;

  // Faça a solicitação POST para o servidor Flask
  String url = "http://" + String(serverIP) + "/create_user";
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");

  Serial.println("Sending POST request...");
  int httpResponseCode = http.POST(jsonData);

  if (httpResponseCode > 0) {
    Serial.println("Account created successfully!");
  } else {
    Serial.println("Error creating account. HTTP response code: " + String(httpResponseCode));
  }

  http.end();
  digitalWrite(pinGreenLed, LOW);
}

void openDoor() {
  int fingerCode = getFingerprintID();
  if (fingerCode == -1) {
    digitalWrite(pinRedLed, LOW);
    return;
  }

  digitalWrite(pinRedLed, HIGH);
  delay(3000);
  digitalWrite(pinRedLed, LOW);
}

int getFingerprintID() {
  // Envie os comandos necessários para obter o código da impressão digital do sensor
  mySerial.write(0xEF);
  mySerial.write(0x01);
  mySerial.write(0xFF);
  mySerial.write(0xFF);
  mySerial.write(0xFF);
  mySerial.write(0xFF);
  mySerial.write(0x01);
  mySerial.write(0x00);

  // Aguarde a resposta do sensor
  while (mySerial.available() < 9)
    ;

  if (mySerial.read() != 0xEF) return -1;
  if (mySerial.read() != 0x01) return -1;

  int len = mySerial.read();
  len <<= 8;
  len |= mySerial.read();

  if (mySerial.read() != 0x00) return -1;

  uint8_t high = mySerial.read();
  uint8_t low = mySerial.read();
  int fingerCode = high << 8 | low;

  mySerial.read();
  mySerial.read();

  return fingerCode;
}
