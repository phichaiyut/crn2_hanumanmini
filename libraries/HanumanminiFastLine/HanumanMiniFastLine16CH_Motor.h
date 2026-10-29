// **********************************************************************************
//  HanumanMiniFastLine16CH_Motor.h
//  คำสั่งมอเตอร์พื้นฐาน (เรียกใช้ motor() จากไลบรารี HanumanMini)
//  ความเร็ว -100 ถึง 100 (ค่าลบ = ถอยหลัง)
// **********************************************************************************

#ifndef HanumanMiniFastLine16CH_Motor_h
#define HanumanMiniFastLine16CH_Motor_h

#include <Arduino.h>

void Motor(int leftsp, int rightsp) {  // สั่งมอเตอร์ซ้าย/ขวา ค้างไว้
  motor(1, leftsp);
  motor(2, rightsp);
}

void Move(int l, int r, int deelay) {  // สั่งมอเตอร์ซ้าย/ขวา ตามเวลา (ms)
  Motor(l, r);
  delay(deelay);
}

void MotorStop() {  // หยุดมอเตอร์ แล้วหน่วง 5 ms
  Motor(0, 0);
  delay(5);
}

void MotorStop(int tim) {  // หยุดมอเตอร์ แล้วหน่วงตามเวลา (ms)
  Motor(0, 0);
  delay(tim);
}

#endif
