#include <WiFi.h>
#include <PubSubClient.h>
WiFiClient espClient;
PubSubClient client(espClient);
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
const char* ssid = "PANTECH e-Learning";
const char* password = "pantech2020";
const char* mqtt_server = "broker.mqtt-dashboard.com";

long lastMsg = 0;
char msg[50];
int value = 0;
String message="";
char val[50];
int beat = 0,count = 0,drop = 0;



void setup_wifi() {

    delay(10);
  
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
}

void callback(char* topic, byte* payload, unsigned int length) {
    for (int i = 0; i < length; i++) {
      message=message+(char)payload[i];
      }
    Serial.println(message);
    Serial.println();
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);
        if (client.connect(clientId.c_str())) {
          Serial.println("connected");
          client.publish("outTopic", "hello world");
          client.subscribe("node1");
        } 
        else {
          Serial.print("failed, rc=");
          Serial.print(client.state());
          Serial.println(" try again in 5 seconds");
          delay(5000);
        }
    }
}

void setup() {
    Serial.begin(9600);
    pinMode(18,INPUT);
    setup_wifi();
    pinMode(19,OUTPUT);
    pinMode(5,OUTPUT);
    client.setServer(mqtt_server, 1883);
    lcd.begin();
    lcd.backlight();
 }

void loop() {
       client.setCallback(callback);
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
  

 int ldr = analogRead(34);
   Serial.print("LDR");
   Serial.println(ldr);

 count++;

//Serial.println(analogRead(A0));
  if(ldr > 3500 )
{
  drop++;
  Serial.print("DROPS: ");
  Serial.println(drop);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("DROPS:");
  lcd.println(drop);
  if(count>80)
  {
    
  Serial.print("DROPS PER MIN: ");
  Serial.println(drop);
  delay(3000);
  drop=0;
  count=0;
    
  }
}


//Serial.print("count:");
//Serial.println(count);


 if(drop>40){
//  Serial.println("2");
  client.publish("M_alert", "Drop rate high");
   digitalWrite(5,HIGH);
 }
 else{
    digitalWrite(5,LOW);

 }
 Serial.println(digitalRead(18));
  if(digitalRead(18)==0){
//  Serial.println("3");
  client.publish("M_alert", "level low");
  lcd.setCursor(0,1);
  lcd.print("Level low");
  digitalWrite(19,HIGH);
  delay(1000);
   digitalWrite(19,LOW);
 }
 else{
    lcd.setCursor(0,1);
  lcd.print("Level HIGH");
  
 }
 
 delay(400);
}
