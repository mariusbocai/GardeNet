#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <stdio.h>
#include <stdlib.h>

/*Functinality switches here*/
#define useWdt 0

/*Ok, let's start!*/
#if useWdt
EspClass ESPClass;
unsigned char WdtStart = 0;
unsigned long wdtTimeVal = 0;
#endif

// Update these with values suitable for your network.
//const char* ssid = "Tenda_828F60";
//const char* password = "headtable315";

//const char* ssid = "GardeNet";
//const char* password = "SolariileMaAn";
const char* ssid = "GardeNet1";
const char* password = "SolariileMaAn1";
//const char* ssid = "GardeNet2";
//const char* password = "SolariileMaAn2";
//const char* ssid = "Tenda_2EC6E0";
//const char* password = "gamechair955";
const char* mqtt_server = "192.168.0.79";

WiFiClient espClient;
PubSubClient client(espClient);
int RelayPin1 = 4;  //D2
int RelayPin2 = 0; //D3

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
  if (strcmp(topic,"/Pump/Control")==0) {
    /*Yep, it's a request*/
    if ((char)payload[0] == '1')
    {

    }
    else
    {

    }
  }
#if useWdt
  else if(strcmp(topic,"WatchdogFeed")==0)
  {
    wdtTimeVal = millis(); /*Store the time of the last Wdt trigger*/
    WdtStart = 1;
  }
#endif
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
      // Once connected, publish an announcement...
      //client.publish("/Pump/Status", "Reconectat");
      // ... and resubscribe
      //client.subscribe("/Pump/Control");
      #if useWdt
      client.subscribe("WatchdogFeed");
      #endif
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in a second");
      // Wait 1 second before retrying
      delay(1000);
    }
  }
}

void setup() {
  pinMode(RelayPin1, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(RelayPin2, OUTPUT);
  //pinMode(buttonPin, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}


void loop() {

  if (WiFi.status() != WL_CONNECTED)
  {
   setup_wifi();
  }
  else
  {
    if (!client.connected()) 
    {
      reconnect();
    }
  }
  client.loop();
  /*Watchdog functioanlity*/
  #if useWdt
  if(WdtStart == 1)
  {
    unsigned long wdtNow;
    wdtNow = millis();
    if(wdtNow>wdtTimeVal)
    {
      if((wdtNow - wdtTimeVal)>10000)
      {
        ESPClass.wdtDisable();
        while(1) {}; /*Endless loop to trigger a reset*/
      }
    }
  }
  #endif
 
  /*take a break for 1 second*/
  //delay(1000);
}
