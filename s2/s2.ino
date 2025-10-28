 #include <WiFi.h>
#include <PubSubClient.h>

WiFiClient client;          
PubSubClient mqtt(client);

const string broker_user = "";
const string broker_pass = "";

const string MyTopic = "TopicoHelenaa";
const string OtherTopic = "TopicoPerola";

void setup(){
  Serial.begin(115200);
  Serial.println("Conectando ao WiFi");
  WiFi.begin(SSID,PASS);
  while (WiFi.status() !=WL_CONNECTED){
    Serial.print(".");
    delay (200);
  }
  Serial.println("\nConectado!")
  Serial.println ("Conectando ao broker...");]
  mqtt.setServer(URL.c_str(), PORT);
  while(!mqtt.connected()){
    String ID = "S1-"
    ID+= String (Random(0xffff),HEX);
    mqtt.connect(ID.c_str(),broker_user.c_str(),broker_pass.c_str());
  }
}
 
void loop() {
String mensagem = "Helena: ";
if(Serial.available()>0){
mensagem += Serial.readStringUntil('\n');
mqtt.publish(OtherTopic.c_str(), mensagem.c_str()); 
}
mqtt.loop();
delay(2000);
I
}
void callback(char* topic, byte* payload, unsigned int length) {
String mensagem = "";
for(int i = 0 i < length; i++){
mensagem += (char) payload[i];
}
Serial.print("Recebido: ");
Serial.println(mensagem);
}