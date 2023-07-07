#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

#define pinGreenLed 16
#define pinRedLed 4
#define pinButton 0

const char* ssid = "WIFI_ALUNOS";
const char* password = "WIFIgps123";

SoftwareSerial mySerial(12, 14); // RX, TX
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup() {
  Serial.begin(9600);

  pinMode(pinRedLed, OUTPUT);
  pinMode(pinGreenLed, OUTPUT);
  pinMode(pinButton, INPUT);

  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) {
      digitalWrite(pinRedLed, digitalRead(pinRedLed) == HIGH ? LOW : HIGH);
      delay(1); 
    }
  }

  // Connect to WiFi
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
  Serial.println("aa");
  if (digitalRead(pinButton) == LOW) {
    createUser();
  } else {
    openDoor();
  }
}

int getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  return finger.fingerID;
}

void createUser() {
  digitalWrite(pinRedLed, LOW);
  digitalWrite(pinGreenLed, HIGH);

  int fingerprintCode = getFingerprintID();
  while (fingerprintCode == -1) {
    fingerprintCode = getFingerprintID();
    digitalWrite(pinGreenLed, digitalRead(pinGreenLed) == HIGH ? LOW : HIGH);
    delay(300);
  }

  Serial.println(fingerprintCode);

  WiFiClient client;
  HTTPClient http;

  String url = "http://10.0.0.138:5000/create_user";
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");

  String jsonData = "{\"fingerprint_code\": " + String(fingerprintCode) + "}";
  int httpResponseCode = http.POST(jsonData);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("User created successfully.");
    Serial.println(response);
  } else {
    Serial.println("Error creating user. HTTP response code: " + String(httpResponseCode));
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

  WiFiClient client;
  HTTPClient http;

  String url = "http://10.0.0.138:5000/unlock";
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");

  String jsonData = "{\"user_id\": " + String(fingerCode) + "}";
  int httpResponseCode = http.POST(jsonData);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Door unlocked and access time added.");
    Serial.println(response);
  } else {
    Serial.println("Error unlocking door and adding access time. HTTP response code: " + String(httpResponseCode));
  }

  http.end();

  delay(3000);
  digitalWrite(pinRedLed, LOW);
}