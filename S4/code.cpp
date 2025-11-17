#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Servo.h>

// Configurações Wi-Fi
const char* ssid = "FIESC_IOT_EDU";
const char* password = "8120gv08";

// Configurações MQTT seguras (porta 8883, autenticação)
const char* mqtt_server = "broker.escola.com"; // URL do broker oficial (HiveMQ)
const int mqtt_port = 8883;                    // Porta MQTT segura TLS
const char* mqtt_user = "placa1-braian";       // Usuário HiveMQ
const char* mqtt_password = "Braian07";        // Senha HiveMQ

const char* nomeProjeto = "RAILTRACK";

// Pinos para controle do motor conforme esquema
#define MOTOR_PIN_21 21
#define MOTOR_PIN_18 18
#define LED_PIN 2   // LED para indicar status/PWM motor

WiFiClientSecure wifiClient;  
PubSubClient client(wifiClient);

// Configura PWM para motor (canal 0, freq 5kHz, resolução 8 bits)
void initPWM() {
  ledcSetup(0, 5000, 8);
  ledcAttachPin(LED_PIN, 0);
}

void setMotor(int speed) {
  // speed de -255 até 255
  if(speed > 0) {
    digitalWrite(MOTOR_PIN_21, HIGH);
    digitalWrite(MOTOR_PIN_18, LOW);
    ledcWrite(0, speed);
  } else if(speed < 0) {
    digitalWrite(MOTOR_PIN_21, LOW);
    digitalWrite(MOTOR_PIN_18, HIGH);
    ledcWrite(0, -speed);
  } else {
    digitalWrite(MOTOR_PIN_21, LOW);
    digitalWrite(MOTOR_PIN_18, LOW);
    ledcWrite(0, 0);
  }
}

// Callback MQTT para receber comandos do trem
void callback(char* topic, byte* payload, unsigned int length) {
  String messageTemp;
  for (unsigned int i = 0; i < length; i++) {
    messageTemp += (char)payload[i];
  }
  Serial.print("Mensagem recebida no tópico ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(messageTemp);

  String topControle = String(nomeProjeto) + "/Trem/Controle";

  if(String(topic) == topControle) {
    int speed = messageTemp.toInt();
    if(speed > 255) speed = 255;
    if(speed < -255) speed = -255;
    setMotor(speed);
  }
}

// Conecta WiFi
void setupWiFi() {
  Serial.print("Conectando em ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

// Reconecta MQTT
void reconnect() {
  while(!client.connected()){
    Serial.print("Conectando no broker MQTT...");
    if(client.connect("ESP32_Trem_RAILTRACK", mqtt_user, mqtt_password)){
      Serial.println("conectado");
      client.subscribe((String(nomeProjeto)+"/Trem/Controle").c_str());
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(". Tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(MOTOR_PIN_21, OUTPUT);
  pinMode(MOTOR_PIN_18, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  setupWiFi();

  // Configura certificado (se tiver), ou desativa verificação para teste:
  // wifiClient.setCACert(ca_cert_pem_start); // Usar certificado root CA se houver
  wifiClient.setInsecure();  // Somente para teste, aceita qualquer certificado

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  initPWM();
}

void loop() {
  if(!client.connected()){
    reconnect();
  }
  client.loop();

  // Pode adicionar leitura e publicação dos sensores se desejar...

  delay(100);
}
