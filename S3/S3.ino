/* #include <WiFi.h>
#include <PubSubClient.h>

WiFiClient client;          
PubSubClient mqtt(client);  

const  SSID = "FIESC_IOT_EDU";
const  PASS = "8120gv08";

const int PORT = 1883;
const URL = "test.mosquitto.org";
const TOPIC = "DSM2";
const broker_user = "";
const broker_pass = "";

void setup() {
  Serial.begin(115200);
  Serial.println("Conectando ao WiFi");
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nConectado!");
  Serial.println("Conectando ao broker...");
  mqtt.setServer(URL, PORT);
  while (!mqtt.connected()) {
    String ID = "S1 - ";
    ID += String(random(0xffff), HEX);
    mqtt.connect(ID.c_str(), broker_user, broker_pass);
    delay(200);
    Serial.print(".");
  }
  Serial.println("\nConectado ao broker com sucesso!");
}

void loop() {
  String mensagem = "Perola";
  mensagem += "oi";
  mqtt.publish(TOPIC, mensagem.c_str());
  mqtt.loop();
  delay(2000);
} */

#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient client;
PubSubClient mqtt(client);

const SSID = "FIESC_IOT_EDU";
const PASS = "8120gv08";

const int PORT = 1883;
const URL = "test.mosquitto.org";
const MyTopic = "TopicoPerola";
const OtherTopic = "TopicoHelenaa";

void callback(char* topic, byte* payload, unsigned int length) {
  String mensagem = "";
  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }

  Serial.print("Mensagem recebida: ");
  Serial.println(mensagem);

  if (mensagem == "Acender") {
    digitalWrite(led, HIGH);
    Serial.println("LED ligado!");
  } else if (mensagem == "Apagar") {
    digitalWrite(led, LOW);
    Serial.println("LED desligado!");
  }
}

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(115200);

  Serial.println("Conectando ao WiFi...");
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }

  Serial.println("\nWiFi conectado!");
  mqtt.setServer(URL, PORT);
  mqtt.setCallback(callback);

  Serial.println("Conectando ao broker MQTT...");
  while (!mqtt.connected()) {
    String ID = "ESP32-";
    ID += String(random(0xffff), HEX);
    if (mqtt.connect(ID.c_str())) {
      Serial.println("\nConectado ao broker!");
    } else {
      Serial.print(".");
      delay(500);
    }
  }

  mqtt.subscribe(MyTopic);
  Serial.println("Inscrito no tópico com sucesso!");
}

void loop() {
static unsigned long ultimoTempo = 0;
  if (millis() - ultimoTempo > 5000) {
    ultimoTempo = millis();
    String mensagem = "Temperatura: 25°C";
    mqtt.publish(OtherTopic, mensagem.c_str());
    Serial.println("Mensagem enviada: " + mensagem);
  }

  mqtt.loop();
}