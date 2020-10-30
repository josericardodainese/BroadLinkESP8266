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
const char *MQTT_IR_TOPIC = strcat(MQTT_PREFIX_TOPIC,"ir/change");
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
String getHtml();
String readFile(String path);
void dump(decode_results *results);
FS *fileSystem = &LittleFS;

String httpGETRequest(const char *serverName);
void getRemoteHtml();

/**
 * 
 * All setups functions implementations
 * **/

void setupLittleFS()
{
  if (!LittleFS.begin())
  {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
}

void setupServer()
{
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
  pinMode(LED_PIN, OUTPUT);
  Serial.println("Estado do led");
}

void setupOTA()
{
  Serial.println("Iniciando...");
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
    Serial.println("nFim!");
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
  irsend.begin();
}

/*** 
 * Setup WIFI Connection
 * ***/
void setupWifi()
{

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  Serial.print("\nA tentar ligar ao WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println(String("\nLigado a rede: (") + ssid + ")");
}

void setupFirebase()
{
  Serial.println("Setup Firebase Initialized.");
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

void getRemoteHtml()
{
  String openWeatherMapApiKey = "24137916f961924d477b51f4910b830c";

  String city = "Mogi Guacu";
  String countryCode = "BR";

  String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;


  JSONVar myObject = JSON.parse(httpGETRequest(serverPath.c_str()));
  // JSON.typeof(jsonVar) can be used to get the type of the var

  if (JSON.typeof(myObject) == "undefined")
  {
    Serial.println("Parsing input failed!");
    return;
  }

  Serial.print("JSON object = ");
  Serial.println(myObject);


  Serial.println();
  Serial.println();
  Serial.println();

  Serial.print("Weather For: ");
  Serial.println(myObject["name"]);

  Serial.print("Weather Main: ");
  Serial.println(myObject["weather"][0]["main"]);

  Serial.print("Weather Description: ");
  Serial.println(myObject["weather"][0]["description"]);

  Serial.print("Weather Icon: ");
  Serial.println(myObject["weather"][0]["icon"]);
  Serial.print("http://openweathermap.org/img/w/");
  Serial.print(myObject["weather"][0]["icon"]);
  Serial.print(".png");
  // http://openweathermap.org/img/w/02n.png

  Serial.print("Temperature: ");
  Serial.println(myObject["main"]["temp"]);

  Serial.print("Pressure: ");
  Serial.println(myObject["main"]["pressure"]);


  Serial.print("Humidity: ");
  Serial.println(myObject["main"]["humidity"]);

  Serial.print("Wind Speed: ");
  Serial.println(myObject["wind"]["speed"]);
}

String httpGETRequest(const char *serverName)
{
  HTTPClient http;

  // Your IP address with path or Domain name with URL path
  http.begin(serverName);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

void sendIRCommand(uint16_t CMD[67])
{
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

  server.send(200, "text/html", getHtml());
}

void handleIr()
{
  for (uint8_t i = 0; i < server.args(); i++)
  {
    if (server.argName(i) == "liga_desliga")
    {
      Serial.println();
      Serial.println("-----------BOTAO PRESSIONADO-----------");
      Serial.println("Liga Desliga");
      Serial.println("-----------BOTAO PRESSIONADO-----------");
      Serial.println();

      sendIRCommand(LD);
    }
    else if (server.argName(i) == "vol_mais")
    {
      Serial.println();
      Serial.println("-----------BOTAO PRESSIONADO-----------");
      Serial.println("Volume Mais");
      Serial.println("-----------BOTAO PRESSIONADO-----------");
      Serial.println();
      sendIRCommand(VOL_MAIS);
    }
    else if (server.argName(i) == "vol_menos")
    {
      Serial.println();
      Serial.println("-----------BOTAO PRESSIONADO-----------");
      Serial.println("Volume Menos");
      Serial.println("-----------BOTAO PRESSIONADO-----------");
      Serial.println();
      sendIRCommand(VOL_MENOS);
    }
    else if (server.argName(i) == "sbt")
    {
      Serial.println();
      Serial.println("-----------BOTAO PRESSIONADO-----------");
      Serial.println("Mudando Para SBT");
      Serial.println("-----------BOTAO PRESSIONADO-----------");
      Serial.println();
      //0426
      sendIRCommand(DIGIT_4);
      sendIRCommand(DIGIT_2);
      sendIRCommand(DIGIT_6);
    }
    else if (server.argName(i) == "comedy")
    {
      Serial.println();
      Serial.println("-----------BOTAO PRESSIONADO-----------");
      Serial.println("Mudando Para Comedy Central");
      Serial.println("-----------BOTAO PRESSIONADO-----------");
      Serial.println();

      //0092
      sendIRCommand(DIGIT_9);
      sendIRCommand(DIGIT_2);
    }
    else if (server.argName(i) == "baixo")
    {
      Serial.println();
      Serial.println("-----------BOTAO PRESSIONADO-----------");
      Serial.println("Baixo");
      Serial.println("-----------BOTAO PRESSIONADO-----------");
      Serial.println();

      sendIRCommand(BAIXO);
    }
    else if (server.argName(i) == "cima")
    {
      Serial.println();
      Serial.println("-----------BOTAO PRESSIONADO-----------");
      Serial.println("Cima");
      Serial.println("-----------BOTAO PRESSIONADO-----------");
      Serial.println();

      sendIRCommand(CIMA);
    }
  }
  handleRoot();
}

void handleNotFound()
{
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

String getHtml()
{
  const char *htmlMessage = R"(
<html>
<head>
  <title>IR Control</title>
</head>
<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@4.5.3/dist/css/bootstrap.min.css"
  integrity="sha384-TX8t27EcRE3e/ihU7zmQxVncDAy5uIKz4rEkgIXeMed4M0jlfIDPvg6uqKI2xXr2" crossorigin="anonymous">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<body>
  <nav class="navbar navbar-expand-lg navbar-light bg-light">
    <a class="navbar-brand" href="#">
      <img src="https://www.blueletro.com/media/catalog/category/azamerica.jpg" height="30" alt="" loading="lazy">
    </a>
    <a class="navbar-brand" href="#">TV</a>
    <ul class="navbar-nav mr-auto" style="display:flex; flex-direction: row;">
      <li class="nav-item">
        <a class="nav-link" href="ir?vol_mais">
          <button class="btn btn-outline-success my-2 my-sm-0">Vol +</button>&nbsp;
        </a>
      </li>
      <li class="nav-item">
        <a class="nav-link" href="ir?vol_menos">
          <button class="btn btn-outline-success my-2 my-sm-0">Volume -</button>
        </a>
      </li>
    </ul>
    <a class="nav-link" href="ir?liga_desliga">
      <button class="btn btn-outline-success my-2 my-sm-0">Ligar/Desligar</button>
    </a>
  </nav>

  <div class="container">
    <div class="d-flex align-items-center">
      <div>
        <a class="nav-link" href="ir?sbt">
          <img width="50" src="https://logospng.org/wp-content/uploads/sbt.png" class="rounded" alt="SBT">
        </a>
      </div>
      <div>
        <a class="nav-link" href="ir?tlc">
          <img width="50" src="https://catwerk.com/wp-content/uploads/tlc.png" class="rounded" alt="TLC">
        </a>
      </div>
      <div>
        <a class="nav-link" href="ir?globo">
          <img width="50" src="https://pbs.twimg.com/profile_images/585934109664481281/IZCvb1oW_400x400.png"
            class="rounded" alt="Globo">
        </a>
      </div>
      <div>
        <a class="nav-link" href="ir?comedy">
          <img width="50" src="https://fastly.4sqi.net/img/user/130x130/XYECDNTZMG31SWL0.png" class="rounded"
            alt="Comedy Central">
        </a>
      </div>
    </div>
  </div>
  <nav class="navbar navbar-expand-lg navbar-light bg-light">
    <ul class="navbar-nav mr-auto" style="display:flex; flex-direction: row;">
      <li class="nav-item">
        <a class="nav-link" href="ir?esquerda">
          <button class="btn btn-outline-success my-2 my-sm-0">Esq</button>&nbsp;
        </a>
      </li>
      <li class="nav-item">
        <a class="nav-link" href="ir?direita">
          <button class="btn btn-outline-success my-2 my-sm-0">Dir</button>&nbsp;
        </a>
      </li>
      <li class="nav-item">
        <a class="nav-link" href="ir?cima">
          <button class="btn btn-outline-success my-2 my-sm-0">Cima</button>&nbsp;
        </a>
      </li>
      <li class="nav-item">
        <a class="nav-link" href="ir?baixo">
          <button class="btn btn-outline-success my-2 my-sm-0">Baixo</button>&nbsp;
        </a>
      </li>
      <li class="nav-item">
        <a class="nav-link" href="ir?recal">
          <button class="btn btn-outline-success my-2 my-sm-0">Ultimos</button>&nbsp;
        </a>
      </li>
      <li class="nav-item">
        <a class="nav-link" href="ir?menu">
          <button class="btn btn-outline-success my-2 my-sm-0">Menu</button>&nbsp;
        </a>
      </li>
    </ul>
  </nav>
</body>
)";

  return htmlMessage;
}

/***
 * MQTT Message Callback
 *  ***/
void mqttCallback(char *topic, byte *payload, unsigned int length)
{
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

String readFile(String path)
{
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

/**
 * 
 * Arduino Initial Setup that call all others setups
 * **/

void setup()
{
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
  setupServer();
  setupIRSender();
  getRemoteHtml();
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
