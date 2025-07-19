// --- Global PID & Filter Constants for easy tuning ---
// การย้ายค่าคงที่มาไว้ข้างนอก ทำให้ปรับจูนได้สะดวก
float g_kp = 3.5;          // Proportional Gain
float g_ki = 0.05;         // Integral Gain (ควรเริ่มจากค่าน้อยมากๆ)
float g_kd = 1.5;          // Derivative Gain
float g_alpha = 0.3;       // Filter coefficient (0.0 to 1.0), ค่ามาก = ตอบสนองไวขึ้น, กรองน้อยลง

void FF_Test_Optimized(int speed, float timer) {
  // --- Function-level settings ---
  int min_speed = 10;
  int max_speed = speed;
  int ramp_up = 100;
  int ramp_down = 200;
  int max_correction = 70; // (ข้อเสนอแนะ 5) จำกัดค่าการแก้ไขสูงสุด (ปรับค่านี้ได้)
  int integral_active_zone = 5; // (ข้อเสนอแนะ 2) จะเริ่มคำนวณ I-term เมื่อ error น้อยกว่าค่านี้ (หน่วย: องศา)

  // --- PID & Filter Variables Initialization ---
  unsigned long timer_forward = millis();
  unsigned long previous_time = millis();

  // (ข้อเสนอแนะ 4) อ่านค่าเริ่มต้นและกำหนดค่าตัวกรองนอก Loop
  float initial_angle = angleRead(YAW);
  current_degree = initial_angle; // ตั้งค่าเป้าหมายเป็นองศาปัจจุบัน ณ จุดเริ่มต้น
  float filtered_angle = initial_angle;
  
  float previous_filtered_error = 0;
  float integral = 0;
  
  while (1) {
    unsigned long current_time = millis();
    unsigned long elapsed_time = current_time - timer_forward;

    // --- Loop Exit Condition ---
    if (elapsed_time >= timer) { // แก้ไขเงื่อนไขการหยุดให้ถูกต้อง
      // motorStop(); // ใช้ฟังก์ชันหยุดมอเตอร์ของคุณ
      motorWrite(1, 0);
      motorWrite(2, 0);
      break;
    }

    // --- Speed Ramping ---
    unsigned long remaining_time = timer - elapsed_time;
    int current_speed;
    if (elapsed_time <= ramp_up) {
      current_speed = map(elapsed_time, 0, ramp_up, min_speed, max_speed);
    } else if (remaining_time <= ramp_down) {
      current_speed = map(remaining_time, ramp_down, 0, max_speed, min_speed);
    } else {
      current_speed = max_speed;
    }
    
    // --- Sensor Reading and Filtering ---
    // (ข้อเสนอแนะ 3) กรองสัญญาณจากเซ็นเซอร์เพียงครั้งเดียว
    float raw_angle = angleRead(YAW);
    filtered_angle = (g_alpha * raw_angle) + ((1.0 - g_alpha) * filtered_angle);
    
    // --- PID Calculation ---
    // P (Proportional)
    float error = current_degree - filtered_angle;
    if (error > 180) error -= 360;
    else if (error < -180) error += 360;
    
    float p_term = g_kp * error;

    // I (Integral) with Anti-Windup
    // (ข้อเสนอแนะ 2) เพิ่มค่า I ต่อเมื่อ error อยู่ในโซนที่กำหนดเท่านั้น
    if (abs(error) < integral_active_zone) {
      integral += error * ( (current_time - previous_time) / 1000.0 );
    } else {
      integral = 0; // Reset ค่า Integral ถ้า error ใหญ่เกินไป
    }
    float i_term = g_ki * integral;

    // D (Derivative)
    // (ข้อเสนอแนะ 1) คำนวณ D โดยหารด้วย dt (เวลาที่ผ่านไปจริง)
    float dt = (current_time - previous_time) / 1000.0; // เวลาในหน่วยวินาที
    float derivative = (error - previous_filtered_error) / dt;
    float d_term = g_kd * derivative;

    // --- State Update for next loop ---
    previous_time = current_time;
    previous_filtered_error = error;
    
    // --- Final PID Output ---
    int pid_value = round(p_term + i_term + d_term);

    // Constrain the correction value
    pid_value = constrain(pid_value, -max_correction, max_correction);
    
    // --- Motor Control ---
    int speed_left = current_speed + pid_value;
    int speed_right = current_speed - pid_value;
    
    motorWrite(1, speed_left);
    motorWrite(2, speed_right);

    delay(10); // หน่วงเวลาเล็กน้อยเพื่อให้ระบบเสถียรและ dt มีค่าไม่น้อยเกินไป
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// --- Global PID & Filter Constants for easy tuning ---
// การย้ายค่าคงที่มาไว้ข้างนอก ทำให้ปรับจูนได้สะดวก
// สามารถใช้ค่าชุดเดียวกับ FF_Test_Optimized หรือปรับจูนใหม่สำหรับฟังก์ชันนี้โดยเฉพาะก็ได้
float g_bb_kp = 3.5;         // Proportional Gain
float g_bb_ki = 0.05;        // Integral Gain (ควรเริ่มจากค่าน้อยๆ)
float g_bb_kd = 1.0;         // Derivative Gain (ปรับค่าจากของเดิม)
float g_bb_alpha = 0.3;      // Filter coefficient (0.0 to 1.0), ค่ามาก = ตอบสนองไว, กรองน้อย

void BB_Optimized(int speed, float timer) {
  // --- Function-level settings ---
  int min_speed = 15;        // ความเร็วเริ่มต้น (จากโค้ดเดิม)
  int max_speed = speed;
  int ramp_up = 200;         // Ramp Up time (จากโค้ดเดิม)
  int ramp_down = 250;       // Ramp Down time (จากโค้ดเดิม)
  int max_correction = 70;   // จำกัดค่าการแก้ไขสูงสุด (ปรับค่านี้ได้)
  int integral_active_zone = 5; // จะเริ่มคำนวณ I-term เมื่อ error น้อยกว่าค่านี้

  // --- PID & Filter Variables Initialization ---
  unsigned long timer_forward = millis();
  unsigned long previous_time = millis();

  // อ่านค่าเริ่มต้นและตั้งเป็นเป้าหมาย (สำคัญมาก)
  float initial_angle = angleRead(YAW);
  float current_degree = initial_angle;
  float filtered_angle = initial_angle;

  float previous_filtered_error = 0;
  float integral = 0;

  while (1) {
    unsigned long current_time = millis();
    unsigned long elapsed_time = current_time - timer_forward;

    // --- เงื่อนไขการออกจาก Loop ---
    // ใช้ timer ตรงๆ และหยุดมอเตอร์ให้ถูกต้อง
    if (elapsed_time >= timer) { 
      motorStop(); // หรือ motorWrite(1, 0); motorWrite(2, 0);
      break;
    }

    // --- Speed Ramping (การไต่ระดับความเร็ว) ---
    int current_speed;
    if (elapsed_time <= ramp_up) {
      current_speed = map(elapsed_time, 0, ramp_up, min_speed, max_speed);
    } else {
      unsigned long remaining_time = (timer > elapsed_time) ? (timer - elapsed_time) : 0;
      if (remaining_time <= ramp_down) {
        current_speed = map(remaining_time, ramp_down, 0, max_speed, min_speed);
      } else {
        current_speed = max_speed;
      }
    }
    // ป้องกันความเร็วต่ำกว่า min_speed ในช่วง ramp down
    if (current_speed < min_speed) current_speed = min_speed;
    
    // --- Sensor Reading and Filtering ---
    // กรองสัญญาณจากเซ็นเซอร์เพื่อลด Noise ก่อนนำไปคำนวณ
    float raw_angle = angleRead(YAW);
    filtered_angle = (g_bb_alpha * raw_angle) + ((1.0 - g_bb_alpha) * filtered_angle);

    // --- PID Calculation ---
    // P (Proportional)
    float error = current_degree - filtered_angle;
    if (error > 180) error -= 360;
    else if (error < -180) error += 360;

    float p_term = g_bb_kp * error;

    // I (Integral) with Anti-Windup
    // เพิ่มค่า I ต่อเมื่อ error อยู่ในโซนที่กำหนดเท่านั้น เพื่อป้องกัน I-windup
    float dt = (current_time - previous_time) / 1000.0; // เวลาในหน่วยวินาที
    if (abs(error) < integral_active_zone) {
      integral += error * dt;
    } else {
      integral = 0; // Reset ค่า Integral ถ้า error ใหญ่เกินไป
    }
    float i_term = g_bb_ki * integral;

    // D (Derivative)
    // คำนวณ D โดยหารด้วย dt (เวลาที่ผ่านไปจริง) เพื่อความแม่นยำ
    float derivative = (error - previous_filtered_error) / dt;
    float d_term = g_bb_kd * derivative;

    // --- อัปเดตสถานะสำหรับ Loop ถัดไป ---
    previous_time = current_time;
    previous_filtered_error = error;

    // --- ผลรวม PID สุดท้าย ---
    int pid_value = round(p_term + i_term + d_term);

    // จำกัดค่าการแก้ไขสูงสุดเพื่อไม่ให้หุ่นยนต์กระตุก
    pid_value = constrain(pid_value, -max_correction, max_correction);

    // --- Motor Control ---
    // สังเกต: โค้ดเดิมมีการกลับเครื่องหมายที่ motorWrite (-speed_left, -speed_right)
    // โค้ดใหม่นี้จะคำนวณทิศทางให้ถูกต้องเลย ทำให้ควบคุมง่ายขึ้น
    // ถ้าหุ่นเลี้ยวผิดทาง ให้สลับ +pid_value และ -pid_value ที่สองบรรทัดล่าง
    int speed_left = current_speed + pid_value;
    int speed_right = current_speed - pid_value;
    
    // ใช้ motorWrite ตามรูปแบบของฟังก์ชันเป้าหมาย
    motorWrite(1, -speed_left);
    motorWrite(2, -speed_right);

    delay(10); // หน่วงเวลาเล็กน้อยเพื่อให้ระบบเสถียรและ dt มีค่าไม่น้อยเกินไป
  }
}

///////////////////////////////////////////////////////////////////

// --- Global PID & Filter Constants for easy tuning ---
// แนะนำให้ใช้ค่าคงที่ชุดใหม่ หรือปรับจูนสำหรับฟังก์ชันนี้โดยเฉพาะ
float g_mtd_kp = 3.5;       // Proportional Gain (ใช้ค่าเดิมเป็นจุดเริ่มต้น)
float g_mtd_ki = 0.05;      // Integral Gain (เพิ่มเข้ามาใหม่)
float g_mtd_kd = 1.2;       // Derivative Gain (อาจต้องปรับค่าใหม่เพื่อให้เหมาะกับ dt)
float g_mtd_alpha = 0.3;    // Filter coefficient

void motorTimeDegree_Optimized(int speed, float timer) {
  // --- Function-level settings ---
  int min_speed = 15;        // ความเร็วต่ำสุดช่วงเริ่มและจบ Ramping
  int target_speed = speed;  // ความเร็วเป้าหมาย
  int ramp_up = 200;         // เวลาที่ใช้ในการไต่ระดับความเร็ว (ms)
  int ramp_down = 250;       // เวลาที่ใช้ในการลดระดับความเร็ว (ms)
  int max_correction = 70;   // จำกัดค่าการแก้ไขสูงสุด (ปรับค่าได้)
  int integral_active_zone = 5; // โซนที่ I-term จะเริ่มทำงาน (องศา)

  // --- PID & Filter Variables Initialization ---
  unsigned long timer_forward = millis();
  unsigned long previous_time = millis();

  // อ่านค่าองศาเริ่มต้น และตั้งเป็นเป้าหมายทันที
  float initial_angle = angleRead(YAW);
  float current_degree = initial_angle;
  float filtered_angle = initial_angle;

  float previous_filtered_error = 0;
  float integral = 0;

  while (1) {
    unsigned long current_time = millis();
    unsigned long elapsed_time = current_time - timer_forward;

    // --- เงื่อนไขการออกจาก Loop ---
    if (elapsed_time >= timer) {
      motorStop();
      break;
    }

    // --- Speed Ramping (เพิ่มเข้ามาเพื่อความนุ่มนวล) ---
    int current_speed;
    if (elapsed_time <= ramp_up) {
      current_speed = map(elapsed_time, 0, ramp_up, min_speed, target_speed);
    } else {
      unsigned long remaining_time = (timer > elapsed_time) ? (timer - elapsed_time) : 0;
      if (remaining_time <= ramp_down) {
        current_speed = map(remaining_time, ramp_down, 0, target_speed, min_speed);
      } else {
        current_speed = target_speed;
      }
    }
    if (current_speed < 0) current_speed = 0; // ป้องกันความเร็วติดลบ

    // --- Sensor Reading and Filtering ---
    float raw_angle = angleRead(YAW);
    filtered_angle = (g_mtd_alpha * raw_angle) + ((1.0 - g_mtd_alpha) * filtered_angle);
    
    // --- PID Calculation ---
    // P (Proportional)
    float error = current_degree - filtered_angle;
    if (error > 180) error -= 360;
    else if (error < -180) error += 360;

    float p_term = g_mtd_kp * error;

    // I (Integral) with Anti-Windup
    float dt = (current_time - previous_time) / 1000.0;
    if (abs(error) < integral_active_zone) {
      integral += error * dt;
    } else {
      integral = 0; // Reset ถ้า error ใหญ่เกินไป
    }
    float i_term = g_mtd_ki * integral;

    // D (Derivative) - คำนวณเทียบกับเวลาจริง (dt)
    float derivative = (dt > 0) ? ((error - previous_filtered_error) / dt) : 0;
    float d_term = g_mtd_kd * derivative;

    // --- อัปเดตสถานะสำหรับ Loop ถัดไป ---
    previous_time = current_time;
    previous_filtered_error = error;
    
    // --- ผลรวม PID สุดท้าย ---
    int pid_value = round(p_term + i_term + d_term);

    // จำกัดค่าการแก้ไขสูงสุด
    pid_value = constrain(pid_value, -max_correction, max_correction);
    
    // --- Motor Control ---
    // ใช้ current_speed ที่ผ่านการ Ramping มาแล้วเป็นความเร็วหลัก
    int speed_left = current_speed + pid_value;
    int speed_right = current_speed - pid_value;
    
    motorWrite(1, speed_left);
    motorWrite(2, speed_right);

    delay(10); // หน่วงเวลาเล็กน้อยเพื่อความเสถียร
  }
}

///////////////////////////////////////////////////////

// --- Global PID & Filter Constants for Spin (ควรปรับจูนชุดนี้โดยเฉพาะ) ---
float g_spin_kp = 1.2;       // P-gain สำหรับการหมุน มักจะน้อยกว่าการวิ่งตรง
float g_spin_ki = 0.08;      // I-gain ช่วยขจัด error สุดท้ายที่ค้างอยู่
float g_spin_kd = 1.5;       // D-gain ช่วยลดการแกว่ง (Overshoot)
float g_spin_alpha = 0.4;    // ค่ากรองสัญญาณ Gyro

void spinDegree_Optimized(int relative_degree) {
  // --- Function-level settings ---
  int max_turn_speed = 90;     // ความเร็วในการหมุนสูงสุดที่อนุญาต
  int min_turn_speed = 18;     // ความเร็วขั้นต่ำที่มอเตอร์จะเริ่มหมุน (กันมอเตอร์คราง)
  float stop_threshold_angle = 1.5; // ความคลาดเคลื่อนขององศาที่ยอมรับได้เพื่อหยุด
  unsigned long timeout = 3000;     // (3 วินาที) ป้องกัน Loop ค้างถ้าหมุนไม่ถึงเป้า

  // --- Initialization ---
  unsigned long loop_start_time = millis();
  unsigned long previous_time = millis();

  // 1. คำนวณองศาเป้าหมายจากตำแหน่งปัจจุบัน (สำคัญมาก)
  float start_angle = angleRead(YAW);
  float filtered_angle = start_angle; // เริ่มต้นค่าที่กรองแล้ว
  float target_degree = start_angle + relative_degree;

  // จัดการการม้วนรอบขององศา (-180 ถึง +180)
  if (target_degree > 180) target_degree -= 360;
  if (target_degree < -180) target_degree += 360;
  
  // 2. ตั้งค่าตัวแปร PID
  float previous_filtered_error = 0;
  float integral = 0;

  while (1) {
    // --- Timeout Check ---
    if (millis() - loop_start_time > timeout) {
      motorStop();
      break;
    }
    
    // --- Sensor Reading & Filtering ---
    float raw_angle = angleRead(YAW);
    filtered_angle = (g_spin_alpha * raw_angle) + ((1.0 - g_spin_alpha) * filtered_angle);
    
    // --- PID Calculation ---
    // P-term
    float error = target_degree - filtered_angle;
    if (error > 180) error -= 360;
    else if (error < -180) error += 360;
    
    float p_term = g_spin_kp * error;

    // I-term (with Anti-Windup, จะเริ่มทำงานเมื่อเข้าใกล้เป้าหมาย)
    float dt = (millis() - previous_time) / 1000.0;
    // เราสามารถให้ I-term ทำงานตลอดเวลาสำหรับการหมุน หรือจะใส่ active_zone ก็ได้
    integral += error * dt; 
    // ถ้า I ทำให้แกว่งมากไป ให้ Reset เมื่อ error มีค่ามาก: if (abs(error) > 20) integral = 0;
    float i_term = g_spin_ki * integral;

    // D-term (based on filtered error, using dt)
    float derivative = (dt > 0) ? (error - previous_filtered_error) / dt : 0;
    float d_term = g_spin_kd * derivative;
    
    // --- State Update ---
    previous_time = millis();
    previous_filtered_error = error;
    
    // 3. ผลลัพธ์จาก PID คือความเร็วในการหมุน
    int turn_speed = round(p_term + i_term + d_term);

    // --- Stopping Condition (เงื่อนไขการหยุดที่ฉลาดขึ้น) ---
    // หยุดเมื่อ: เข้าใกล้องศาเป้าหมายแล้ว และ ความเร็วในการหมุนเหลือน้อยมาก (แสดงว่าหุ่นเริ่มนิ่ง)
    if (abs(error) < stop_threshold_angle && abs(turn_speed) < 5) {
        motorStop();
        // เพิ่มการหน่วงเวลาเล็กน้อยเพื่อให้แน่ใจว่าหยุดนิ่งสนิทแล้วจึงออกจากฟังก์ชัน
        delay(100); 
        motorStop(); // สั่งหยุดซ้ำอีกครั้งกันไหล
        break; 
    }
    
    // 4. ควบคุมความเร็วอย่างนุ่มนวล
    // จำกัดความเร็วสูงสุด
    turn_speed = constrain(turn_speed, -max_turn_speed, max_turn_speed);

    // กำหนดความเร็วขั้นต่ำ เพื่อให้มอเตอร์มีแรงหมุน (เอาชนะแรงเสียดทาน)
    if (turn_speed > 0 && turn_speed < min_turn_speed) {
      turn_speed = min_turn_speed;
    }
    if (turn_speed < 0 && turn_speed > -min_turn_speed) {
      turn_speed = -min_turn_speed;
    }

    // --- Motor Control ---
    motorWrite(1, turn_speed);
    motorWrite(2, -turn_speed);
    
    delay(10); // หน่วงเวลาให้ระบบทำงานเสถียร
  }
}

//////////////////////////////////////////////////////////////////////////////////

// --- Global PID & Filter Constants for Pivot Turns (ควรปรับจูนชุดนี้โดยเฉพาะ) ---
float g_turn_kp = 1.5;      // P-gain สำหรับ pivot turn อาจต้องใช้ค่าสูงกว่า spin
float g_turn_ki = 0.1;      // I-gain ช่วยดันให้ถึงองศาสุดท้าย
float g_turn_kd = 1.8;      // D-gain ช่วยเบรกและลดการแกว่ง
float g_turn_alpha = 0.4;   // ค่ากรองสัญญาณ Gyro

void turnDegreeFront_Optimized(int relative_degree) {
  // --- Function-level settings ---
  int max_speed = 90;          // ความเร็วสูงสุดของล้อที่เคลื่อนที่
  int min_speed = 20;          // ความเร็วต่ำสุดเพื่อเอาชนะแรงเสียดทาน
  float stop_threshold_angle = 1.5; // ความคลาดเคลื่อนที่ยอมรับได้เพื่อหยุด
  unsigned long timeout = 4000;    // (4 วินาที) ป้องกัน Loop ค้าง

  // --- Initialization ---
  unsigned long loop_start_time = millis();
  unsigned long previous_time = millis();
  
  // 1. คำนวณองศาเป้าหมาย
  float start_angle = angleRead(YAW);
  float filtered_angle = start_angle;
  float target_degree = start_angle + relative_degree;

  if (target_degree > 180) target_degree -= 360;
  if (target_degree < -180) target_degree += 360;

  float previous_filtered_error = 0;
  float integral = 0;

  while (1) {
    // --- Timeout & Sensor ---
    if (millis() - loop_start_time > timeout) {
      motorStop();
      break;
    }
    float raw_angle = angleRead(YAW);
    filtered_angle = (g_turn_alpha * raw_angle) + ((1.0 - g_turn_alpha) * filtered_angle);

    // --- PID Calculation ---
    float error = target_degree - filtered_angle;
    if (error > 180) error -= 360;
    else if (error < -180) error += 360;

    float dt = (millis() - previous_time) / 1000.0;
    integral += error * dt;
    float derivative = (dt > 0) ? (error - previous_filtered_error) / dt : 0;
    
    // คำนวณค่า PID ทั้งหมด
    float pid_output = (g_turn_kp * error) + (g_turn_ki * integral) + (g_turn_kd * derivative);

    // --- State Update ---
    previous_time = millis();
    previous_filtered_error = error;
    
    // --- Smart Stop Condition ---
    // หยุดเมื่อ: องศาถูกต้อง และ พลังงานในการเลี้ยว (pid_output) เกือบเป็นศูนย์
    if (abs(error) < stop_threshold_angle && abs(pid_output) < min_speed) {
      motorStop();
      delay(100); // รอให้หุ่นหยุดนิ่งสนิท
      motorStop();
      break;
    }

    // --- 2. แปลง PID Output เป็นความเร็วล้อแบบ Pivot Turn (ส่วนที่สำคัญที่สุด) ---
    // pid_output > 0  --> ต้องเลี้ยวซ้าย (ล้อขวาเดินหน้า, ล้อซ้ายหยุด)
    // pid_output < 0  --> ต้องเลี้ยวขวา (ล้อซ้ายเดินหน้า, ล้อขวาหยุด)

    // จำกัดความเร็วสูงสุด
    pid_output = constrain(pid_output, -max_speed, max_speed);
    
    int speed_left = 0;
    int speed_right = 0;

    if (pid_output > 0) { // ต้องการเลี้ยวซ้าย
      speed_right = pid_output;
      // กำหนดความเร็วขั้นต่ำ
      if (speed_right < min_speed) speed_right = min_speed;

    } else { // ต้องการเลี้ยวขวา (pid_output เป็นลบหรือศูนย์)
      speed_left = -pid_output; // ทำให้ค่าเป็นบวกเพื่อเป็นความเร็ว
      // กำหนดความเร็วขั้นต่ำ
      if (speed_left > 0 && speed_left < min_speed) speed_left = min_speed;
    }

    // --- Motor Control ---
    motorWrite(1, speed_left);
    motorWrite(2, speed_right);

    delay(10);
  }
}

/////////////////////////////////////////////////////////////////////////////////

// --- ค่าคงที่สำหรับ SetFront (ควรปรับจูนชุดนี้โดยเฉพาะ) ---
float g_setfront_kp_line = 0.2; // KP สำหรับการปรับตามเส้น (Line Following)
float g_setfront_kp_gyro = 1.0; // KP สำหรับการรักษาทิศทาง (Heading)
float g_setfront_kd_gyro = 1.2; // KD สำหรับการรักษาทิศทาง (Heading)

void SetFront_Optimized(int speed) {
  // --- Function-level settings ---
  int base_speed = speed;          // ความเร็วพื้นฐานในการเคลื่อนที่
  int black_threshold = 50;        // ค่าที่ถือว่าเซ็นเซอร์เห็นสีดำ
  unsigned long timeout = 3000;    // (3 วินาที) ป้องกันการทำงานค้าง

  // --- Initialization ---
  unsigned long start_time = millis();
  unsigned long previous_time = millis();

  // 1. "ล็อค" องศาเป้าหมาย คือ องศาปัจจุบันที่หันหน้าอยู่
  float target_degree = angleRead(YAW);
  float previous_error_gyro = 0;

  while (1) {
    // --- เงื่อนไขการออกจาก Loop ---
    readLine();
    // 1. เงื่อนไขสำเร็จ: เมื่อเซ็นเซอร์หน้าทั้งสองตัวเจอเส้นดำ
    if (line_value[0] <= black_threshold && line_value[1] <= black_threshold) {
      motorStop();
      // ไม่ต้องอัปเดต current_degree เพราะเราพยายามรักษามันไว้ตลอดทางแล้ว
      break;
    }
    // 2. เงื่อนไขล้มเหลว: หมดเวลา
    if (millis() - start_time > timeout) {
      motorStop();
      break;
    }
    
    // --- Controller 1: Line Centering (P-Controller) ---
    // คำนวณ Error จากความแตกต่างของเซ็นเซอร์ซ้าย-ขวา
    // สมมติ: line_value[0] คือซ้าย, line_value[1] คือขวา
    // error > 0 : เอียงไปทางซ้าย, ต้องหักขวา
    // error < 0 : เอียงไปทางขวา, ต้องหักซ้าย
    float line_error = line_value[0] - line_value[1]; 
    int line_correction = round(g_setfront_kp_line * line_error);

    // --- Controller 2: Gyro Stabilization (PD-Controller) ---
    float current_angle = angleRead(YAW);
    float gyro_error = target_degree - current_angle;
    if (gyro_error > 180) gyro_error -= 360;
    else if (gyro_error < -180) gyro_error += 360;
    
    float dt = (millis() - previous_time) / 1000.0;
    float derivative = (dt > 0) ? (gyro_error - previous_error_gyro) / dt : 0;
    int gyro_correction = round((g_setfront_kp_gyro * gyro_error) + (g_tsetfront_kd_gyro * derivative));
    
    previous_error_gyro = gyro_error;
    previous_time = millis();

    // --- รวมค่าแก้ไขและควบคุมมอเตอร์ ---
    // speed_left = base - line_correction + gyro_correction
    // speed_right = base + line_correction + gyro_correction
    int speed_left = base_speed - line_correction + gyro_correction;
    int speed_right = base_speed + line_correction + gyro_correction;

    // จำกัดความเร็วเพื่อความปลอดภัย
    speed_left = constrain(speed_left, -60, 80);
    speed_right = constrain(speed_right, -60, 80);

    motorWrite(1, speed_left);
    motorWrite(2, speed_right);

    delay(10);
  }
  // หน่วงเวลาเพื่อให้แน่ใจว่าหุ่นหยุดสนิท
  delay(50);
  motorStop();
}

/////////////////////////////////////////////////////////////////////////////////

// --- ค่าคงที่สำหรับ SetBack (สามารถใช้ร่วมกับ SetFront หรือปรับจูนใหม่ก็ได้) ---
float g_setback_kp_line = 0.2; // KP สำหรับการปรับตามเส้น
float g_setback_kp_gyro = 1.0; // KP สำหรับการรักษาทิศทาง (Heading)
float g_setback_kd_gyro = 1.2; // KD สำหรับการรักษาทิศทาง (Heading)

void SetBack_Optimized(int speed) {
  // --- Function-level settings ---
  int base_speed = -abs(speed); // **สำคัญ: ทำให้เป็นค่าลบเพื่อถอยหลังเสมอ**
  int black_threshold = 50;     // ค่าที่ถือว่าเซ็นเซอร์เห็นสีดำ
  unsigned long timeout = 3000; // (3 วินาที) ป้องกันการทำงานค้าง

  // --- Initialization ---
  unsigned long start_time = millis();
  unsigned long previous_time = millis();

  // 1. "ล็อค" องศาเป้าหมาย คือ องศาปัจจุบัน
  float target_degree = angleRead(YAW);
  float previous_error_gyro = 0;

  while (1) {
    // --- เงื่อนไขการออกจาก Loop ---
    readLine();
    // 1. เงื่อนไขสำเร็จ: เซ็นเซอร์หลังทั้งสองตัวเจอเส้น
    // จากโค้ดเดิม: line_value[5] คือหลังซ้าย, line_value[2] คือหลังขวา
    if (line_value[5] <= black_threshold && line_value[2] <= black_threshold) {
      motorStop();
      break;
    }
    // 2. เงื่อนไขล้มเหลว: หมดเวลา
    if (millis() - start_time > timeout) {
      motorStop();
      break;
    }
    
    // --- Controller 1: Line Centering (P-Controller) ---
    // error > 0 : เอียงซ้าย, ต้องหักขวา
    // error < 0 : เอียงขวา, ต้องหักซ้าย
    // ใช้คู่หลัง: line_value[5] (ซ้าย) vs line_value[2] (ขวา)
    float line_error = line_value[5] - line_value[2]; 
    int line_correction = round(g_setback_kp_line * line_error);

    // --- Controller 2: Gyro Stabilization (PD-Controller) ---
    float current_angle = angleRead(YAW);
    float gyro_error = target_degree - current_angle;
    if (gyro_error > 180) gyro_error -= 360;
    else if (gyro_error < -180) gyro_error += 360;
    
    float dt = (millis() - previous_time) / 1000.0;
    float derivative = (dt > 0) ? (gyro_error - previous_error_gyro) / dt : 0;
    int gyro_correction = round((g_setback_kp_gyro * gyro_error) + (g_setback_kd_gyro * derivative));
    
    previous_error_gyro = gyro_error;
    previous_time = millis();

    // --- รวมค่าแก้ไขและควบคุมมอเตอร์สำหรับการถอยหลัง ---
    // หลักการ: เรายังคงต้องการให้ gyro_correction ดันหุ่นกลับไปทางเดิมเสมอ
    // แต่ line_correction จะทำงานกลับด้านเมื่อเทียบกับการเดินหน้า
    // speed_left = base(ลบ) + line_correction + gyro_correction
    // speed_right = base(ลบ) - line_correction + gyro_correction
    int speed_left = base_speed + line_correction + gyro_correction;
    int speed_right = base_speed - line_correction + gyro_correction;

    // จำกัดความเร็วเพื่อความปลอดภัย
    speed_left = constrain(speed_left, -80, 60);
    speed_right = constrain(speed_right, -80, 60);

    motorWrite(1, speed_left);
    motorWrite(2, speed_right);

    delay(10);
  }
  // หน่วงเวลาเพื่อให้แน่ใจว่าหุ่นหยุดสนิท
  delay(50);
  motorStop();
}