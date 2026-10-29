// **********************************************************************************
//  HanumanMiniFastLine16CH.h
//  ไลบรารีหุ่นยนต์เดินตามเส้น HanumanMini + เซนเซอร์ 16 ช่อง (CD74HC4067)
//
//  วิธีใช้ใน Sketch:
//    #include <HanumanMini.h>              // ต้อง include ก่อน (มีฟังก์ชัน motor())
//    #include <HanumanMiniFastLine16CH.h>
//
//  คู่มือการใช้งานฉบับเต็ม: README.md
// **********************************************************************************

#ifndef HanumanMiniFastLine16CH_h
#define HanumanMiniFastLine16CH_h

#include <Arduino.h>

#include "HanumanMiniFastLine16CH_Sensor.h"
#include "HanumanMiniFastLine16CH_Motor.h"
#include "HanumanMiniFastLine16CH_Initial.h"
#include "HanumanMiniFastLine16CH_ForwardBackwardAndTurnl.h"
#include "HanumanMiniFastLine16CH_PID.h"

// ==================================================================================
//  Mode() : เมนูเริ่มต้นตอนเปิดเครื่อง
//    - กดปุ่ม OK สั้น ๆ (น้อยกว่า 2 วินาที)  -> ออกจากเมนู ไปทำงานต่อ
//    - กดปุ่ม OK ค้าง 2 วินาทีขึ้นไป        -> Calibrate เซนเซอร์ แล้วบันทึกลง EEPROM
// ==================================================================================
void Mode() {
  unsigned long pressStart = 0;

  // รอจนกว่าจะกดปุ่ม OK
  while (OK_PUSH() != PRESS)
    ;

  pressStart = millis();  // เวลาเริ่มกด

  // รอจนกว่าจะปล่อยปุ่ม
  while (OK_PUSH() == PRESS)
    ;

  unsigned long pressDuration = millis() - pressStart;

  if (pressDuration >= 2000) {
    // กดค้างเกิน 2 วินาที
    b_beebb();
    delay(300);
    OK();
    Serial.println(">>> Calibrate Sensor Start");
    CalibrateSensor(15, 150);
    SaveCalibration();
    b_beebb();
    Serial.println(">>> Calibrate Sensor Done");
    OK();
  } else {
    // กดสั้นกว่า 2 วินาที
    Serial.println("Exit Mode (Short Press)");
    return;
  }
}

#endif
