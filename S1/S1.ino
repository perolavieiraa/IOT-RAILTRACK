// CÓDIGO S1 - SA RAILTRACK 

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Servo.h>

const char* ssid = "FIESC_IOT_EDU";
const char* password = "8120gv08";

const char* mqtt_server = "f518a08331f047ffaba17f3a43c5066c.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "placa1-heloysa";
const char* mqtt_password = "Heloysa2025";

WiFiClientSecure espClient;
PubSubClient client(espClient);

const int pinTemp = 34;
const int pinUmid = 35;
const int pinLuz  = 32;
const int pinPresenca1 = 33;

const int ledIlumPin = 2;
const int ledRPin = 25;
const int ledGPin = 26;
const int ledBPin = 27;

const char* topicTemp = "RAILTRACK/S1/Temp";
const char* topicUmid = "RAILTRACK/S1/Umid";
const char* topicLum = "RAILTRACK/S1/Iluminacao";
const char* topicPres = "RAILTRACK/S1/Presenca1";

const char* topicLedIlumComando = "RAILTRACK/Atuadores/S1/LED_Iluminacao";
const char* topicLedStatusRGBComando = "RAILTRACK/Atuadores/S1/LED_StatusRGB";

long lastSendTime = 0;
const long sendInterval = 5000;

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32_S1", mqtt_user, mqtt_password)) {
      client.subscribe(topicLedIlumComando);
      client.subscribe(topicLedStatusRGBComando);
    } else {
      delay(5000);
    }
  }
}

void setRgbLed(char color) {
  digitalWrite(ledRPin, LOW);
  digitalWrite(ledGPin, LOW);
  digitalWrite(ledBPin, LOW);
  switch (color) {
    case 'R': digitalWrite(ledRPin, HIGH); break;
    case 'G': digitalWrite(ledGPin, HIGH); break;
    case 'B': digitalWrite(ledBPin, HIGH); break;
    default: break;
  }
}

float readTemperature() {
  int val = analogRead(pinTemp);
  float voltage = val * (3.3 / 4095.0);
  float tempC = (voltage - 0.5) * 100.0;
  return tempC;
}

float readHumidity() {
  int val = analogRead(pinUmid);
  float umidPercent = (val / 4095.0) * 100.0;
  return umidPercent;
}

int readLuminosity() {
  int val = analogRead(pinLuz);
  return val;
}

bool readPresence() {
  return digitalRead(pinPresenca1) == HIGH;
}

void callback(char* topic, byte* payload, unsigned int length) {
  String messageTemp;
  for (unsigned int i = 0; i < length; i++) {
    messageTemp += (char)payload[i];
  }
  messageTemp.trim();

  if (String(topic) == String(topicLedIlumComando)) {
    if (messageTemp == "1" || messageTemp.equalsIgnoreCase("on")) {
      digitalWrite(ledIlumPin, HIGH);
    } else if (messageTemp == "0" || messageTemp.equalsIgnoreCase("off")) {
      digitalWrite(ledIlumPin, LOW);
    }
  }
  else if (String(topic) == String(topicLedStatusRGBComando)) {
    if(messageTemp.length() == 1) {
      char c = toupper(messageTemp.charAt(0));
      if (c == 'R' || c == 'G' || c == 'B') {
        setRgbLed(c);
      } else if (c == 'O' || messageTemp.equalsIgnoreCase("off")) {
        setRgbLed('X');
      }
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(pinTemp, INPUT);
  pinMode(pinUmid, INPUT);
  pinMode(pinLuz, INPUT);
  pinMode(pinPresenca1, INPUT);

  pinMode(ledIlumPin, OUTPUT);
  digitalWrite(ledIlumPin, LOW);

  pinMode(ledRPin, OUTPUT);
  pinMode(ledGPin, OUTPUT);
  pinMode(ledBPin, OUTPUT);
  setRgbLed('X');

  setup_wifi();

  espClient.setInsecure();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastSendTime > sendInterval) {
    lastSendTime = now;

float temp = readTemperature();
    float umid = readHumidity();
    int lum = readLuminosity();
    bool pres = readPresence();

    char buf[16];

    dtostrf(temp, 6, 2, buf);
    client.publish(topicTemp, buf);

    dtostrf(umid, 6, 2, buf);
    client.publish(topicUmid, buf);

    itoa(lum, buf, 10);
    client.publish(topicLum, buf);

    client.publish(topicPres, pres ? "1" : "0");

    Serial.print("Dados enviados - Temp: "); Serial.print(buf);
    Serial.print(", Umid: "); Serial.print(umid);
    Serial.print(", Lum: "); Serial.print(lum);
    Serial.print(", Presenca1: "); Serial.println(pres ? "1" : "0");
  }
}