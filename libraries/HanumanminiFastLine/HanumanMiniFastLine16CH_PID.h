// **********************************************************************************
//  HanumanMiniFastLine16CH_PID.h
//  การหาตำแหน่งเส้น, PID เดินตามเส้น, จับแยก และเดินตามเส้นตามเวลา
//
//  ตำแหน่งเส้น (Position) : 0 = เส้นอยู่ช่อง 0, 7500 = กลาง, 15000 = เส้นอยู่ช่อง 15
// **********************************************************************************

#ifndef HanumanMiniFastLine16CH_PID_h
#define HanumanMiniFastLine16CH_PID_h

#include <Arduino.h>
#include "HanumanMiniFastLine16CH_Sensor.h"
#include "HanumanMiniFastLine16CH_Motor.h"
#include "HanumanMiniFastLine16CH_Initial.h"
#include "HanumanMiniFastLine16CH_ForwardBackwardAndTurnl.h"

int MaxSpeed = 100;     // ความเร็วสูงสุดที่ PID สั่งได้
int MinSpeed = 0;       // ความเร็วต่ำสุดที่ PID สั่งได้
int ModePidStatus = 0;  // โหมดจำกัดความเร็ว (ดู LimitPower)
int LastError;          // Error รอบก่อนหน้า (ใช้คำนวณ D)
int dottedline = 0;     // 1 = มีเส้นประ (หลุดเส้นแล้วใช้ตำแหน่งเดิม)

// ==================================================================================
//  ตั้งค่า
// ==================================================================================
void Dottedline(int Dot) {  // 0 = ไม่มีเส้นประ, 1 = มีเส้นประ
  dottedline = Dot;
}

void ModeSpdPID(int moD, int maX, int miN) {  // (โหมด, ความเร็วสูงสุด, ความเร็วต่ำสุด)
  ModePidStatus = moD;
  MaxSpeed = maX;
  MinSpeed = miN;
}

// ==================================================================================
//  จำกัดความเร็วมอเตอร์หลังคำนวณ PID ตามโหมด ModePidStatus
//    0 : เกิน MaxSpeed -> MaxSpeed, ติดลบ -> MinSpeed
//    1 : จำกัดอยู่ระหว่าง MinSpeed ถึง MaxSpeed (โหมดปกติ)
//    2 : จำกัดอยู่ระหว่าง -Speed ถึง Speed ของแต่ละล้อ (โค้งเยอะ วิ่งช้า)
//    3 : เกิน MaxSpeed -> MaxSpeed, ติดลบ -> -BaseSpeed
//    อื่น ๆ : จำกัดอยู่ระหว่าง 0 ถึง MaxSpeed
// ==================================================================================
void LimitPower(int &LeftPower, int &RightPower, int SpeedL, int SpeedR) {
  switch (ModePidStatus) {
    case 0:
      if (LeftPower > MaxSpeed) LeftPower = MaxSpeed;
      if (LeftPower < 0) LeftPower = MinSpeed;
      if (RightPower > MaxSpeed) RightPower = MaxSpeed;
      if (RightPower < 0) RightPower = MinSpeed;
      break;
    case 1:
      if (LeftPower > MaxSpeed) LeftPower = MaxSpeed;
      if (LeftPower < MinSpeed) LeftPower = MinSpeed;
      if (RightPower > MaxSpeed) RightPower = MaxSpeed;
      if (RightPower < MinSpeed) RightPower = MinSpeed;
      break;
    case 2:
      if (LeftPower > SpeedL) LeftPower = SpeedL;
      if (LeftPower < -SpeedL) LeftPower = -SpeedL;
      if (RightPower > SpeedR) RightPower = SpeedR;
      if (RightPower < -SpeedR) RightPower = -SpeedR;
      break;
    case 3:
      if (LeftPower > MaxSpeed) LeftPower = MaxSpeed;
      if (LeftPower < 0) LeftPower = -BaseSpeed;
      if (RightPower > MaxSpeed) RightPower = MaxSpeed;
      if (RightPower < 0) RightPower = -BaseSpeed;
      break;
    default:
      if (LeftPower > MaxSpeed) LeftPower = MaxSpeed;
      if (LeftPower < 0) LeftPower = 0;
      if (RightPower > MaxSpeed) RightPower = MaxSpeed;
      if (RightPower < 0) RightPower = 0;
  }
}

// ==================================================================================
//  หาตำแหน่งเส้น ใช้เซนเซอร์ครบ 16 ช่อง
//    Track = ค่าที่ถือว่าเจอเส้น, noise = ค่าที่ต่ำกว่านี้ไม่นำมาคิด
// ==================================================================================
int readPosition(int Track, int noise) {
  unsigned char i, online = 0;
  unsigned long avg = 0;
  unsigned int sum = 0;
  static int last_value = 0;

  ReadCalibrate();
  for (i = 0; i < NUM_SENSORS; i++) {
    int values = F[i];
    if (values > Track) online = 1;
    if (values > noise) {
      avg += (long)(values) * (i * 1000);
      sum += values;
    }
  }
  if (!online) {  // หลุดเส้น
    if (dottedline) return last_value;
    if (last_value < (NUM_SENSORS - 1) * 1000 / 2) return 0;
    return (NUM_SENSORS - 1) * 1000;
  }
  last_value = avg / sum;
  return last_value;
}

// ==================================================================================
//  หาตำแหน่งเส้น ใช้เฉพาะเซนเซอร์ช่อง start ถึง end
// ==================================================================================
int readPosition(int start, int end, int Track, int noise) {
  unsigned char i, online = 0;
  unsigned long avg = 0;
  unsigned int sum = 0;
  static int last_value = 0;

  ReadCalibrate();
  for (i = start; i <= end; i++) {
    int values = F[i];
    if (values > Track) online = 1;
    if (values > noise) {
      avg += (long)(values) * (i * 1000);
      sum += values;
    }
  }
  if (!online) {  // หลุดเส้น
    if (dottedline) return last_value;
    if (last_value < (NUM_SENSORS - 1) * 1000 / 2) return 0;
    return (NUM_SENSORS - 1) * 1000;
  }
  last_value = avg / sum;
  return last_value;
}

// ==================================================================================
//  PID 1 รอบ (เซนเซอร์ 16 ช่อง)  สูตร: Kp * (Error / 1000) + (Kd / 100) * (Error - LastError)
// ==================================================================================
void PID(int SpeedL, int SpeedR, float Kp, float Kd) {
  int Pos = readPosition(250, 50);
  int Error = Pos - 7500;
  int PID_Value = (Kp * (Error / 1000)) + ((Kd / 100) * (Error - LastError));
  LastError = Error;
  int LeftPower = SpeedL + PID_Value;
  int RightPower = SpeedR - PID_Value;
  LimitPower(LeftPower, RightPower, SpeedL, SpeedR);
  Motor(LeftPower, RightPower);
}

// ==================================================================================
//  PID 1 รอบ (เซนเซอร์ช่อง start ถึง end)  สูตร: (Kp / 1000) * Error + (Kd / 100) * (Error - LastError)
// ==================================================================================
void PID(int start, int end, int SpeedL, int SpeedR, float Kp, float Kd) {
  int Pos = readPosition(start, end, 250, 50);
  int Error = Pos - ((NUM_SENSORS - 1) * 1000 / 2);
  int PID_Value = ((Kp / 1000) * Error) + ((Kd / 100) * (Error - LastError));
  LastError = Error;
  int LeftPower = SpeedL + PID_Value;
  int RightPower = SpeedR - PID_Value;
  LimitPower(LeftPower, RightPower, SpeedL, SpeedR);
  Motor(LeftPower, RightPower);
}

// ==================================================================================
//  แสดงตำแหน่งเส้นและ Error ทาง Serial Monitor (วนไม่รู้จบ)
// ==================================================================================
void SerialPosition() {
  while (1) {
    int Pos = readPosition(250, 50);
    int Error_F = Pos - 7500;
    Serial.print("Pos = ");
    Serial.print(Pos);
    Serial.print("                  Error = ");
    Serial.println(Error_F);
    delay(50);
  }
}

// ==================================================================================
//  คำสั่งเมื่อเจอแยก
//    's' = หยุด,  'p' = วิ่งตรงผ่านแยก,  'l' = เลี้ยวซ้าย,  'r' = เลี้ยวขวา
// ==================================================================================
void TrackSelect(int spd, char x) {
  if (x == 's') {
    MotorStop();
  } else if (x == 'p') {
    BZon();
    Motor(spd, spd);
    delay(5);
    while (1) {
      ReadCalibrate();
      if (F[4] < REF && F[11] < REF) {
        BZoff();
        break;
      }
      Motor(spd, spd);
    }
  } else if (x == 'l') {
    while (1) {
      BZon();
      Motor(spd / 2, spd / 2);
      ReadCalibrate();
      if (F[4] < REF && F[11] < REF) {
        BZoff();
        break;
      }
    }
    TurnLeft();
  } else if (x == 'r') {
    BZon();
    while (1) {
      Motor(spd / 2, spd / 2);
      ReadCalibrate();
      if (F[4] < REF && F[11] < REF) {
        BZoff();
        break;
      }
    }
    TurnRight();
  }
}

// ==================================================================================
//  เดินตามเส้นจนเจอแยก แล้วทำคำสั่ง select
// ==================================================================================
void TrackCrossL(int Speed, float Kp, float Kd, char select) {  // ใช้เซนเซอร์คู่ซ้าย (ช่อง 4, 7)
  BaseSpeed = Speed;
  InitialSpeed();
  while (1) {
    ReadCalibrate();
    if (F[4] >= REF && F[7] >= REF) break;
    PID(LeftBaseSpeed, RightBaseSpeed, Kp, Kd);
  }
  TrackSelect(Speed, select);
}

void TrackCrossR(int Speed, float Kp, float Kd, char select) {  // ใช้เซนเซอร์คู่ขวา (ช่อง 8, 11)
  BaseSpeed = Speed;
  InitialSpeed();
  while (1) {
    ReadCalibrate();
    if (F[8] >= REF && F[11] >= REF) break;
    PID(LeftBaseSpeed, RightBaseSpeed, Kp, Kd);
  }
  TrackSelect(Speed, select);
}

void TrackCrossC(int Speed, float Kp, float Kd, char select) {  // ใช้เซนเซอร์คู่กลาง (ช่อง 5, 10)
  BaseSpeed = Speed;
  InitialSpeed();
  while (1) {
    ReadCalibrate();
    if (F[5] >= REF && F[10] >= REF) break;
    PID(LeftBaseSpeed, RightBaseSpeed, Kp, Kd);
  }
  TrackSelect(Speed, select);
}

void TrackCross(int Speed, float Kp, float Kd, char select) {  // ต้องเจอทั้ง 3 คู่ (ช่อง 4, 5, 7, 8, 10, 11)
  BaseSpeed = Speed;
  InitialSpeed();
  while (1) {
    ReadCalibrate();
    if ((F[4] >= REF && F[7] >= REF) && (F[5] >= REF && F[10] >= REF) && (F[8] >= REF && F[11] >= REF)) break;
    PID(LeftBaseSpeed, RightBaseSpeed, Kp, Kd);
  }
  TrackSelect(Speed, select);
}

// ==================================================================================
//  เดินตามเส้นตามเวลา TotalTime (ms)
// ==================================================================================
void TrackTime(int Speed, float Kp, float Kd, int TotalTime) {  // ใช้เซนเซอร์ 16 ช่อง
  BaseSpeed = Speed;
  InitialSpeed();
  unsigned long StartTime = millis();
  unsigned long EndTime = StartTime + TotalTime;
  while (millis() <= EndTime) {
    PID(LeftBaseSpeed, RightBaseSpeed, Kp, Kd);
  }
}

void TrackTimeForLineFast(int start, int end, int Speed, float Kp, float Kd, int TotalTime) {  // ใช้เซนเซอร์ช่อง start-end
  BaseSpeed = Speed;
  InitialSpeed();
  unsigned long StartTime = millis();
  unsigned long EndTime = StartTime + TotalTime;
  while (millis() <= EndTime) {
    PID(start, end, LeftBaseSpeed, RightBaseSpeed, Kp, Kd);
  }
}

#endif
