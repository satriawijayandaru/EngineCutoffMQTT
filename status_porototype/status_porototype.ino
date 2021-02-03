#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Synology";
const char* password = "namaguee";
const char* mqtt_server = "11.11.11.150";

#define internalLED 2

WiFiClient espClient;
PubSubClient client(espClient);

int value = 0;
long lastMsg = 0;


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Menyambungkan ke wifi : ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(internalLED, LOW);
    delay(50);
    digitalWrite(internalLED, HIGH);
  }
  Serial.println("");
  Serial.println("WiFi Tersambung");
  Serial.println("Alamat IP MQTT Client : ");
  Serial.println(WiFi.localIP()); 
}
void callback(char* topic, byte* payload, unsigned int length) {
  
  if (strcmp(topic,"report")==0){
      Serial.print("Pesan diterima pada topik [");
      Serial.print(topic);
      Serial.print("] -> ");
      for (int i = 0; i < length; i++) {
          Serial.print((char)payload[i]);
      }
      Serial.println();
      blinkingstatus();
  }
  if (strcmp(topic,"reportonline")==0){
      if ((char)payload[0] == '1') {
        for (int i = 0; i < length; i++) {
          Serial.print((char)payload[i]);
      }
        Serial.println("");
        blinkingstatus();
      }     
  }  
}
void reconnect() {
  while (!client.connected()) {
    Serial.println("Menyambungkan ke MQTT broker");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX); 
    if (client.connect(clientId.c_str())) {
      Serial.println("MQTT broker tersambung");
      Serial.println("Alamat IP MQTT Broker : ");
      Serial.println(mqtt_server);
      client.publish("reportonline","MQTT Monitor Online");
      client.subscribe("skripsi");
      client.subscribe("report");
      client.subscribe("reportonline");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}
void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
void blinkingstatus(){
    digitalWrite(internalLED, LOW);
    delay(100);
    digitalWrite(internalLED, HIGH);
  }

void reportonlinestatusled(){
    long now = millis();    
    digitalWrite(internalLED, LOW);
    if (now - lastMsg > 2000) {
        lastMsg = now;
        Serial.println("Client Offline!!!");
        digitalWrite(internalLED, HIGH);
  }
}
