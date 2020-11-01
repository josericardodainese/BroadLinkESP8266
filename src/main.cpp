#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <LittleFS.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

#include <FirebaseArduino.h>

#include "fauxmoESP.h"
#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>
#include <Arduino_JSON.h>
#include <HttpClient.h>
#include <Base64.h>

/***
 * MQTT Import
 ***/
#include <PubSubClient.h>

/***
 * MQTT Config
 * 
 * HOSTNAME -> Device name, should be a uniq name
 * MQTT_IR_TOPIC -> Subcribe topic that receive the IR command button to send.
 * MQTT_STATE_TOPIC -> Publish Topic
 * MQTT_SERVER -> Broker MQTT Address
 * 
 * 
 ***/

const String HOSTNAME = "DeviceESP8266";
char *MQTT_PREFIX_TOPIC = "josericardodainese@gmail.com/";
const char *MQTT_IR_TOPIC = strcat(MQTT_PREFIX_TOPIC, "ir/change");
const char *MQTT_SERVER = "www.maqiatto.com";
#define MQTT_AUTH true
#define MQTT_USERNAME "josericardodainese@gmail.com"
#define MQTT_PASSWORD "gcd7Y0gdeDch"

ESP8266WebServer server(80);

/***
 * Digital Pin that IR receiver are connected.
 * GPIO12 / PINO D6
 ***/
int RECV_PIN = D1;
int LED_PIN = D6;

/***
 * Variables
 * 
 * fauxmo -> variable of Alexa Library
 * wclient -> WiFi Client used in MQTT connection
 * client -> PubSub MQTT Client
 * irrecv -> IR Receiver
 * results -> Variable that store IR command that was sent
 * raw_lenght -> IR lenth of raw command
 * raw_frequency -> IR raw frequency
 ***/
fauxmoESP fauxmo;
WiFiClient wclient;
PubSubClient client(MQTT_SERVER, 1883, wclient);
IRrecv irrecv(RECV_PIN);
IRsend irsend(D2);
decode_results results;
int raw_lenght = 68;
int raw_frequency = 32;
WiFiClientSecure httpclient;
const int kNetworkTimeout = 30 * 1000;
const int kNetworkDelay = 1000;

/***
 * Raw commands without spaces and hiffes
 ***/
uint16_t DIGIT_1[67] = {9100, 4500, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 1700, 650, 1700, 650, 1700, 650, 1700, 650, 1700, 650, 1700, 650};
uint16_t DIGIT_2[67] = {9200, 4450, 650, 550, 700, 1650, 600, 550, 700, 500, 700, 500, 700, 500, 650, 500, 700, 550, 650, 500, 600, 1700, 600, 550, 650, 550, 700, 500, 600, 600, 650, 500, 600, 600, 650, 500, 650, 1650, 700, 500, 650, 500, 650, 500, 700, 500, 650, 500, 650, 500, 700, 1650, 600, 550, 700, 1600, 700, 1650, 700, 1600, 700, 1650, 600, 1700, 600, 1700, 700};
uint16_t DIGIT_3[67] = {9100, 4500, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 1700, 650, 1700, 650, 1700, 650, 1700, 650, 1700, 650};
uint16_t DIGIT_4[67] = {9100, 4500, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 1700, 650, 550, 650, 1700, 650, 1700, 650, 1700, 650, 1700, 650, 1700, 650};
uint16_t DIGIT_5[67] = {9100, 4500, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 1700, 650, 1700, 650, 1700, 650, 1700, 650, 1700, 650};
uint16_t DIGIT_6[67] = {9100, 4500, 700, 500, 700, 1600, 700, 500, 650, 550, 700, 450, 700, 500, 700, 500, 650, 550, 700, 450, 700, 1600, 650, 550, 650, 550, 650, 550, 700, 450, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 1650, 700, 450, 700, 450, 700, 500, 650, 550, 700, 500, 700, 1600, 700, 500, 700, 500, 700, 1600, 650, 1700, 650, 1700, 650, 1700, 700, 1600, 700};
uint16_t DIGIT_7[67] = {9100, 4500, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 1700, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 1700, 650, 1700, 650, 1700, 650, 1700, 650};
uint16_t DIGIT_8[67] = {9100, 4500, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 1700, 650, 1700, 650, 550, 650, 1700, 650, 1700, 650, 1700, 650, 1700, 650};
uint16_t DIGIT_9[67] = {9100, 4500, 650, 550, 650, 1700, 650, 550, 700, 500, 650, 550, 650, 550, 650, 550, 700, 500, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 1700, 650, 550, 650, 1700, 650, 1700, 650, 1700, 650, 1700, 650};
uint16_t DIGIT_0[67] = {9200, 4450, 700, 500, 650, 1700, 650, 550, 650, 550, 700, 500, 650, 550, 700, 450, 700, 500, 700, 500, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 700, 500, 650, 550, 700, 500, 650, 550, 650, 550, 700, 500, 700, 500, 650, 550, 700, 500, 650, 1700, 700, 1600, 700, 1600, 700, 1600, 650, 1700, 650, 1700, 700, 1600, 700, 1600, 650};

uint16_t LD[67] = {9100, 4500, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 600, 600, 1650, 650, 550, 650, 1700, 650, 550, 650, 1650, 650, 1650, 650, 1700, 650, 1700, 650};
uint16_t VOL_MAIS[67] = {9100, 4500, 650, 550, 650, 1700, 650, 550, 600, 550, 650, 550, 650, 550, 650, 550, 650, 550, 600, 550, 650, 1700, 650, 550, 650, 550, 700, 500, 700, 500, 600, 550, 650, 550, 700, 500, 700, 500, 650, 500, 600, 600, 650, 1650, 650, 550, 700, 500, 600, 550, 700, 1650, 700, 1650, 700, 1650, 700, 1650, 700, 500, 700, 1650, 700, 1650, 700, 1650, 700};
uint16_t VOL_MENOS[67] = {9100, 4500, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 700, 500, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 600, 550, 650, 550, 650, 1700, 650, 550, 700, 500, 650, 550, 650, 550, 700, 1600, 650, 1700, 650, 1650, 650, 550, 650, 1700, 650, 1700, 650, 1700, 650};
uint16_t ESQ[67] = {9150, 4500, 650, 550, 600, 1700, 650, 550, 650, 550, 700, 500, 650, 550, 650, 500, 600, 600, 600, 600, 600, 1750, 650, 500, 650, 500, 600, 550, 650, 550, 650, 550, 650, 550, 650, 1650, 650, 550, 650, 500, 700, 500, 650, 1700, 700, 500, 600, 600, 650, 550, 600, 600, 650, 1700, 650, 1700, 650, 1650, 650, 550, 600, 1750, 600, 1700, 700, 1600, 650};
uint16_t DIR[67] = {9050, 4550, 650, 550, 650, 1700, 650, 500, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1650, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 1700, 650, 1700, 650, 1700, 650, 550, 650, 1700, 650, 1700, 650, 1700, 650};
uint16_t CIMA[67] = {9100, 4500, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1650, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1650, 650, 1700, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 1700, 650, 1700, 650, 1700, 650, 1700, 650};
uint16_t BAIXO[67] = {9100, 4500, 650, 550, 650, 1650, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1650, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 600, 1700, 650, 1700, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1650, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 1700, 650, 1700, 650, 1700, 650};
uint16_t LAST[67] = {9150, 4450, 700, 500, 700, 1600, 700, 500, 700, 500, 700, 500, 700, 500, 700, 500, 700, 500, 700, 500, 700, 1650, 700, 500, 700, 500, 700, 500, 700, 500, 700, 500, 700, 500, 700, 500, 700, 1600, 700, 500, 700, 500, 700, 1600, 700, 500, 700, 500, 700, 500, 700, 1650, 700, 500, 700, 1600, 700, 1600, 700, 500, 700, 1600, 700, 1600, 700, 1600, 700};
uint16_t MENU[67] = {9100, 4500, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 600, 1700, 650, 550, 600, 1700, 600, 600, 650, 550, 650, 550, 650, 1700, 650, 550, 600, 550, 650, 1700, 650, 550, 650, 1700, 650, 1700, 650, 1650, 650};
uint16_t TV_R[67] = {9100, 4500, 650, 550, 650, 1650, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1650, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 1700, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 1700, 650, 1700, 650};
uint16_t SAT[67] = {9100, 4500, 650, 550, 600, 1700, 600, 600, 650, 550, 600, 600, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 600, 600, 650, 550, 600, 600, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 1700, 650, 1700, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 1700, 650, 1700, 650};
uint16_t EXIT[67] = {9100, 4500, 700, 500, 650, 1700, 650, 500, 650, 500, 650, 550, 700, 500, 650, 550, 650, 550, 650, 550, 650, 1650, 700, 500, 650, 550, 650, 550, 700, 500, 650, 550, 650, 550, 650, 1700, 650, 1700, 700, 1650, 650, 1700, 650, 1650, 700, 500, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 1700, 650, 1650, 650};
uint16_t OK_LIST[67] = {9150, 4500, 600, 600, 650, 1700, 600, 600, 700, 500, 650, 500, 700, 500, 650, 550, 600, 600, 650, 550, 650, 1700, 650, 550, 650, 500, 600, 600, 600, 600, 650, 550, 650, 500, 650, 1700, 600, 600, 650, 1700, 700, 1600, 650, 550, 650, 500, 650, 550, 600, 600, 650, 500, 650, 1700, 650, 550, 700, 500, 700, 1600, 650, 1700, 600, 1700, 600, 1750, 650};
uint16_t FAV[67] = {9100, 4500, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1650, 650, 1700, 650, 550, 650, 1650, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 1650, 650, 1700, 650, 1700, 650, 1700, 650};
uint16_t FIND[67] = {9100, 4500, 650, 550, 650, 1700, 650, 500, 650, 500, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1650, 650, 550, 600, 600, 650, 550, 650, 550, 650, 500, 650, 550, 650, 1700, 650, 550, 650, 1650, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 500, 650, 1700, 650, 550, 650, 1700, 700, 500, 650, 1700, 650, 1650, 700, 1600, 650};
uint16_t AUDIO[67] = {9100, 4500, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 500, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 1650, 650, 1700, 650, 1650, 650, 1700, 650, 1700, 650, 1700, 650, 550, 650, 1700, 650};
uint16_t PAUSE[67] = {9100, 4500, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1650, 650, 550, 650, 550, 650, 1700, 650, 1700, 650, 1700, 650, 1700, 650, 1700, 650, 550, 650, 1700, 650};
uint16_t ZOOM[67] = {9100, 4500, 650, 550, 600, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 1700, 650, 550, 650, 1650, 600, 1700, 650, 1650, 650, 1700, 650, 550, 650, 1650, 650};
uint16_t INFO[67] = {9100, 4500, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1650, 650, 1650, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1650, 650, 550, 650, 550, 650, 550, 650, 1650, 650, 1700, 650, 1700, 650, 1700, 650, 550, 650, 1650, 650};
uint16_t EPG[67] = {9100, 4500, 600, 600, 650, 1650, 600, 600, 650, 550, 600, 600, 600, 600, 650, 500, 650, 550, 650, 500, 650, 1700, 650, 550, 700, 500, 650, 500, 650, 550, 600, 600, 600, 550, 650, 1700, 700, 1600, 600, 1750, 700, 500, 650, 1700, 650, 550, 650, 550, 600, 600, 650, 500, 650, 500, 650, 550, 600, 1750, 650, 550, 650, 1700, 650, 1700, 650, 1700, 650};
uint16_t SLEEP[67] = {9050, 4550, 700, 500, 700, 1650, 700, 500, 600, 600, 700, 450, 700, 500, 700, 500, 700, 500, 650, 550, 600, 1700, 700, 500, 700, 500, 700, 450, 700, 500, 650, 550, 650, 550, 700, 450, 700, 500, 700, 500, 700, 1600, 700, 1600, 700, 500, 700, 500, 700, 500, 650, 1700, 700, 1600, 700, 1600, 700, 500, 650, 550, 700, 1600, 700, 1600, 700, 1600, 700};
uint16_t MODE[67] = {9100, 4500, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 700, 500, 650, 550, 600, 1700, 650, 550, 650, 550, 650, 500, 700, 500, 650, 550, 650, 550, 650, 1700, 650, 1700, 700, 1600, 650, 550, 650, 550, 650, 550, 650, 1700, 650, 550, 700, 500, 650, 550, 650, 550, 700, 1600, 600, 1750, 650, 1700, 650, 550, 650, 1700, 650};
uint16_t DISPLAY_[67] = {9100, 4500, 650, 550, 650, 1650, 600, 600, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1650, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 550, 650, 1650, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 550, 650, 1700, 650, 550, 650, 500, 650, 1700, 650, 1700, 650, 550, 650, 1700, 650, 1650, 650, 550, 650, 1650, 650};

uint16_t *remoteControl[] = {
    DIGIT_0, DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4,
    DIGIT_5, DIGIT_6, DIGIT_7, DIGIT_8, DIGIT_9,
    LD, VOL_MAIS, VOL_MENOS, ESQ, DIR, CIMA,
    BAIXO, LAST, MENU, TV_R, SAT, EXIT, OK_LIST, FAV,
    FIND, AUDIO, PAUSE, ZOOM, INFO, EPG, SLEEP, MODE,
    DISPLAY_};

/***
 * Rede Wifi Credentials
 ***/
const char *ssid = "Developer 2.4GHz";
const char *password = "19931995";

/***
 * Web Page Credentials
 ***/
const char *www_username = "admin";
const char *www_password = "esp8266";

String htmlContent;

/***
 * Setup Functions
 ***/
String uint64ToString(uint64_t input);
void setupOTA();
void setupLed();
void setupWifi();
void setupIRSender();
void setupServer();
void setupLittleFS();
void setupFirebase();

/***
 * Loop Functions
 ***/
void loopLed();
void loopMQTT();
void loopServer();
void IRDecodeLoop();

/***
 * Setup Aux Functions
 ***/
bool checkMqttConnection();
void sendIRCommand(uint16_t CMD[67]);
void mqttCallback(char *topic, byte *payload, unsigned int length);
void handleRoot();
void handleIr();
void handleNotFound();
String readFile(String path);
void dump(decode_results *results);
JSONVar getDownloadURLRepoFile();
JSONVar parseToJson(String content);
String httpGETRequest(const char *serverName);
String httpFileGet(const char *urlFile);
String getRemoteHtml();
void logPressButton(String button);
void downloadFile(String urlFile);

FS *fileSystem = &LittleFS;

int input2Len = 0;

/**
 * 
 * All setups functions implementations
 * **/

void setupLittleFS()
{
  Serial.println("Executando setupLittleFS()");
  if (!LittleFS.begin())
  {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
}

void setupServer()
{
  Serial.println("Executando setupServer()");
  server.on("/", handleRoot);
  server.on("/ir", handleIr);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  // server.serveStatic("/data/js", LittleFS, "/data/js");
  // server.serveStatic("/data/css", LittleFS, "/data/css");
  // server.serveStatic("/data/img", LittleFS, "/data/img");
  server.serveStatic("index.html", LittleFS, "index.html");

  server.begin();
  Serial.println("HTTP server started");
}

void setupLed()
{
  Serial.println("Executando setupLed()");
  pinMode(LED_PIN, OUTPUT);
  Serial.println("Estado do led");
}

void setupOTA()
{
  Serial.println("Executando setupOTA()");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Conexao falhou! Reiniciando...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.onStart([]() {
    Serial.println("Inicio...");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("Fim!");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progresso: %u%%r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Erro [%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Autenticacao Falhou");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Falha no Inicio");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Falha na Conexao");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Falha na Recepcao");
    else if (error == OTA_END_ERROR)
      Serial.println("Falha no Fim");
  });
  ArduinoOTA.begin();
  Serial.println("Pronto");
  Serial.print("Endereco IP: ");
  Serial.println(WiFi.localIP());
}

/*** 
 * Setup IR sender object
 * ***/
void setupIRSender()
{
  Serial.println("Executando setupIRSender()");
  irsend.begin();
}

/*** 
 * Setup WIFI Connection
 * ***/
void setupWifi()
{
  Serial.println("Executando setupWifi()");
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  Serial.println("A tentar ligar ao WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println(String("Ligado a rede: (") + ssid + ")");
}

void setupFirebase()
{
  Serial.println("Executando setupFirebase()");
}

/**
 * 
 * All loop functions implementations
 * **/

void IRDecodeLoop()
{
  /***
   * Return the information about IR command identified.
   ***/
  if (irrecv.decode(&results))
  {
    Serial.println("-------------START-------------");
    Serial.println(uint64ToString(results.value));
    dump(&results);

    /***
     * Receive the next value.
     ***/
    irrecv.resume();
  }
}

void loopMQTT()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    if (checkMqttConnection())
    {
      client.loop();
    }
  }
}

/***
 *Loop Led 
 ****/
void loopLed()
{
  digitalWrite(LED_PIN, HIGH);
  Serial.println("Ligado");
  delay(1500);
  digitalWrite(LED_PIN, LOW);
  Serial.println("Desligado");
  delay(1500);
}
void loopServer()
{
  server.handleClient();
}

/***
 * Aux functions implementations 
 ***/

JSONVar getDownloadURLRepoFile()
{
  Serial.println("Executando getDownloadURLRepoFile()");
  String ulrFull = "https://api.github.com/repos/josericardodainese/BroadLinkESP8266/contents/src/data/index.html?access_token=bfc103ae0ea1ccb9996bd66a45e8ffda8f485692";

  JSONVar jsonParsed = parseToJson(httpGETRequest(ulrFull.c_str()));
  return jsonParsed;
}

JSONVar parseToJson(String content)
{
  Serial.println("Executando parseToJson()");
  JSONVar responseJson = JSON.parse(content);

  if (JSON.typeof(responseJson) == "undefined")
  {
    return "Parsing Url Repo Failed!";
  }

  return responseJson;
}

String getRemoteHtml()
{
  Serial.println("Executando getRemoteHtml()");
  JSONVar jsonArquivo = getDownloadURLRepoFile();
  String urlFile = JSON.stringify(jsonArquivo["download_url"]);
  Serial.println("URL Arquivo Antes de Chamar o download");
  Serial.println(urlFile.c_str());
  downloadFile(urlFile);

  return "<h1>Teste</h1>";
}

String httpFileGet(const char *urlFile)
{
  Serial.println("Executando httpFileGet()");
  HttpClient http(httpclient);

  /***
  * Make a HTTP request
  ***/
  int status = http.get(urlFile, "");

  if (status == 0)
  {
    Serial.println("Request for Download File HTML OK");
    int err = http.responseStatusCode();
    Serial.printf("HTTP response status code for call %s", err);
    if (err >= 0)
    {
      err = http.skipResponseHeaders();
      Serial.printf("HTTP skipResponseHeaders for call %s", err);
      if (err >= 0)
      {
        int bodyLen = http.contentLength();
        Serial.print("Content length is: ");
        Serial.println(bodyLen);
        Serial.println();
        Serial.println("Body returned follows:");
        /***
        * Now we've got to the body, so we can print it out
        ***/
        // return http.readString();
      }
    }
  }
}

String httpGETRequest(const char *serverName)
{
  Serial.println("Executando httpGETRequest()");
  std::unique_ptr<BearSSL::WiFiClientSecure> httpclient(new BearSSL::WiFiClientSecure);
  httpclient->setFingerprint("DF:B2:29:C6:A6:38:1A:59:9D:C9:AD:92:2D:26:F5:3C:83:8F:A5:87");

  HTTPClient https;
  // https.setAuthorization("DF:B2:29:C6:A6:38:1A:59:9D:C9:AD:92:2D:26:F5:3C:83:8F:A5:87");
  https.setAuthorization("josericardodainese", "bfc103ae0ea1ccb9996bd66a45e8ffda8f485692");
  https.begin(*httpclient, "https://api.github.com");

  if (https.begin(*httpclient, serverName))
  {

    Serial.print("[HTTPS] Start GET...\n");
    /***
    * Start Cnnection And Send HTTP Header
    ***/
    int httpCode = https.GET();
    /***
    * httpCode will be negative on error
    ***/
    if (httpCode > 0)
    {
      /***
      * HTTP header has been send and Server response header has been handled
      ***/
      Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
      /***
      * File found at server
      ***/
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
      {
        String payload = https.getString();
        return payload;
      }
    }
    else
    {
      Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
    }

    https.end();
  }
  else
  {
    Serial.printf("[HTTPS] Unable to connect\n");
  }
  delay(10000);
}

void sendIRCommand(uint16_t CMD[67])
{
  Serial.println("Executando sendIRCommand()");
  irsend.sendRaw(CMD, raw_lenght, raw_frequency);
  Serial.println("Comando enviado");
  Serial.println();
  delay(1000);
}

void handleRoot()
{
  // if (!server.authenticate(www_username, www_password))
  // {
  //   return server.requestAuthentication();
  // }

  server.send(200, "text/html", htmlContent);
}

void handleIr()
{
  for (uint8_t i = 0; i < server.args(); i++)
  {
    if (server.argName(i) == "liga_desliga")
    {
      logPressButton("Liga Desliga");
      sendIRCommand(LD);
    }
    else if (server.argName(i) == "vol_mais")
    {
      logPressButton("Volume Mais");
      sendIRCommand(VOL_MAIS);
    }
    else if (server.argName(i) == "vol_menos")
    {
      logPressButton("Volume Menos");
      sendIRCommand(VOL_MENOS);
    }
    else if (server.argName(i) == "sbt")
    {
      logPressButton("Mudando Para SBT");
      sendIRCommand(DIGIT_4);
      sendIRCommand(DIGIT_2);
      sendIRCommand(DIGIT_6);
    }
    else if (server.argName(i) == "comedy")
    {
      logPressButton("Mudando Para Comedy Central");
      sendIRCommand(DIGIT_9);
      sendIRCommand(DIGIT_2);
    }
    else if (server.argName(i) == "baixo")
    {
      logPressButton("Baixo");
      sendIRCommand(BAIXO);
    }
    else if (server.argName(i) == "cima")
    {
      logPressButton("Cima");
      sendIRCommand(CIMA);
    }
  }
  handleRoot();
}

void handleNotFound()
{
  Serial.println("Executando handleNotFound()");
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  server.send(404, "text/plain", message);
}

/***
 * MQTT Message Callback
 *  ***/
void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.println("Executando mqttCallback()");
  String payloadStr = "";
  for (int i = 0; i < int(length); i++)
  {
    Serial.println(payload[i]);
    payloadStr += (char)payload[i];
  }
  String topicStr = String(topic);
  Serial.println("TOPIC => " + topicStr);
  Serial.println("MESSSAGE => " + payloadStr);
  if (topicStr.equals(MQTT_IR_TOPIC))
  {
    // unsigned int MQTT_IR_COMMAND[68] = {payloadStr.toInt()};
    // sendIRCommand(MQTT_IR_COMMAND);
  }
}

/***
 * Method responsable for doing a decode of the IR signal received 
 * where the data are passed to the library IR Remote that make all 
 * treatment and return the informations accord the protocol recognized.
 ***/
void dump(decode_results *results)
{
  Serial.println("Executando dump()");
  int count = results->rawlen;

  if (results->decode_type == UNKNOWN)
  {
    Serial.print("Unknown encoding: ");
  }
  else if (results->decode_type == NEC)
  {
    Serial.print("Decoded NEC: ");
  }
  else if (results->decode_type == SONY)
  {
    Serial.print("Decoded SONY: ");
  }
  else if (results->decode_type == RC5)
  {
    Serial.print("Decoded RC5: ");
  }
  else if (results->decode_type == RC6)
  {
    Serial.print("Decoded RC6: ");
  }
  else if (results->decode_type == PANASONIC)
  {
    Serial.print("Decoded PANASONIC - Address: ");
    Serial.print(results->address, HEX);
    Serial.print(" Value: ");
  }
  else if (results->decode_type == LG)
  {
    Serial.print("Decoded LG: ");
  }
  else if (results->decode_type == JVC)
  {
    Serial.print("Decoded JVC: ");
  }
  else if (results->decode_type == AIWA_RC_T501)
  {
    Serial.print("Decoded AIWA RC T501: ");
  }
  else if (results->decode_type == WHYNTER)
  {
    Serial.print("Decoded Whynter: ");
  }

  /***
   * Show the information on Serial Monitor
   ***/
  Serial.print(uint64ToString(results->value));
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
  Serial.print("Raw (");
  Serial.print(count, DEC);
  Serial.print("): ");

  for (int i = 1; i < count; i++)
  {
    if (i & 1)
    {
      Serial.print(results->rawbuf[i] * kRawTick, DEC);
    }
    else
    {
      Serial.write('-');
      Serial.print((unsigned long)results->rawbuf[i] * kRawTick, DEC);
    }
    Serial.print(" ");
  }
  Serial.println();
  Serial.println("-------------END-------------");
}

/***
 * MQTT Init
 * Check if connection state is active or else try connect
 ***/
bool checkMqttConnection()
{
  if (!client.connected())
  {
    if (MQTT_AUTH ? client.connect(HOSTNAME.c_str(), MQTT_USERNAME, MQTT_PASSWORD) : client.connect(HOSTNAME.c_str()))
    {
      Serial.println("Ligado ao broker mqtt " + String(MQTT_SERVER));

      /*** Subscribe Topic ***/
      client.subscribe(MQTT_IR_TOPIC);
    }
    else
    {
      Serial.println("Erro ao Conectar ao broker mqtt " + String(MQTT_SERVER));
    }
  }
  return client.connected();
}

String uint64ToString(uint64_t input)
{
  Serial.println("Executando uint64ToString()");
  String result = "";
  uint8_t base = 10;

  do
  {
    char c = input % base;
    input /= base;

    if (c < 10)
      c += '0';
    else
      c += 'A' - 10;
    result = c + result;
  } while (input);
  return result;
}

void downloadFile(String urlFile)
{

  Serial.println("Executando httpGETRequest()");
  std::unique_ptr<BearSSL::WiFiClientSecure> httpclient(new BearSSL::WiFiClientSecure);
  httpclient->setFingerprint("DF:B2:29:C6:A6:38:1A:59:9D:C9:AD:92:2D:26:F5:3C:83:8F:A5:87");

  HTTPClient https;
  // https.setAuthorization("DF:B2:29:C6:A6:38:1A:59:9D:C9:AD:92:2D:26:F5:3C:83:8F:A5:87");
  https.setAuthorization("josericardodainese", "bfc103ae0ea1ccb9996bd66a45e8ffda8f485692");
  https.begin(*httpclient, "https://api.github.com");

  if (https.begin(*httpclient, "https://raw.githubusercontent.com/josericardodainese/BroadLinkESP8266/master/src/data/index.html?token=AAZC2F2PMPGXPFD7MGO66YC7TWYEM"))
  {

    Serial.print("[HTTPS] Start GET...\n");
    /***
    * Start Cnnection And Send HTTP Header
    ***/
    int httpCode = https.GET();
    /***
    * httpCode will be negative on error
    ***/
    if (httpCode > 0)
    {
      /***
      * HTTP header has been send and Server response header has been handled
      ***/
      Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
      /***
      * File found at server
      ***/
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
      {
        File f = LittleFS.open("/index.html", "w");
        if (f)
        {
          Serial.println("Arquivo index.html Aberto");
          https.writeToStream(&f);
        }
      }
    }
    else
    {
      Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
    }

    https.end();
  }
  else
  {
    Serial.printf("[HTTPS] Unable to connect\n");
  }

  readFile("/index.html");
  delay(10000);
}

String readFile(String path)
{
  Serial.println("Executando readFile()");
  LittleFS.remove(F("/index.html"));
  File rFile = LittleFS.open(F("/index.html"), "r");
  if (!rFile)
  {
    Serial.println("Erro ao abrir arquivo!");
  }
  String content = rFile.readString(); //desconsidera '\r\n'
  Serial.print("leitura de estado: ");
  Serial.println(content);
  rFile.close();
  return content;
}

void logPressButton(String button)
{
  Serial.println();
  Serial.println(button);
  Serial.println();
}

/**
 * 
 * Arduino Initial Setup that call all others setups
 * **/

void setup()
{
  Serial.println("Executando setup()");
  /***
   * Initialize the Serial monitor.
   ***/
  Serial.begin(115200);
  setupLittleFS();
  setupOTA();
  setupLed();
  /***
   * Initialize the receiver.
   ***/
  irrecv.enableIRIn();

  setupWifi();
  /***
   * Process MQTT Messages
   *  ***/
  client.setCallback(mqttCallback);
  setupLittleFS();
  setupFirebase();

  htmlContent = getRemoteHtml();
  Serial.println(htmlContent);
  setupServer();
  setupIRSender();
}

/**
 * 
 * Arduino Initial loop that call all others loops
 * **/

void loop()
{
  IRDecodeLoop();
  ArduinoOTA.handle();
  loopServer();
  loopMQTT();
}
