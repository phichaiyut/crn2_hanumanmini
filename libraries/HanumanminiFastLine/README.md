# HanumanMiniFastLine16CH — คู่มือการใช้งาน

ไลบรารีสำหรับหุ่นยนต์เดินตามเส้นบนบอร์ด **HanumanMini** ใช้เซนเซอร์ 16 ช่องอ่านค่าผ่าน Multiplexer **CD74HC4067**

---

## สารบัญ

1. [การติดตั้ง](#1-การติดตั้ง)
2. [โครงสร้างไฟล์](#2-โครงสร้างไฟล์)
3. [การต่อขา](#3-การต่อขา)
4. [เริ่มต้นใช้งาน](#4-เริ่มต้นใช้งาน)
5. [ขั้นตอนการ Calibrate เซนเซอร์](#5-ขั้นตอนการ-calibrate-เซนเซอร์)
6. [คำสั่งทั้งหมด](#6-คำสั่งทั้งหมด)
7. [โหมดจำกัดความเร็ว PID (ModeSpdPID)](#7-โหมดจำกัดความเร็ว-pid-modespdpid)
8. [การจูน Kp / Kd](#8-การจูน-kp--kd)
9. [ตัวอย่างภารกิจ](#9-ตัวอย่างภารกิจ)
10. [ตัวแปร Global ที่ใช้ได้](#10-ตัวแปร-global-ที่ใช้ได้)
11. [ปัญหาที่พบบ่อย](#11-ปัญหาที่พบบ่อย)

---

## 1. การติดตั้ง

1. คัดลอกโฟลเดอร์ `HanumanminiFastLine` ไปไว้ที่ `Documents/Arduino/libraries/`
2. ติดตั้งไลบรารี **HanumanMini** (มีคำสั่ง `motor()`) และ **Servo** ด้วย
3. เปิด Arduino IDE แล้วเลือก **File → Examples → HanumanMiniFastLine16CH → HanumanMini_FastLine16Ch**
4. เลือกบอร์ด **Arduino Nano** แล้วอัปโหลด

ใน Sketch ต้อง include ตามลำดับนี้ (**HanumanMini ต้องมาก่อน**):

```cpp
#include <HanumanMini.h>
#include <HanumanMiniFastLine16CH.h>
```

> ทุกไฟล์ `.h` มี Include Guard (`#ifndef ... #define ... #endif`) แล้ว ถ้า include ซ้ำก็ไม่เกิด error

---

## 2. โครงสร้างไฟล์

### ไฟล์ไลบรารี

| ไฟล์ | หน้าที่ |
|---|---|
| `HanumanMiniFastLine16CH.h` | ไฟล์หลัก (include ไฟล์อื่นทั้งหมด) และคำสั่ง `Mode()` |
| `HanumanMiniFastLine16CH_CD74HC4067.h` | ตัวควบคุม Multiplexer 16 ช่อง (ฝังมาในไลบรารี ไม่ต้องติดตั้งเพิ่ม) |
| `HanumanMiniFastLine16CH_Sensor.h` | อ่านเซนเซอร์, Calibrate, EEPROM, ปุ่ม OK, Buzzer |
| `HanumanMiniFastLine16CH_Motor.h` | คำสั่งมอเตอร์พื้นฐาน `Motor`, `Move`, `MotorStop` |
| `HanumanMiniFastLine16CH_Initial.h` | ตารางชดเชยความเร็วและ KP/KD ตามช่วงความเร็ว |
| `HanumanMiniFastLine16CH_ForwardBackwardAndTurnl.h` | เดินหน้า ถอยหลัง เลี้ยว |
| `HanumanMiniFastLine16CH_PID.h` | หาตำแหน่งเส้น, PID, จับแยก, เดินตามเส้นตามเวลา |

### ไฟล์ตัวอย่าง (`examples/HanumanMini_FastLine16Ch/`)

| ไฟล์ | หน้าที่ |
|---|---|
| `HanumanMini_FastLine16Ch.ino` | `setup()` / `loop()` |
| `setting.ino` | ตั้งค่าเซนเซอร์ การเลี้ยว เสียง (`_setting()`) |
| `setup_speed.ino` | ตาราง KP/KD และค่าชดเชยความเร็ว |
| `Mission.ino` | เขียนภารกิจที่หุ่นต้องวิ่ง (`Mission()`) |

---

## 3. การต่อขา

| อุปกรณ์ | ขา |
|---|---|
| CD74HC4067 S0, S1, S2, S3 | 18, 17, 16, 15 (A4, A3, A2, A1) |
| CD74HC4067 SIG (สัญญาณ) | A0 |
| ปุ่ม OK | 8 (กด = HIGH) |
| Buzzer | 13 (เปลี่ยนได้ด้วย `SetBUZZER_PIN()`) |
| มอเตอร์ซ้าย / ขวา | ตามไลบรารี HanumanMini (`motor(1, …)` / `motor(2, …)`) |

ลำดับเซนเซอร์: **ช่อง 0 = ซ้ายสุด … ช่อง 15 = ขวาสุด** (กลางอยู่ระหว่างช่อง 7 และ 8)

---

## 4. เริ่มต้นใช้งาน

ลำดับการทำงานของตัวอย่าง:

```
เปิดเครื่อง → Beep 2 ครั้ง → _setting() → Mode() รอกดปุ่ม OK
   ├─ กดสั้น (< 2 วินาที)  → Beep → Mission()
   └─ กดค้าง (≥ 2 วินาที) → Calibrate → บันทึก EEPROM → กด OK → Mission()
หลัง Mission() จบ → loop() สั่งหยุดมอเตอร์ตลอด
```

โครง `setup()` ขั้นต่ำ:

```cpp
#include <HanumanMini.h>
#include <HanumanMiniFastLine16CH.h>

void setup() {
  Serial.begin(115200);
  pinMode(signal_pin, INPUT);
  _setting();   // ตั้งค่า (setting.ino)
  Mode();       // รอกดปุ่ม OK
  Beep(200);
  Mission();    // ภารกิจ (Mission.ino)
}

void loop() {
  MotorStop();
  delay(50);
}
```

---

## 5. ขั้นตอนการ Calibrate เซนเซอร์

การ Calibrate คือการหาค่าต่ำสุด (พื้น) และสูงสุด (เส้น) ของแต่ละช่อง แล้วนำไปแปลงค่าเป็น **0-1000**

### วิธีที่ 1: กดปุ่ม OK ค้าง (แนะนำ)

1. วางหุ่นบนสนามให้เซนเซอร์คร่อมเส้น
2. เปิดเครื่อง แล้ว **กดปุ่ม OK ค้าง 2 วินาที** แล้วปล่อย → ได้ยินเสียง `b_beebb`
3. กดปุ่ม OK อีกครั้ง → เริ่ม Calibrate (ประมาณ 2-3 วินาที) ระหว่างนี้ให้**เลื่อนหุ่นไปมาให้เซนเซอร์ทุกช่องผ่านทั้งพื้นและเส้น**
4. ค่าจะถูกบันทึกลง EEPROM อัตโนมัติ → ได้ยินเสียง `b_beebb`
5. กด OK เพื่อเริ่ม Mission

### วิธีที่ 2: ผ่าน Serial Monitor

1. เปิด `CaliberateRobotSensor();` ใน `setting.ino`
2. เปิด Serial Monitor (115200) แล้วกด OK แล้วเลื่อนหุ่นไปมา
3. คัดลอกบรรทัด `SensorValue (...);` ที่พิมพ์ออกมา ไปวางแทนของเดิมใน `setting.ino` (ค่าจะถูกบันทึกลง EEPROM ด้วย)

> **ข้อสำคัญ:** `LoadCalibration()` ใน `setting.ino` ถูกเรียก**หลัง** `SensorValue(...)` ค่าจาก EEPROM จึงเขียนทับค่าที่พิมพ์ไว้ ถ้าต้องการใช้ค่าจาก `SensorValue(...)` ให้ปิด `LoadCalibration();`

### ตรวจผล

- `SerialSensor();` ดูค่าดิบ (0-1023)
- `SerialCalibrateSensor();` ดูค่าหลัง Calibrate: บนเส้นควรได้ใกล้ **1000** บนพื้นควรได้ใกล้ **0**
- `SerialPosition();` ดูตำแหน่งเส้น: กลางเส้น ≈ **7500**, Error ≈ **0**

---

## 6. คำสั่งทั้งหมด

### 6.1 ตั้งค่าทั่วไป

| คำสั่ง | คำอธิบาย |
|---|---|
| `TrackLineColor(col)` | สีของเส้น: `0` = เส้นดำบนพื้นขาว, `1` = เส้นขาวบนพื้นดำ |
| `RefLineValue(x)` | ค่าอ้างอิงจับเส้น (0-1000) ใช้กับจับแยกและเลี้ยว เช่น `400` |
| `Dottedline(dot)` | `0` = ไม่มีเส้นประ, `1` = มีเส้นประ (หลุดเส้นแล้ววิ่งตามตำแหน่งเดิม) |
| `ModeSpdPID(mode, max, min)` | โหมดจำกัดความเร็ว PID (ดูหัวข้อ 7) |
| `SetBUZZER_PIN(pin)` | เปลี่ยนขา Buzzer (ค่าเริ่มต้น 13) |
| `SetVolumeSound(hz)` | ความถี่เสียง `Beep` (ค่าเริ่มต้น 2500) |
| `TurnSpeedLeft(ch, l, r, de)` | ตั้งค่าเลี้ยวซ้ายที่แยก: ช่องเซนเซอร์ที่หยุด, ความเร็วล้อซ้าย, ล้อขวา, เวลาเลี้ยวก่อนหาเส้น (ms) |
| `TurnSpeedRight(ch, l, r, de)` | ตั้งค่าเลี้ยวขวาที่แยก (ความหมายเหมือนด้านบน) |

### 6.2 เซนเซอร์และ Calibrate

| คำสั่ง | คำอธิบาย |
|---|---|
| `Read()` | อ่านค่าดิบ 16 ช่อง เก็บใน `F[0..15]` |
| `ReadCalibrate()` | อ่านค่าแล้วแปลงเป็น 0-1000 เก็บใน `F[0..15]` (ค่ามาก = อยู่บนเส้น) |
| `analog16(ch)` | อ่านค่าดิบ 1 ช่องผ่าน Multiplexer (0-15) |
| `analog(ch)` | อ่านขา A0-A7 ของบอร์ดโดยตรง (ช่อง 0-7) |
| `CalibrateSensor(pause, samples)` | เก็บค่า Min/Max จำนวน `samples` ครั้ง เว้นครั้งละ `pause` ms |
| `CaliberateRobotSensor()` | รอกด OK → Calibrate → พิมพ์ `SensorValue(...)` → บันทึก EEPROM |
| `SensorValue(min0, …, min15, max0, …, max15)` | ใส่ค่า Min 16 ค่า และ Max 16 ค่าเอง |
| `SaveCalibration()` | บันทึก Min/Max ลง EEPROM |
| `LoadCalibration()` | โหลด Min/Max จาก EEPROM และพิมพ์ออก Serial |
| `SerialSensor()` | แสดงค่าดิบทาง Serial (วนไม่รู้จบ) |
| `SerialCalibrateSensor()` | แสดงค่าหลัง Calibrate ทาง Serial (วนไม่รู้จบ) |
| `SerialPosition()` | แสดงตำแหน่งเส้นและ Error ทาง Serial (วนไม่รู้จบ) |

### 6.3 ปุ่มและเสียง

| คำสั่ง | คำอธิบาย |
|---|---|
| `OK()` | รอจนกดปุ่ม OK แล้ว Beep |
| `OK_PUSH()` | คืนค่า `PRESS` (1) ถ้ากำลังกดปุ่ม ไม่กดคืน `0` |
| `Mode()` | เมนูเริ่มต้น: กดสั้น = ออก, กดค้าง 2 วินาที = Calibrate + บันทึก |
| `Beep(ms)` | ส่งเสียงตามเวลา |
| `BZon()` / `BZoff()` | เปิด / ปิดเสียงค้าง |
| `b_beebb()` | เสียงแจ้งเตือน 4 จังหวะ |

### 6.4 มอเตอร์และการเคลื่อนที่

ความเร็ว **-100 ถึง 100** (ค่าลบ = ถอยหลัง), เวลาเป็น **มิลลิวินาที** (1000 = 1 วินาที)

| คำสั่ง | คำอธิบาย |
|---|---|
| `Motor(l, r)` | สั่งล้อซ้าย/ขวาค้างไว้ |
| `Move(l, r, time)` | สั่งล้อซ้าย/ขวาตามเวลา (ไม่หยุดเอง) |
| `MotorStop()` | หยุดมอเตอร์ (หน่วง 5 ms) |
| `MotorStop(time)` | หยุดมอเตอร์แล้วหน่วงตามเวลา |
| `FD(speed, time)` | เดินหน้าตรง (ใช้ค่าชดเชย `setBalanceSpeed`) |
| `BK(speed, time)` | ถอยหลังตรง (ใช้ค่าชดเชย `setBalanceBackSpeed`) |
| `TL(speed, time)` / `TR(speed, time)` | เลี้ยวซ้าย/ขวาแบบล้อเดียวตามเวลา แล้วหยุด |
| `SL(speed, time)` / `SR(speed, time)` | หมุนตัวอยู่กับที่ซ้าย/ขวาตามเวลา แล้วหยุด |
| `TurnLeft()` / `TurnRight()` | เลี้ยวจนช่องที่ตั้งใน `TurnSpeedLeft/Right` เจอเส้น (≥ `REF`) |
| `Left(ch, speed)` / `Right(ch, speed)` | หมุนตัวจนช่อง `ch` เจอเส้น (≥ 500) |
| `TestMotor()` | ทดสอบ: ล้อซ้าย 0.5 วินาที แล้วล้อขวา 0.5 วินาที |

### 6.5 ค่าชดเชยความเร็วและ KP/KD (`setup_speed.ino`)

ช่วงความเร็ว: `SPD_10` (≤10), `SPD_20` (11-20), `SPD_30` (21-30) … `SPD_100` (>90)

| คำสั่ง | คำอธิบาย |
|---|---|
| `setBalanceSpeed(spd, l, r)` | ค่าชดเชยเดินหน้า: ความเร็วจริง = `Speed - l` / `Speed - r` |
| `setBalanceBackSpeed(spd, l, r)` | ค่าชดเชยถอยหลัง |
| `Set_KP_KD(spd, kp, kd)` | เก็บค่า KP/KD ของช่วงความเร็ว ลงใน `PID_KP` / `PID_KD` |
| `InitialSpeed()` | คำนวณ `LeftBaseSpeed`, `RightBaseSpeed`, `BackLeftBaseSpeed`, `BackRightBaseSpeed`, `PID_KP`, `PID_KD` จาก `BaseSpeed` |

ตัวอย่าง: หุ่นวิ่งเดินหน้าที่ความเร็ว 40 แล้วเบี้ยวไปทางซ้าย (ล้อขวาแรงกว่า) → `setBalanceSpeed(SPD_40, 0, 3);` (ลดล้อขวา 3)

> คำสั่ง `TrackCross…` / `TrackTime…` ใช้ค่า `Kp`, `Kd` ที่ส่งเข้าไปในคำสั่งโดยตรง **ไม่ได้ใช้ตาราง `Set_KP_KD`** ถ้าต้องการใช้ตาราง ให้ส่ง `PID_KP`, `PID_KD` หลังเรียก `InitialSpeed()` เอง

### 6.6 เดินตามเส้น (PID)

| คำสั่ง | คำอธิบาย |
|---|---|
| `TrackCross(speed, kp, kd, sel)` | PID จนเจอแยกเต็ม (ช่อง 4, 5, 7, 8, 10, 11 ≥ `REF` พร้อมกัน) แล้วทำ `sel` |
| `TrackCrossL(speed, kp, kd, sel)` | PID จนคู่ซ้าย (ช่อง 4 และ 7) ≥ `REF` แล้วทำ `sel` |
| `TrackCrossR(speed, kp, kd, sel)` | PID จนคู่ขวา (ช่อง 8 และ 11) ≥ `REF` แล้วทำ `sel` |
| `TrackCrossC(speed, kp, kd, sel)` | PID จนคู่กลาง (ช่อง 5 และ 10) ≥ `REF` แล้วทำ `sel` |
| `TrackTime(speed, kp, kd, time)` | PID ตามเวลา ใช้เซนเซอร์ครบ 16 ช่อง |
| `TrackTimeForLineFast(start, end, speed, kp, kd, time)` | PID ตามเวลา ใช้เฉพาะเซนเซอร์ช่อง `start` ถึง `end` |
| `TrackSelect(speed, sel)` | ทำคำสั่งเจอแยก (เรียกเองได้) |
| `PID(l, r, kp, kd)` | PID 1 รอบ (16 ช่อง) — ใช้ในลูปที่เขียนเอง |
| `PID(start, end, l, r, kp, kd)` | PID 1 รอบ (ช่อง `start`-`end`) |
| `readPosition(track, noise)` | คืนตำแหน่งเส้น 0-15000 (16 ช่อง) |
| `readPosition(start, end, track, noise)` | คืนตำแหน่งเส้นจากช่อง `start`-`end` |

**คำสั่งเจอแยก (`sel`)**

| ค่า | ความหมาย |
|---|---|
| `'p'` | วิ่งตรงผ่านแยก (เปิดเสียงจนช่อง 4 และ 11 พ้นเส้น) |
| `'l'` | วิ่งช้าจนพ้นแยก แล้ว `TurnLeft()` |
| `'r'` | วิ่งช้าจนพ้นแยก แล้ว `TurnRight()` |
| `'s'` | หยุด |

**ตำแหน่งเส้น:** `0` = เส้นอยู่ใต้ช่อง 0 (ซ้ายสุด), `7500` = กลาง, `15000` = ใต้ช่อง 15 (ขวาสุด) ถ้าหลุดเส้นจะคืน `0` หรือ `15000` ตามด้านที่หลุด (ถ้า `Dottedline(1)` จะคืนค่าเดิม)

**สูตร PID** (ใช้เหมือนกันทุกคำสั่ง)

```
PID = Kp × Error + Kd × (Error − LastError)
```

- **Kp** ใส่เป็นทศนิยม 3 ตำแหน่ง (`0.000`) เช่น `0.015`
- **Kd** ใส่เป็นทศนิยม 2 ตำแหน่ง (`0.00`) เช่น `0.25`
- Error อยู่ในช่วง −7500 ถึง 7500 เช่น Kp = `0.010` ตอนเส้นอยู่ขอบสุด จะได้ PID = 75

ล้อซ้าย = `Speed + PID`, ล้อขวา = `Speed − PID` แล้วจำกัดค่าด้วย `ModeSpdPID`

> **ถ้าย้ายมาจากเวอร์ชันเก่า** (ที่ Kp/Kd เป็นจำนวนเต็ม): Kp ใหม่ = Kp เก่า ÷ 1000, Kd ใหม่ = Kd เก่า ÷ 100 เช่น `TrackCross(40, 10, 80, 'p')` → `TrackCross(40, 0.010, 0.80, 'p')`

---

## 7. โหมดจำกัดความเร็ว PID (ModeSpdPID)

`ModeSpdPID(mode, MaxSpeed, MinSpeed);`

| โหมด | เมื่อค่าเกิน MaxSpeed | เมื่อค่าติดลบ / ต่ำ | เหมาะกับ |
|---|---|---|---|
| `0` | = MaxSpeed | ติดลบ → = MinSpeed | ใช้ MinSpeed เป็นค่าถอยคงที่ |
| `1` | = MaxSpeed | ต่ำกว่า MinSpeed → = MinSpeed | โหมดปกติทั่วไป |
| `2` | = Speed ของล้อนั้น | ต่ำกว่า −Speed → = −Speed | สนามโค้งเยอะ (วิ่งช้า นิ่ง) |
| `3` | = MaxSpeed | ติดลบ → = −BaseSpeed | เข้าโค้งแรงด้วยการถอยล้อใน |
| อื่น ๆ (เช่น `4`) | = MaxSpeed | ติดลบ → = 0 | ไม่ให้ล้อถอยหลัง |

---

## 8. การจูน Kp / Kd

ช่วงค่าที่ใช้บ่อย: **Kp = 0.005 - 0.030**, **Kd = 0.10 - 0.80**

1. ตั้งความเร็วต่ำก่อน (30-40) และ `Kd` = 0.00
2. เพิ่ม `Kp` ทีละ 0.002 จนหุ่นเข้าโค้งได้ทัน (ถ้าส่ายมากแปลว่า Kp สูงไป)
3. เพิ่ม `Kd` ทีละ 0.05 จนหุ่นหยุดส่าย วิ่งนิ่ง
4. เพิ่มความเร็วทีละ 10 แล้วจูนซ้ำ จดค่าลงใน `setup_speed.ino`

| อาการ | แก้ไข |
|---|---|
| เข้าโค้งไม่ทัน หลุดโค้ง | เพิ่ม Kp |
| ส่ายซ้าย-ขวาตลอด | ลด Kp หรือเพิ่ม Kd |
| สั่นถี่ ๆ | ลด Kd |
| วิ่งตรงแล้วเบี้ยว | ปรับ `setBalanceSpeed` |

---

## 9. ตัวอย่างภารกิจ

```cpp
void Mission() {
  ModeSpdPID(1, 100, -100);
  TrackLineColor(0);     // เส้นดำ
  Dottedline(0);
  RefLineValue(400);

  TrackCross(40, 0.010, 0.80, 'p');  // ผ่านแยกที่ 1 ตรงไป
  TrackCross(40, 0.010, 0.80, 'l');  // แยกที่ 2 เลี้ยวซ้าย
  TrackCrossR(40, 0.010, 0.80, 'r'); // เจอแยกขวา เลี้ยวขวา
  TrackTime(50, 0.020, 0.40, 1500);  // วิ่งตามเส้น 1.5 วินาที
  TrackTimeForLineFast(1, 14, 50, 0.025, 0.25, 1000);  // วิ่งเร็วด้วยช่อง 1-14 นาน 1 วินาที
  TrackCross(30, 0.010, 0.80, 's');  // เจอแยกแล้วหยุด

  FD(30, 300);           // เดินหน้า 0.3 วินาที
  MotorStop(500);
  SL(40, 250);           // หมุนซ้าย 0.25 วินาที
  BK(30, 300);           // ถอยหลัง 0.3 วินาที
  MotorStop();
}
```

ตัวอย่างลูป PID ที่เขียนเอง (วิ่งจนช่อง 0 เจอเส้น):

```cpp
BaseSpeed = 40;
InitialSpeed();
while (1) {
  ReadCalibrate();
  if (F[0] >= REF) break;
  PID(LeftBaseSpeed, RightBaseSpeed, 0.010, 0.80);
}
MotorStop();
```

---

## 10. ตัวแปร Global ที่ใช้ได้

| ตัวแปร | ความหมาย |
|---|---|
| `F[16]` | ค่าเซนเซอร์ล่าสุด (หลัง `Read()` หรือ `ReadCalibrate()`) |
| `MinValue[16]`, `MaxValue[16]` | ค่า Calibrate |
| `REF` | ค่าอ้างอิงจับเส้น (`RefLineValue`) |
| `LineColor` | สีเส้น |
| `BaseSpeed` | ความเร็วฐานล่าสุด |
| `LeftBaseSpeed`, `RightBaseSpeed` | ความเร็วเดินหน้าหลังชดเชย |
| `BackLeftBaseSpeed`, `BackRightBaseSpeed` | ความเร็วถอยหลังหลังชดเชย |
| `PID_KP`, `PID_KD` | ค่าจากตาราง `Set_KP_KD` ของช่วงความเร็วปัจจุบัน |
| `MaxSpeed`, `MinSpeed`, `ModePidStatus` | ค่าจาก `ModeSpdPID` |
| `LastError` | Error รอบก่อนหน้าของ PID |
| `NUM_SENSORS` | จำนวนเซนเซอร์ (16) |
| `signal_pin` | ขาสัญญาณ Multiplexer (A0) |

---

## 11. ปัญหาที่พบบ่อย

| ปัญหา | สาเหตุ / วิธีแก้ |
|---|---|
| `'motor' was not declared` | ไม่ได้ `#include <HanumanMini.h>` หรือ include หลังไลบรารีนี้ |
| `Servo.h: No such file` | ติดตั้งไลบรารี Servo (Library Manager) |
| หุ่นไม่เห็นเส้น / วิ่งออกนอกเส้น | Calibrate ใหม่, ตรวจ `TrackLineColor` ให้ตรงกับสีเส้น |
| ไม่จับแยก | ลด `RefLineValue` (เช่น 300) ดูค่าใน `SerialCalibrateSensor()` |
| จับแยกผิดตอนเข้าโค้ง | เพิ่ม `RefLineValue` หรือใช้ `TrackCross` (ต้องเจอทั้ง 3 คู่) |
| เลี้ยวที่แยกแล้วเลยเส้น | ลดความเร็วใน `TurnSpeedLeft/Right` หรือลดเวลา `de` |
| ค่าที่ใส่ใน `SensorValue` ไม่มีผล | `LoadCalibration()` เขียนทับ ให้ปิดคำสั่งนั้น |
| `Serial…()` ค้าง ไม่ทำคำสั่งต่อ | ปกติ: คำสั่งทดสอบวนไม่รู้จบ ปิดคำสั่งนั้นก่อนใช้งานจริง |

---

ไฟล์ `HanumanMiniFastLine16CH_CD74HC4067.h` ดัดแปลงจาก light_CD74HC4067 โดย Sunit Raut ใช้สัญญาอนุญาต GPL-3.0
