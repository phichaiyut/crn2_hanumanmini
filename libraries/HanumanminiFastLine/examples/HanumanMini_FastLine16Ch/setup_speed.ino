// ตาราง KP/KD ของแต่ละช่วงความเร็ว Set_KP_KD(ช่วงความเร็ว, KP, KD);
void SetKpKd() {
  Set_KP_KD(SPD_10, 4, 8);
  Set_KP_KD(SPD_20, 5, 10);
  Set_KP_KD(SPD_30, 14, 28);
  Set_KP_KD(SPD_40, 14, 14);
  Set_KP_KD(SPD_50, 15, 28);
  Set_KP_KD(SPD_60, 22, 18);
  Set_KP_KD(SPD_70, 26, 20);
  Set_KP_KD(SPD_80, 30, 60);
  Set_KP_KD(SPD_90, 12, 24);
  Set_KP_KD(SPD_100, 13, 26);
}

// ค่าชดเชยเดินหน้า: ข้างไหนแรงกว่า ให้ใส่ค่าลดที่ข้างนั้น
// setBalanceSpeed(ช่วงความเร็ว, ลดข้างซ้าย, ลดข้างขวา);
void SetBalanceSpeedForward() {
  setBalanceSpeed(SPD_10, 0, 0);   // ความเร็ว 1-10
  setBalanceSpeed(SPD_20, 0, 0);   // ความเร็ว 11-20
  setBalanceSpeed(SPD_30, 0, 0);   // ความเร็ว 21-30
  setBalanceSpeed(SPD_40, 0, 3);   // ความเร็ว 31-40
  setBalanceSpeed(SPD_50, 0, 0);   // ความเร็ว 41-50
  setBalanceSpeed(SPD_60, 0, 0);   // ความเร็ว 51-60
  setBalanceSpeed(SPD_70, 0, 0);   // ความเร็ว 61-70
  setBalanceSpeed(SPD_80, 0, 0);   // ความเร็ว 71-80
  setBalanceSpeed(SPD_90, 0, 0);   // ความเร็ว 81-90
  setBalanceSpeed(SPD_100, 0, 0);  // ความเร็ว 91-100
}

// ค่าชดเชยถอยหลัง: ข้างไหนแรงกว่า ให้ใส่ค่าลดที่ข้างนั้น
// setBalanceBackSpeed(ช่วงความเร็ว, ลดข้างซ้าย, ลดข้างขวา);
void SetBalanceSpeedBackward() {
  setBalanceBackSpeed(SPD_10, 0, 0);   // ความเร็ว 1-10
  setBalanceBackSpeed(SPD_20, 0, 0);   // ความเร็ว 11-20
  setBalanceBackSpeed(SPD_30, 0, 0);   // ความเร็ว 21-30
  setBalanceBackSpeed(SPD_40, 0, 0);   // ความเร็ว 31-40
  setBalanceBackSpeed(SPD_50, 0, 0);   // ความเร็ว 41-50
  setBalanceBackSpeed(SPD_60, 0, 0);   // ความเร็ว 51-60
  setBalanceBackSpeed(SPD_70, 0, 0);   // ความเร็ว 61-70
  setBalanceBackSpeed(SPD_80, 0, 0);   // ความเร็ว 71-80
  setBalanceBackSpeed(SPD_90, 0, 0);   // ความเร็ว 81-90
  setBalanceBackSpeed(SPD_100, 0, 0);  // ความเร็ว 91-100
}
