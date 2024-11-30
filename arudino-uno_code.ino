#include <SoftwareSerial.h>
#include <TM1637Display.h>  


const int RED_PIN = 9;
const int GREEN_PIN = 11;
const int BLUE_PIN = 10;


#define CLK 13  // Clock pin
#define DIO 12  // Data I/O pin


TM1637Display display(CLK, DIO);

//Segment definicja
#define SEG_A   0b00000001 // Segment A
#define SEG_B   0b00000010 // Segment B
#define SEG_C   0b00000100 // Segment C
#define SEG_D   0b00001000 // Segment D
#define SEG_E   0b00010000 // Segment E
#define SEG_F   0b00100000 // Segment F
#define SEG_G   0b01000000 // Segment G
#define SEG_DP  0b10000000 // Decimal Point

//Map
const uint8_t digitToSegment[] = {
  // XGFEDCBA
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111  // 9
};

uint8_t flipSegment(uint8_t seg) {
  uint8_t flippedSeg = 0;

  if (seg & SEG_A) flippedSeg |= SEG_D;
  if (seg & SEG_B) flippedSeg |= SEG_E;
  if (seg & SEG_C) flippedSeg |= SEG_F;
  if (seg & SEG_D) flippedSeg |= SEG_A;
  if (seg & SEG_E) flippedSeg |= SEG_B;
  if (seg & SEG_F) flippedSeg |= SEG_C;
  if (seg & SEG_G) flippedSeg |= SEG_G; 
  if (seg & SEG_DP) flippedSeg |= SEG_DP; 

  return flippedSeg;
}

void setup() {
  Serial.begin(9600);      


  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);


  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(BLUE_PIN, HIGH);

  
  display.setBrightness(0x0f);

  Serial.println("Arduino ready to receive data from ESP8266.");
}

void loop() {

  if (Serial.available() > 0) {
    delay(1000);
    String value = Serial.readStringUntil('\n');// citaj data od esp
    value.trim(); 
    int sensorValue = value.toInt();

    Serial.println("Received Sensor Value: " + String(sensorValue));

    int displayValue = sensorValue;

    //sredi gi cifrite
    uint8_t digits[4];
    digits[0] = (displayValue / 1000) % 10;
    digits[1] = (displayValue / 100) % 10;
    digits[2] = (displayValue / 10) % 10;
    digits[3] = displayValue % 10;

   
    uint8_t flippedSegments[4];
    for (int i = 0; i < 4; i++) {
      uint8_t seg = digitToSegment[digits[i]]; 
      uint8_t flippedSeg = flipSegment(seg);  
      flippedSegments[3 - i] = flippedSeg;     
    }

    display.setSegments(flippedSegments);

    if (sensorValue > 90) {
      setColor(1, 0, 0); 
    } else if (sensorValue > 50) {
      setColor(1, 1, 0);
    } else {
      setColor(0, 1, 0); 
    }

    //sekoi 10 minuti nova data ima
    delay(600000);
  }
}


void setColor(int red, int green, int blue) {
  digitalWrite(RED_PIN, red ? LOW : HIGH);
  digitalWrite(GREEN_PIN, green ? LOW : HIGH);
  digitalWrite(BLUE_PIN, blue ? LOW : HIGH);
}
