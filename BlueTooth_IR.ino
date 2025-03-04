

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <AceButton.h>
using namespace ace_button;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run
`make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;



#define RelayPin1 23  
#define RelayPin2 22  


#define SwitchPin1 13  
#define SwitchPin2 12  


#define IR_RECV_PIN 35  //D35

int toggleState_1 = 1; 
int toggleState_2 = 1; 


String bt_data; 

IRrecv irrecv(IR_RECV_PIN);
decode_results results;

ButtonConfig config1;
AceButton button1(&config1);
ButtonConfig config2;
AceButton button2(&config2);


void handleEvent1(AceButton*, uint8_t, uint8_t);
void handleEvent2(AceButton*, uint8_t, uint8_t);


void relayOnOff(int relay){

    switch(relay){
      case 1: 
             if(toggleState_1 == 1){
              digitalWrite(RelayPin1, LOW); 
              toggleState_1 = 0;
              Serial.println("Device1 ON");
              }
             else{
              digitalWrite(RelayPin1, HIGH); 
              toggleState_1 = 1;
              Serial.println("Device1 OFF");
              }
             delay(100);
      break;
      case 2: 
             if(toggleState_2 == 1){
              digitalWrite(RelayPin2, LOW); 
              toggleState_2 = 0;
              Serial.println("Device2 ON");
              }
             else{
              digitalWrite(RelayPin2, HIGH); // turn off 
              toggleState_2 = 1;
              Serial.println("Device2 OFF");
              }
             delay(100);
      break;
      
      default : break;      
      }  
}


void all_Switch_ON(){
  digitalWrite(RelayPin1, LOW); toggleState_1 = 0; delay(100);
  digitalWrite(RelayPin2, LOW); toggleState_2 = 0; delay(100);

}

void all_Switch_OFF(){
  digitalWrite(RelayPin1, HIGH); toggleState_1 = 1; delay(100);
  digitalWrite(RelayPin2, HIGH); toggleState_2 = 1; delay(100);

}

void Bluetooth_handle()
{
  Serial.print(SerialBT.read());
  bt_data = SerialBT.read();
  Serial.println(bt_data);
  delay(20);

}

void ir_remote(){
  if (irrecv.decode(&results)) {
    
      switch(results.value){
          case 0x11D9D827:  relayOnOff(1);  break;
          case 0x11D99867:  relayOnOff(2);  break;
          case 0x11D9906F:  all_Switch_OFF();  break;
          case 0x11D930CF:  all_Switch_ON();  break;
          default : break;         
        }     
        irrecv.resume();   
  } 
}

void setup()
{
  Serial.begin(9600);

  irrecv.enableIRIn(); 
  btStart();  
  
  SerialBT.begin("ESP32_BT"); 
  Serial.println("The device started, now you can pair it with bluetooth!");
  delay(5000);

  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);


  pinMode(SwitchPin1, INPUT_PULLUP);
  pinMode(SwitchPin2, INPUT_PULLUP);


  digitalWrite(RelayPin1, toggleState_1);
  digitalWrite(RelayPin2, toggleState_2);


  config1.setEventHandler(button1Handler);
  config2.setEventHandler(button2Handler);


  button1.init(SwitchPin1);
  button2.init(SwitchPin2);


  delay(200);
}

void loop()
{  
    ir_remote();
    
    if (SerialBT.available()){
     Bluetooth_handle();
   }

    button1.check();
    button2.check();

}

void button1Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT1");
  relayOnOff(1);
}
void button2Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT2");
  relayOnOff(2);
}