#include <PTBOTAtomVX.h>
#include <ESP32Encoder.h>
#include <SoftI2C.h>

#define NO_SWITCH_LR 0
#define SWITCH_LR 1

/* ค่าเซ็นเซอร์ตรวจจับเส้น */
//                  S0   S1   S2   S3   S4   S5
int sensor_min[] = { 95, 135, 147, 106, 90, 127 };    // ค่าสีดำ
int sensor_max[] = { 447, 573, 795, 549, 502, 724 };  // ค่าสีขาว

/* ค่าเซ็นเซอร์อ่านค่าสี */
int clear_color_left = 6290;    // ค่าสีเหลืองมือจับด้านซ้าย
int clear_color_right = 6302;   // ค่าสีเหลืองมือจับด้านขวา

/* ค่าระยะจอดหน้าแท่นวาง */
float place_left_side = 2.5;      // ค่าระยะจอดกรณีมือซ้ายวางกระป๋องซ้าย
float place_left_center = 2.8;    // ค่าระยะจอดกรณีมือซ้ายวางกระป๋องกลาง
float place_right_side = 2.5;     // ค่าระยะจอดกรณีมือขวาวางกระป๋องขวา
float place_right_center = 2.8;   // ค่าระยะจอดกรณีมือขวาวางกระป๋องกลาง

float power_factor = 1.0;

int line_value[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
int current_degree = 0;
float previous_error_forward = 0;
byte servo_pin[] = { 6, 5, 4, 0, 1, 2, 3 };

long delayALL = 150;

TaskHandle_t sonarTaskHandle = NULL;
TaskHandle_t switchGripperTaskHandle = NULL;

void setup() {
  initialize();
  Serial.begin(115200);
  xTaskCreatePinnedToCore(liftLoop, "liftLoop", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(sonarLoop, "sonarLoop", 2048, NULL, 1, &sonarTaskHandle, 0);
  xTaskCreatePinnedToCore(switchGripperLoop, "switchGripperLoop", 2048, NULL, 1, &switchGripperTaskHandle, 1);
  vTaskSuspend(sonarTaskHandle);
  vTaskSuspend(switchGripperTaskHandle);
  setupColorSensor();
  setupServo();
  lift(3);
  ///////////////////////เปิดอ่านค่า เซนเซอร์ ////////////////////////////////
  // ShowValue_Sensor();    // โชว์ค่าเซ็นเซอร์ตรวจจับเส้น
  // ShowDistanceSensor();  // โชว์ค่าเซ็นเซอร์วัดระยะทาง
  // calibrateCanColor();   // Calibrate เซ็นเซอร์อ่านค่าสี
  //////////////////////////////////////////////////////////
}

void loop() {
  STOP();
  setAngleOffset();
  current_degree = 0;

  Can1();        // ออกจากจุดสตาร์ท ไปหยิบกระป๋อง และไปวางกระป๋องที่ 1
  Can2();        // ไปวางกระป๋องที่ 2
  Can3();        // ไปหยิบกระป๋อง และไปวางกระป๋องที่ 3
  Can4();        // ไปวางกระป๋องที่ 4
  Can5();        // ไปหยิบกระป๋อง และไปวางกระป๋องที่ 5
  Can6();        // ไปวางกระป๋องที่ 6
  EndMission();  // เสร็จแล้ววิ่งไปจุดจบ
}