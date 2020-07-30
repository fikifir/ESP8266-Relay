/*
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "CONNECTED" to the topic "home/esp8266/status" everytime it bootups
  - subscribes to the topic:
    "home/terrace/light"
    "home/room/light"
    "home/livingroom/light"
    printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "home/terrace/light" is an 1, switch ON the ESP pin 5 / D1,
    else switch it OFF
  - If the first character of the topic "home/room/light" is an 1, switch ON the ESP pin 4 / D2,
    else switch it OFF
  - If the first character of the topic "home/livingroom/light" is an 1, switch ON the ESP pin 14 / D5,
    else switch it OFF
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
  - This sketch require 'ESP8266WiFi.h' and 'PubSubClient.h' library
*/

// source:
// https://github.com/fikifir/ESP8266-Relay

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//Update these with values suitable for your network.

//Enter your wifi credentials
const char* ssid = "Fiki's WiFi"; //change Fiki's WiFi to your own SSID wifi
const char* password = "fikiPass"; //change fikiPass to your own wifi password

//Enter Your mqttServer address
const char* mqttServer = "192.168.0.224"; //change 192.168.0.224 to your own mqttServer adresses

WiFiClient espClient;
PubSubClient client(espClient);
int value = 0;

byte relay1 = 5;    //pin D1 - Best for relay output
byte relay2 = 4;    //pin D2 - Best for relay output
byte relay3 = 14;   //pin D5 - Best for relay output
//byte led4 = 12;   //pin D6 - Best for relay output //uncomment to add more relays
//byte led5 = 13;   //pin D7 - Best for relay output //uncomment to add more relays

void setup_wifi(){

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length){
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] - ");
  for (int i = 0; i < length; i++){
    Serial.print((char)payload[i]);
  }
  
  Serial.println();
  // Switch ON the LED if an '1' was received as first character
  if(strcmp(topic, "home/terrace/light") == 0){
        //turn the light ON if the payload is '1' and publish to the MQTT server a confirmation message
        if (payload[0] == '1'){
           digitalWrite(relay1, HIGH);
           client.publish("home/terrace/light/state", "ON");
        }
        else if (payload[0] == '0')
        {
           digitalWrite(relay1, LOW);
           client.publish("home/terrace/light/state", "OFF");
        }
   }
  if(strcmp(topic, "home/room/light") == 0){
        //turn the light ON if the payload is '1' and publish to the MQTT server a confirmation message
        if (payload[0] == '1'){
           digitalWrite(relay2, HIGH);
           client.publish("home/room/light/state", "ON");
        }
        else if (payload[0] == '0')
        {
           digitalWrite(relay2, LOW);
           client.publish("home/room/light/state", "OFF");
        }
   }
  if(strcmp(topic, "home/livingroom/light") == 0){
        //turn the light ON if the payload is '1' and publish to the MQTT server a confirmation message
        if (payload[0] == '1'){
           digitalWrite(relay3, HIGH);
           client.publish("home/livingroom/light/state", "ON");
        }
        else if (payload[0] == '0')
        {
           digitalWrite(relay3, LOW);
           client.publish("home/livingroom/light/state", "OFF");
        }
   }
}

void reconnect(){
  // Loop until we're reconnected
  while (!client.connected()){
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())){
      Serial.println("CONNECTED");
      // Once connected, publish an announcement...
      client.publish("home/esp8266/status", "CONNECTED");
      // ... and resubscribe
      client.subscribe("home/terrace/light");
      client.subscribe("home/room/light");
      client.subscribe("home/livingroom/light");
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup(){
  pinMode(relay1, OUTPUT);  //Initialize the this pin as an output
  pinMode(relay2, OUTPUT);  //Initialize the this pin as an output
  pinMode(relay3, OUTPUT);  //Initialize the this pin as an output
  //pinMode(relay4, OUTPUT);  //uncomment if u use more relays
  //pinMode(relay5, OUTPUT);  //uncomment if u use more relays
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqttServer, 1883);
  client.setCallback(callback);
}

void loop(){
  if (!client.connected()){
    reconnect();
  }
  client.loop();
}
