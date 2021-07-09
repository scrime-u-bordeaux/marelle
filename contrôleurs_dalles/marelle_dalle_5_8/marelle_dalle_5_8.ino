#include <HX711_ADC.h>
//#define DEBUG

HX711_ADC lc1(14, 15); // loadCell(hx711 dout, hx711 sck) => pins!!!
HX711_ADC lc2(16, 17);
HX711_ADC lc3(18, 19);
HX711_ADC lc4(20, 21);

float data1, data2, data3, data4;
long t;
bool issue = 0;

void setup() {
  Serial.begin(57600); delay(10);
  Serial.println();
  Serial.println("Starting...");
  pinMode(13, OUTPUT);
  digitalWrite(13, 1);
  float calibrationValue;
  calibrationValue = 696.0;
  lc1.begin();
  lc2.begin();
  lc3.begin();
  lc4.begin();
  long stabilizingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  byte lc1_rdy = 0;
  byte lc2_rdy = 0;
  byte lc3_rdy = 0;
  byte lc4_rdy = 0;
  while ((lc1_rdy + lc2_rdy + lc3_rdy + lc4_rdy) < 4) { //run startup, stabilization and tare, both modules simultaniously
    if (!lc1_rdy) lc1_rdy = lc1.startMultiple(stabilizingtime, _tare);
    if (!lc2_rdy) lc2_rdy = lc2.startMultiple(stabilizingtime, _tare);
    if (!lc3_rdy) lc3_rdy = lc3.startMultiple(stabilizingtime, _tare);
    if (!lc4_rdy) lc4_rdy = lc4.startMultiple(stabilizingtime, _tare);
  }
  if (lc1.getTareTimeoutFlag()) {
    Serial.println("Timeout, check lc1");
    issue = 1;
  }
  if (lc2.getTareTimeoutFlag()) {
    Serial.println("Timeout, check lc2");
    issue = 1;
  }
  if (lc3.getTareTimeoutFlag()) {
    Serial.println("Timeout, check lc3");
    issue = 1;
  }
  if (lc4.getTareTimeoutFlag()) {
    Serial.println("Timeout, check lc4");
    issue = 1;
  }
  lc1.setCalFactor(500.0); // user set calibration value (float)
  lc2.setCalFactor(500.0);
  lc3.setCalFactor(500.0);
  lc4.setCalFactor(500.0);
  lc1.setSamplesInUse(1);
  lc2.setSamplesInUse(1);
  lc3.setSamplesInUse(1);
  lc4.setSamplesInUse(1);




  if (!issue) digitalWrite(13, 1);

  delay(1000);
  lc1.tareNoDelay();
  lc2.tareNoDelay();
  lc3.tareNoDelay();
  lc4.tareNoDelay();

}

void loop() {
  if (lc1.update()) {
    data1 = -1 * lc1.getData();
  }
  if (lc2.update()) {
    data2 = -1 * lc2.getData();
  }
  if (lc3.update()) {
    data3 = -1 * lc3.getData();
  }
  if (lc4.update()) {
    data4 = -1 * lc4.getData();
  }

#ifdef DEBUG
  Serial.print(lc1.getData());
  Serial.print("\t");
  Serial.print(lc2.getData());
  Serial.print("\t");
  Serial.print(lc3.getData());
  Serial.print("\t");
  Serial.print(lc4.getData());
  Serial.print("\t");
  Serial.println();
#endif

  float total = (data1 + data2 + data3 + data4);

  if (total > 1000) {
    float Xglobal = ((data2 + data4) - (data1 + data3)) / (total);
    float Yglobal = ((data1 + data2) - (data3 + data4)) / (total);
    Xglobal = 64 + Xglobal * 64;
    Xglobal = constrain(Xglobal, 0, 127);
    Yglobal = 64 + Yglobal * 64;
    Yglobal = constrain(Yglobal, 0, 127);
    Serial.print(Xglobal); Serial.print("\t");
    Serial.print(Yglobal); Serial.print("\t");
    //Serial.print(total); Serial.print("\t");
    usbMIDI.sendControlChange(40, Xglobal, 1);
    usbMIDI.sendControlChange(41, Yglobal, 1);
  } else {
    usbMIDI.sendControlChange(40, 64, 1);
    usbMIDI.sendControlChange(41, 64, 1);
  }
  Serial.println();
  delay(50);

}
