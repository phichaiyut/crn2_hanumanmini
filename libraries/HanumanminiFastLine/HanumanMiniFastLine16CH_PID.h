
int MaxSpeed = 100;
int MinSpeed = 0;
int ModePidStatus = 0;
int LastError;
int dottedline = 0;

void Dottedline(int Dot){
  dottedline = Dot;
}
void ModeSpdPID(int moD, int maX, int miN){
  ModePidStatus = moD;
  MaxSpeed = maX;
  MinSpeed = miN;
}

int readPosition(int Track, int noise) {
  unsigned char i, online = 0;
  unsigned long avg = 0;
  unsigned int sum = 0;
  static int last_value = 0;
  ReadCalibrate();
  for (i = 0; i < NUM_SENSORS; i++) {
    int values = F[i];
    if (values > Track) {
      online = 1;
    }
    if (values > noise) {
      avg += (long)(values) * (i * 1000);
      sum += values;
    }
  }
  if (!online) {
    if (dottedline) {
      return last_value;
    }
    if (last_value < (NUM_SENSORS - 1) * 1000 / 2) {
      return 0;
    } else {
      return (NUM_SENSORS - 1) * 1000;
    }
  }
  last_value = avg / sum;
  return last_value;
}

void PID(int SpeedL,int SpeedR, float Kp, float Kd) {
  int Pos = readPosition(250, 50);
  int Error = Pos - 7500;
  int PID_Value = (Kp * (Error/1000)) + ((Kd/100) * (Error - LastError));
  LastError = Error;
  int LeftPower = SpeedL + PID_Value;
  int RightPower = SpeedR - PID_Value;
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
  Motor(LeftPower, RightPower);

}

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

void TrackCrossL(int Speed, float Kp, float Kd, char select) {
    BaseSpeed = Speed;
  InitialSpeed();
  while (1) {
    ReadCalibrate();
    if (F[4] >= REF && F[7] >= REF) {
      break;
    }
    PID(LeftBaseSpeed,RightBaseSpeed, Kp, Kd);
  }
  TrackSelect(Speed, select);
}

void TrackCrossR(int Speed, float Kp, float Kd, char select) {
    BaseSpeed = Speed;
  InitialSpeed();
  while (1) {
    ReadCalibrate();
    if (F[8] >= REF && F[11] >= REF) {
      break;
    }
    PID(LeftBaseSpeed,RightBaseSpeed, Kp, Kd);
  }
  TrackSelect(Speed, select);
}

void TrackCrossC(int Speed, float Kp, float Kd, char select) {
    BaseSpeed = Speed;
  InitialSpeed();
  while (1) {
    ReadCalibrate();
    if (F[5] >= REF && F[10] >= REF) {
      break;
    }
    PID(LeftBaseSpeed,RightBaseSpeed, Kp, Kd);
  }
  TrackSelect(Speed, select);
}

void TrackCross(int Speed, float Kp, float Kd, char select) {
    BaseSpeed = Speed;
  InitialSpeed();
  while (1) {
    ReadCalibrate();
    if ((F[4] >= REF && F[7] >= REF)&& (F[5] >= REF && F[10] >= REF) && (F[8] >= REF && F[11] >= REF)) {
      break;
    }
    PID(LeftBaseSpeed,RightBaseSpeed, Kp, Kd);
  }
  TrackSelect(Speed, select);
}

void TrackTime(int Speed, float Kp, float Kd, int TotalTime) {
    BaseSpeed = Speed;
  InitialSpeed();
  unsigned long StartTime = millis();
  unsigned long EndTime = StartTime + TotalTime;
  while (millis() <= EndTime) {
    PID(LeftBaseSpeed,RightBaseSpeed, Kp, Kd);
  }
}



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
  if (!online) {
    if (dottedline) {
      return last_value;
    }
    if (last_value < (NUM_SENSORS - 1) * 1000 / 2) {
      return 0;
    } else {
      return 15000;
    }
  }
  last_value = avg / sum;
  return last_value;
}

void PID(int start, int end, int SpeedL,int SpeedR, float Kp, float Kd) {
  
  int Pos = readPosition(start, end, 250, 50);
  int Error = Pos - ((NUM_SENSORS - 1) * 1000 / 2);
  int PID_Value = (Kp * Error) + (Kd * (Error - LastError));
  LastError = Error;
  int LeftPower = SpeedL + PID_Value;
  int RightPower = SpeedR - PID_Value;
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
  Motor(LeftPower, RightPower);
}

void TrackTimeForLineFast(int start, int end,int Speed,float Kp, float Kd,int TotalTime) {
  BaseSpeed = Speed;
  InitialSpeed();
  unsigned long StartTime = millis();
  unsigned long EndTime = StartTime + TotalTime;
  while (millis() <= EndTime) {
    PID(start,end,LeftBaseSpeed,RightBaseSpeed, Kp, Kd);
  }
}

// วิ่งตามเส้นพร้อมอ่าน Marker ด้วยเซนเซอร์ C[0] (ซ้าย) และ C[1] (ขวา)
//   C[1] อย่างเดียว  -> นับ Marker ขวา ครบ FinishCount ครั้ง -> วิ่งต่ออีก RunTime ms แล้วหยุด
//   C[0] อย่างเดียว  -> สลับความเร็ว ปกติ <-> ทางโค้ง
//   C[0] และ C[1]    -> เส้นตัด ไม่ทำอะไร
// จะทำงานตอนที่เซนเซอร์ออกจาก Marker แล้ว (C[0] < RefC && C[1] < RefC) โดยดูจากสถานะก่อนหน้า
#define MARK_NONE  0
#define MARK_LEFT  1
#define MARK_RIGHT 2
#define MARK_CROSS 3

// Ramp Speed : เวลา (ms) ต่อการเปลี่ยนความเร็ว 1 ระดับ , 0 = เปลี่ยนทันที
int RampStartTime = 5;  // ตอนออกตัว จาก 0 ถึงความเร็วปกติ เช่น 5 -> จาก 0 ถึง 60 ใช้เวลา 300 ms
int RampUpTime = 5;     // เร่งความเร็ว ตอนออกจากโค้งกลับเป็นความเร็วปกติ
int RampDownTime = 2;   // ลดความเร็ว (ก่อนเข้าโค้งควรลดให้ไว)

void RampSpeed(int startT, int up, int down) {
  RampStartTime = startT;
  RampUpTime = up;
  RampDownTime = down;
}

void RampSpeed(int up, int down) {
  RampSpeed(up, up, down);
}

// ปรับ CurrentSpeed เข้าหา TargetSpeed ทีละ 1 ตามเวลาที่ตั้งไว้
void UpdateRampSpeed(int &CurrentSpeed, int TargetSpeed, unsigned long &LastRamp, int UpTime) {
  if (CurrentSpeed == TargetSpeed) {
    LastRamp = millis();
    return;
  }
  int StepTime = CurrentSpeed < TargetSpeed ? UpTime : RampDownTime;
  if (StepTime <= 0) {
    CurrentSpeed = TargetSpeed;
  } else {
    unsigned long Steps = (millis() - LastRamp) / StepTime;
    if (Steps == 0) return;
    LastRamp += Steps * StepTime;
    int Diff = TargetSpeed - CurrentSpeed;
    if ((unsigned long)abs(Diff) <= Steps) CurrentSpeed = TargetSpeed;
    else CurrentSpeed += Diff > 0 ? (int)Steps : -(int)Steps;
  }
  BaseSpeed = CurrentSpeed;
  InitialSpeed();
}

void TrackMarker(int start, int end, int SpeedNormal, int SpeedCurve, float Kp, float Kd, int FinishCount, int RunTime) {
  int MarkState = MARK_NONE;
  int RightCount = 0;
  bool CurveMode = false;
  int CurrentSpeed = RampStartTime > 0 ? 0 : SpeedNormal;  // ออกตัวจาก 0 แล้วค่อยๆ เร่ง
  bool Starting = true;  // กำลังออกตัว ใช้ RampStartTime
  int TargetSpeed = SpeedNormal;
  unsigned long LastRamp = millis();

  BaseSpeed = CurrentSpeed;
  InitialSpeed();
  while (1) {
    UpdateRampSpeed(CurrentSpeed, TargetSpeed, LastRamp, Starting ? RampStartTime : RampUpTime);
    if (CurrentSpeed == TargetSpeed) Starting = false;
    ReadCalibrateC();
    bool Left = C[0] > RefC;
    bool Right = C[1] > RefC;

    if (Left && Right) {
      BZon();
      MarkState = MARK_CROSS;
    } else if (!Left && Right) {
      BZon();
      if (MarkState != MARK_CROSS) MarkState = MARK_RIGHT;
    } else if (Left && !Right) {
      BZon();
      if (MarkState != MARK_CROSS) MarkState = MARK_LEFT;
    } else {
      BZoff();
      if (MarkState == MARK_RIGHT) {
        RightCount++;
        if (RightCount >= FinishCount) break;
      } else if (MarkState == MARK_LEFT) {
        CurveMode = !CurveMode;
        TargetSpeed = CurveMode ? SpeedCurve : SpeedNormal;
        Starting = false;
      }
      MarkState = MARK_NONE;
    }
    PID(start, end, LeftBaseSpeed, RightBaseSpeed, Kp, Kd);
  }

  unsigned long EndTime = millis() + RunTime;
  while (millis() <= EndTime) {
    UpdateRampSpeed(CurrentSpeed, TargetSpeed, LastRamp, Starting ? RampStartTime : RampUpTime);
    PID(start, end, LeftBaseSpeed, RightBaseSpeed, Kp, Kd);
  }
  BZoff();
  MotorStop();
  TrackTimeForLineFast(start, end, 0, Kp, Kd, 300);  // จัดหุ่นให้ตรงเส้นกับที่ 300 ms
  MotorStop();
}
