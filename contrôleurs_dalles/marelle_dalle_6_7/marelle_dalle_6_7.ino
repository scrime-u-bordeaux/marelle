#include <HX711_ADC.h>
//#define DEBUG

HX711_ADC lc1(4, 5); // loadCell(hx711 dout, hx711 sck) => pins!!!
HX711_ADC lc2(6, 7);

float data1, data2;
long t;
bool issue = 0;

void setup() {
  Serial.begin(57600); delay(10);
  Serial.println();
  Serial.println("Starting...");
  pinMode(13, OUTPUT);
  digitalWrite(13, 0);
  float calibrationValue;
  calibrationValue = 696.0;
  lc1.begin();
  lc2.begin();
  long stabilizingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  byte lc1_rdy = 0;
  byte lc2_rdy = 0;
  while (lc1_rdy + lc2_rdy  < 1) { //run startup, stabilization and tare, both modules simultaniously
    if (!lc1_rdy) lc1_rdy = lc1.startMultiple(stabilizingtime, _tare);
    if (!lc2_rdy) lc2_rdy = lc2.startMultiple(stabilizingtime, _tare);
  }
  if (lc1.getTareTimeoutFlag()) {
    Serial.println("Timeout, check lc1");
    issue = 1;
  }
  if (lc2.getTareTimeoutFlag()) {
    Serial.println("Timeout, check lc2");
    issue = 1;
  }
  lc1.setCalFactor(500.0);
  lc2.setCalFactor(500.0);
  lc1.setSamplesInUse(1);
  lc2.setSamplesInUse(1);

  if (!issue)  digitalWrite(13, 1);

  delay(1000);
  lc1.tareNoDelay();
  lc2.tareNoDelay();

  while (issue) {
    digitalWrite(13, 1);
    delay(500);
    digitalWrite(13, 0);
    delay(500);
  }
}

void loop() {
  if (lc1.update()) {
    data1 = lc1.getData();
  }
  if (lc2.update()) {
    data2 = lc2.getData();
  }

#ifdef DEBUG
  Serial.print(data1);
  Serial.print("\t");
  Serial.print(data2);
  Serial.print("\t");
#endif

  if (data1 + data2 > 500) {
    float Rglobal = data1 / (data1 + data2);
    float Lglobal = data2 / (data1 + data2);
    float total = (Rglobal - Lglobal)*64 +64;
    total = constrain(total, 0, 127);
    Serial.print(total); Serial.print("\t");
    usbMIDI.sendControlChange(42, total, 1);
  } else {
    usbMIDI.sendControlChange(42, 64, 1);
  }
  Serial.println();
  delay(50);

}
