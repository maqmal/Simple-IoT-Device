#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Rumah Apple";
const char* password = "apple123home";

int gpio_13_led = 13;
int gpio_12_relay = 12;

WiFiClient espClient;
PubSubClient client(espClient);

const char* mqttServer = "tailor.cloudmqtt.com";
const int mqttPort = 10690;
const char* mqttUser = "uszadmvh";
const char* mqttPassword = "Ui9_Zhpy_us8";


void setup(void){
  pinMode(gpio_13_led, OUTPUT);
  digitalWrite(gpio_13_led, HIGH);
  
  pinMode(gpio_12_relay, OUTPUT);
  digitalWrite(gpio_12_relay, HIGH);
  
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(gpio_13_led, LOW);
    delay(500);
    Serial.print(".");
    digitalWrite(gpio_13_led, HIGH);
    Serial.println("Connecting to WiFi..");
    delay(500);
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
  client.subscribe("sonoff/relay");
}

/* > on    
    digitalWrite(gpio_13_led, LOW);
    digitalWrite(gpio_12_relay, HIGH);
    delay(1000);
*/

/* < off
    digitalWrite(gpio_13_led, HIGH);
    digitalWrite(gpio_12_relay, LOW);
    delay(1000); 
*/ 

void callback(char* topic, byte* payload, unsigned int length) {
 
  
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
 
  String message;
  message = (char)payload[0];  
  Serial.print(message);
  if(message == "0") {
    digitalWrite(gpio_13_led, LOW);
    digitalWrite(gpio_12_relay, HIGH);
    delay(1000);
  }
  if(message == "1") {
    digitalWrite(gpio_13_led, HIGH);
    digitalWrite(gpio_12_relay, LOW);
    delay(1000); 
  } 
 
  Serial.println();
  Serial.println("-----------------------");  
}


void loop() { 
  client.loop();
}
