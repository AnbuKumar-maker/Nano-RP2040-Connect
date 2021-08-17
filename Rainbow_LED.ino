#include <WiFiNINA.h>

int rgbValues[] = {255, 0, 0}; // 0=Red, 1=Green and 2=Blue
int upIndex=0, downIndex=1;

void setup() {
  // Set the RGB pins to output
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
}

void loop() {
  rgbValues[upIndex] += 1;
  rgbValues[downIndex] -= 1;

  if(rgbValues[upIndex] > 255){
    rgbValues[upIndex] = 255;
    upIndex = upIndex + 1;

    if(upIndex > 2){
      upIndex = 0;
    }
  }

  if(rgbValues[downIndex] < 0){
    rgbValues[downIndex] = 0;
    downIndex = downIndex + 1;

    if(downIndex > 2){
      downIndex = 0;
    }
  }

  analogWrite(LEDR, 255 - rgbValues[0]);
  analogWrite(LEDG, 255 - rgbValues[1]);
  analogWrite(LEDB, 255 - rgbValues[2]);  
  
  delay(5);
}
