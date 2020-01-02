/*Open Hardware Algerie
Sujet : acquisition de l’information température et pression l’aide d’un capteur BMP180
Designer: Areour mohamed Cherif (OHA)
Date    : 20/11/2019

Ce code source est sous licence Gpl v3
outils nécessaire :          - une carte arduino Yùn
                             - un module bmp180
                             - une breadboard
                             - quelques jumper wirers
VCC --> 5.0V
GND --> GND
SCL --> Analog 5 or SCL
SDA --> Analog 4 or SDA

*/

// Définition des librairies nécessaire

#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <YunClient.h>
#include <PubSubClient.h>
#include <Bridge.h>

float pression;             // pour stocké la valeur de la pression 
float t;                    // pour stocké la valeur de la température 

// les topic pour la température et de l'humidité
const char* topic_t="sensors/capteur01/temperature";
const char* topic_p="sensors/capteur01/pression";

// les chaine de caractère pour transmettre des données avec MQTT
char pres[10],temp[10];

// instance de la classe Adafruit_BMP085
Adafruit_BMP085 bmp; 

// L'adresse du Broker
const char* server = "192.168.1.240";

// instance de la classe EthernetClient
YunClient YClient;

// l'instance de la classe MQTT
PubSubClient mqtt(YClient);

// compatibilité avec les anciennes version de l'IDE
void reconnect(void);
void pubCapteur(void);

void setup()
{
  // Initialisation de la liaison série  
  Serial.begin(9600);

  // Initialisation du capteur BMP085
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
  
  // Initialisation du la liaison entre le MCU et la SoC de l'arduino Yùn
  Bridge.begin();
  
  // Initialisation de la connexion au Broker  
  mqtt.setServer(server, 1883);
 
  delay(1500);
}

void loop()
{
  if(mqtt.connected())
  {
    // Si la connexion au Broker est réalisé envoyer les données
    pubCapteur();
    mqtt.loop();
  }else {
    // Sinon se reconnecté au Broker
    reconnect();
  }
}

void pubCapteur(void)
{
  // récupération des valeurs depuis le capteur   
  t = bmp.readTemperature();
  pression = bmp.readPressure();
  Serial.println(t);

  // convertion des valeurs capturé en chaine de caractére   
  dtostrf(pression, 7, 4, pres);
  dtostrf(t, 7, 4, temp);
  String payload;
  payload ="capteur01 temperature=";
  payload += temp;
  Serial.println (payload);
  mqtt.publish(topic_t,(char*)payload.c_str());
  
  payload ="capteur01 pression=";
  payload += pres;
  Serial.println (payload);
  // publication des donnée sur la Broker  
  mqtt.publish(topic_p,(char*)payload.c_str());
  

  delay(10000); 
}

void reconnect(void) {
  // Loop until we're reconnected
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqtt.connect("oha")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
