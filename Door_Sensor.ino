
#define DOOR_SENSOR_PIN  19  
#include <HTTPClient.h>
#include <UrlEncode.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

const char DEVICE_LOGIN_NAME[]  = "e464a694-2a6c-4ff3-8513-db4df1a37b4e";

const char SSID[]               = xxxxxxx;    
const char PASS[]               = xxx;    
const char DEVICE_KEY[]  = xxxxxxxxxxxxxxx;    
void onServoChange();

int servo;
bool door;

void initProperties(){

  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);
  ArduinoCloud.addProperty(servo, READWRITE, ON_CHANGE, onServoChange);
  ArduinoCloud.addProperty(door, READ, ON_CHANGE, NULL);

}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);

#define BOT_TOKEN "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
#define CHAT_ID "xxxxxxxxxxxxxxxxxxxxxxxx"

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

int doorState;
bool doorStateChangeDetected = false;
bool wasDoorOpen = false;
bool dataSent = false;
const char* scriptId = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
const char* sheetName = "Sensor_Data";
float sensor2Value = 0.0;

void setup() {
  Serial.begin(9600);
  delay(1500);

  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
  pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP);
}

void loop() {
  ArduinoCloud.update();
  doorState = digitalRead(DOOR_SENSOR_PIN);
  delay(1000);

  if (doorState == HIGH) {
    if (!doorStateChangeDetected) {
      if (!dataSent) {
        Serial.println("The door is open");
        bot.sendMessage(CHAT_ID, "The door is open", "");
        sendMessage("The door is open");
        door = true;
        doorStateChangeDetected = true;
        String sensor1Value = "open";
        sendGoogle(sensor1Value, sensor2Value);
        dataSent = true;
      }
    }
  } else {
    doorStateChangeDetected = false;
    if (wasDoorOpen) {
      Serial.println("The door is closed");
      bot.sendMessage(CHAT_ID, "The door is closed", "");
      sendMessage("The door is closed");
      door = false;
      wasDoorOpen = false;
      String sensor1Value = "closed";
      sendGoogle(sensor1Value, sensor2Value);
      dataSent = false;
    }
  }

  if (doorState == HIGH) {
    wasDoorOpen = true;
  }

  delay(1000);
}

void sendGoogle(String sensor1, float sensor2) {
  HTTPClient http;
  String url = "https://script.google.com/macros/s/" + String(scriptId) + "/exec?id=" + sheetName;
  url += "&Sensor1=" + String(sensor1);
  url += "&Sensor2=" + String(sensor2);

  http.begin(url);
  int httpCode = http.GET();
  http.end();
}

void sendMessage(String message) {
  String url = "https://api.callmebot.com/whatsapp.php?phone=917557899105&text=" + urlEncode(message) + "&apikey=4720753";
  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpResponseCode = http.POST(url);
  http.end();
}

void onServoChange() {
  // Add your code here to act upon Servo change
}
