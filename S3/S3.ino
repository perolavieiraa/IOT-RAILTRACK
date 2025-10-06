#include <WiFi.h>
const String SSID = "FIESC_IOT_EDU";
const String PASS = "8120gv08";

void setup() {
Serial.begin(115200);
Serial.println("Conectando ao WiFi"); 
WiFi.begin(SSID,PASS);
while(WiFi.status() != WL_CONNECTED){
  Serial.print(".");
  delay(200);
}
Serial.println ("\nConectado!");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED){
Serial.begin(115200);
Serial.println("Conectando ao WiFi"); 
WiFi.begin(SSID,PASS);
while(WiFi.status() != WL_CONNECTED){
  Serial.print(".");
  delay(200);
  }
}
