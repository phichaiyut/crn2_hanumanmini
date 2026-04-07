void _setting() {
  SensorValue(66, 65, 61, 64, 65, 62, 62, 61, 61, 60, 63, 62, 65, 62, 62, 62, 849, 832, 795, 822, 828, 827, 826, 801, 810, 783, 833, 806, 829, 792, 788, 817);
  TurnSpeedLeft(4, 25, 80, 50);    // ค่าเลี้ยวซ้าย TurnSpeedLeft(ช่องเซนเซอร์, มอเตอร์ซ้าย, มอเตอร์ขวา, เวลาเลี้ยว);
  TurnSpeedRight(11, 80, 25, 50);  // ค่าเลี้ยวขวา TurnSpeedRight(ช่องเซนเซอร์, มอเตอร์ซ้าย, มอเตอร์ขวา, เวลาเลี้ยว);
  TrackLineColor(1);                // สีของเส้น 1 = ขาว , 0 = ดำ
  SetBUZZER_PIN(13);
  SetKpKd();
  
  SetBalanceSpeedForward();
  SetBalanceSpeedBackward();
  SetVolumeSound(2500);
  ModeSpdPID(1, 100, -25);
  Dottedline(0);      //ไม่มีเส้นประใส่ 0  >> มีเส้นประใส่ 1
  
  LoadCalibration();  //โหลดค่าจาก EEPROM

  // SerialSensor();           // Serial Monitor ค่าของ Sensor
  // SerialCalibrateSensor();  // Serial Monitor ค่าของ Sensor ที่ Calibrate แล้ว
  // CaliberateRobotSensor();  // Serial Monitor Calibrate ค่าแสงหุ่นยนต์
  // SerialPosition();
}