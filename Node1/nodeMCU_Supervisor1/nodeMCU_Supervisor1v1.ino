#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <stdio.h>
#include <stdlib.h>

/*Functinality switches here*/
#define useWdt 0
#define loggingEn 1

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

/*Working variables*/
unsigned char tokenEn, tokenIsMine;
unsigned long now, nextExpectedAct;

enum States{
  Idle =0,
  StartUp,
  Waiting,
  EndAll};

WiFiClient espClient;
PubSubClient client(espClient);
int RelayPin1 = 4;  //D2
int RelayPin2 = 0; //D3

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  #if loggingEn
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  #endif
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    #if loggingEn
    Serial.print(".");
    #endif
  }

  randomSeed(micros());
  #if loggingEn
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  #endif
  digitalWrite(BUILTIN_LED, LOW);
}

void callback(char* topic, byte* payload, unsigned int length) {
  #if loggingEn
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  #endif
  if (strcmp(topic,"TokenAlive")==0) {
    /*Yep, it's a request*/
    if ((char)payload[0] == '1')
    {
      /*in this case, start the Token algortihm;*/
      tokenEn = 1;
      now = millis();
      nextExpectedAct = now + 3600000;
    }
    else
    {
      tokenEn = 0;
    }
  }
  else if(strcmp(topic,"TokenPass")==0)
  {
    if ((char)payload[0] == '1')
    {
      tokenIsMine = 1;
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
    #if loggingEn
    Serial.print("Attempting MQTT connection...");
    #endif
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      #if loggingEn
      Serial.println("connected");
      #endif
      client.subscribe("TokenAlive");
      client.subscribe("TokenPass");
      #if useWdt
      client.subscribe("WatchdogFeed");
      #endif
    } else {
      #if loggingEn
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in a second");
      #endif
      // Wait 1 second before retrying
      delay(1000);
    }
  }
}

void setup() {
  pinMode(RelayPin1, OUTPUT); //this will be the Router Reset Relay
  pinMode(RelayPin2, OUTPUT);
  //pinMode(buttonPin, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
  digitalWrite(RelayPin1,LOW);
  digitalWrite(RelayPin2,LOW);
  tokenEn = 0;
  tokenIsMine = 0;
  now = 0;
  nextExpectedAct = 0;
  #if loggingEn
  Serial.begin(115200);
  #endif
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void StateMachine(void)
{
  static unsigned char currentState = 0;

  switch(currentState)
  {
    case Idle:
    {
      if(tokenEn == 1)
      {
        currentState = StartUp;
      }
      break;
    }
    case StartUp:
    {
      client.publish("TokenPass", "2"); /*Pass the token to Node2*/
      tokenIsMine = 0;
      currentState = Waiting;
      break;
    }
    case Waiting:
    {
      now = millis();
      if (now > nextExpectedAct)
      {
        currentState = Idle;
        /*No reply received, a reset will be performed!!!!*/
        digitalWrite(RelayPin1,HIGH);
        delay(1000);    /*1 second reset should be enough*/
        digitalWrite(RelayPin1,LOW);
      }
      else
      {
        if(tokenIsMine == 1)
        {
          currentState = EndAll;
        }
      }
      break;
    }
    case EndAll:
    {
      /*Cycle finished succesfully, wait to cycle end and restart the process*/
      now = millis();
      if(now > nextExpectedAct)
      {
        currentState = Idle;
      }
      break;
    }
  }
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
  StateMachine();
  /*take a break for 1 milisecond*/
  //delay(1);
}
