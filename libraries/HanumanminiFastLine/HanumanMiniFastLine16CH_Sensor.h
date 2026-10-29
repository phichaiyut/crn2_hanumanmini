// **********************************************************************************
//  HanumanMiniFastLine16CH_Sensor.h
//  เซนเซอร์ 16 ช่อง (ผ่าน Multiplexer CD74HC4067), ปุ่ม OK, Buzzer และการ Calibrate
// **********************************************************************************

#ifndef HanumanMiniFastLine16CH_Sensor_h
#define HanumanMiniFastLine16CH_Sensor_h

#include <Arduino.h>
#include <EEPROM.h>
#include "HanumanMiniFastLine16CH_CD74HC4067.h"

#define NUM_SENSORS 16  // จำนวนเซนเซอร์หน้า
#define OK_PIN 8        // ขาปุ่ม OK
#define PRESS 1         // ค่าที่อ่านได้เมื่อกดปุ่ม

// ----------------------------------------------------------------------------------
//  ตัวแปรของเซนเซอร์
// ----------------------------------------------------------------------------------
HanumanMini_CD74HC4067 my_mux(18, 17, 16, 15);  // ขาเลือกช่อง S0, S1, S2, S3
const int signal_pin = A0;                      // ขาอ่านค่าสัญญาณจาก Multiplexer

int MinValue[NUM_SENSORS];  // ค่าต่ำสุดของแต่ละช่อง (ได้จากการ Calibrate)
int MaxValue[NUM_SENSORS];  // ค่าสูงสุดของแต่ละช่อง (ได้จากการ Calibrate)
uint8_t F_PIN[NUM_SENSORS] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
uint16_t F[NUM_SENSORS];  // ค่าเซนเซอร์ล่าสุด (ดิบ หรือ 0-1000 หลัง Calibrate)
uint16_t sensorValues[NUM_SENSORS];

// ----------------------------------------------------------------------------------
//  ตัวแปรการเลี้ยว (ตั้งค่าผ่าน TurnSpeedLeft / TurnSpeedRight)
// ----------------------------------------------------------------------------------
int LCh, LTurnSpdL, LTurnSpdR, TurnDelayL;
int RCh, RTurnSpdL, RTurnSpdR, TurnDelayR;

// ----------------------------------------------------------------------------------
//  ตัวแปรตั้งค่าทั่วไป
// ----------------------------------------------------------------------------------
int LineColor = 0;       // 0 = เส้นดำ, 1 = เส้นขาว
int BUZZER_PIN = 13;     // ขา Buzzer
int volumesound = 2500;  // ความถี่เสียง Beep (Hz)
int REF = 0;             // ค่าอ้างอิงในการจับเส้น (0-1000)

// ==================================================================================
//  ตั้งค่า
// ==================================================================================
void TrackLineColor(int Col) {
  LineColor = Col;
}

void RefLineValue(int x) {
  REF = x;
}

void SetBUZZER_PIN(int x) {
  BUZZER_PIN = x;
}

void SetVolumeSound(int x) {
  volumesound = x;
}

// ==================================================================================
//  เสียง Buzzer
// ==================================================================================
void Beep(int delayb) {
  tone(BUZZER_PIN, volumesound, delayb);
  delay(delayb);
  noTone(BUZZER_PIN);
}

void BZon() {
  tone(BUZZER_PIN, volumesound);  // เปิดเสียงค้างไว้
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

// ==================================================================================
//  ปุ่ม OK
// ==================================================================================
void OK() {  // รอจนกว่าจะกดปุ่ม OK แล้ว Beep
  while (1) {
    if (digitalRead(OK_PIN) == PRESS) {
      break;
    }
  }
  Beep(100);
  delay(300);
}

int OK_PUSH() {  // คืนค่า PRESS ถ้ากำลังกดปุ่มอยู่ ไม่กดคืนค่า 0
  if (digitalRead(OK_PIN) == PRESS) return PRESS;
  return 0;
}

// ==================================================================================
//  อ่านค่า Analog ของบอร์ดโดยตรง (ช่อง 0-7 = A0-A7)
// ==================================================================================
int analog(int __ch) {
  if (__ch == 0) return analogRead(A0);
  if (__ch == 1) return analogRead(A1);
  if (__ch == 2) return analogRead(A2);
  if (__ch == 3) return analogRead(A3);
  if (__ch == 4) return analogRead(A4);
  if (__ch == 5) return analogRead(A5);
  if (__ch == 6) return analogRead(A6);
  if (__ch == 7) return analogRead(A7);
  return 0;  // ช่องไม่ถูกต้อง
}

// ==================================================================================
//  บันทึก / โหลดค่า Calibrate กับ EEPROM
// ==================================================================================
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

// ==================================================================================
//  อ่านค่าเซนเซอร์
// ==================================================================================
int analog16(int channel) {  // อ่านค่าดิบ 1 ช่องผ่าน Multiplexer (0-15)
  my_mux.channel(channel);
  int val = analogRead(signal_pin);
  return val;
}

void Read() {  // อ่านค่าดิบทั้ง 16 ช่อง เก็บใน F[]
  for (int _sensor = 0; _sensor < NUM_SENSORS; _sensor++) {
    F[_sensor] = analog16(F_PIN[_sensor]);
  }
}

void ReadCalibrate() {  // อ่านค่าแล้วแปลงเป็น 0-1000 (ค่ามาก = อยู่บนเส้น)
  Read();
  for (int i = 0; i < NUM_SENSORS; i++) {
    unsigned int calmin = MinValue[i];
    unsigned int calmax = MaxValue[i];
    long x = map(F[i], calmin, calmax, 0, 1000);
    if (LineColor != 0) x = 800 - x;  // เส้นขาว: กลับค่า
    if (x < 0) x = 0;
    if (x > 1000) x = 1000;
    F[i] = x;
  }
}

// ==================================================================================
//  Calibrate
// ==================================================================================
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

  // พิมพ์ออกมาเป็นคำสั่ง SensorValue(...) พร้อมคัดลอกไปใส่ใน setting.ino
  Serial.print("SensorValue (");
  for (int i = 0; i < NUM_SENSORS; i++) {  // MinValue
    Serial.print(MinValue[i]);
    Serial.print(",");
  }
  for (int i = 0; i < NUM_SENSORS; i++) {  // MaxValue
    Serial.print(MaxValue[i]);
    if (i < NUM_SENSORS - 1) Serial.print(",");
    else Serial.println(");");
  }

  SaveCalibration();
  b_beebb();
}

// ตั้งค่า Min (16 ค่า) และ Max (16 ค่า) ของเซนเซอร์เอง
void SensorValue(uint16_t minF0, uint16_t minF1, uint16_t minF2, uint16_t minF3,
                 uint16_t minF4, uint16_t minF5, uint16_t minF6, uint16_t minF7,
                 uint16_t minF8, uint16_t minF9, uint16_t minF10, uint16_t minF11,
                 uint16_t minF12, uint16_t minF13, uint16_t minF14, uint16_t minF15,
                 uint16_t maxF0, uint16_t maxF1, uint16_t maxF2, uint16_t maxF3,
                 uint16_t maxF4, uint16_t maxF5, uint16_t maxF6, uint16_t maxF7,
                 uint16_t maxF8, uint16_t maxF9, uint16_t maxF10, uint16_t maxF11,
                 uint16_t maxF12, uint16_t maxF13, uint16_t maxF14, uint16_t maxF15) {
  const uint16_t mins[NUM_SENSORS] = { minF0, minF1, minF2, minF3, minF4, minF5, minF6, minF7,
                                       minF8, minF9, minF10, minF11, minF12, minF13, minF14, minF15 };
  const uint16_t maxs[NUM_SENSORS] = { maxF0, maxF1, maxF2, maxF3, maxF4, maxF5, maxF6, maxF7,
                                       maxF8, maxF9, maxF10, maxF11, maxF12, maxF13, maxF14, maxF15 };
  for (int i = 0; i < NUM_SENSORS; i++) {
    MinValue[i] = mins[i];
    MaxValue[i] = maxs[i];
  }
}

// ==================================================================================
//  แสดงค่าเซนเซอร์ทาง Serial Monitor (วนไม่รู้จบ ใช้ตอนทดสอบ)
// ==================================================================================
void SerialSensor() {  // ค่าดิบ
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

void SerialCalibrateSensor() {  // ค่าที่ Calibrate แล้ว (0-1000)
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

#endif
