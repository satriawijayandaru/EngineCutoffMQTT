#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Satria Wijayandaru";
const char* password = "12345678";
const char* mqtt_server = "192.168.8.111";

#define internalLED 2
#define relay 14

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
int value = 0;

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
  Serial.print("Pesan diterima pada topik [");
  Serial.print(topic);
  Serial.print("] -> ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  blinkingstatus();
  if ((char)payload[0] == '1') {
    digitalWrite(internalLED, LOW);
    digitalWrite(relay, LOW);
    Serial.println("Publish ke topik [report] -> Relay Aktif");
    client.publish("report", "Sukses! - Relay Aktif");
  } if ((char)payload[0] == '0') {
    digitalWrite(internalLED, HIGH);
    digitalWrite(relay, HIGH);
    Serial.println("Publish ke topik [report] -> Relay Non Aktif");
    client.publish("report", "Sukses! - Relay Non Aktif");
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
      client.subscribe("satria");
    } else {
      Serial.print("Koneksi broker gagal, rc=");
      Serial.println(client.state());
      Serial.println("Menghubungkan kembali");
      delay(5000);
    }
  }
}
void setup() {
  pinMode(internalLED, OUTPUT);
  pinMode(relay, OUTPUT);
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
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
//    Serial.println("1 Report Online ");
//    client.publish("reportonline", "1 Client Online");
//    blinkingstatus();
  }
}
void blinkingstatus(){
    digitalWrite(internalLED, LOW);
    delay(100);
    digitalWrite(internalLED, HIGH);
  }
