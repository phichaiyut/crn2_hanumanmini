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

void TestMotor() {
  Motor(100, 0);  //สั่งมอเตอร์ซ้าย
  delay(500);         // หน่วงเวลา 0.5 วินาที
  MotorStop();        // หยุดการทำงานมอเตอร์
  Motor(0, 100);  //สั่งมอเตอร์ขวา
  delay(500);         // หน่วงเวลา 0.5 วินาที
  MotorStop();        // หยุดการทำงานมอเตอร์
}
