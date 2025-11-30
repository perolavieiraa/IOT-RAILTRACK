#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Servo.h>

// Configurações Wi-Fi
const char* ssid = "FIESC_IOT_EDU";
const char* password = "8120gv08";

// Configurações MQTT
const char* mqtt_server = "broker.escola.com"; //URL do hivemq
const int mqtt_port = 8883;                    //PORT do hivemq
const char* mqtt_user = "placa2-helena";       //Usuário criado no hivemq
const char* mqtt_password = "Helena2025";      //Senha criada no hivemq

const int presenca2Pin = 14; 
const int presenca4Pin = 27;  
const int ledIluminacaoPin = 2;  
Servo servo3;  
const int servo3Pin = 13;

 const char* topic_presenca2 = "Railtrack/Sensores/Presenca2";
const char* topic_presenca4 = "Railtrack/Sensores/Presenca4";
const char* topic_led = "Railtrack/Atuadores/S2/LED"; 
const char* topic_servo = "Railtrack/Atuadores/S2/Servo3";

WiFiClientSecure espClient;
PubSubClient client(espClient);

unsigned long lastPublishTime = 0;
const unsigned long publishInterval = 3000;  

void setup_wifi() {
  delay(10);
  Serial.println("Conectando ao Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Mensagem no tópico ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(message);

  if (String(topic) == topic_led) {
    if (message == "1") {
      digitalWrite(ledIluminacaoPin, HIGH);
      Serial.println("LED Iluminação ligado");
    } else if (message == "0") {
      digitalWrite(ledIluminacaoPin, LOW);
      Serial.println("LED Iluminação desligado");
    }
  } else if (String(topic) == topic_servo) {
    int angle = message.toInt();
    if (angle >= 0 && angle <= 180) {
      servo3.write(angle);
      Serial.print("Servo3 movido para ângulo: ");
      Serial.println(angle);
    } else {
      Serial.println("Ângulo inválido para servo");
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");
    if (client.connect("ESP32_S2", mqtt_user, mqtt_password)) {
      Serial.println("Conectado");
      client.subscribe(topic_led);
      client.subscribe(topic_servo);
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5s");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  client.setInsecure();
  pinMode(presenca2Pin, INPUT);
  pinMode(presenca4Pin, INPUT);
  pinMode(ledIluminacaoPin, OUTPUT);
  digitalWrite(ledIluminacaoPin, LOW);

  servo3.attach(servo3Pin);
  servo3.write(0);  // Posição inicial

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long currentTime = millis();
  if (currentTime - lastPublishTime > publishInterval) {
    lastPublishTime = currentTime;

    int presenca2 = digitalRead(presenca2Pin);
    int presenca4 = digitalRead(presenca4Pin);

    client.publish(topic_presenca2, presenca2 ? "1" : "0");
    client.publish(topic_presenca4, presenca4 ? "1" : "0");

    Serial.print("Presença 2: ");
    Serial.println(presenca2);
    Serial.print("Presença 4: ");
    Serial.println(presenca4);
  }
}
