#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define LCD pinout
const int  en = 2, rw = 1, rs = 0, d4 = 4, d5 = 5, d6 = 6, d7 = 7, bl = 3;

// Define I2C Address - change if reqiuired
const int i2c_addr = 0x27;

LiquidCrystal_I2C lcd(i2c_addr, en, rw, rs, d4, d5, d6, d7, bl, POSITIVE);
// Define the RX and TX pins for the MCU
#define MCU_RX A1
#define MCU_TX A0

// Create an instance of the SoftwareSerial library for the MCU
SoftwareSerial mcu(MCU_RX, MCU_TX);



// Values read from Bluetooth or Serial
float fValue = 5000.00;
int sensorValue = 50;
int n2Value = 0;
int n3Value = 0;
int i=0;

//1 Mux control pins
int m1s0 = 2;
int m1s1 = 3;
int m1s2 = 4;
int m1s3 = 5;

int m2s0 = 6;
int m2s1 = 7;
int m2s2 = 8;
int m2s3 = 9;

int RELAY_PIN = 12;

//make enable pin HIGH

//int m1en=22;
//int m2en=23;

void setup() {
   lcd.begin(16,2); 
  lcd.setCursor(0,0);
  lcd.print("BioZ IHUB");
 // delay(300);
 // lcd.clear();

  lcd.setCursor(0,1);
  lcd.print("Setting up");


  // Begin serial communication with the Arduino Serial Monitor at 9600 baud
  Serial.begin(9600);

  
  // Begin serial communication with the MCU at 230400 baud
  mcu.begin(230400);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); 


  pinMode(m1s0, OUTPUT); 
  pinMode(m1s1, OUTPUT); 
  pinMode(m1s2, OUTPUT); 
  pinMode(m1s3, OUTPUT); 

  digitalWrite(m1s0, LOW);
  digitalWrite(m1s1, LOW);
  digitalWrite(m1s2, LOW);
  digitalWrite(m1s3, LOW);

  pinMode(m2s0, OUTPUT); 
  pinMode(m2s1, OUTPUT); 
  pinMode(m2s2, OUTPUT); 
  pinMode(m2s3, OUTPUT); 

  digitalWrite(m2s0, LOW);
  digitalWrite(m2s1, LOW);
  digitalWrite(m2s2, LOW);
  digitalWrite(m2s3, LOW);

  // Set the motor enable pins as outputs
 // pinMode(m1en, OUTPUT);
 // pinMode(m2en, OUTPUT);

  // Make the motor enable pins HIGH
 // digitalWrite(m1en, HIGH);
 // digitalWrite(m2en, HIGH);



}

void relay_switch(){
  digitalWrite(RELAY_PIN, LOW);
  delay(10);
  digitalWrite(RELAY_PIN, HIGH);
  delay(10);
   
}


void checkForDataTransfer() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Reading from AD3029");
  Serial.println("Entered reading mode");


  // Add a delay of 50ms. This will allow your Arduino to read data from the MCU at a rate of 20 times per second.
 // delay(50);
  // Check if data is available to read from the MCU
  if (mcu.available()) {
    // Read the incoming line
    //Serial.println("Trying to read");

    String incomingLine = mcu.readStringUntil('\n');

    Serial.print("Freq:");
    Serial.print(fValue);
    Serial.println(incomingLine);


  //Serial.println("Read successful");

} while (mcu.available()); // Continue looping as long as there is data available from the MCU and the counter is less than 10

  
  //Serial.println("Exited reading mode");
}
void printSensorData(float f, int sensor, int n2, int n3) {
  
    Serial.print("Read values: ");
    Serial.print(f);
    Serial.print(", ");
    Serial.print(sensor);
    Serial.print(", ");
    Serial.println(n3);
lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Writing to AD3029");
   lcd.setCursor(0,1);
  lcd.print(f);///change here to diaplay ther parameters, later add scroll view here
//sending values to mcu
    mcu.print(fValue);
    mcu.print(",");
    mcu.println(sensorValue);
}
void readData() {
   lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Reading from App");
   Serial.println("enter values: ");
  // delay(5000); // Delay to ensure all data is received
     
  
  while (Serial.available() > 0) {
      
      

      String data = Serial.readStringUntil('\n');
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(data);
      fValue = data.substring(0, data.indexOf(',')).toFloat();
       
      data = data.substring(data.indexOf(',') + 1);
      sensorValue = data.substring(0, data.indexOf(',')).toInt();
      data = data.substring(data.indexOf(',') + 1);
      n2Value = data.substring(0, data.indexOf(',')).toInt();
      data = data.substring(data.indexOf(',') + 1);
      n3Value = data.toInt();
      printSensorData(fValue, sensorValue, n2Value, n3Value);
    
  }
}

void readMux(int m1channel, int m2channel){
  int m1controlPin[] = {m1s0, m1s1, m1s2, m1s3};
  int m2controlPin[] = {m2s0, m2s1, m2s2, m2s3};

  int muxChannel[16][4]={
    {0,0,0,0}, //channel 0
    {1,0,0,0}, //channel 1
    {0,1,0,0}, //channel 2
    {1,1,0,0}, //channel 3
    {0,0,1,0}, //channel 4
    {1,0,1,0}, //channel 5
    {0,1,1,0}, //channel 6
    {1,1,1,0}, //channel 7
    {0,0,0,1}, //channel 8
    {1,0,0,1}, //channel 9
    {0,1,0,1}, //channel 10
    {1,1,0,1}, //channel 11
    {0,0,1,1}, //channel 12
    {1,0,1,1}, //channel 13
    {0,1,1,1}, //channel 14
    {1,1,1,1}  //channel 15
  };

    for(i = 0; i < 4; i ++){
    digitalWrite(m1controlPin[i], muxChannel[m1channel][i]);
    digitalWrite(m2controlPin[i], muxChannel[m2channel][i]);
  }
}

void loop() {
  readData();
  readMux(n2Value,n3Value);

  Serial.println("switch");
  relay_switch();

 while(true) {
    // Check if data is available to read
    if (Serial.available() > 0) {
      char incomingCharacter = Serial.read(); // Read the incoming character

      // Check if the incoming character is 'b' or 'B'
      if (incomingCharacter == 'b' || incomingCharacter == 'B') {
        Serial.println("break recieved");
        break; // Break the loop
      }
    }

    checkForDataTransfer();
  }


}

