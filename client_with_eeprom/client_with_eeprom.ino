#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>

const char* ssid = "XDA";
const char* password = "namaguee";
const char* mqtt_server = "11.11.11.109";

#define ledblue 12
#define ledred 0
#define ledgreen 14
#define relay 2

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
int value = 0;
int readeeprom;
int writeeeprom;
int alamat = 0;

void setup_wifi() {
  relaycontrol();
  delay(10);
  Serial.println("Client Start");
  Serial.println();
  Serial.print("Menyambungkan ke wifi : ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(ledblue, HIGH);
    delay(50);
    digitalWrite(ledblue, LOW);
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
  blinkingblue();
  if ((char)payload[0] == '1') {
    EEPROM.begin(32);
    writeeeprom = 1;
    EEPROM.write(alamat, writeeeprom);
    EEPROM.commit();
    client.publish("report","Client state = 1");
    Serial.print("isi eeprom = ");
    Serial.println(EEPROM.read(alamat));
    
  } 
  if ((char)payload[0] == '0') {
    EEPROM.begin(32);
    writeeeprom = 0;
    EEPROM.write(alamat, writeeeprom);
    EEPROM.commit();
    client.publish("report","Client state = 0");
    Serial.print("isi eeprom = ");
    Serial.println(EEPROM.read(alamat));

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
      digitalWrite(ledgreen, LOW);
      digitalWrite(ledblue, LOW);
      client.subscribe("skripsi");
    } else {
      Serial.print("Koneksi broker gagal, rc=");
      
      Serial.println(client.state());
      Serial.println("Menghubungkan kembali");
      digitalWrite(ledgreen, HIGH);
      digitalWrite(ledblue, HIGH);
      delay(5000);
//      digitalWrite(ledred, HIGH);
    }
  }
}
void setup() {
  EEPROM.begin(512);
  pinMode(ledblue, OUTPUT);
  pinMode(ledred, OUTPUT);
  pinMode(ledgreen, OUTPUT);
  pinMode(relay, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
}

void loop() {
  relaycontrol();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    ++value;
    Serial.println("1 Report Online ");
    client.publish("reportonline", "1 Client Online");
    blinkingteal();
  }
}

void relaycontrol(){
  if (EEPROM.read(alamat) == 1){
    digitalWrite(ledred, HIGH);
    digitalWrite(relay, LOW);
//    Serial.println("relay nyala");
    }
  if (EEPROM.read(alamat) == 0){
    digitalWrite(ledred, LOW);
    digitalWrite(relay, HIGH);
//    Serial.println("relay mati");
    }
    }

void blinkingteal(){
    digitalWrite(ledgreen, HIGH);
    delay(50);
    digitalWrite(ledgreen, LOW);
  }
  
void blinkingblue(){
    digitalWrite(ledblue, HIGH);
    delay(50);
    digitalWrite(ledblue, LOW);
  }
void blinkingred(){
    digitalWrite(ledred, HIGH);
    delay(50);
    digitalWrite(ledred, LOW);
  }
