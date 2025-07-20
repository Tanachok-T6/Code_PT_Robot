#include <PTBOTAtomVX.h>  // เรียกใช้งาน Library
#include <ESP32Encoder.h>

#define NO_SWITCH_LR 0
#define SWITCH_LR 1

//                    S0   S1   S2   S3   S4   S5
int sensor_min[] = { 175, 150, 150, 50, 50, 230 };    // ค่าสีดำ
int sensor_max[] = { 685, 520, 630, 250, 300, 900 };  // ค่าสีขาว
float power_factor = 1.0;

int line_value[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
int current_degree = 0;
float previous_error_forward = 0;
byte servo_pin[] = { 6, 5, 4, 0, 1, 2, 3 };

long delayALL = 150;

TaskHandle_t sonarTaskHandle = NULL;
TaskHandle_t switchGripperTaskHandle = NULL;
// มอเตอร์ ต่อ 1 ซ้าย 2 ขวา
//เซ็นเซอร์ ซ้ายหน้า = 0 ขวา หน้า 1
//เซ้็นเซอร์ หลัง ขวา 2-ซ้ายสุด 5
void setup() {
  initialize();          // เริ่มต้นการทำงานบอร์ด Atom VX  จะแสดง รูป PT
  Serial.begin(115200);  // เริ่มต้นใช้งาน Serial
  xTaskCreatePinnedToCore(liftLoop, "liftLoop", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(sonarLoop, "sonarLoop", 2048, NULL, 1, &sonarTaskHandle, 1);
  xTaskCreatePinnedToCore(switchGripperLoop, "switchGripperLoop", 2048, NULL, 1, &switchGripperTaskHandle, 1);
  vTaskSuspend(sonarTaskHandle);
  vTaskSuspend(switchGripperTaskHandle);
  setupDistanceSensor();
  setupServo();
  // lift(3);  // ยกแขน 3 จากจุดเดิม *ก่อนเปิดหุ่นต้องกดลงให้สุดก่อน ** สำคัญต้องปรับก่อน
  ///////////////////////เปิดอ่าค่า เซนเซอร์ ////////////////////////////////
  // ShowValue_Sensor(); // โชว์ค่าเซ็นเซอร์ตรวจจับเส้น
  // ShowDistanceSensor(); // โชว์ค่าเซ็นเซอร์วัดระยะทาง
  //////////////////////////////////////////////////////////
}

void loop() {
  // lift(2000); delay(1500); lift(0);
  STOP();            //สั่งหยุด กด SW1 or SW2 เพื่อทำงานต่อ
  setAngleOffset();  // เช็ตตั้งค่า servo
  current_degree = 0;

  /////////////////////////
  // BoxsAll(); //--->>> เดินทุกบล็อก
  Box1(); //--->>> เดินบล็อก 1
  Box2(); //--->>> เดินบล็อก 2
  Box3(); //--->>> เดินบล็อก 3
  Box4(); //--->>> เดินบล็อก 4
  Box5(); //--->>> เดินบล็อก 5
  Box6(); //--->>> เดินบล็อก 6
  Box7(); //--->>> เดินบล็อก 7
  Box8(); //--->>> เดินบล็อก 8
  Box9(); //--->>> เดินบล็อก 9
  Box10(); //--->>> เดินบล็อก 10
  // Box11(); //--->>> เดินบล็อก 11
  // Box12(); //--->>> เดินบล็อก 12
  // Box13(); //--->>> เดินบล็อก 13
  // Box14(); //--->>> เดินบล็อก 14
  // Box15(); //--->>> เดินบล็อก 15
  // Test_Function(); //--->>> เทสฟังชั่นต่างๆ
  // Test_F_Move_Turn();
  End();
}
void Test_F_Move_Turn() {
  // servoCloseLeft();
  // servoCloseRight();
  // delay(500);
  // lift(2);
  // delay(250);
  // SetBack(20, 200, 'L');
  // placeBox(5, 4, 'r', 'r');
  // placeBox(5, 4, 'l', 'l');
  Servo_LRfast('L');
}
void BoxsAll() {
  Box1();
  Box2();
  Box3();
  Box4();
  Box5();
  Box6();
  Box7();
  Box8();
  Box9();
  Box10();
  Box11();
  Box12();
  Box13();
  Box14();
  Box15();
}