#include "HanumanMiniFastLine16CH_Sensor.h"
#include "HanumanMiniFastLine16CH_Motor.h"
#include "HanumanMiniFastLine16CH_Initial.h"
#include "HanumanMiniFastLine16CH_ForwardBackwardAndTurnl.h"
#include "HanumanMiniFastLine16CH_PID.h"



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
    b_beebb();
    delay(300);
    OK();
    // กดค้างเกิน 2 วินาที
    Serial.println(">>> Calibrate Sensor Start");
    CalibrateSensor(15, 150);
SaveCalibration();
    b_beebb();
    Serial.println(">>> Calibrate Sensor Done");
    OK();
  } else {
    // กดสั้นกว่า 2 วิ
    Serial.println("Exit Mode (Short Press)");
    return;
  }
}