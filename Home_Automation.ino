#include <Arduino.h>
#include <ESP32Servo.h>
#include <WiFiManager.h>
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include <IRremote.hpp>
 
#include <ArduinoIoTCloud.h>
 
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

char SSID[32];    // Network SSID (name)
char PASS[63];

void preferredConnectionHandler(char* ssid, char* pass) {
  char SSID[32];    // Network SSID (name)
char PASS[63];
  strcpy(SSID, ssid);
  strcpy(PASS, pass);
}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);

#define IR_RECEIVE_PIN 15
#define SERVO_PIN 13
#define RELAY1_PIN 2
#define RELAY2_PIN 4
#define RELAY3_PIN 16
#define RELAY4_PIN 17
#define SWITCH1_PIN 18
#define SWITCH2_PIN 19
#define SWITCH3_PIN 21
#define SWITCH4_PIN 22

Servo myServo;
bool relayState1 = false, relayState2 = false, relayState3 = false, relayState4 = false;
int servoPosition = 0;

void toggleRelay(int pin, bool &state) {
    state = !state;
    digitalWrite(pin, state ? HIGH : LOW);
}

void IRAM_ATTR switchHandler1() { toggleRelay(RELAY1_PIN, relayState1); }
void IRAM_ATTR switchHandler2() { toggleRelay(RELAY2_PIN, relayState2); }
void IRAM_ATTR switchHandler3() { toggleRelay(RELAY3_PIN, relayState3); }
void IRAM_ATTR switchHandler4() { toggleRelay(RELAY4_PIN, relayState4); }

void setup() {
    Serial.begin(115200);
    WiFiManager wifiManager;
    wifiManager.autoConnect("ESP32_Switches");
    
    pinMode(RELAY1_PIN, OUTPUT);
    pinMode(RELAY2_PIN, OUTPUT);
    pinMode(RELAY3_PIN, OUTPUT);
    pinMode(RELAY4_PIN, OUTPUT);
    pinMode(SWITCH1_PIN, INPUT_PULLUP);
    pinMode(SWITCH2_PIN, INPUT_PULLUP);
    pinMode(SWITCH3_PIN, INPUT_PULLUP);
    pinMode(SWITCH4_PIN, INPUT_PULLUP);
    myServo.attach(SERVO_PIN);
    
    attachInterrupt(digitalPinToInterrupt(SWITCH1_PIN), switchHandler1, FALLING);
    attachInterrupt(digitalPinToInterrupt(SWITCH2_PIN), switchHandler2, FALLING);
    attachInterrupt(digitalPinToInterrupt(SWITCH3_PIN), switchHandler3, FALLING);
    attachInterrupt(digitalPinToInterrupt(SWITCH4_PIN), switchHandler4, FALLING);
    
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    Serial.println("Setup complete. Waiting for commands...");
}

void loop() {
    if (IrReceiver.decode()) {
        unsigned long command = IrReceiver.decodedIRData.command;
        IrReceiver.resume();
        
        switch (command) {
            case 0x10: toggleRelay(RELAY1_PIN, relayState1); break;
            case 0x20: toggleRelay(RELAY2_PIN, relayState2); break;
            case 0x30: toggleRelay(RELAY3_PIN, relayState3); break;
            case 0x40: toggleRelay(RELAY4_PIN, relayState4); break;
            case 0x50: 
                servoPosition = (servoPosition == 0) ? 90 : 0;
                myServo.write(servoPosition);
                break;
        }
    }
}
