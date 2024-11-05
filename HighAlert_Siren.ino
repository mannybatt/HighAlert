



// ***************************************
// ********** Global Variables ***********
// ***************************************


//Globals fo Wifi Setup and OTA
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//WiFi Credentials
#ifndef STASSID
#define STASSID "Frostmourne"
#define STAPSK  "warproom"
#endif
const char* ssid = STASSID;
const char* password = STAPSK;

//Globals for MQTT
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#define MQTT_CONN_KEEPALIVE 300
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "atomisk966"
#define AIO_KEY         "e0c3d4198f7c42c6843cc67d87c9629d"

//MQTT Startup
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe highAlert = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/HighAlert");

//MP3 Player
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>
DFRobotDFPlayerMini myDFPlayer;
SoftwareSerial mySoftwareSerial(D3, D2);  //Pins for MP3 Player Serial (RX, TX)

//EEPROM
#include <ESP_EEPROM.h>

//Input-Output
#define power D6
#define pattern D5

//Variables
int numberOfSongs = 485; //Not counting boot sound
int randomList[485] = {138, 272, 349, 405, 190, 391, 94, 260, 465, 170, 438, 451, 163, 208, 431, 382, 230, 303, 482, 213, 11, 250, 420, 65, 310, 120, 74, 459, 40, 309, 52, 263, 86, 425, 448, 273, 483, 460, 449, 418, 135, 429, 198, 378, 348, 336, 186, 413, 276, 201, 18, 128, 411, 164, 377, 205, 305, 98, 234, 173, 366, 408, 452, 389, 66, 109, 242, 121, 246, 224, 471, 222, 211, 275, 270, 335, 24, 41, 15, 437, 161, 237, 82, 286, 195, 184, 189, 267, 344, 146, 185, 221, 397, 162, 468, 108, 130, 227, 314, 22, 231, 399, 327, 148, 345, 383, 245, 243, 266, 285, 269, 143, 107, 252, 265, 27, 35, 479, 45, 38, 25, 167, 318, 95, 328, 364, 292, 317, 308, 182, 291, 295, 347, 458, 400, 134, 238, 253, 315, 57, 12, 313, 256, 132, 355, 450, 343, 445, 216, 140, 376, 77, 444, 239, 476, 62, 194, 457, 166, 302, 106, 461, 282, 226, 196, 159, 179, 1, 2, 204, 430, 3, 290, 390, 249, 251, 100, 470, 326, 261, 36, 372, 363, 4, 369, 280, 139, 47, 131, 101, 288, 478, 339, 91, 294, 7, 410, 149, 403, 380, 402, 480, 373, 145, 141, 212, 214, 467, 301, 21, 330, 118, 466, 346, 209, 463, 63, 206, 197, 271, 331, 115, 39, 147, 264, 156, 393, 277, 174, 332, 407, 219, 446, 439, 401, 416, 202, 171, 64, 353, 323, 199, 215, 13, 70, 8, 322, 404, 477, 188, 183, 304, 235, 26, 154, 436, 9, 126, 192, 81, 37, 284, 88, 423, 44, 443, 354, 60, 169, 454, 395, 105, 136, 203, 356, 398, 485, 255, 342, 307, 117, 473, 58, 475, 113, 481, 367, 232, 340, 71, 75, 359, 334, 129, 124, 427, 311, 441, 287, 168, 6, 20, 262, 415, 137, 268, 419, 14, 434, 93, 417, 357, 119, 324, 180, 112, 240, 228, 43, 53, 388, 133, 54, 381, 217, 289, 421, 325, 341, 361, 78, 67, 248, 28, 30, 84, 176, 187, 278, 79, 99, 175, 73, 352, 103, 374, 371, 220, 50, 127, 306, 338, 456, 61, 279, 218, 29, 193, 453, 92, 165, 333, 362, 433, 46, 17, 16, 281, 123, 97, 375, 152, 31, 387, 210, 233, 229, 435, 464, 85, 385, 207, 225, 111, 384, 365, 153, 110, 368, 158, 258, 59, 125, 320, 394, 386, 321, 360, 424, 172, 68, 474, 337, 469, 23, 379, 412, 283, 51, 350, 274, 72, 80, 102, 178, 34, 160, 392, 244, 200, 300, 428, 254, 150, 181, 142, 55, 432, 42, 191, 299, 406, 87, 69, 5, 298, 316, 484, 49, 236, 177, 472, 96, 89, 351, 151, 296, 116, 293, 122, 426, 257, 83, 241, 33, 56, 358, 247, 76, 447, 104, 90, 462, 297, 48, 422, 114, 223, 19, 329, 144, 440, 10, 157, 455, 32, 312, 409, 396, 319, 442, 155, 370, 414, 259};
int currentSongNumber = 0;
//Default lighter flicking boot sound is 0486




// ***************************************
// *************** Setup *****************
// ***************************************


void setup() {

  //Initialize Switches
  pinMode(power, OUTPUT);
  pinMode(pattern, OUTPUT);
  digitalWrite(power, LOW);
  digitalWrite(pattern, LOW);

  //Initialize Serial, WiFi, & OTA
  wifiSetup();

  //Initialize MQTT
  mqtt.subscribe(&highAlert);
  MQTT_connect();
  delay(1000);

  //EEPROM variable size initialization
  EEPROM.begin(16);

  /******* TO RESET CURRENT SONG UNCOMMENT BELOW *******/
  //Place data into EEPROM
  //EEPROM.put(0, currentSongNumber);  // int - so 4 bytes (next address is '4')

  //Actually write that data to EEPROM
  //boolean ok = EEPROM.commit();
  //Serial.println((ok) ? "First commit OK" : "Commit failed");

  //Load current song from EEPROM
  EEPROM.get(0, currentSongNumber);

  //MP3
  mySoftwareSerial.begin(9600);
  delay(1000);
  myDFPlayer.begin(mySoftwareSerial);
  Serial.println();
  Serial.println("DFPlayer initialized!");
  myDFPlayer.setTimeOut(500); //Timeout serial 500ms
  myDFPlayer.volume(0); //Volume 0-30
  myDFPlayer.EQ(DFPLAYER_EQ_ROCK); //Equalization normal
  delay(1000);
  myDFPlayer.volume(20);
  myDFPlayer.play(486);
  delay(1000);

  //Done
  Serial.println("setup complete");
}




// ***************************************
// ************* Da Loop *****************
// ***************************************


void loop() {

  //Network Housekeeping
  ArduinoOTA.handle();
  MQTT_connect();

  //State Manager
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(10))) {
    //Serial.println("Subscription Recieved");
    uint16_t value = atoi((char *)highAlert.lastread);

    //High Alert Siren
    if (value == 420) {
      highSounds();
      digitalWrite(power, HIGH);
      delay(13500);
      digitalWrite(power, LOW);
    }
    //Turn on
    if (value == 1) {
      digitalWrite(power, HIGH);
    }
    //Next Pattern
    if (value == 2) {
      digitalWrite(pattern, HIGH);
      delay(400);
      digitalWrite(pattern, LOW);
    }
    //Turn off
    if (value == 0) {
      digitalWrite(power, LOW);
    }
    //Do nothing
    if (value == 10) {}


    //**** TimeClockParty ****//
    //Play Boot Sound
    if (value == 101) {
      myDFPlayer.play(152);
      delay(1000);
    }
    //Play one of da Soundz
    if (value == 104) {
      digitalWrite(power, HIGH);
      int chosenOne = randomList[currentSongNumber];
      myDFPlayer.play(chosenOne);
      Serial.print("Song #");
      Serial.println(chosenOne);
      delay(1000);
      //Determine length of sound and end siren asap
      for (int i = 0; i < 158; i++) {
        int daStateOfDFPlayer = myDFPlayer.readState();
        Serial.print("Seconds: ");
        Serial.print(i);
        Serial.print("   ");
        Serial.println(daStateOfDFPlayer);

        if(daStateOfDFPlayer == 512 && myDFPlayer.readState() == 512){
          i = 999;
        }
        
        delay(200);
      }
      digitalWrite(power, LOW);
      currentSongNumber++;
      if (currentSongNumber > numberOfSongs) {
        currentSongNumber = 1;
      }

      //Commit currentSong to EEPROM
      EEPROM.put(0, currentSongNumber);
      boolean ok = EEPROM.commit();
      Serial.println((ok) ? "First commit OK" : "Commit failed");

      Serial.println("Songo Donezo");
    }
  }
  delay(50);
}




// ***************************************
// ********** Backbone Methods ***********
// ***************************************


void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

void highSounds() {
  myDFPlayer.play(2);
}

void wifiSetup() {

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  ArduinoOTA.setHostname("HighAlert-Siren");
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    //Serial.println("Connected");
    return;
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      //while (1);
      Serial.println("Wait 10 min to reconnect");
      delay(600000);
    }
  }
  Serial.println("MQTT Connected!");
}
