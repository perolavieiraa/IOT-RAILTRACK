#include <WiFi.h>
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
  String mensagem = "Helena";
  mensagem += "oi";
  mqtt.publish(TOPIC, mensagem.c_str());
  mqtt.loop();
  delay(2000);
}