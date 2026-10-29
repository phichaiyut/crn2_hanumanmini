void _setting() {
  // ค่า Min 16 ค่า ตามด้วยค่า Max 16 ค่า (ได้จาก CaliberateRobotSensor())
  // หมายเหตุ: LoadCalibration() ด้านล่างจะเขียนทับด้วยค่าที่บันทึกไว้ใน EEPROM
  SensorValue(66, 65, 61, 64, 65, 62, 62, 61, 61, 60, 63, 62, 65, 62, 62, 62,
              849, 832, 795, 822, 828, 827, 826, 801, 810, 783, 833, 806, 829, 792, 788, 817);

  TurnSpeedLeft(4, 25, 80, 50);    // ค่าเลี้ยวซ้าย TurnSpeedLeft(ช่องเซนเซอร์, มอเตอร์ซ้าย, มอเตอร์ขวา, เวลาเลี้ยว);
  TurnSpeedRight(11, 80, 25, 50);  // ค่าเลี้ยวขวา TurnSpeedRight(ช่องเซนเซอร์, มอเตอร์ซ้าย, มอเตอร์ขวา, เวลาเลี้ยว);
  TrackLineColor(1);               // สีของเส้น 1 = ขาว, 0 = ดำ
  SetBUZZER_PIN(13);               // ขา Buzzer
  SetVolumeSound(2500);            // ความถี่เสียง Beep (Hz)

  SetKpKd();                  // ตาราง KP/KD (setup_speed.ino)
  SetBalanceSpeedForward();   // ค่าชดเชยเดินหน้า (setup_speed.ino)
  SetBalanceSpeedBackward();  // ค่าชดเชยถอยหลัง (setup_speed.ino)

  ModeSpdPID(1, 100, -25);  // (โหมด PID, ความเร็วสูงสุด, ความเร็วต่ำสุด)
  Dottedline(0);            // ไม่มีเส้นประใส่ 0 >> มีเส้นประใส่ 1

  LoadCalibration();  // โหลดค่า Calibrate จาก EEPROM

  // ------------------------- คำสั่งทดสอบ (เปิดใช้ทีละอัน) -------------------------
  // SerialSensor();           // Serial Monitor ค่าดิบของ Sensor
  // SerialCalibrateSensor();  // Serial Monitor ค่าของ Sensor ที่ Calibrate แล้ว
  // CaliberateRobotSensor();  // Calibrate ค่าแสง แล้วพิมพ์คำสั่ง SensorValue(...)
  // SerialPosition();         // Serial Monitor ตำแหน่งเส้นและ Error
}
