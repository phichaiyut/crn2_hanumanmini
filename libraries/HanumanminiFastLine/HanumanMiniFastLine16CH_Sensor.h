#include <light_CD74HC4067.h>
#include <EEPROM.h>
#define NUM_SENSORS 16
#define OK_PIN 8

#define PRESS 1

CD74HC4067 my_mux(18, 17, 16, 15);
const int signal_pin = A0;
int MinValue[NUM_SENSORS];
int MaxValue[NUM_SENSORS];
uint8_t F_PIN[NUM_SENSORS] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
uint16_t F[NUM_SENSORS];
uint16_t sensorValues[NUM_SENSORS];
int LCh,LTurnSpdL, LTurnSpdR, TurnDelayL;
int RCh,RTurnSpdL, RTurnSpdR, TurnDelayR;
int LineColor = 0;
int  BUZZER_PIN = 13;
int volumesound = 2500;
int REF = 0;


void TrackLineColor(int Col) {
  LineColor = Col;
}

void RefLineValue(int x) {
  REF = x;
}

void SetBUZZER_PIN(int x){
    BUZZER_PIN = x;
}

void SetVolumeSound(int x){
    volumesound = x;
}

void Beep(int delayb) {
  tone(BUZZER_PIN, volumesound, delayb);
  delay(delayb);
  noTone(BUZZER_PIN);
}
void BZon() {
  tone(BUZZER_PIN, volumesound);  // ส่งเสียง 1 kHz
}
void BZoff() {
  noTone(BUZZER_PIN);  // หยุดเสียง
}


void b_beebb() {
  tone(BUZZER_PIN, 2000, 80);
  delay(100);
  tone(BUZZER_PIN, 1000, 80);
  delay(100);
  tone(BUZZER_PIN, 2000, 80);
  delay(100);
  tone(BUZZER_PIN, 2500, 150);
  delay(100);
}
void OK() {
  while (1) {
    if (digitalRead(OK_PIN) == PRESS) {
      break;
    }
  }
  Beep(100);
  delay(300);
}

int OK_PUSH() {
  if (digitalRead(OK_PIN) == PRESS) return PRESS;
  else {
    return 0;
  }
}

int analog(int __ch) {
  if(__ch==0) return analogRead(A0);
  if(__ch==1) return analogRead(A1);
  if(__ch==2) return analogRead(A2);
  if(__ch==3) return analogRead(A3);
  if(__ch==4) return analogRead(A4);
  if(__ch==5) return analogRead(A5);
  if(__ch==6) return analogRead(A6);
  if(__ch==7) return analogRead(A7);
}

void SaveCalibration() {
  int addr = 0;

  // Save Min
  for (int i = 0; i < NUM_SENSORS; i++) {
    EEPROM.put(addr, MinValue[i]);
    addr += sizeof(int);
  }

  // Save Max
  for (int i = 0; i < NUM_SENSORS; i++) {
    EEPROM.put(addr, MaxValue[i]);
    addr += sizeof(int);
  }

  Serial.println("Saved to EEPROM");
}

#include <EEPROM.h>

void LoadCalibration() {
  int addr = 0;

  // Load Min
  for (int i = 0; i < NUM_SENSORS; i++) {
    EEPROM.get(addr, MinValue[i]);
    addr += sizeof(int);
  }

  // Load Max
  for (int i = 0; i < NUM_SENSORS; i++) {
    EEPROM.get(addr, MaxValue[i]);
    addr += sizeof(int);
  }

  Serial.println("=== Loaded Calibration from EEPROM ===");

  // Show Min values
  Serial.print("Min: ");
  for (int i = 0; i < NUM_SENSORS; i++) {
    Serial.print(MinValue[i]);
    if (i < NUM_SENSORS - 1) Serial.print(", ");
  }
  Serial.println();

  // Show Max values
  Serial.print("Max: ");
  for (int i = 0; i < NUM_SENSORS; i++) {
    Serial.print(MaxValue[i]);
    if (i < NUM_SENSORS - 1) Serial.print(", ");
  }
  Serial.println();

  Serial.println("=====================================");
}

int analog16(int channel) {
  my_mux.channel(channel);
  int val = analogRead(signal_pin);
  return val;
}

void Read() {
  for (int _sensor = 0; _sensor < NUM_SENSORS; _sensor++) {
    F[_sensor] = analog16(F_PIN[_sensor]);
  }
}

void ReadCalibrate() {
  if(LineColor ==0){
 Read();
  for (int i = 0; i < NUM_SENSORS; i++) {
    unsigned int calmin, calmax;
    long x = 0;
    calmin = MinValue[i];
    calmax = MaxValue[i];
    x = map(F[i], calmin, calmax, 0, 1000);
    if (x < 0) x = 0;
    if (x > 1000) x = 1000;
    F[i] = x;
  }
  }else{
        Read();
    for (int i = 0; i < NUM_SENSORS; i++) {
      unsigned int calmin, calmax;
      int x = 0;
      calmin = MinValue[i];
      calmax = MaxValue[i];
      x = map(F[i], calmin, calmax, 0, 1000);
      x = 800 - x;
      if (x < 0) x = 0;
      if (x > 1000) x = 1000;
      F[i] = x;
    }
  }
 
}

void CalibrateSensor(int pauseTime, int samples) {
  for (int i = 0; i < NUM_SENSORS; i++) {
    MinValue[i] = 1000;
    MaxValue[i] = 0;
  }
  for (int startSamp = 0; startSamp <= samples; startSamp++) {
    Read();
    for (int i = 0; i < NUM_SENSORS; i++) {
      MinValue[i] = (F[i] <= MinValue[i] ? F[i] : MinValue[i]);
      MaxValue[i] = (F[i] >= MaxValue[i] ? F[i] : MaxValue[i]);
    }
    delay(pauseTime);
  }
  for (int i = 0; i < NUM_SENSORS; i++) {
    MinValue[i] += 10;
    MaxValue[i] -= 40;
  }
}

void CaliberateRobotSensor() {
  Serial.println("Press OK to start caribrate Front Sensor");
  OK();
  Serial.println("Caribrating");
  delay(500);
  CalibrateSensor(20, 200);
  delay(500);
  Serial.println("Finish");
  Serial.println("  ");
  Serial.print("SensorValue (");
  //MinValue
  for (int i = 0; i < NUM_SENSORS; i++) {
    Serial.print(MinValue[i]);
    if (i < NUM_SENSORS - 1) Serial.print(",");
    else Serial.print(",");
  }
  //MaxValue
  for (int i = 0; i < NUM_SENSORS; i++) {
    Serial.print(MaxValue[i]);
    if (i < NUM_SENSORS - 1) Serial.print(",");
    else Serial.println(");");
  }

  SaveCalibration();
  b_beebb();
//   while (1)
//     ;
}

void SensorValue(uint16_t minF0, uint16_t minF1, uint16_t minF2, uint16_t minF3, uint16_t minF4, uint16_t minF5, uint16_t minF6, uint16_t minF7, uint16_t minF8, uint16_t minF9, uint16_t minF10, uint16_t minF11, uint16_t minF12, uint16_t minF13, uint16_t minF14, uint16_t minF15, uint16_t maxF0, uint16_t maxF1, uint16_t maxF2, uint16_t maxF3,
                 uint16_t maxF4, uint16_t maxF5, uint16_t maxF6, uint16_t maxF7, uint16_t maxF8, uint16_t maxF9, uint16_t maxF10, uint16_t maxF11, uint16_t maxF12, uint16_t maxF13, uint16_t maxF14, uint16_t maxF15) {
  MinValue[0] = minF0;
  MinValue[1] = minF1;
  MinValue[2] = minF2;
  MinValue[3] = minF3;
  MinValue[4] = minF4;
  MinValue[5] = minF5;
  MinValue[6] = minF6;
  MinValue[7] = minF7;
  MinValue[8] = minF8;
  MinValue[9] = minF9;
  MinValue[10] = minF10;
  MinValue[11] = minF11;
  MinValue[12] = minF12;
  MinValue[13] = minF13;
  MinValue[14] = minF14;
  MinValue[15] = minF15;

  MaxValue[0] = maxF0;
  MaxValue[1] = maxF1;
  MaxValue[2] = maxF2;
  MaxValue[3] = maxF3;
  MaxValue[4] = maxF4;
  MaxValue[5] = maxF5;
  MaxValue[6] = maxF6;
  MaxValue[7] = maxF7;
  MaxValue[8] = maxF8;
  MaxValue[9] = maxF9;
  MaxValue[10] = maxF10;
  MaxValue[11] = maxF11;
  MaxValue[12] = maxF12;
  MaxValue[13] = maxF13;
  MaxValue[14] = maxF14;
  MaxValue[15] = maxF15;
}



void SerialSensor() {
  while (1) {
    Read();
    for (int _serialF = 0; _serialF < NUM_SENSORS; _serialF++) {
      Serial.print(F[_serialF]);
      Serial.print("\t");
    }
    Serial.println("");
    delay(50);
  }
}



void SerialCalibrateSensor() {
  while (1) {
    ReadCalibrate();
    for (int _serialF = 0; _serialF < NUM_SENSORS; _serialF++) {
      Serial.print(F[_serialF]);
      Serial.print("\t");
    }
    Serial.println("");
    delay(100);
  }
}