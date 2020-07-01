#include <ArduinoBLE.h>  
    
BLEService HDService("1101");  
BLEUnsignedIntCharacteristic HDChar("2101", BLENotify);  
   

/*variable setup*/
int Buzzer = 2;                // choose the pin for the buzzer (Digital 2)
int Sensor = 3;                // choose the input pin for PIR sensor (Digital 3)
int pirState = LOW;            // intial state assumes no motion is detected
int val = 0;                   // variable for reading the pin status
int counter =0;                // counter to keep track of how many times face is touched
  
  
void setup() {   
    Serial.begin(9600);  
     
    /*bluetooth setup*/  
    while (!Serial);  
   
    pinMode(LED_BUILTIN, OUTPUT);  
    if (!BLE.begin())  
    {  
      Serial.println("starting BLE failed!");  
      while (1);  
    }  
    
    BLE.setLocalName("HandDetector");  
    BLE.setAdvertisedService(HDService);  

    // add service and characteristic  
    HDService.addCharacteristic(HDChar);  
    BLE.addService(HDService);  
    HDChar.writeValue(0);  
      
    BLE.advertise();  
    Serial.println("Bluetooth device active, waiting for connections...");   

    /*PIR setup*/
    pinMode(Buzzer, OUTPUT);      // declare Buzzer as output
    pinMode(Sensor, INPUT);       // declare PIR sensor as input
 
}  
  
   
void loop() {  

  val = digitalRead(Sensor);      // read input value
  if (val == HIGH) {              // check if the input is HIGH
    digitalWrite(Buzzer, HIGH);   // turn buzzer on if user is about to touch their face
    if (pirState == LOW) {
      // hand is in threshold region near face
      Serial.println("Motion detected!");
      pirState = HIGH;
      counter = counter + 1;     // increment number of times face is touched 
    }
  } else {
    digitalWrite(Sensor, LOW);  // turn buzzer off if hand is removed from face
    if (pirState == HIGH){
      Serial.println("Motion ended!");
      pirState = LOW;
    }
  }  
   
 /*data transmission once hand motion is detected*/  
     if (pirState == HIGH){  
        //connect to bluetooth device to transmit counter
        BLEDevice central = BLE.central();  
   
        if (central)  
        {  
          digitalWrite(LED_BUILTIN, HIGH);  
   
          if (central.connected()) {  
               HDChar.writeValue(counter);  // transmit value to app via bluetooth
               Serial.println(counter);     // print value onto serial monitor for verification 
           }  
         }  
         digitalWrite(LED_BUILTIN, LOW);  
         Serial.println("Disconnected from central");  
       }  
}  
   
