#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>

#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>


/***
 * Digital Pin that IR receiver are connected.
 * GPIO12 / PINO D6
 ***/
int RECV_PIN = D1;

/***
 * Variables
 ***/
IRrecv irrecv(RECV_PIN); // Variable IRrecv.
decode_results results;  // Varible that store the results.

/***
 * Rede Wifi Credentials
 ***/
const char *ssid = "Developer 2.4GHz"; // nome do seu roteador WIFI (SSID)
const char *password = "19931995";     // senha do roteador WIFI

/***
 * Functions
 ***/
String uint64ToString(uint64_t input);
void setupOTA();

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

void setup()
{
  /***
   * Initialize the serial monitor.
   ***/
  Serial.begin(115200);
  setupOTA();
  /***
   * Initialize the receiver.
   ***/
  irrecv.enableIRIn();
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

void loop()
{
  IRDecodeLoop();
  ArduinoOTA.handle();
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