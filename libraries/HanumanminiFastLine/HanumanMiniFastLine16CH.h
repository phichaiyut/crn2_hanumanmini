#include "HanumanMiniFastLine16CH_Sensor.h"
#include "HanumanMiniFastLine16CH_Motor.h"
#include "HanumanMiniFastLine16CH_Initial.h"
#include "HanumanMiniFastLine16CH_ForwardBackwardAndTurnl.h"
#include "HanumanMiniFastLine16CH_PID.h"

#define PRESS_TIMEOUT 1000  // ถ้าไม่กดปุ่มเพิ่มภายในเวลานี้ (ms) ถือว่ากดเสร็จแล้ว
#define PRESS_MAX 10


// นับจำนวนครั้งที่กดปุ่ม OK ติดกัน (เรียกหลังจากกดครั้งแรกและปล่อยปุ่มแล้ว)
int CountPress() {
  int count = 1;
  Beep(50);
  unsigned long lastPress = millis();
  while (millis() - lastPress < PRESS_TIMEOUT) {
    if (OK_PUSH() == PRESS) {
      delay(30);
      while (OK_PUSH() == PRESS)
        ;
      delay(30);
      if (count < PRESS_MAX) count++;
      Beep(50);
      lastPress = millis();
    }
  }
  Serial.print("Press Count = ");
  Serial.println(count);
  return count;
}

// รอกดปุ่ม OK ครั้งแรกแล้วนับจำนวนครั้งที่กดติดกัน
int WaitPress() {
  while (OK_PUSH() != PRESS)
    ;
  delay(30);
  while (OK_PUSH() == PRESS)
    ;
  delay(30);
  return CountPress();
}

// กดค้างเกิน 2 วินาที = Calibrate , กดสั้น = เลือกโหมด
// คืนค่าจำนวนครั้งที่กดปุ่มติดกัน (1 - 10)
int Mode() {
  unsigned long pressStart = 0;

  // รอจนกว่าจะกดปุ่ม OK
  while (OK_PUSH() != PRESS)
    ;

  pressStart = millis();  // เวลาเริ่มกด

  // รอจนกว่าจะปล่อยปุ่ม
  while (OK_PUSH() == PRESS)
    ;
  delay(30);

  unsigned long pressDuration = millis() - pressStart;

  if (pressDuration >= 2000) {
    b_beebb();
    delay(300);
    OK();
    // กดค้างเกิน 2 วินาที
    Serial.println(">>> Calibrate Sensor Start");
    CalibrateSensor(15, 150);
    Beep(300);
    OK();
    Serial.println(">>> Calibrate Sensor C Start");
    CalibrateSensorC(15, 150);
SaveCalibration();
    b_beebb();
    Serial.println(">>> Calibrate Sensor Done");
    return WaitPress();
  } else {
    // กดสั้นกว่า 2 วิ
    Serial.println("Exit Mode (Short Press)");
    return CountPress();
  }
}
