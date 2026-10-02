
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

// ============================== Mark (เซนเซอร์ C) ==============================
// เซนเซอร์ C[0] = Mark ซ้าย , C[1] = Mark ขวา
// เงื่อนไขการตัดสิน Mark
//   1. เซนเซอร์ต้องเห็น Mark ต่อเนื่องอย่างน้อย MarkMinTime ms ถึงจะนับว่าเจอ (กันสัญญาณรบกวน / จุดสกปรก)
//   2. ตัดสินหลังจากพ้น Mark ทั้งสองข้างต่อเนื่อง MarkClearTime ms แล้ว (Mark เดิมนับได้ครั้งเดียว)
//      ถ้าในช่วงนั้นเห็นทั้งซ้ายและขวา (แม้จะไม่พร้อมกัน เช่น หุ่นเอียงตอนข้ามเส้นตัด) = เส้นตัด
int MarkMinTime = 2;     // ms
int MarkClearTime = 20;  // ms

void MarkFilter(int minTime, int clearTime) {
  MarkMinTime = minTime;
  MarkClearTime = clearTime;
}

// เส้นตัดจากเซนเซอร์หน้า : ถ้าเซนเซอร์หน้าเห็นเส้นพร้อมกันตั้งแต่ CrossSensorCount ช่องขึ้นไป = เส้นตัด
//   - ระหว่างอยู่บนเส้นตัด หุ่นวิ่งตรงไม่ใช้ PID (เส้นตัดทำให้ตำแหน่งเส้นเพี้ยน)
//   - Mark ที่เซนเซอร์ C เริ่มเจอภายใน CrossTime ms หลังจากนั้น ถือเป็นเส้นตัด
//     (เซนเซอร์หน้าอยู่หน้าเซนเซอร์ C จึงเจอเส้นตัดก่อน)
int CrossSensorCount = 8;  // 0 = ปิด
int CrossTime = 50;        // ms

void CrossFilter(int sensorCount, int time) {
  CrossSensorCount = sensorCount;
  CrossTime = time;
}

// หลุดเส้น (เซนเซอร์หน้าไม่เห็นเส้นเลย) นานเกิน LineLostTime ms -> หยุดหุ่นทันที กันหุ่นวิ่งออกนอกสนาม
int LineLostTime = 0;  // ms , 0 = ปิด

void LineLostStop(int time) {
  LineLostTime = time;
}

// เวลา (ms) ต่อการเปลี่ยนความเร็ว 1 ระดับ ใช้กับ TrackMarker / TrackMap , 0 = เปลี่ยนทันที
int RampStartTime = 5;  // ออกตัว
int RampUpTime = 5;     // เร่ง
int RampDownTime = 2;   // ลด

void RampSpeed(int start, int up, int down) {
  RampStartTime = start;
  RampUpTime = up;
  RampDownTime = down;
}

void RampSpeed(int up, int down) {
  RampSpeed(up, up, down);
}

// จับเวลาที่เซนเซอร์อยู่บน Mark ต่อเนื่อง , คืนค่า true เมื่อนานพอ
bool MarkDebounce(bool OnMark, unsigned long &OnTime, unsigned long Now) {
  if (!OnMark) {
    OnTime = 0;
    return false;
  }
  if (OnTime == 0) OnTime = Now | 1;  // 0 ใช้แทนสถานะ "ไม่อยู่บน Mark"
  return Now - OnTime >= (unsigned long)MarkMinTime;
}

// นับจำนวนเซนเซอร์หน้าที่เห็นเส้น (ใช้ค่า F[] จาก ReadCalibrate ครั้งล่าสุด)
int CountOnLine(int start, int end) {
  int OnLine = 0;
  for (int i = start; i <= end; i++) {
    if (F[i] >= REF) OnLine++;
  }
  return OnLine;
}

// ---- DetectGeo : อ่าน Mark แบบไม่ค้าง เรียกซ้ำใน loop ได้เลย ----
//   ระหว่างอยู่บน Mark จะจำว่าเคยเห็นข้างไหนบ้าง (ซ้ายเห็นก่อน ขวาเห็นตาม = 3)
//   คืนค่าครั้งเดียวต่อ 1 Mark หลังพ้น Mark ทั้งสองข้างต่อเนื่อง MarkClearTime ms
//   GEO_NONE = ยังไม่มี Mark ใหม่ , GEO_LEFT = Mark ซ้าย , GEO_RIGHT = Mark ขวา , GEO_CROSS = เส้นตัด
#define GEO_NONE 0
#define GEO_LEFT 1
#define GEO_RIGHT 2
#define GEO_CROSS 3

int GeoMax = 0;                  // ข้างที่เคยเห็นใน Mark นี้ (bit 1 = ซ้าย , bit 2 = ขวา) , 0 = ไม่อยู่บน Mark
bool GeoIgnore = false;          // Mark นี้เริ่มตอนเซนเซอร์หน้าเพิ่งเจอเส้นตัด -> ถือเป็นเส้นตัด
unsigned long GeoOnTimeL = 0, GeoOnTimeR = 0, GeoLastOnMark = 0;
unsigned long GeoCrossAt = 0;    // เวลาล่าสุดที่เซนเซอร์หน้าเห็นเส้นตัด , 0 = ไม่เคย

void ResetGeo() {
  GeoMax = 0;
  GeoIgnore = false;
  GeoOnTimeL = 0;
  GeoOnTimeR = 0;
  GeoCrossAt = 0;
}

int DetectGeo() {
  unsigned long Now = millis();
  ReadCalibrateC();
  bool RawL = C[0] >= RefC;
  bool RawR = C[1] >= RefC;
  int Geo = 0;
  if (MarkDebounce(RawL, GeoOnTimeL, Now)) Geo |= GEO_LEFT;
  if (MarkDebounce(RawR, GeoOnTimeR, Now)) Geo |= GEO_RIGHT;
  if (Geo != 0 && GeoMax == 0 && GeoCrossAt != 0 && Now - GeoCrossAt <= (unsigned long)CrossTime) {
    GeoIgnore = true;
  }
  GeoMax |= Geo;
  if (RawL || RawR) GeoLastOnMark = Now;

  if (GeoMax != 0 && !RawL && !RawR && Now - GeoLastOnMark >= (unsigned long)MarkClearTime) {
    Geo = GeoIgnore ? GEO_CROSS : GeoMax;
    GeoMax = 0;
    GeoIgnore = false;
    return Geo;
  }
  return GEO_NONE;
}

// แบบใช้เซนเซอร์หน้าช่วยกรองเส้นตัด (เรียกหลัง PID / ReadCalibrate เพื่อให้ F[] เป็นค่าล่าสุด)
int DetectGeo(int start, int end) {
  if (CrossSensorCount > 0 && CountOnLine(start, end) >= CrossSensorCount) GeoCrossAt = millis() | 1;
  return DetectGeo();
}

// ============================== Track Mapping ==============================
// รอบสำรวจ (TrackMapExplore) : วิ่งความเร็วคงที่ จดเวลาทางตรงแต่ละช่วง (ระหว่าง Mark ซ้าย) แล้วบันทึกลง EEPROM
// รอบทำเวลา (TrackMapRun)    : ทางตรงวิ่ง Speed แล้วลดเป็น CurveSpeed ก่อนถึงโค้ง BrakeTime ms
//   เวลาที่คาดว่าจะถึงโค้ง = เวลาที่จดไว้ x ความเร็วรอบสำรวจ / Speed
// ช่วงทางตรง : เริ่มนับที่ Mark ขวาแรก (เส้นเริ่ม) และหลัง Mark ออกโค้ง , จบที่ Mark เข้าโค้ง หรือเส้นชัย
#define MAP_MAX 50
#define MAP_ADDR (((NUM_SENSORS * 2) + 4) * sizeof(int))  // ต่อจากค่า Calibrate ใน EEPROM

#define MAP_OFF 0
#define MAP_EXPLORE 1
#define MAP_RUN 2

unsigned long MapTime[MAP_MAX];  // เวลาทางตรงแต่ละช่วง (ms)
int MapCount = 0;                // จำนวนช่วงทางตรงที่จดไว้
int MapSpeed = 0;                // ความเร็วตอนสำรวจ

void SaveMap() {
  int addr = MAP_ADDR;
  EEPROM.put(addr, MapCount);
  addr += sizeof(int);
  EEPROM.put(addr, MapSpeed);
  addr += sizeof(int);
  for (int i = 0; i < MapCount; i++) {
    EEPROM.put(addr, MapTime[i]);
    addr += sizeof(unsigned long);
  }
}

void SerialMap() {
  Serial.print("Map Speed = ");
  Serial.print(MapSpeed);
  Serial.print("  Count = ");
  Serial.println(MapCount);
  for (int i = 0; i < MapCount; i++) {
    Serial.print("  [");
    Serial.print(i);
    Serial.print("] ");
    Serial.print(MapTime[i]);
    Serial.println(" ms");
  }
}

void LoadMap() {
  int addr = MAP_ADDR;
  EEPROM.get(addr, MapCount);
  addr += sizeof(int);
  EEPROM.get(addr, MapSpeed);
  addr += sizeof(int);
  if (MapCount < 0 || MapCount > MAP_MAX || MapSpeed <= 0 || MapSpeed > 100) {
    MapCount = 0;  // ยังไม่เคยสำรวจ / ข้อมูลเสีย
    MapSpeed = 0;
    Serial.println("No Map in EEPROM");
    return;
  }
  for (int i = 0; i < MapCount; i++) {
    EEPROM.get(addr, MapTime[i]);
    addr += sizeof(unsigned long);
  }
  SerialMap();
}

// ทางตรงช่วงนี้ ถึงเวลาลดความเร็วก่อนเข้าโค้งหรือยัง (ไม่มีข้อมูลช่วงนี้ = ลดไว้ก่อน)
bool MapBrake(int Index, unsigned long Elapsed, int Speed, int BrakeTime) {
  if (Index >= MapCount || MapSpeed <= 0 || Speed <= 0) return true;
  long Expected = (long)MapTime[Index] * MapSpeed / Speed;
  return (long)Elapsed >= Expected - BrakeTime;
}

// วิ่งตามเส้นพร้อมอ่าน Mark (ใช้ร่วมกันทั้ง TrackMarker และ TrackMap)
//   Mark ขวา        -> นับ ครบ FinishCount ครั้ง -> วิ่งต่ออีก RunTime ms แล้วหยุด
//   Mark ซ้าย       -> สลับ ทางตรง <-> ทางโค้ง
//   เส้นตัด         -> ไม่ทำอะไร (คงสถานะเดิม)
void TrackMarkerCore(int start, int end, int Speed, int CurveSpeed, float Kp, float Kd, int FinishCount, int RunTime, int MapMode, int BrakeTime) {
  int CurSpeed = 0;           // ความเร็วปัจจุบัน (ค่อยๆ เปลี่ยนตาม RampSpeed)
  bool Starting = true;       // ช่วงออกตัว
  bool Curve = false;         // false = ทางตรง , true = ทางโค้ง
  int RightCount = 0;         // จำนวน Mark ขวาที่นับได้
  bool Finish = false;
  unsigned long FinishTime = 0;

  bool InCross = false;       // เซนเซอร์หน้ากำลังอยู่บนเส้นตัด
  unsigned long LostAt = 0;   // เวลาที่เริ่มหลุดเส้น , 0 = ไม่หลุด

  int MapIndex = 0;                  // ช่วงทางตรงปัจจุบัน
  unsigned long SegStart = millis(); // เวลาเริ่มทางตรงช่วงนี้

  unsigned long LastRamp = millis();
  LastError = 0;
  BaseSpeed = 0;
  InitialSpeed();
  ResetGeo();

  while (1) {
    unsigned long Now = millis();

    // ---- ความเร็วเป้าหมาย ----
    int Target = Speed;
    if (Curve) {
      Target = CurveSpeed;
    } else if (MapMode == MAP_RUN && MapBrake(MapIndex, Now - SegStart, Speed, BrakeTime)) {
      Target = CurveSpeed;
    }

    // ---- ค่อยๆ ปรับความเร็วเข้าหาเป้าหมาย ----
    if (CurSpeed != Target) {
      int StepTime = (CurSpeed < Target) ? (Starting ? RampStartTime : RampUpTime) : RampDownTime;
      int Step = 0;
      if (StepTime <= 0) {
        Step = abs(Target - CurSpeed);
      } else {
        Step = (Now - LastRamp) / StepTime;
        LastRamp += (unsigned long)Step * StepTime;
      }
      if (Step > abs(Target - CurSpeed)) Step = abs(Target - CurSpeed);
      if (Step > 0) {
        CurSpeed += (CurSpeed < Target) ? Step : -Step;
        BaseSpeed = CurSpeed;
        InitialSpeed();
      }
    } else {
      LastRamp = Now;
      Starting = false;
    }

    // ---- วิ่งตามเส้น (บนเส้นตัดวิ่งตรง ไม่ใช้ PID) ----
    if (InCross) {
      ReadCalibrate();
      Motor(LeftBaseSpeed, RightBaseSpeed);
    } else {
      PID(start, end, LeftBaseSpeed, RightBaseSpeed, Kp, Kd);
    }

    // ---- ตรวจเส้นตัด / หลุดเส้น จากเซนเซอร์หน้า ----
    int OnLine = CountOnLine(start, end);
    InCross = (CrossSensorCount > 0 && OnLine >= CrossSensorCount);
    if (InCross) GeoCrossAt = Now | 1;

    if (OnLine == 0) {
      if (LostAt == 0) LostAt = Now | 1;
      if (LineLostTime > 0 && Now - LostAt >= (unsigned long)LineLostTime) {
        MotorStop();
        BZoff();
        return;
      }
    } else {
      LostAt = 0;
    }

    // ---- วิ่งต่ออีก RunTime ms หลังเจอ Mark ขวาครบ แล้วหยุด ----
    if (Finish) {
      if (Now - FinishTime >= (unsigned long)RunTime) {
        MotorStop();
        BZoff();
        if (MapMode == MAP_EXPLORE) {
          SaveMap();  // บันทึกหลังหยุดแล้ว (เขียน EEPROM ช้า)
          SerialMap();
        }
        return;
      }
      continue;
    }

    // ---- อ่าน Mark ด้วยเซนเซอร์ C ----
    int Geo = DetectGeo();
    if (GeoMax != 0) BZon();
    if (Geo == GEO_NONE) continue;
    BZoff();

    if (Geo == GEO_RIGHT) {
      // Mark ขวา : ครั้งแรก = เริ่ม , ครบ FinishCount = จบ
      RightCount++;
      if (RightCount >= FinishCount) {
        Finish = true;
        FinishTime = Now;
        if (MapMode == MAP_EXPLORE) {
          // ทางตรงช่วงสุดท้ายก่อนเส้นชัย (ถ้าจบในโค้ง ช่วงนี้จดไว้ตอนเข้าโค้งแล้ว)
          if (!Curve && MapIndex < MAP_MAX) MapTime[MapIndex] = Now - SegStart;
          MapCount = min(MapIndex + 1, MAP_MAX);
          MapSpeed = Speed;
        }
      } else if (RightCount == 1 && MapMode != MAP_OFF) {
        // เส้นเริ่ม : เริ่มนับทางตรงช่วงแรกใหม่
        Curve = false;
        MapIndex = 0;
        SegStart = Now;
      }
    } else if (Geo == GEO_LEFT) {
      // Mark ซ้าย : สลับ ทางตรง <-> ทางโค้ง
      Curve = !Curve;
      if (Curve) {
        // เข้าโค้ง : จดเวลาทางตรงช่วงนี้
        if (MapMode == MAP_EXPLORE && MapIndex < MAP_MAX) MapTime[MapIndex] = Now - SegStart;
      } else {
        // ออกโค้ง : เริ่มทางตรงช่วงถัดไป
        MapIndex++;
        SegStart = Now;
      }
    }
    // GEO_CROSS : เส้นตัด -> คงสถานะเดิม
  }
}

void TrackMarker(int start, int end, int Speed, int CurveSpeed, float Kp, float Kd, int FinishCount, int RunTime) {
  TrackMarkerCore(start, end, Speed, CurveSpeed, Kp, Kd, FinishCount, RunTime, MAP_OFF, 0);
}

// รอบสำรวจ : ความเร็วคงที่ทั้งสนาม (เวลาที่จดจะได้ไม่เพี้ยนจากการเร่ง/ลด) แล้วบันทึกลง EEPROM
void TrackMapExplore(int start, int end, int Speed, float Kp, float Kd, int FinishCount, int RunTime) {
  TrackMarkerCore(start, end, Speed, Speed, Kp, Kd, FinishCount, RunTime, MAP_EXPLORE, 0);
}

// รอบทำเวลา : ทางตรงวิ่ง Speed , ก่อนถึงโค้ง BrakeTime ms และในโค้งวิ่ง CurveSpeed
void TrackMapRun(int start, int end, int Speed, int CurveSpeed, int BrakeTime, float Kp, float Kd, int FinishCount, int RunTime) {
  if (MapCount == 0) LoadMap();
  TrackMarkerCore(start, end, Speed, CurveSpeed, Kp, Kd, FinishCount, RunTime, MAP_RUN, BrakeTime);
}

