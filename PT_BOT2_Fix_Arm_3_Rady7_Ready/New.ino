void motorTimeDegree_Enhanced(int acc_speed, int speed, float timer)
{
  motorTimeDegree(acc_speed, 125);
  motorTimeDegree(speed, timer);
  motorTimeDegree(acc_speed, 125);
}
void motorTimeDegree_Enhanced(int acc_speed, int speed, float timer, char select)
{
  motorTimeDegree(acc_speed, 125);
  motorTimeDegree(speed, timer);
  motorTimeDegree(acc_speed, 125);

  delay(delayALL);
  if (select == 'L') {
    spinDegree(-90);
  } else if (select == 'R') {
    spinDegree(90);
  } else if (select == 'l') {
    turnDegreeFront(-90);
  } else if (select == 'r') {
    turnDegreeFront(90);
  }
  delay(delayALL);

  motorStop(); delay(50);
}
void BackmotorTimeDegree_Enhanced(int acc_speed, int speed, float timer)
{
  motorTimeDegree(-acc_speed, 125);
  motorTimeDegree(-speed, timer);
  motorTimeDegree(-acc_speed, 125);
  motorStop();delay(50);
}
void BackmotorTimeDegree_Enhanced(int acc_speed, int speed, float timer, char select)
{
  motorTimeDegree(-acc_speed, 125);
  motorTimeDegree(-speed, timer);
  motorTimeDegree(-acc_speed, 125);

  motorStop(); delay(50);
  delay(delayALL);
  if (select == 'L') {
    spinDegree(-90);
  } else if (select == 'R') {
    spinDegree(90);
  } else if (select == 'l') {
    turnDegreeFront(-90);
  } else if (select == 'r') {
    turnDegreeFront(90);
  }
  delay(delayALL);

  motorStop(); delay(50);
}
////////////////////////////////////

void SetFront_Wall(int speed)
{
  lift(2);
  servoWrite(chanal_Swing_Right, 180);
  servoWrite(chanal_Swing_Left, 180);
  servoCloseRight();
  servoCloseLeft();
  SetFront(speed);
  servoWrite(chanal_Swing_Right, Swing_Right);
  servoWrite(chanal_Swing_Left, Swing_Left);
}
void SetFront_Wall(int speed, float timer)
{
  lift(2);
  servoWrite(chanal_Swing_Right, 180);
  servoWrite(chanal_Swing_Left, 180);
  servoCloseRight();
  servoCloseLeft();
  SetFront(speed, timer);
  servoWrite(chanal_Swing_Right, Swing_Right);
  servoWrite(chanal_Swing_Left, Swing_Left);
}
void SetFront_Wall(int speed, float timer, char select)
{
  lift(2);
  servoWrite(chanal_Swing_Right, 179);
  servoWrite(chanal_Swing_Left, 179);
  servoCloseRight();
  servoCloseLeft();
  SetFront(speed, timer, select);
  servoWrite(chanal_Swing_Right, Swing_Right);
  servoWrite(chanal_Swing_Left, Swing_Left);
}

void Balance_Arm(char select){
  if (select == 'C') {
  servoWrite(chanal_Gripper_Left, Close_Left);
  servoWrite(chanal_Gripper_Right, Close_Right);
  servoWrite(chanal_Swing_Left, 40);
  servoWrite(chanal_Swing_Right, 40);
  } else if (select == 'O') {
  servoWrite(chanal_Swing_Left, Swing_Left);
  servoWrite(chanal_Swing_Right, Swing_Right);
  servoWrite(chanal_Gripper_Left, Open_Left);
  servoWrite(chanal_Gripper_Right, Open_Right);
  }
}

/////////////////////////////////////

void SetFront_Test(int speed) {
  previous_error_forward = 0;
  while (1) {
    readLine();
    if (line_value[0] <= 50 && line_value[1] <= 50) {
      motorStop();
      break;
    }
    else {
      if (line_value[0] <= 50) motorWrite(1, -20);
      else motorWrite(1, speed);
      if (line_value[1] <= 50) motorWrite(2, -20);
      else motorWrite(2, speed);
    }
  }
  delay(200);
  current_degree = angleRead(YAW);
}

void SetBack_Test(int speed) {
  previous_error_forward = 0;
  while (1) {
    readLine();
    if (line_value[5] <= 50 && line_value[2] <= 50) {
      motorStop();
      break;
    }
    else {
      if (line_value[2] <= 50) motorWrite(2, 20);
      else motorWrite(2, -speed);
      if (line_value[5] <= 50) motorWrite(1, 20);
      else motorWrite(1, -speed);
    }
  }
  delay(200);
  current_degree = angleRead(YAW);
}

/////////////////////////////////////////

void SetFront_N(int speed) {
  previous_error_forward = 0;
  while (1) {
    readLine();
    if (line_value[0] <= 2 && line_value[1] <= 2) {
      motorStop();
      break;
    }
    else {
      if (line_value[0] <= 2) motorWrite(1, -15);
      else motorWrite(1, speed);
      if (line_value[1] <= 2) motorWrite(2, -15);
      else motorWrite(2, speed);
    }
  }
  delay(100);
  current_degree = angleRead(YAW) + 2;
}


void SetBack_N(int speed) {
  previous_error_forward = 0;
  while (1) {
    readLine();
    if (line_value[5] <= 2 && line_value[2] <= 2) {
      motorStop();
      break;
    }
    else {
      if (line_value[2] <= 2) motorWrite(2, 15);
      else motorWrite(2, -speed);
      if (line_value[5] <= 2) motorWrite(1, 15);
      else motorWrite(1, -speed);
    }
  }
  delay(100);
  current_degree = angleRead(YAW);
}

////////////////////////////////////////////

void motorTimeDegree_Enhanced_Test(int acc_speed, int speed, float timer)
{
  motorTimeDegree(acc_speed, 150);
  motorTimeDegree(speed, timer);
  motorTimeDegree(acc_speed, 150);
}
void motorTimeDegree_Enhanced_Test(int acc_speed, int speed, float timer, char select)
{
  motorTimeDegree(acc_speed, 150);
  motorTimeDegree(speed, timer);
  motorTimeDegree(acc_speed, 150);

  delay(delayALL);
  if (select == 'L') {
    spinDegree(-90);
  } else if (select == 'R') {
    spinDegree(90);
  } else if (select == 'l') {
    turnDegreeFront(-90);
  } else if (select == 'r') {
    turnDegreeFront(90);
  }
  delay(delayALL);

  motorStop(); delay(50);
}

///////////////////////

void pickBox(int pick_height, bool switch_lr, char select) {
  Balance_Arm('O');
  lift(pick_height);
  if (pick_height != 0) moveToCan(4);
  else moveToCan0(18);
  delay(750);
  servoCloseLeft();
  servoCloseRight();
  delay(200);
  lift(pick_height + 2);
  delay(200);
  if (switch_lr == 1) {
    vTaskResume(switchGripperTaskHandle);
    BB(30, 500);
    delay(10);
    lift(0);
    delay(500);
    vTaskResume(switchGripperTaskHandle);
    servoCloseRight();
    servoCloseLeft();
    servoWrite(chanal_Swing_Left, 36);
    delay(500);
    servoWrite(chanal_Gripper_Left, Open_Left);
    delay(500);
    lift(pick_height + 18);
    delay(950);
    Servo_LRfast('R');
    servoWrite(chanal_Swing_Right, 40);
    servoWrite(chanal_Swing_Left, 110);
    servoWrite(chanal_Gripper_Right, Open_Right - 10);
    delay(250);
    lift(0);
    delay(750);
    servoCloseLeft();
    delay(1000);
    FF(10, 100);
    motorStop();
    delay(100);
    servoCloseRight();
    servoWrite(chanal_Gripper_Left, Open_Left);
    servoCloseLeft();
    delay(150);
    lift(1);
    delay(250);
    servoWrite(chanal_Swing_Right, Swing_Right);
    servoWrite(chanal_Swing_Left, Swing_Left);
    motorStop();
    delay(50);
  } else if (switch_lr == 0) {
    BB(30, 500);
  }
  lift(2);delay(200);
  BackmotorTimeDegree_Enhanced(30, 40, 65);
  servoWrite(chanal_Swing_Left, Swing_Left);
  servoWrite(chanal_Swing_Right, Swing_Right);
  delay(delayALL);
  if (select == 'L') {
    spinDegree(-90);
  } else if (select == 'R') {
    spinDegree(90);
  } else if (select == 'l') {
    turnDegreeFront(-90);
  } else if (select == 'r') {
    turnDegreeFront(90);
  }
  delay(delayALL);
  motorStop();delay(150);
}

void pickCan_up(int pick_height, bool switch_lr) {
  Balance_Arm('O');
  lift(pick_height);
  moveToCan(5); // จับด้านบน
  delay(500);
  servoCloseLeft();
  servoCloseRight();
  delay(200);
  lift(pick_height + 2);
  delay(200);
  if (switch_lr == 1) {
    vTaskResume(switchGripperTaskHandle);
    BB(30, 500);
    delay(10);
    lift(0);
    delay(500);
    vTaskResume(switchGripperTaskHandle);
    servoCloseRight();
    servoCloseLeft();
    servoWrite(chanal_Swing_Left, 36);
    delay(500);
    servoWrite(chanal_Gripper_Left, Open_Left);
    delay(500);
    lift(pick_height + 18);
    delay(950);
    Servo_LRfast('R');
    servoWrite(chanal_Swing_Right, 40);
    servoWrite(chanal_Swing_Left, 110);
    servoWrite(chanal_Gripper_Right, Open_Right - 10);
    delay(250);
    lift(0);
    delay(750);
    servoCloseLeft();
    delay(1000);
    FF(10, 100);
    motorStop();
    delay(100);
    servoCloseRight();
    servoWrite(chanal_Gripper_Left, Open_Left);
    servoCloseLeft();
    delay(150);
    lift(1);
    delay(250);
    servoWrite(chanal_Swing_Right, Swing_Right);
    servoWrite(chanal_Swing_Left, Swing_Left);
    motorStop();
    delay(50);
  } else if (switch_lr == 0) {
    BB(30, 500);
  }
  lift(2);
  servoWrite(chanal_Swing_Left, Swing_Left);
  servoWrite(chanal_Swing_Right, Swing_Right);
  motorStop();delay(150);
}
void pickCan_low(int pick_height, bool switch_lr) {
  Balance_Arm('O');
  lift(pick_height);
  moveToCan0(19); // จับด้านล่าง
  delay(500);
  servoCloseLeft();
  servoCloseRight();
  delay(200);
  lift(pick_height + 2);
  delay(500);
  if (switch_lr == 1) {
    vTaskResume(switchGripperTaskHandle);
    BB(30, 500);
    delay(10);
    lift(0);
    delay(500);
    vTaskResume(switchGripperTaskHandle);
    servoCloseRight();
    servoCloseLeft();
    servoWrite(chanal_Swing_Left, 36);
    delay(500);
    servoWrite(chanal_Gripper_Left, Open_Left);
    delay(500);
    lift(pick_height + 18);
    delay(950);
    Servo_LRfast('R');
    servoWrite(chanal_Swing_Right, 40);
    servoWrite(chanal_Swing_Left, 110);
    servoWrite(chanal_Gripper_Right, Open_Right - 10);
    delay(250);
    lift(0);
    delay(750);
    servoCloseLeft();
    delay(1000);
    FF(10, 100);
    motorStop();
    delay(100);
    servoCloseRight();
    servoWrite(chanal_Gripper_Left, Open_Left);
    servoCloseLeft();
    delay(150);
    lift(1);
    delay(250);
    servoWrite(chanal_Swing_Right, Swing_Right);
    servoWrite(chanal_Swing_Left, Swing_Left);
    motorStop();
    delay(50);
  } else if (switch_lr == 0) {
    BB(30, 500);
  }
  lift(2);
  servoWrite(chanal_Swing_Left, Swing_Left);
  servoWrite(chanal_Swing_Right, Swing_Right);
  motorStop();delay(150);
}

void placeBox(int dropheight, int distance, char fromgripper, char droppoint) {
  CloseARM();
  lift(dropheight + 3);
  delay(250);
  if (fromgripper == 'l' && droppoint == 'l') {
    swingGripper('l', Swing_Left + 23);
    swingGripper('r', Swing_Right + 90);
  } else if (fromgripper == 'l' && droppoint == 'c') {
    swingGripper('l', Swing_Left - (30 + 5));
    swingGripper('r', Swing_Right + 90);
  } else if (fromgripper == 'r' && droppoint == 'r') {
    swingGripper('l', Swing_Left + 90);
    swingGripper('r', Swing_Right + 23);
  } else if (fromgripper == 'r' && droppoint == 'c') {
    swingGripper('l', Swing_Left + 90);
    swingGripper('r', Swing_Right - (30 + 5));
  } else if (fromgripper == 'l' && droppoint == 's') {
    swingGripper('r', Swing_Right + 90);
  } else if (fromgripper == 'r' && droppoint == 's') {
    swingGripper('l', Swing_Left + 90);
  } else {
    vTaskResume(switchGripperTaskHandle);
    delay(20);
    switchGripper(fromgripper, droppoint);
  }
  moveToCan(distance);
  beep(100);
  delay(50);
  lift(dropheight);
  delay(300);
  dropBox(fromgripper);
  delay(250);
  BB(30, 400);
  lift(2);
  Swingarm();
  vTaskSuspend(switchGripperTaskHandle);
}
void dropBox(char gripper) {
  if (gripper == 'l') {
    servoWrite(chanal_Gripper_Left, Close_Left - 10);
    delay(400);
    servoOpenLeft();
  } else if (gripper == 'r') {
    servoWrite(chanal_Gripper_Right, Close_Right - 10);
    delay(400);
    servoOpenRight();
  } else if (gripper == 'a') {
    servoWrite(chanal_Gripper_Left, Close_Left - 10);
    servoWrite(chanal_Gripper_Right, Close_Right - 10);
    delay(400);
    servoOpenLeft();
    servoOpenRight();
  }
}
void placeBox2(int drop_height, int distance, char left_gripper, char right_gripper) {
  servoWrite(chanal_Gripper_Left, Close_Left);
  servoWrite(chanal_Gripper_Right, Close_Right);
  if (left_gripper == 'r' && right_gripper == 'l') {
    vTaskResume(switchGripperTaskHandle);
    delay(10);
    lift(0);
    delay(200);
    swingGripper('l', Swing_Left - (switch_degree + 13));
    delay(200);
    servoWrite(chanal_Gripper_Left, Open_Left);
    delay(200);
    lift(13);
    delay(1000);
    switchGripper('r', 'l');
    delay(1000);
    servoWrite(chanal_Gripper_Right, Open_Right);
    swingGripper('r', Swing_Right - (switch_degree + 10));
    lift(0);
    delay(1700);
    FF(20, 100);
    servoCloseRight();
    delay(200);
    servoWrite(chanal_Swing_Left, Swing_Left);
    servoWrite(chanal_Swing_Right, Swing_Right);
    lift(drop_height + 7);
    swingGripper('l', Swing_Left + switch_degree);
    swingGripper('r', Swing_Right + switch_degree);
    moveToCan(3);
    lift(drop_height + 4);
    delay(200);
    dropCan_NoShake('a');
  } else {
    delay(250);
    lift(drop_height + 3);
    if (left_gripper == 'l' && right_gripper == 'r') {
      swingGripper('l', Swing_Left + switch_degree);
      swingGripper('r', Swing_Right + switch_degree);
      moveToCan(distance);
      lift(drop_height);
      delay(200);
      dropCan_NoShake('a');
    } else if (left_gripper == 'l' && right_gripper == 'c') {
      swingGripper('l', Swing_Left + switch_degree);
      swingGripper('r', Swing_Right - (switch_degree + 10));
      moveToCan(distance);
      lift(drop_height);
      delay(200);
      dropCan_NoShake('a');
    } else if (left_gripper == 'c' && right_gripper == 'r') {
      swingGripper('l', Swing_Left - (switch_degree + 10));
      swingGripper('r', Swing_Right + switch_degree);
      moveToCan(distance);
      lift(drop_height);
      delay(200);
      dropCan_NoShake('a');
    } else if (left_gripper == 's' || right_gripper == 's') {
      moveToCan(distance + 1);
      lift(drop_height);
      delay(200);
      dropCan_NoShake('a');
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
        dropCan_NoShake('l');
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
        dropCan_NoShake('r');
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

//////////////////////////////////

void FF_Test(int speed, float timer) {
  int min_speed = 10;
  int max_speed = speed;
  // ปรับค่า PD ให้ต่ำลงเพื่อลดการแกว่ง
  float kp = 3.5;           // ลด KP จาก 6 → 1.0
  float kd = 0.8;           // ลด KD จาก 1.2 → 0.2
  ///
  float integral = 0;
  float ki = 0.1;         // เพิ่มค่า KI สำหรับเทคนิค PID แบบสมบูรณ์
  int ramp_up = 100;
  int ramp_down = 200;
  int current_speed = min_speed;
  unsigned long timer_forward = millis();
  unsigned long previous_time = millis();
  
  // เพิ่มตัวแปรกรองสัญญาณ
  float previous_error = 0;
  float filtered_error = 0;
  float alpha = 0.2;         // ค่า Filter (0.1-0.3)
  
  while (1) {
    unsigned long elapsed_time = millis() - timer_forward;
    unsigned long remaining_time = timer - elapsed_time;
    unsigned long current_time = millis();
    unsigned long dt = current_time - previous_time;
    previous_time = current_time;
    
    // คำนวณความเร็ว (เหมือนเดิม)
    if (elapsed_time <= ramp_up) {
      current_speed = min_speed + (float)elapsed_time / ramp_up * (max_speed - min_speed);
    } else if (remaining_time <= ramp_down) {
      current_speed = min_speed + (float)remaining_time / ramp_down * (max_speed - min_speed);
      if (current_speed < min_speed) current_speed = min_speed;
    } else {
      current_speed = max_speed;
    }
    
    // อ่านมุมและกรองสัญญาณรบกวน
    float raw_angle = angleRead(YAW);
    static float prev_filtered = 0;
    float filtered_angle = alpha * raw_angle + (1 - alpha) * prev_filtered;
    prev_filtered = filtered_angle;
    
    // คำนวณ error
    float error = current_degree - filtered_angle;
    if (error > 180) error -= 360;
    else if (error < -180) error += 360;
    
    // กรอง error (ลดการเปลี่ยนแปลงฉับพลัน)
    filtered_error = alpha * error + (1 - alpha) * filtered_error;
    
    // คำนวณอนุพันธ์แบบกรองแล้ว
    float derivative = filtered_error - previous_error;
    previous_error = filtered_error;
    
    //คำนวนค่าKi
    integral = integral + error * (dt / 1000.0); // คำนวณค่า Integral
    if (integral > max_speed / ki) integral = max_speed / ki; // จำกัดค่า Integral
    if (integral < -max_speed / ki) integral = -max_speed / ki; // จำกัดค่า Integral

    // คำนวณ PID + จำกัดค่าสูงสุด
    int pid_value = (filtered_error * kp) + (derivative * kd) + (integral * ki);
    pid_value = constrain(pid_value, -max_speed/3, max_speed/3); // จำกัดผล PID
    
    int speed_left = current_speed + pid_value;
    int speed_right = current_speed - pid_value;
    
    motorWrite(1, speed_left);
    motorWrite(2, speed_right);
    
    if (elapsed_time >= timer * power_factor) {
      motorStop();
      break;
    }
  }
}