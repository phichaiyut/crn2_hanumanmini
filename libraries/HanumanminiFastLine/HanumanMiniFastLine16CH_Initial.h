// **********************************************************************************
//  HanumanMiniFastLine16CH_Initial.h
//  ตารางชดเชยความเร็ว (Balance) และค่า KP/KD แยกตามช่วงความเร็ว 10, 20, ..., 100
// **********************************************************************************

#ifndef HanumanMiniFastLine16CH_Initial_h
#define HanumanMiniFastLine16CH_Initial_h

#include <Arduino.h>

int BaseSpeed, LeftBaseSpeed, RightBaseSpeed, BackLeftBaseSpeed, BackRightBaseSpeed;
float PID_KP, PID_KD;

int L[10], R[10];     // ค่าชดเชยเดินหน้า (ซ้าย, ขวา)
int BL[10], BR[10];   // ค่าชดเชยถอยหลัง (ซ้าย, ขวา)
float KP[10], KD[10]; // ค่า KP, KD ของแต่ละช่วงความเร็ว

// index ของช่วงความเร็ว
#define SPD_10 0
#define SPD_20 1
#define SPD_30 2
#define SPD_40 3
#define SPD_50 4
#define SPD_60 5
#define SPD_70 6
#define SPD_80 7
#define SPD_90 8
#define SPD_100 9

// ตั้งค่าชดเชยเดินหน้า: ความเร็วจริง = BaseSpeed - spdL / BaseSpeed - spdR
void setBalanceSpeed(int ch, int spdL, int spdR) {
  L[ch] = spdL;
  R[ch] = spdR;
}

// ตั้งค่าชดเชยถอยหลัง
void setBalanceBackSpeed(int ch, int spdL, int spdR) {
  BL[ch] = spdL;
  BR[ch] = spdR;
}

// ตั้งค่า KP, KD ของช่วงความเร็ว
void Set_KP_KD(int ch, float kp, float kd) {
  KP[ch] = kp;
  KD[ch] = kd;
}

// คำนวณความเร็วซ้าย/ขวา และ KP/KD จาก BaseSpeed
//   BaseSpeed <= 10 -> SPD_10, 11-20 -> SPD_20, ..., มากกว่า 90 -> SPD_100
void InitialSpeed() {
  int idx;
  if (BaseSpeed <= 10) idx = SPD_10;
  else if (BaseSpeed > 90) idx = SPD_100;
  else idx = (BaseSpeed - 1) / 10;

  LeftBaseSpeed = BaseSpeed - L[idx];
  RightBaseSpeed = BaseSpeed - R[idx];
  BackLeftBaseSpeed = BaseSpeed - BL[idx];
  BackRightBaseSpeed = BaseSpeed - BR[idx];
  PID_KP = KP[idx];
  PID_KD = KD[idx];
}

#endif
