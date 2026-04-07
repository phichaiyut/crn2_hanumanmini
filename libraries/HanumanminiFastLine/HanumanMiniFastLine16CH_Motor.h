
void Motor(int leftsp, int rightsp) {
  motor(1,leftsp);
  motor(2,rightsp);
}

void Move(int l, int r, int deelay) {
  Motor(l, r);
  delay(deelay);
}

void MotorStop() {
  Motor(0,0);
  delay(5);
}

void MotorStop(int tim) {
  Motor(0,0);
  delay(tim);
}