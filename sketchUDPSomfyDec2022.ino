#include <EEPROM.h>
#include <EEPROMRollingCodeStorage.h>
#include <SomfyRemote.h>
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008
// pin 13,12,11,10 and 4 can NOT be used; they are used by the Ethernet shield

/*AUREL TRANSCEIVER PINS (from left to right):
pin 1: Antenna
pin 2: Ground
pin 3: N/A
pin 4: TX DATA
pin 5: RX/TX toggle 
pin 6: TX ENABLE - HIGH for Receiver; LOW for TRANSMITTER - ensure to include resistor here (e.g. 10kOhm)
pin 7: Ground
pin 8: (Analog Out)
pin 9: RX DATA = DATA OUT = DATA from receiver towards Arduino for decoding
pin 10: VCC
*/

#define SS_SD_CARD 4
#define SS_ETHERNET 10
#define SENDPIN 3
#define RECEIVEPIN 2
#define LED 8
#define BUTTON 9
#define ENABLE 7
#define TOGGLE 6  // HIGH = transmit: LOW = receive
#define REMOTE1 0x65dc00
#define REMOTE2 0x25b5d5
#define REMOTE3 0xc6c78f
#define REMOTE4 0x59714b

#define DEBUG true

#define localPort 1235  // local port to listen on for incoming UDP messages -- should be changed?
#define sendPort 1243  // receiver's port to  send UDP messages to

unsigned long lastPing = 0; // hold timer for ping response
char packetBuffer[63];          // buffer to hold incoming packet

EEPROMRollingCodeStorage rollingCodeStorage1(0);
EEPROMRollingCodeStorage rollingCodeStorage2(2);
EEPROMRollingCodeStorage rollingCodeStorage3(4);
EEPROMRollingCodeStorage rollingCodeStorage4(6);
SomfyRemote somfyRemote1(SENDPIN, REMOTE1, &rollingCodeStorage1);
SomfyRemote somfyRemote2(SENDPIN, REMOTE2, &rollingCodeStorage2);
SomfyRemote somfyRemote3(SENDPIN, REMOTE3, &rollingCodeStorage3);
SomfyRemote somfyRemote4(SENDPIN, REMOTE4, &rollingCodeStorage4);

EthernetUDP Udp;
EthernetClient client; 
IPAddress ip(192, 168, 15, 47);
//IPAddress ip(192, 168, 15, 49);// test IP
IPAddress gateway(192, 168, 15, 1);
IPAddress subnet(255, 255, 255, 0);
byte mac[] = {0xA8, 0x41, 0x0A, 0xAE, 0xA9, 0x86}; //correct voor dit bord!


void setup() {
	Serial.begin(115200);
	pinMode(SENDPIN, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(ENABLE, OUTPUT);
  pinMode(TOGGLE, OUTPUT);
	pinMode(RECEIVEPIN, INPUT);
  pinMode(SS_SD_CARD, OUTPUT);
  pinMode(SS_ETHERNET, OUTPUT);
  digitalWrite(SS_ETHERNET, LOW); // Ethernet active
  digitalWrite(SS_SD_CARD, HIGH); // SDCARD not active
  delay(1000); // give time to Ethernet shield to boot
  setethernet();
  //attachInterrupt(digitalPinToInterrupt(RECEIVEPIN), stateChange, CHANGE);  //only in case I would enable the RECEIVE function

}

void loop() {
  if (!checkping()) {
    setethernet();
  } 
  
  char* returnboodschap = getUDPmessage();
  if (DEBUG) {
    if (returnboodschap != "") {
      Serial.print(F("volgende boodschap kwam binnen: "));
      Serial.println(returnboodschap);
    }
  }
  delay(2);

  if (strcmp_P(returnboodschap, PSTR("SEND=raamhaard:omhoog")) == 0) {
    raamhaardomhoog();
  }  
 if (strcmp_P(returnboodschap, PSTR("SEND=raamhaard:omlaag")) == 0) {
    raamhaardomlaag();
  } 
   if (strcmp_P(returnboodschap, PSTR("SEND=raamliving:omhoog")) == 0) {
    raamlivingomhoog();
  }   
 if (strcmp_P(returnboodschap, PSTR("SEND=raamliving:omlaag")) == 0) {
    raamlivingomlaag();
  } 
  if (strcmp_P(returnboodschap, PSTR("SEND=screenhaard:omhoog")) == 0) {
    screenhaardomhoog();
  }  
 if (strcmp_P(returnboodschap, PSTR("SEND=screenhaard:omlaag")) == 0) {
    screenhaardomlaag();
  } 
   if (strcmp_P(returnboodschap, PSTR("SEND=screenliving:omhoog")) == 0) {
    screenlivingomhoog();
  }   
 if (strcmp_P(returnboodschap, PSTR("SEND=screenliving:omlaag")) == 0) {
    screenlivingomlaag();
  } 
  
 if (pushbutton()){
   Serial.println("PROGRAM BUTTON ON!!");
   // start the programming of the selected Remote
    programRemote();   
 }

}

void raamhaardomhoog() {
  digitalWrite(ENABLE, HIGH);
  delay(50);
  digitalWrite(TOGGLE, HIGH);
  delay(50);
  somfyRemote1.sendCommand(Command::Up, 1);
  sendUDPmessage("REPLY=raamhaard:omhoog");
  delay(200);
  digitalWrite(TOGGLE, LOW);
  delay(10);
  digitalWrite(ENABLE, LOW);
}

void raamhaardomlaag() {
  digitalWrite(ENABLE, HIGH);
  delay(50);
  digitalWrite(TOGGLE, HIGH);
  delay(50);
  somfyRemote1.sendCommand(Command::Down, 1);
  sendUDPmessage("REPLY=raamhaard:omlaag");
  delay(200);
  digitalWrite(TOGGLE, LOW);
  delay(10);
  digitalWrite(ENABLE, LOW);
}

void raamlivingomhoog() {
  digitalWrite(ENABLE, HIGH);
  delay(50);
  digitalWrite(TOGGLE, HIGH);
  delay(50);
  somfyRemote2.sendCommand(Command::Up, 1);
  sendUDPmessage("REPLY=raamliving:omhoog");
  delay(200);
  digitalWrite(TOGGLE, LOW);
  delay(10);
  digitalWrite(ENABLE, LOW);
}

void raamlivingomlaag() {
  digitalWrite(ENABLE, HIGH);
  delay(50);
  digitalWrite(TOGGLE, HIGH);
  delay(50);
  somfyRemote2.sendCommand(Command::Down, 1);
  sendUDPmessage("REPLY=raamliving:omhoog");
  delay(200);
  digitalWrite(TOGGLE, LOW);
  delay(10);
  digitalWrite(ENABLE, LOW);
}


void screenhaardomhoog() {
  digitalWrite(ENABLE, HIGH);
  delay(50);
  digitalWrite(TOGGLE, HIGH);
  delay(50);
  somfyRemote4.sendCommand(Command::Up, 1);
  sendUDPmessage("REPLY=screenhaard:omhoog");
  delay(200);
  digitalWrite(TOGGLE, LOW);
  delay(10);
  digitalWrite(ENABLE, LOW);
}

void screenhaardomlaag() {
  digitalWrite(ENABLE, HIGH);
  delay(50);
  digitalWrite(TOGGLE, HIGH);
  delay(50);
  somfyRemote4.sendCommand(Command::Down, 1);
  sendUDPmessage("REPLY=screenhaard:omlaag");
  delay(200);
  digitalWrite(TOGGLE, LOW);
  delay(10);
  digitalWrite(ENABLE, LOW);
}

void screenlivingomhoog() {
  digitalWrite(ENABLE, HIGH);
  delay(50);
  digitalWrite(TOGGLE, HIGH);
  delay(50);
  somfyRemote3.sendCommand(Command::Up, 1);
  sendUDPmessage("REPLY=screenliving:omhoog");
  delay(200);
  digitalWrite(TOGGLE, LOW);
  delay(10);
  digitalWrite(ENABLE, LOW);
}

void screenlivingomlaag() {
  digitalWrite(ENABLE, HIGH);
  delay(50);
  digitalWrite(TOGGLE, HIGH);
  delay(50);
  somfyRemote3.sendCommand(Command::Down, 1);
  sendUDPmessage("REPLY=screenliving:omlaag");
  delay(200);
  digitalWrite(TOGGLE, LOW);
  delay(10);
  digitalWrite(ENABLE, LOW);
}

bool pushbutton(){
  int temp = 5;
  for (int i = 0; i< 5; i++){
    temp = temp - digitalRead(BUTTON);
    //Serial.print(F("we do a digital Read of the button, the result is: "));Serial.println(digitalRead(BUTTON));  
    } 
  if (temp < 2) {
    return 1;
  }
  else {
    return 0;
  }    
}

void programRemote(){
  Serial.println(F("type the nr of the Remote you wish to control (followed by Enter), to CANCEL: press 0"));
  String remotestring;
  int remotenr=0;
  unsigned long start = 0;
  // check input for 30 sec; otherwise continue the program
  start = millis();
  while (millis()-start < 30000){  
    digitalWrite(LED,HIGH);      
    if (Serial.available() > 0) {
      remotestring = Serial.readStringUntil('\n'); 
      remotenr = remotestring.toInt();   
      break;
 	  } 
  }  
  digitalWrite(LED,LOW); 
  Serial.print(F("we will now program remote nr: "));Serial.println(remotenr);
  digitalWrite(ENABLE, HIGH);
  delay(50);
  digitalWrite(TOGGLE, HIGH);
  delay(50);    
  switch (remotenr) {
    case 0:    
      break;
    case 1:
      somfyRemote1.sendCommand(Command::Prog, 1);  
      Serial.println("progam commend executed"); 
      break;
    case 2:
        somfyRemote2.sendCommand(Command::Prog, 1); 
        Serial.println("progam commend executed"); 
      break;
    case 3:
        somfyRemote3.sendCommand(Command::Prog, 1); 
        Serial.println("progam commend executed"); 
      break;
    case 4:
        somfyRemote4.sendCommand(Command::Prog, 1);
        Serial.println("progam commend executed");  
      break;
    default:
        Serial.println("no valid input");        
  }
  digitalWrite(ENABLE, LOW);
  delay(50);
  digitalWrite(TOGGLE, LOW);
  delay(50); 
}

void sendUDPmessage(char* message) {
  if (DEBUG) {
    Serial.println(F("we will now send a message"));
  }
  Udp.beginPacket(IPAddress(192, 168, 15, 39), sendPort);
  uint8_t countBeforeReset = 0;
  while (Udp.write(message) == 0) {
    if (countBeforeReset == 3) {
      setethernet();
      Udp.write(message);
      Serial.println(message);
      break;
    }
    if (DEBUG) {
      Serial.println(F("reconnecting to udp"));
    }
    setudp();
    countBeforeReset ++;
  }
  Udp.endPacket();
  delay(100);
}

char* getUDPmessage() {
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    if (DEBUG) {
      Serial.print(F("Received packet of size "));
      Serial.println(packetSize);
      Serial.print(F("From "));
      IPAddress remoteIp = Udp.remoteIP();
      Serial.print(remoteIp);
      Serial.print(F(", port "));
      Serial.println(Udp.remotePort());
    }
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;
    }
    delay(100);
    return packetBuffer;
  }
  return "";
}

void setethernet() {
  Ethernet.begin(mac, ip, gateway, subnet);
  //Ethernet.begin(mac, ip);
  delay(2000);
  Serial.print(F("IP = "));
  Serial.println(Ethernet.localIP());
  setudp();
  sendUDPmessage("SCREENS:live!!");
  delay(30);
  //sendUDPmessage(" ");
}

void setudp() {
  Udp.stop();
  if (DEBUG) {
    Serial.print(F("UDP stopped..."));
  }
  delay(100);
  Udp.begin(localPort);
  if (DEBUG) {
    Serial.print(F("Listening on port "));
    Serial.println(localPort);
  }
  delay(10);
}

bool checkping() {
  if (millis() - lastPing > 600000) {
    lastPing = millis();
    int pingResult = 0;
    pingResult = client.connect("192.168.15.4", 80);
    Serial.print(F("result van pingResult: ")); Serial.println(pingResult);
    if (pingResult == 1) {
      if (DEBUG) {
        Serial.println(F("ping ok - connected...."));
      }
      client.stop();
      return 1;
    } else {
      //try to ping again
      delay(100);
      pingResult = client.connect("192.168.15.4", 80);      
      if (pingResult == 1) { 
        if (DEBUG) {
        Serial.println(F("ping ok - connected in second attempt...."));
      }     
        client.stop();
        return 1;
      }  
      else {  
        if (DEBUG) {
        Serial.println(F("can not ping...."));
        }    
        return 0;
      }
    }
  }
  else {
    return 1;
  }
}