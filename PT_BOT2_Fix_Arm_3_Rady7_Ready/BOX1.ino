// S 40 D 700 = 1Box
// S 60 D 535 = 1Box
// S 60 D 840 = 2Box
// S 60 D 1220 = 3Box
///////--> motorTimeDegree_Enhanced( 30, 60, 840, 'L');
// 1 Box = 185
// 2 Box = 525
// 3 Box = 825
// 4 Box = 1175
///////--> BackmotorTimeBackDegree_Enhanced( 30, 60, 840, 'L');
// --> เซ็ตหน้า/หลัง
// --> 330/275 //เส้นนอก
// --> 300/200 //เส้นใน
// --> ตอนคีบแล้วจะถอยหลังมากลางบล็อก
// --> -150
// SetBack(20, 300);
// SetFront(20, 300, 'L');
// SetFront_Wall(20, 300, 'R');
void Box1() {
  motorTimeDegree_Enhanced( 30, 60, 200);SetFront(20, 300, 'L');
}
void Box2() {
  pickCan(5, NO_SWITCH_LR);
  BackmotorTimeDegree_Enhanced( 30, 40, 80, 'R');
  BackmotorTimeDegree_Enhanced( 30, 60, 200);SetBack(20, 275, 'L');
}
void Box3() {
  placeBox(5, 4, 'l', 'l'); //เขียว
  delay(250);
  BackmotorTimeDegree_Enhanced( 30, 40, 80, 'R');
  motorTimeDegree_Enhanced( 30, 60, 200);SetFront(20, 300, 'L');
  BackmotorTimeDegree_Enhanced( 30, 60, 200);SetBack(20, 300, 'L');
  Servo_LRfast('R');
}
void Box4() {
  placeBox(15, 5, 'l', 'l');  //แดง
  delay(1350);
  BackmotorTimeDegree_Enhanced( 30, 40, 80, 'R');SetBack(20, 300, 'R');
  Balance_Arm('C');
  motorTimeDegree_Enhanced( 30, 60, 230, 'L');
  motorTimeDegree_Enhanced( 30, 60, 200, 'R');SetBack(20, 300);
}
void Box5() {
  pickCan(5, NO_SWITCH_LR);
  SetBack(20, 330, 'L');
  motorTimeDegree_Enhanced( 30, 60, 200);SetFront_Wall(20, 300, 'R');
}
void Box6() {
  placeBox2(10, 4, 's', 's'); //เหลือง2
  delay(550);
  BackmotorTimeDegree_Enhanced( 30, 40, 80, 'R');SetBack(20, 275);
  motorTimeDegree_Enhanced( 30, 60, 525, 'L');
  motorTimeDegree_Enhanced( 30, 60, 200);SetFront_Wall(20, 300, 'R');
}
void Box7() {
  pickCan(0, NO_SWITCH_LR);
  BackmotorTimeDegree_Enhanced( 30, 40, 80, 'R');SetBack(20, 275);
  motorTimeDegree_Enhanced( 30, 60, 525, 'R');SetBack(20, 275, 'L');
}
void Box8() {
  placeBox(15, 5, 'r', 's');  //แดง
  delay(1350);
  BackmotorTimeDegree_Enhanced( 30, 40, 80, 'L');SetFront(20, 300);
  BackmotorTimeDegree_Enhanced( 30, 60, 250, 'L');
  BackmotorTimeDegree_Enhanced( 30, 60, 225);SetBack(20, 275, 'L');
}
void Box9() {
  Servo_LRfast('L');
  delay(250);
  placeBox(5, 4, 'r', 'r'); //เขียว
  delay(250);Balance_Arm('C');
  BackmotorTimeDegree_Enhanced( 30, 40, 80, 'R');SetBack(20, 275); 
  motorTimeDegree_Enhanced( 30, 60, 200, 'R');
  motorTimeDegree_Enhanced( 30, 60, 200);SetFront(20, 330, 'L');
  motorTimeDegree_Enhanced( 30, 60, 200, 'R');
  motorTimeDegree_Enhanced( 30, 60, 200);SetFront(20, 330, 'R');
  motorTimeDegree_Enhanced( 30, 60, 200);SetFront(20, 330, 'L');
  motorTimeDegree_Enhanced( 30, 60, 200, 'R');
  motorTimeDegree_Enhanced( 30, 60, 200, 'L');
  BackmotorTimeDegree_Enhanced( 30, 60, 230);motorStop(); delay(150);
}
void Box10() {
  motorTimeDegree_Enhanced( 30, 60, 210, 'L');SetBack(20, 250);
  motorTimeDegree_Enhanced( 30, 60, 200, 'R');
  motorTimeDegree_Enhanced( 30, 50, 550);SetFront(20, 300, 'L');
  motorTimeDegree_Enhanced( 30, 50, 550);SetFront_Wall(20, 300, 'L');
  motorTimeDegree_Enhanced( 30, 60, 525);motorStop(); delay(150);
  BackmotorTimeDegree_Enhanced( 30, 60, 525, 'L');SetBack(20, 250);
  motorTimeDegree_Enhanced( 30, 50, 550);SetFront(20, 300, 'R');SetBack(20, 300);
  motorTimeDegree_Enhanced( 30, 50, 800);SetFront(20, 300, 'R');
  motorTimeDegree_Enhanced( 30, 60, 200, 'L');SetBack(20, 300);
  motorTimeDegree_Enhanced( 30, 60, 200, 'L');
  motorTimeDegree_Enhanced( 30, 60, 200, 'R');SetBack(20, 300);
  motorTimeDegree_Enhanced( 30, 60, 250, 'R');
  BackmotorTimeDegree_Enhanced( 30, 60, 200);
}
void Box11() {

}
void Box12() {

}
void Box13() {
  
}
void Box14() {
  
}
void Box15() {
  
}