#include <WiFiManager.h>
#include <ESP32Servo.h>
 
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

char ssid[32]; // SSID char limit     
char pass[63];

const int resetButtonPin = 26;

#define IR_RECV_PIN 35
#define RelayPin1 23  
#define RelayPin2 22  
#define RelayPin3 21  
#define RelayPin4 19  
int servoPin = 18;
Servo myservo;

#define SwitchPin1 13  
#define SwitchPin2 12  
#define SwitchPin3 14  
#define SwitchPin4 27  

IRrecv irrecv(IR_RECV_PIN);
decode_results results;

int toggleState_1 = 0;
int toggleState_2 = 0;
int toggleState_3 = 0;
int toggleState_4 = 0;

bool SwitchState_1 = LOW;
bool SwitchState_2 = LOW;
bool SwitchState_3 = LOW;
bool SwitchState_4 = LOW;

CloudSwitch switch1;
CloudSwitch switch2;
CloudSwitch switch3;
CloudSwitch switch4;

void onSwitch1Change();
void onSwitch2Change();
void onSwitch3Change();
void onSwitch4Change();
void onServoChange();

void initProperties() {
    ArduinoCloud.setBoardId("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    ArduinoCloud.setSecretDeviceKey("xxxxxxxxxxxxxxxxx");
    ArduinoCloud.addProperty(switch1, READWRITE, ON_CHANGE, onSwitch1Change);
    ArduinoCloud.addProperty(switch2, READWRITE, ON_CHANGE, onSwitch2Change);
    ArduinoCloud.addProperty(switch3, READWRITE, ON_CHANGE, onSwitch3Change);
    ArduinoCloud.addProperty(switch4, READWRITE, ON_CHANGE, onSwitch4Change);
    ArduinoCloud.addProperty(myservo, READWRITE, ON_CHANGE, onServoChange);
}

void ir_remote_control() {
    if (irrecv.decode(&results)) {
        switch (results.value) {
            case 0x11D9D827: relayOnOff(1); switch1 = toggleState_1; break;
            case 0x11D99867: relayOnOff(2); switch2 = toggleState_2; break;
            case 0x11D9906F: relayOnOff(3); switch3 = toggleState_3; break;
            case 0x11D930CF: relayOnOff(4); switch4 = toggleState_4; break;
            case 0x11D9807F: myservo.write(min(myservo.read() + 30, 180)); break;
            case 0x11D9B04F: myservo.write(max(myservo.read() - 30, 0)); break;
            default: break;
        }
        Serial.println(results.value, HEX);
        irrecv.resume();
    }
}

void relayOnOff(int relay) {
    int* toggleState;
    int relayPin;
    CloudSwitch* cloudSwitch;
    switch (relay) {
        case 1: toggleState = &toggleState_1; relayPin = RelayPin1; cloudSwitch = &switch1; break;
        case 2: toggleState = &toggleState_2; relayPin = RelayPin2; cloudSwitch = &switch2; break;
        case 3: toggleState = &toggleState_3; relayPin = RelayPin3; cloudSwitch = &switch3; break;
        case 4: toggleState = &toggleState_4; relayPin = RelayPin4; cloudSwitch = &switch4; break;
        default: return;
    }
    digitalWrite(relayPin, *toggleState == 0 ? LOW : HIGH);
    *toggleState = !*toggleState;
    *cloudSwitch = *toggleState;
    Serial.printf("Device%d %s\n", relay, *toggleState ? "ON" : "OFF");
    delay(100);
}

void manual_control() {
    struct {
        int switchPin;
        int relayPin;
        int* toggleState;
        bool* switchState;
        CloudSwitch* cloudSwitch;
    } switches[] = {
        {SwitchPin1, RelayPin1, &toggleState_1, &SwitchState_1, &switch1},
        {SwitchPin2, RelayPin2, &toggleState_2, &SwitchState_2, &switch2},
        {SwitchPin3, RelayPin3, &toggleState_3, &SwitchState_3, &switch3},
        {SwitchPin4, RelayPin4, &toggleState_4, &SwitchState_4, &switch4}
    };

    for (auto& sw : switches) {
        if (digitalRead(sw.switchPin) == LOW && *sw.switchState == LOW) {
            digitalWrite(sw.relayPin, LOW);
            *sw.toggleState = 1;
            *sw.switchState = HIGH;
            *sw.cloudSwitch = *sw.toggleState;
            Serial.printf("Switch-%d on\n", sw.relayPin);
        } else if (digitalRead(sw.switchPin) == HIGH && *sw.switchState == HIGH) {
            digitalWrite(sw.relayPin, HIGH);
            *sw.toggleState = 0;
            *sw.switchState = LOW;
            *sw.cloudSwitch = *sw.toggleState;
            Serial.printf("Switch-%d off\n", sw.relayPin);
        }
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(RelayPin1, OUTPUT);
    pinMode(RelayPin2, OUTPUT);
    pinMode(RelayPin3, OUTPUT);
    pinMode(RelayPin4, OUTPUT);
    pinMode(SwitchPin1, INPUT_PULLUP);
    pinMode(SwitchPin2, INPUT_PULLUP);
    pinMode(SwitchPin3, INPUT_PULLUP);
    pinMode(SwitchPin4, INPUT_PULLUP);
    myservo.attach(servoPin);
    irrecv.enableIRIn();
    initProperties();
}

void loop() {
    ArduinoCloud.update();
    ir_remote_control();
    manual_control();
}
