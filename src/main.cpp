#include <Arduino.h>
#include <IRrecv.h>

/***
 * Digital Pin that IR receiver are connected.
 * GPIO12 / PINO D6
 ***/
int RECV_PIN = 12;

/***
 * Variable IRrecv.
 ***/
IRrecv irrecv(RECV_PIN);

/***
 * Varible that store the results.
 ***/
const decode_results results;
 
void setup(){
  /***
   * Initialize the serial monitor.
   ***/
  Serial.begin(115200); //INICIALIZA A SERIAL
  
  /***
   * Initialize the receiver.
   ***/
  irrecv.enableIRIn();  //INICIALIZA O RECEPTOR
}

/***
 * Method responsable for doing a decode of the IR signal received 
 * where the data are passed to the library IR Remote that make all 
 * treatment and return the informations accord the protocol recognized.
 ***/
void dump(decode_results *results) {
  int count = results->rawlen;
  if (results->decode_type == UNKNOWN) {
    Serial.print("Unknown encoding: ");
  }
  else if (results->decode_type == NEC) {
    Serial.print("Decoded NEC: ");
  }
  else if (results->decode_type == SONY) {
    Serial.print("Decoded SONY: ");
  }
  else if (results->decode_type == RC5) {
    Serial.print("Decoded RC5: ");
  }
  else if (results->decode_type == RC6) {
    Serial.print("Decoded RC6: ");
  }
  else if (results->decode_type == PANASONIC) {
    Serial.print("Decoded PANASONIC - Address: ");
    Serial.print(results->panasonicAddress, HEX);
    Serial.print(" Value: ");
  }
  else if (results->decode_type == LG) {
    Serial.print("Decoded LG: ");
  }
  else if (results->decode_type == JVC) {
    Serial.print("Decoded JVC: ");
  }
  else if (results->decode_type == AIWA_RC_T501) {
    Serial.print("Decoded AIWA RC T501: ");
  }
  else if (results->decode_type == WHYNTER) {
    Serial.print("Decoded Whynter: ");
  }
  Serial.print(results->value, HEX);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
  Serial.print("Raw (");
  Serial.print(count, DEC);
  Serial.print("): ");
 
  for (int i = 1; i < count; i++) {
    if (i & 1) {
      Serial.print(results->rawbuf[i]*USECPERTICK, DEC);
    }
    else {
      Serial.write('-');
      Serial.print((unsigned long) results->rawbuf[i]*USECPERTICK, DEC);
    }
    Serial.print(" ");
  }
  Serial.println();
}
 
void loop() {
  //RETORNA NA SERIAL AS INFORMAÇÕES FINAIS SOBRE O COMANDO IR QUE FOI IDENTIFICADO
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    dump(&results);
    irrecv.resume(); //RECEBE O PRÓXIMO VALOR
  }
}