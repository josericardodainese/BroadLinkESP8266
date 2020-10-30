# BroadLinkESP8266

IoT Broadlink to clone IR commands and send to control a device.

# Hardware

#### [ESP8266](https://randomnerdtutorials.com/getting-started-with-esp8266-wifi-transceiver-review/) - The ESP8266 is a Wi-Fi module great for IoT and Home Automation projects. This article is a getting started guide for the ESP8266 development board.

 
### Libraries

#### [crankyoldgit/IRremoteESP8266](https://github.com/crankyoldgit/IRremoteESP8266?utm_source=platformio&utm_medium=piohome) - Library to handle with IR


#### ssl fingerprint generate

If you have curl and openssl with you, you can do the following:

1) Get the certificate from your site (443=HTTPS port)
Code: [Select]
openssl s_client -connect dweet.io:443


2) copy the certificate (from "-----BEGIN CERTIFICATE-----" to "-----END CERTIFICATE-----") and paste into a file (cert.perm).

3) Then use the cert.perm to generate SHA1 fingerprint
Code: [Select]
openssl x509 -noout -in ./cert.perm -fingerprint -sha1


4) Copy the SHA1 key, and put into your code. :)


#### OLD CODE
´´´
String openWeatherMapApiKey = "24137916f961924d477b51f4910b830c";
String city = "Mogi Guacu";
String countryCode = "BR";
String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;
´´´

´´´
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
  http://openweathermap.org/img/w/02n.png

  Serial.print("Temperature: ");
  Serial.println(myObject["main"]["temp"]);

  Serial.print("Pressure: ");
  Serial.println(myObject["main"]["pressure"]);


  Serial.print("Humidity: ");
  Serial.println(myObject["main"]["humidity"]);

  Serial.print("Wind Speed: ");
  Serial.println(myObject["wind"]["speed"]);
  ´´´