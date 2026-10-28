
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
  int PID_Value = ((Kp / 1000) * Error) + ((Kd / 100) * (Error - LastError));
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