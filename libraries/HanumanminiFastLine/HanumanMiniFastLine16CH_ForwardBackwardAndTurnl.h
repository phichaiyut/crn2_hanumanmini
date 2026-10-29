// **********************************************************************************
//  HanumanMiniFastLine16CH_ForwardBackwardAndTurnl.h
//  คำสั่งเดินหน้า ถอยหลัง และเลี้ยว
// **********************************************************************************

#ifndef HanumanMiniFastLine16CH_ForwardBackwardAndTurnl_h
#define HanumanMiniFastLine16CH_ForwardBackwardAndTurnl_h

#include <Arduino.h>
#include "HanumanMiniFastLine16CH_Sensor.h"
#include "HanumanMiniFastLine16CH_Motor.h"
#include "HanumanMiniFastLine16CH_Initial.h"

// ==================================================================================
//  ตั้งค่าการเลี้ยวที่แยก (ใช้กับ TurnLeft / TurnRight / TrackCross...)
//    Ch = ช่องเซนเซอร์ที่ใช้หยุดเลี้ยว, l/r = ความเร็วมอเตอร์ซ้าย/ขวา, de = เวลาเลี้ยวก่อนเริ่มหาเส้น (ms)
// ==================================================================================
void TurnSpeedLeft(int Ch, int l, int r, int de) {
  LCh = Ch;
  LTurnSpdL = l;
  LTurnSpdR = r;
  TurnDelayL = de;
}

void TurnSpeedRight(int Ch, int l, int r, int de) {
  RCh = Ch;
  RTurnSpdL = l;
  RTurnSpdR = r;
  TurnDelayR = de;
}

// ==================================================================================
//  เดินหน้า / ถอยหลัง (ใช้ค่าชดเชยจาก setBalanceSpeed / setBalanceBackSpeed)
// ==================================================================================
void FD(int Speed, int Time) {
  BaseSpeed = Speed;
  InitialSpeed();
  Motor(LeftBaseSpeed, RightBaseSpeed);
  delay(Time);
}

void BK(int Speed, int Time) {
  BaseSpeed = Speed;
  InitialSpeed();
  Motor(-BackLeftBaseSpeed, -BackRightBaseSpeed);
  delay(Time);
}

// ==================================================================================
//  เลี้ยวตามเวลา
//    TL / TR = เลี้ยวล้อเดียว,  SL / SR = หมุนตัวอยู่กับที่
// ==================================================================================
void TL(int Speed, int Time) {
  MotorStop(0);
  Motor(0, Speed);
  delay(Time);
  MotorStop(0);
}

void TR(int Speed, int Time) {
  MotorStop(0);
  Motor(Speed, 0);
  delay(Time);
  MotorStop(0);
}

void SL(int Speed, int Time) {
  MotorStop(0);
  Motor(-Speed, Speed);
  delay(Time);
  MotorStop(0);
}

void SR(int Speed, int Time) {
  MotorStop(0);
  Motor(Speed, -Speed);
  delay(Time);
  MotorStop(0);
}

// ==================================================================================
//  เลี้ยวจนเจอเส้น (ใช้ค่าจาก TurnSpeedLeft / TurnSpeedRight และ REF)
// ==================================================================================
void TurnLeft() {
  Motor(-LTurnSpdL, LTurnSpdR);
  delay(TurnDelayL);
  while (1) {
    ReadCalibrate();
    if (F[LCh] >= REF) {
      Motor(LTurnSpdL, -LTurnSpdR);
      MotorStop();
      break;
    }
    Motor(-LTurnSpdL, LTurnSpdR);
  }
}

void TurnRight() {
  Motor(RTurnSpdL, -RTurnSpdR);
  delay(TurnDelayR);
  while (1) {
    ReadCalibrate();
    if (F[RCh] >= REF) {
      Motor(-RTurnSpdL, RTurnSpdR);
      MotorStop();
      break;
    }
    Motor(RTurnSpdL, -RTurnSpdR);
  }
}

// หมุนซ้าย/ขวาจนเซนเซอร์ช่อง Ch เจอเส้น (ค่า >= 500)
void Left(int Ch, int Speed) {
  Motor(-Speed, Speed);
  delay(TurnDelayL);
  while (1) {
    Motor(-Speed, Speed);
    ReadCalibrate();
    if (F[Ch] >= 500) {
      Motor(Speed, -Speed);
      delay(20);
      MotorStop();
      break;
    }
  }
}

void Right(int Ch, int Speed) {
  Motor(Speed, -Speed);
  delay(TurnDelayR);
  while (1) {
    Motor(Speed, -Speed);
    ReadCalibrate();
    if (F[Ch] >= 500) {
      Motor(-Speed, Speed);
      delay(20);
      MotorStop();
      break;
    }
  }
}

// ==================================================================================
//  ทดสอบมอเตอร์: ซ้ายหมุน 0.5 วินาที แล้วขวาหมุน 0.5 วินาที
// ==================================================================================
void TestMotor() {
  Motor(100, 0);  // สั่งมอเตอร์ซ้าย
  delay(500);     // หน่วงเวลา 0.5 วินาที
  MotorStop();    // หยุดการทำงานมอเตอร์
  Motor(0, 100);  // สั่งมอเตอร์ขวา
  delay(500);     // หน่วงเวลา 0.5 วินาที
  MotorStop();    // หยุดการทำงานมอเตอร์
}

#endif
