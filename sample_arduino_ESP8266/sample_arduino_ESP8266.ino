#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT22  

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = ""; //change with your ssid name
const char* password = ""; //change with your ssid password
const char* mqtt_server = "broker.hivemq.com";
const char* mqtt_topic = "greenhouse/dev/suhu/001";
const char* mqtt_username = "";
const char* mqtt_password = "";
const int mqtt_port = 1883;
String deviceSerial = "dev-suhu-001";
String deviceMAC;

float Analog0, Analog1, Analog2, Analog3;

StaticJsonBuffer<800> jsonBufferIdentity;
JsonObject& identity = jsonBufferIdentity.createObject();
StaticJsonBuffer<800> jsonBufferSensing;
JsonObject& sensing = jsonBufferSensing.createObject();

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(100);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("MAC address: ");
  Serial.println(WiFi.macAddress());
  deviceMAC = WiFi.macAddress();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266_www.kijang.id";
    clientId += String(random(0xffff), HEX);
    //if you MQTT broker has clientID,username and password
    //please change following line to    if (client.connect(clientId.c_str(),mqtt_username,mqtt_password)) {
    //if you MQTT broker has clientID
    //please change following line       if (client.connect(clientId.c_str())) {
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(6000);
    }
  }
} 

void setup() {
  Serial.begin (115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

String getIp() {
  WiFiClient client;
  if (client.connect("api.ipify.org", 80)) {
      Serial.println("connected");
      client.println("GET / HTTP/1.0");
      client.println("Host: api.ipify.org");
      client.println();
  } else {
      Serial.println("Connection to ipify.org failed");
      return String();
  }
  delay(1000);
  String line;
  while(client.available()) {
    line = client.readStringUntil('\n');
    Serial.println(line);
  }
  return line;
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  Analog0 = dht.readTemperature();
  Analog1 = dht.readHumidity();
 // Analog2 = analogRead(A0) + 20;
 // Analog3 = analogRead(A0) + 30;

  identity["serial"] = deviceSerial;
  identity["mac_address"] = deviceMAC;
  identity["ip_address"] = getIp();

  sensing["data1"] = Analog0;
  sensing["data2"] = Analog1;
  //sensing["data3"] = Analog2;
  //sensing["data4"] = Analog3;

  char JSONidentityBuffer[1000];
  identity.printTo(JSONidentityBuffer, sizeof(JSONidentityBuffer));

  char JSONsensingBuffer[1000];
  sensing.printTo(JSONsensingBuffer, sizeof(JSONsensingBuffer));

  if (Analog0 > 25) {
    client.publish("greenhouse/dev/sonoff/001", "1");
  }else {
    client.publish("greenhouse/dev/sonoff/001", "0");
  }

  Serial.println(JSONidentityBuffer);
  Serial.println(JSONsensingBuffer);
  
  delay(5000);
  client.publish(mqtt_topic, JSONidentityBuffer);
  client.publish(mqtt_topic, JSONsensingBuffer);
}
