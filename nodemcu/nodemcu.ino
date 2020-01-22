
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <PubSubClient.h>

#define DHTPIN 2
#define DHTTYPE DHT22 

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "AndroidAPF9C3";
const char* password = "aqmal2000";

const char* mqttServer = "tailor.cloudmqtt.com";
const int mqttPort = 10690;
const char* mqttUser = "uszadmvh";
const char* mqttPassword = "Ui9_Zhpy_us8";

WiFiClient espClient;
PubSubClient client(espClient);

String temp_str; 
String hum_str;
char temp[50];
char hum[50];

const long interval = 2000;  
unsigned long previousMillis = 0;    // will store last time DHT was updated

void setup(){
  
  Serial.begin(9600);
  dht.begin();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
  Serial.println();
  //client.subscribe("sensor/temp");
  //client.subscribe("sensor/humi");
  client.subscribe("sonoff/relay");
}


void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
  Serial.println("-----------------------");
 
}


void loop() {
  
  client.loop();
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
    
    float newT = dht.readTemperature();
    
    float newH = dht.readHumidity();


    if (isnan(newT)||isnan(newH)) {
      Serial.println("Failed to read from sensor!");
    }
    else {
      float t = newT;
      if (t>25){
        client.publish("sonoff/relay", "1");
      }
      else{
        client.publish("sonoff/relay", "0");
      }
      
      temp_str = String(t); //converting ftemp (the float variable above) to a string
      temp_str.toCharArray(temp, temp_str.length() + 1); //packaging up the data to publish to mqtt whoa
      Serial.println(t);
      
      float h = newH;
    
      hum_str = String(h); //converting Humidity (the float variable above) to a string
      hum_str.toCharArray(hum, hum_str.length() + 1); //packaging up the data to publish to mqtt whoa
      Serial.println(h);
      
      //client.publish("sensor/humi", hum);
      //client.publish("sensor/temp", temp);
    }
  }

  

}
