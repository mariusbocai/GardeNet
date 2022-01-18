#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <stdio.h>
#include <stdlib.h>

const char* ssid = "GardeNet1";
const char* password = "SolariileMaAn1";
const char* mqtt_server = "192.168.0.79";

WiFiClient espClient;
PubSubClient client(espClient);

// defines pins numbers
const int PumpPin = 16;  //D0
const int AuxPin = 5;  //D1

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
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
  digitalWrite(BUILTIN_LED, LOW);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  /*Check if there is a pump request*/
  if (strcmp(topic,"/Level/Query")==0) {
    /*Yep, it's a request*/
    if ((char)payload[0] == '1')
    {
      
    }
  }
}

void reconnect() {

  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("/Level/Query");
      client.subscribe("/Level/Status");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(PumpPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(AuxPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(9600); // Starts the serial communication
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
