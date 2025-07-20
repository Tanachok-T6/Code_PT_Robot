#define chanal_Swing_Left 0
#define chanal_Swing_Right 1
#define chanal_Gripper_Left 2
#define chanal_Gripper_Right 3
////////////////////////////////////////////////////////
int Swing_Left = 80;   //SO สวิงแขนซ้ายเพิ่มง้างออก ลด หุบเข้า
int Swing_Right = 95;  //S1 สวิงแขนขวาเพิ่มง้างออก ลด หุบเข้า
////////////////////////////////////////////////////////
int Open_Left = 35;   //S2 อ้ามือ ชุดมือจับมือซ้ายค่ามากยิ่งหุบ 35
int Close_Left = 67;  //S2 หุบมือ ชุดมือจับมือซ้ายค่าน้อยยิ่งอ้า
////////////////////////////////////////////////////
int Open_Right = 45;   //S3 อ้ามือ ชุดมือจับมือขวาค่ามากยิ่งหุบ 35
int Close_Right = 76;  //S3 หุบมือ ชุดมือจับมือขวา ค่าน้อยยิ่งอ้า
////////////////////////////////////////////////////
int switch_degree = 35;

int switch_flag = 0;
int lift_target = 0;
int swing_current_degree[] = { 0, 0 };
char can_in_gripper[2] = { 'u', 'u' };

void setupServo() {
  servoWrite(chanal_Swing_Left, Swing_Left);
  servoWrite(chanal_Swing_Right, Swing_Right);
  servoWrite(chanal_Gripper_Left, Open_Left);
  servoWrite(chanal_Gripper_Right, Open_Right);
}

void servoOpenLeft() {
  for (int i = Close_Left; i > Open_Left; i--) {
    servoWrite(chanal_Gripper_Left, i);
    delay(5);
  }
}

void servoCloseLeft() {
  servoWrite(chanal_Gripper_Left, Close_Left);
}

void servoOpenRight() {
  for (int i = Close_Right; i > Open_Right; i--) {
    servoWrite(chanal_Gripper_Right, i);
    delay(5);
  }
}

void servoCloseRight() {
  servoWrite(chanal_Gripper_Right, Close_Right);
}

void switchGripperLoop(void *pvParameters) {
  while (1) {
    if (switch_flag == -1) {
      servoWrite(chanal_Gripper_Left, Open_Left);
      servoWrite(chanal_Swing_Right, Swing_Right + 45);
      delay(200);
      servoWrite(chanal_Swing_Left, Swing_Left - 45);
      servoWrite(chanal_Swing_Right, Swing_Right - 45);
      delay(200);
      servoCloseLeft();
      delay(150);
      servoWrite(chanal_Gripper_Right, Open_Right);
      delay(150);
      swing_current_degree[0] = Swing_Left + switch_degree;
      swing_current_degree[1] = Swing_Right + 50;
      servoWrite(chanal_Swing_Left, swing_current_degree[0]);
      servoWrite(chanal_Swing_Right, swing_current_degree[1]);
      switch_flag = 0;
    } else if (switch_flag == 1) {
      servoWrite(chanal_Gripper_Right, Open_Right);
      servoWrite(chanal_Swing_Left, Swing_Left + 45);
      delay(200);
      servoWrite(chanal_Swing_Left, Swing_Left - 45);
      servoWrite(chanal_Swing_Right, Swing_Right - 45);
      delay(200);
      servoCloseRight();
      delay(150);
      servoWrite(chanal_Gripper_Left, Open_Left);
      delay(150);
      swing_current_degree[0] = Swing_Left + 50;
      swing_current_degree[1] = Swing_Right + switch_degree;
      servoWrite(chanal_Swing_Left, swing_current_degree[0]);
      servoWrite(chanal_Swing_Right, swing_current_degree[1]);
      switch_flag = 0;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void switchGripper(char from_gripper, char to_gripper) {
  if (from_gripper == 'l' && to_gripper == 'r') switch_flag = 1;
  else if (from_gripper == 'r' && to_gripper == 'l') switch_flag = -1;
}

void swingGripper(char gripper, int degree) {
  if (gripper == 'l') {
    swing_current_degree[0] = degree;
    servoWrite(chanal_Swing_Left, swing_current_degree[0]);
  } else if (gripper == 'r') {
    swing_current_degree[1] = degree;
    servoWrite(chanal_Swing_Right, swing_current_degree[1]);
  }
}

void liftLoop(void *pvParameters) {
  const int CLK = 39;
  const int DT = 36;
  float error, previous_error = 0, integral = 0, derivative;
  float p_value, i_value, d_value, motor_speed;
  float Kp = 3.0, Ki = 0.001, Kd = 0.05;

  ESP32Encoder encoder;
  encoder.attachHalfQuad(DT, CLK);
  encoder.setCount(0);

  long last_encoder_count = 0;
  unsigned long last_change_time = millis();
  const unsigned long timeout_ms = 500;

  int last_lift_target = lift_target;
  bool stopped_due_to_timeout = false;

  while (1) {
    long current_count = encoder.getCount() / 2;
    error = current_count - lift_target;

    // ตรวจจับการเปลี่ยนแปลงของ encoder
    if (abs(abs(current_count) - abs(last_encoder_count)) > 5) {
      last_change_time = millis();
      last_encoder_count = current_count;
    }

    // ตรวจสอบว่ามีการเปลี่ยนแปลง lift_target หรือไม่
    if (lift_target != last_lift_target) {
      // หากเคยหยุดจาก timeout ให้เริ่มใหม่
      stopped_due_to_timeout = false;
      last_change_time = millis();
      integral = 0;
      previous_error = 0;
      last_lift_target = lift_target;
    }

    // หยุดหาก encoder ไม่ขยับเป็นระยะเวลาหนึ่ง
    if (!stopped_due_to_timeout && millis() - last_change_time > timeout_ms) {
      motorStop();
      stopped_due_to_timeout = true;
    }

    // หากอยู่ในสถานะหยุดจาก timeout ให้รอจน lift_target เปลี่ยนก่อนจะกลับมาทำงาน
    if (stopped_due_to_timeout) {
      vTaskDelay(10 / portTICK_PERIOD_MS);
      continue;
    }

    // PID control
    p_value = Kp * error;
    integral += error;
    i_value = Ki * integral;
    derivative = error - previous_error;
    d_value = Kd * derivative;
    motor_speed = p_value + i_value + d_value;

    motor_speed = constrain(motor_speed, -255, 40);

    if (abs(error) >= 5)
      motorWrite(3, motor_speed);
    else
      motorStop();

    previous_error = error;
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void lift(int position) {
  if (position > 17) position = 17;
  lift_target = position * 30;
  if (lift_target > 900) lift_target = 900;
}

void moveToCan(int distance) {
  vTaskResume(sonarTaskHandle);
  delay(10);
  unsigned long int timer = millis();
  while (readSonar() > distance || millis() - timer <= 500) FF(15);
  FF(25, 120);
  motorStop();
  vTaskSuspend(sonarTaskHandle);
}

void moveToCan0(int distance) {
  unsigned long int timer = millis();
  while (readCanDistance() > distance || millis() - timer <= 500) FF(15);
  FF(25, 120);
  motorStop();
}
void moveToCanTime(int time) {
  unsigned long int timer = millis();
  while (millis() - timer < time) FF(15);
  motorStop();
}

void shakeGripper(char gripper) {
  int increment = 1;
  int delayTime = 5;
  int swingRange = 20;
  for (int i = 0; i < 3; i++) {
    if (gripper == 'l' || gripper == 'a') {
      for (int i = 0; i <= swingRange; i += increment) {
        servoWrite(chanal_Swing_Left, swing_current_degree[0] - i);
        if (gripper == 'a') servoWrite(chanal_Swing_Right, swing_current_degree[1] - i);
        delay(delayTime);
      }
      for (int i = 0; i <= swingRange * 2; i += increment) {
        servoWrite(chanal_Swing_Left, swing_current_degree[0] - swingRange + i);
        if (gripper == 'a') servoWrite(chanal_Swing_Right, swing_current_degree[1] - swingRange + i);
        delay(delayTime);
      }
      for (int i = 0; i <= swingRange; i += increment) {
        servoWrite(chanal_Swing_Left, swing_current_degree[0] + swingRange - i);
        if (gripper == 'a') servoWrite(chanal_Swing_Right, swing_current_degree[1] + swingRange - i);
        delay(delayTime);
      }
    }
    if (gripper == 'r' || gripper == 'a') {
      for (int i = 0; i <= swingRange; i += increment) {
        servoWrite(chanal_Swing_Right, swing_current_degree[1] - i);
        if (gripper == 'a') servoWrite(chanal_Swing_Left, swing_current_degree[0] - i);
        delay(delayTime);
      }
      for (int i = 0; i <= swingRange * 2; i += increment) {
        servoWrite(chanal_Swing_Right, swing_current_degree[1] - swingRange + i);
        if (gripper == 'a') servoWrite(chanal_Swing_Left, swing_current_degree[0] - swingRange + i);
        delay(delayTime);
      }
      for (int i = 0; i <= swingRange; i += increment) {
        servoWrite(chanal_Swing_Right, swing_current_degree[1] + swingRange - i);
        if (gripper == 'a') servoWrite(chanal_Swing_Left, swing_current_degree[0] + swingRange - i);
        delay(delayTime);
      }
    }
  }
}

void dropCan(char gripper) {
  if (gripper == 'l') {
    servoWrite(chanal_Gripper_Left, Close_Left - 10);
    shakeGripper('l');
    delay(200);
    servoOpenLeft();
  } else if (gripper == 'r') {
    servoWrite(chanal_Gripper_Right, Close_Right - 10);
    shakeGripper('r');
    delay(200);
    servoOpenRight();
  } else if (gripper == 'a') {
    servoWrite(chanal_Gripper_Left, Close_Left - 10);
    servoWrite(chanal_Gripper_Right, Close_Right - 10);
    shakeGripper('a');
    delay(200);
    servoOpenLeft();
    servoOpenRight();
  }
}

void pickCan(int pick_height, bool switch_lr) {
  servoWrite(chanal_Gripper_Left, Open_Left);
  servoWrite(chanal_Gripper_Right, Open_Right);
  lift(pick_height);
  if (pick_height != 0) moveToCan(4);
  else moveToCan0(0);
  delay(100);
  servoCloseLeft();
  servoCloseRight();
  delay(200);
  lift(pick_height + 2);
  delay(200);
  if (switch_lr == 1) {
    vTaskResume(switchGripperTaskHandle);
    delay(10);
    swingGripper('l', Swing_Left - (switch_degree + 13));
    delay(200);
    servoWrite(chanal_Gripper_Left, Open_Left);
    delay(200);
    swingGripper('l', Swing_Left - switch_degree);
    delay(300);
    BB(30, 400);
    switchGripper('r', 'l');
    delay(1200);
    servoWrite(chanal_Gripper_Right, Open_Right);
    swingGripper('r', Swing_Right - (switch_degree + 10));
    lift(pick_height + 4);
    moveToCan(4);
    lift(pick_height + 2);
    delay(400);
    servoCloseRight();
    delay(200);
    vTaskSuspend(switchGripperTaskHandle);
  }
  BB(30, 400);
  lift(3);
  servoWrite(chanal_Swing_Left, Swing_Left);
  servoWrite(chanal_Swing_Right, Swing_Right);
}

void autoPickCan(int pick_height) {
  servoWrite(chanal_Gripper_Left, Open_Left);
  servoWrite(chanal_Gripper_Right, Open_Right);
  lift(pick_height);
  if (pick_height != 0) moveToCan(4);
  else moveToCan0(0);
  delay(100);
  servoCloseLeft();
  servoCloseRight();
  delay(200);
  lift(pick_height + 2);
  delay(300);
  can_in_gripper[0] = readColor('l');
  can_in_gripper[1] = readColor('r');
  if (can_in_gripper[0] == can_in_gripper[1] && pick_height > 0) {
    if (can_in_gripper[0] == 'r') {
      swingGripper('l', Swing_Left + switch_degree + 10);
      delay(200);
      servoWrite(chanal_Gripper_Left, Open_Left);
      delay(200);
      swingGripper('l', Swing_Left + switch_degree);
      delay(300);
      can_in_gripper[0] = 'u';
      can_in_gripper[1] = 'r';
    } else {
      swingGripper('r', Swing_Right + switch_degree + 10);
      delay(200);
      servoWrite(chanal_Gripper_Right, Open_Right);
      delay(200);
      swingGripper('r', Swing_Right + switch_degree);
      delay(300);
      if (can_in_gripper[0] == 'g') can_in_gripper[0] = 'l';
      else if (can_in_gripper[0] == 'y') can_in_gripper[0] = 'c';
      can_in_gripper[1] = 'u';
    }
  } else {
    if (can_in_gripper[0] == 'r' && can_in_gripper[1] == 'g') {
      vTaskResume(switchGripperTaskHandle);
      delay(10);
      swingGripper('l', Swing_Left - (switch_degree + 13));
      delay(200);
      servoWrite(chanal_Gripper_Left, Open_Left);
      delay(200);
      swingGripper('l', Swing_Left - (switch_degree));
      delay(200);
      BB(30, 400);
      switchGripper('r', 'l');
      delay(1200);
      servoWrite(chanal_Gripper_Right, Open_Right);
      swingGripper('r', Swing_Right - (switch_degree + 10));
      lift(pick_height + 4);
      moveToCan(4);
      lift(pick_height + 2);
      delay(400);
      servoCloseRight();
      delay(200);
      vTaskSuspend(switchGripperTaskHandle);
      can_in_gripper[0] = 'l';
      can_in_gripper[1] = 'r';
    } else {
      if (can_in_gripper[0] == 'r') can_in_gripper[0] = 'r';
      else if (can_in_gripper[0] == 'g') can_in_gripper[0] = 'l';
      else if (can_in_gripper[0] == 'y') can_in_gripper[0] = 'c';
      if (can_in_gripper[1] == 'r') can_in_gripper[1] = 'r';
      else if (can_in_gripper[1] == 'g') can_in_gripper[1] = 'l';
      else if (can_in_gripper[1] == 'y') can_in_gripper[1] = 'c';
    }
  }
  BB(30, 400);
  lift(3);
  servoWrite(chanal_Swing_Left, Swing_Left);
  servoWrite(chanal_Swing_Right, Swing_Right);
}

void autoPickCanTime(int time) {
  servoWrite(chanal_Gripper_Left, Open_Left);
  servoWrite(chanal_Gripper_Right, Open_Right);
  lift(0);
  moveToCanTime(time);
  delay(100);
  servoCloseLeft();
  servoCloseRight();
  delay(200);
  lift(2);
  delay(300);
  can_in_gripper[0] = readColor('l');
  can_in_gripper[1] = readColor('r');
  if (can_in_gripper[0] == can_in_gripper[1]) {
    if (can_in_gripper[0] == 'r') {
      servoWrite(chanal_Gripper_Left, Open_Left);
      delay(200);
      can_in_gripper[0] = 'u';
      can_in_gripper[1] = 'r';
    } else {
      servoWrite(chanal_Gripper_Right, Open_Right);
      delay(200);
      if (can_in_gripper[0] == 'g') can_in_gripper[0] = 'l';
      else if (can_in_gripper[0] == 'y') can_in_gripper[0] = 'c';
      can_in_gripper[1] = 'u';
    }
  } else {
    if (can_in_gripper[0] == 'r') can_in_gripper[0] = 'r';
    else if (can_in_gripper[0] == 'g') can_in_gripper[0] = 'l';
    else if (can_in_gripper[0] == 'y') can_in_gripper[0] = 'c';
    if (can_in_gripper[1] == 'r') can_in_gripper[1] = 'r';
    else if (can_in_gripper[1] == 'g') can_in_gripper[1] = 'l';
    else if (can_in_gripper[1] == 'y') can_in_gripper[1] = 'c';
  }

  lift(3);
  delay(300);
  BB(30, 400);
  servoWrite(chanal_Swing_Left, Swing_Left);
  servoWrite(chanal_Swing_Right, Swing_Right);
}

void placeCan1(int drop_height, int distance, char from_gripper, char drop_point) {
  bool switch_can = 0;
  servoWrite(chanal_Gripper_Left, Close_Left);
  servoWrite(chanal_Gripper_Right, Close_Right);
  lift(drop_height + 3);
  if (from_gripper == 'l' && drop_point == 'l') {
    swingGripper('l', Swing_Left + switch_degree - 10);
    swingGripper('r', Swing_Right + 90);
  } else if (from_gripper == 'l' && drop_point == 'c') {
    swingGripper('l', Swing_Left - (switch_degree + 10));
    swingGripper('r', Swing_Right + 90);
  } else if (from_gripper == 'r' && drop_point == 'r') {
    swingGripper('l', Swing_Left + 90);
    swingGripper('r', Swing_Right + switch_degree - 10);
  } else if (from_gripper == 'r' && drop_point == 'c') {
    swingGripper('l', Swing_Left + 90);
    swingGripper('r', Swing_Right - (switch_degree + 10));
  } else {
    vTaskResume(switchGripperTaskHandle);
    delay(10);
    switchGripper(from_gripper, drop_point);
    switch_can = 1;
  }
  moveToCan(distance);
  lift(drop_height);
  delay(200);
  if (switch_can == 0) dropCan(from_gripper);
  else dropCan(drop_point);
  delay(200);
  BB(30, 400);
  lift(3);
  servoWrite(chanal_Swing_Left, Swing_Left);
  servoWrite(chanal_Swing_Right, Swing_Right);
  vTaskSuspend(switchGripperTaskHandle);
}

void placeCan2(int drop_height, int distance, char left_gripper, char right_gripper) {
  servoWrite(chanal_Gripper_Left, Close_Left);
  servoWrite(chanal_Gripper_Right, Close_Right);
  lift(drop_height + 3);
  if (left_gripper == 'r' && right_gripper == 'l') {
    moveToCan(distance + 5);
    vTaskResume(switchGripperTaskHandle);
    delay(10);
    swingGripper('l', Swing_Left - (switch_degree + 13));
    delay(200);
    servoWrite(chanal_Gripper_Left, Open_Left);
    delay(200);
    swingGripper('l', Swing_Left - switch_degree);
    delay(300);
    BB(30, 400);
    switchGripper('r', 'l');
    delay(1200);
    servoWrite(chanal_Gripper_Right, Open_Right);
    swingGripper('r', Swing_Right - (switch_degree + 10));
    lift(drop_height + 3);
    moveToCan(distance + 5);
    servoCloseRight();
    delay(200);
    servoWrite(chanal_Swing_Left, Swing_Left);
    servoWrite(chanal_Swing_Right, Swing_Right);
    vTaskSuspend(switchGripperTaskHandle);
    BB(30, 500);
    placeCan1(drop_height, distance, 'l', 'l');
    BB(30, 100);
    placeCan1(drop_height, distance, 'r', 'r');
  } else {
    if (left_gripper == 'l' && right_gripper == 'r') {
      placeCan1(drop_height, distance, 'l', 'l');
      BB(30, 100);
      placeCan1(drop_height, distance, 'r', 'r');
    } else if (left_gripper == 'l' && right_gripper == 'c') {
      placeCan1(drop_height, distance, 'r', 'c');
      BB(30, 100);
      placeCan1(drop_height, distance, 'l', 'l');
    } else if (left_gripper == 'c' && right_gripper == 'r') {
      placeCan1(drop_height, distance, 'l', 'c');
      BB(30, 100);
      placeCan1(drop_height, distance, 'r', 'r');
    } else {
      vTaskResume(switchGripperTaskHandle);
      delay(10);
      if (left_gripper == 'c') {
        swingGripper('l', Swing_Left - (switch_degree + 10));
        swingGripper('r', Swing_Right + 90);
        moveToCan(distance);
        lift(drop_height);
        delay(200);
        dropCan('l');
        delay(200);
        BB(30, 400);
        lift(drop_height + 3);
        switchGripper('r', 'l');
        delay(1000);
        servoWrite(chanal_Gripper_Left, Close_Left);
        servoWrite(chanal_Gripper_Right, Close_Right);
        swingGripper('r', Swing_Right + 90);
        moveToCan(distance);
        lift(drop_height);
        delay(200);
        dropCan('l');
      } else if (right_gripper == 'c') {
        swingGripper('r', Swing_Right - (switch_degree + 10));
        swingGripper('l', Swing_Left + 90);
        moveToCan(distance);
        lift(drop_height);
        delay(200);
        dropCan('r');
        delay(200);
        BB(30, 400);
        lift(drop_height + 3);
        switchGripper('l', 'r');
        delay(1000);
        servoWrite(chanal_Gripper_Left, Close_Left);
        servoWrite(chanal_Gripper_Right, Close_Right);
        swingGripper('l', Swing_Left + 90);
        moveToCan(distance);
        lift(drop_height);
        delay(200);
        dropCan('r');
      }
    }
  }
  delay(200);
  BB(30, 400);
  lift(3);
  servoWrite(chanal_Swing_Left, Swing_Left);
  servoWrite(chanal_Swing_Right, Swing_Right);
  vTaskSuspend(switchGripperTaskHandle);
}

void autoPlaceCan2(int drop_height, char left_gripper, char right_gripper) {
  servoWrite(chanal_Gripper_Left, Close_Left);
  servoWrite(chanal_Gripper_Right, Close_Right);
  lift(drop_height + 3);
  if (left_gripper == 'r' && right_gripper == 'l') {
    moveToCan(place_left_center + 5);
    vTaskResume(switchGripperTaskHandle);
    delay(10);
    swingGripper('l', Swing_Left - (switch_degree + 13));
    delay(200);
    servoWrite(chanal_Gripper_Left, Open_Left);
    delay(200);
    swingGripper('l', Swing_Left - switch_degree);
    delay(300);
    BB(30, 400);
    switchGripper('r', 'l');
    delay(1200);
    servoWrite(chanal_Gripper_Right, Open_Right);
    swingGripper('r', Swing_Right - (switch_degree + 10));
    lift(drop_height + 3);
    moveToCan(place_left_center + 5);
    servoCloseRight();
    delay(200);
    servoWrite(chanal_Swing_Left, Swing_Left);
    servoWrite(chanal_Swing_Right, Swing_Right);
    vTaskSuspend(switchGripperTaskHandle);
    BB(30, 500);
    placeCan1(drop_height, place_left_side, 'l', 'l');
    BB(30, 100);
    placeCan1(drop_height, place_right_side, 'r', 'r');
  } else {
    if (left_gripper == 'l' && right_gripper == 'r') {
      placeCan1(drop_height, place_right_side, 'l', 'l');
      BB(30, 100);
      placeCan1(drop_height, place_right_side, 'r', 'r');
    } else if (left_gripper == 'l' && right_gripper == 'c') {
      placeCan1(drop_height, place_right_center, 'r', 'c');
      BB(30, 100);
      placeCan1(drop_height, place_left_side, 'l', 'l');
    } else if (left_gripper == 'c' && right_gripper == 'r') {
      placeCan1(drop_height, place_left_center, 'l', 'c');
      BB(30, 100);
      placeCan1(drop_height, place_right_side, 'r', 'r');
    } else {
      vTaskResume(switchGripperTaskHandle);
      delay(10);
      if (left_gripper == 'c') {
        swingGripper('l', Swing_Left - (switch_degree + 10));
        swingGripper('r', Swing_Right + 90);
        moveToCan(place_left_center);
        lift(drop_height);
        delay(200);
        dropCan('l');
        delay(200);
        BB(30, 400);
        lift(drop_height + 3);
        switchGripper('r', 'l');
        delay(1000);
        servoWrite(chanal_Gripper_Left, Close_Left);
        servoWrite(chanal_Gripper_Right, Close_Right);
        swingGripper('r', Swing_Right + 90);
        moveToCan(place_left_side);
        lift(drop_height);
        delay(200);
        dropCan('l');
      } else if (right_gripper == 'c') {
        swingGripper('r', Swing_Right - (switch_degree + 10));
        swingGripper('l', Swing_Left + 90);
        moveToCan(place_right_center);
        lift(drop_height);
        delay(200);
        dropCan('r');
        delay(200);
        BB(30, 400);
        lift(drop_height + 3);
        switchGripper('l', 'r');
        delay(1000);
        servoWrite(chanal_Gripper_Left, Close_Left);
        servoWrite(chanal_Gripper_Right, Close_Right);
        swingGripper('l', Swing_Left + 90);
        moveToCan(place_right_side);
        lift(drop_height);
        delay(200);
        dropCan('r');
      }
    }
  }
  delay(200);
  BB(30, 400);
  lift(3);
  servoWrite(chanal_Swing_Left, Swing_Left);
  servoWrite(chanal_Swing_Right, Swing_Right);
  vTaskSuspend(switchGripperTaskHandle);
}

void autoPlaceCan(int drop_height) {
  servoWrite(chanal_Gripper_Left, Close_Left);
  servoWrite(chanal_Gripper_Right, Close_Right);
  lift(drop_height + 3);
  if (can_in_gripper[0] != 'u' && can_in_gripper[1] != 'u') autoPlaceCan2(drop_height, can_in_gripper[0], can_in_gripper[1]);
  else {
    if (can_in_gripper[0] != 'u') {
      if (can_in_gripper[0] == 'l') placeCan1(drop_height, place_left_side, 'l', can_in_gripper[0]);
      else if (can_in_gripper[0] == 'c') placeCan1(drop_height, place_left_center, 'l', can_in_gripper[0]);
    }
    else {
      if (can_in_gripper[1] == 'r') placeCan1(drop_height, place_right_side, 'r', can_in_gripper[1]);
      else if (can_in_gripper[1] == 'c') placeCan1(drop_height, place_right_center, 'r', can_in_gripper[1]);
    }
  }
}