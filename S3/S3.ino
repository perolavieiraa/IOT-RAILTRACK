// CÓDIGO S3 - SA RAILTRACK

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Servo.h>

const char* ssid = "FIESC_IOT_EDU";
const char* password = "8120gv08";

const char* mqtt_server = "f518a08331f047ffaba17f3a43c5066c.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "placa3-perola";
const char* mqtt_password = "Perola2025";

WiFiClientSecure espClient;
PubSubClient client(espClient);

const int pinSensorPresenca = 15;
const int pinLEDIlum = 2;
const int pinServo1 = 4;
const int pinServo2 = 5;

Servo servo1;
Servo servo2;

bool ledState = false;
int posServo1 = 0;
int posServo2 = 0;
int sensorPresenca = 0;

const char* topicPresenca = "RAILTRACK/S3/Presenca";
const char* topicServo1 = "RAILTRACK/S3/Servo1";
const char* topicServo2 = "RAILTRACK/S3/Servo2";
const char* topicILum = "RAILTRACK/S3/ILum";
const char* topicStatusLED = "RAILTRACK/S3/StatusLED";

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String messageTemp;
  for (unsigned int i = 0; i < length; i++) {
    messageTemp += (char)payload[i];
  }

  if (String(topic) == topicILum) {
    if (messageTemp == "1") {
      digitalWrite(pinLEDIlum, HIGH);
      ledState = true;
    } else if (messageTemp == "0") {
      digitalWrite(pinLEDIlum, LOW);
      ledState = false;
    }
    client.publish(topicStatusLED, ledState ? "1" : "0");
  }

  if (String(topic) == topicServo1) {
    int pos = messageTemp.toInt();
    if (pos >= 0 && pos <= 180) {
      servo1.write(pos);
      posServo1 = pos;
    }
  }

  if (String(topic) == topicServo2) {
    int pos = messageTemp.toInt();
    if (pos >= 0 && pos <= 180) {
      servo2.write(pos);
      posServo2 = pos;
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32_S3_Client", mqtt_user, mqtt_password)) {
      client.subscribe(topicILum);
      client.subscribe(topicServo1);
      client.subscribe(topicServo2);
    } else {
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(pinLEDIlum, OUTPUT);
  digitalWrite(pinLEDIlum, LOW);

  pinMode(pinSensorPresenca, INPUT);

  servo1.attach(pinServo1);
  servo2.attach(pinServo2);

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

  int presencaAtual = digitalRead(pinSensorPresenca);
  if (presencaAtual != sensorPresenca) {
    sensorPresenca = presencaAtual;
    client.publish(topicPresenca, sensorPresenca ? "1" : "0");
  }

  delay(1000);
}