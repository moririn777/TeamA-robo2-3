#include <Arduino.h>
#include <PS4Controller.h>

// 跳ばすモーター２つ
const int THR_RIGHT_DIR_PIN = 21;
const int THR_RIGHT_PWM_PIN = 26;
const int THR_LEFT_DIR_PIN = 19;
const int THR_LEFT_PWM_PIN = 14;

// 車輪のモーター２つ
const int RIGHT_DIR_PIN = 23;
const int RIGHT_PWM_PIN = 13;

const int LEFT_DIR_PIN = 22;
const int LEFT_PWM_PIN = 27;

uint32_t previous_millis = 0;
const int DEBOUNCE_DELAY = 50;

bool triangle_pressed = false;
uint32_t triangle_debounce_time = 0;
bool is_auto_mode = false;

void setup() {
  Serial.begin(115200);

  // 車輪のモーター２つ
  pinMode(RIGHT_DIR_PIN, OUTPUT);
  pinMode(LEFT_DIR_PIN, OUTPUT);
  ledcSetup(4, 4000, 8);
  ledcSetup(5, 4000, 8);
  ledcAttachPin(RIGHT_PWM_PIN, 4);
  ledcAttachPin(LEFT_PWM_PIN, 5);

  // 跳ばすモーター２つ
  pinMode(THR_RIGHT_DIR_PIN, OUTPUT);
  pinMode(THR_LEFT_DIR_PIN, OUTPUT);
  ledcSetup(6, 4000, 8);
  ledcSetup(7, 4000, 8);
  ledcAttachPin(THR_RIGHT_PWM_PIN, 6);
  ledcAttachPin(THR_LEFT_PWM_PIN, 7);

  PS4.begin("48:E7:29:A3:C4:B8");
}

void loop() {

  if (!PS4.isConnected()) {
    ledcWrite(4, 0);
    ledcWrite(5, 0);
    ledcWrite(6, 0);
    ledcWrite(7, 0);
    return;
  }

  int right_motor = PS4.RStickY();
  int left_motor = PS4.LStickY();

  // 車輪のモーター-------------------------------------------------------------
  if (abs(right_motor) > 30) {
    ledcWrite(4, abs(right_motor));
    digitalWrite(RIGHT_DIR_PIN, right_motor > 0 ? LOW : HIGH);
  } else {
    ledcWrite(4, 0);
  }

  if (abs(left_motor) > 30) {
    ledcWrite(5, abs(left_motor));
    digitalWrite(LEFT_DIR_PIN, left_motor > 0 ? HIGH : LOW);
  } else {
    ledcWrite(5, 0);
  }
  // 跳ばすモーター-------------------------------------------------------------
  int rspeed = 0;
  int lspeed = 0;


    if (PS4.Options() && PS4.Circle()) {
        rspeed = map(PS4.R2Value(), 0, 255, 0, 150);
        lspeed = map(PS4.L2Value(), 0, 255, 0, 150);
      } else {
        rspeed = map(PS4.R2Value(), 0, 255, 0, 70);
        lspeed = map(PS4.L2Value(), 0, 255, 0, 70);
      }

    if (abs(rspeed) > 30) { // 両方押されたときは飛ばす方が優先される
      ledcWrite(6, rspeed);
      digitalWrite(THR_RIGHT_DIR_PIN, LOW);
      ledcWrite(7, rspeed);
      digitalWrite(THR_LEFT_DIR_PIN, HIGH);
    } else if (abs(lspeed) > 30) {
      ledcWrite(6, lspeed);
      digitalWrite(THR_RIGHT_DIR_PIN, HIGH);
      ledcWrite(7, lspeed);
      digitalWrite(THR_LEFT_DIR_PIN, LOW);
    } else {
      ledcWrite(6, 0);
      ledcWrite(7, 0);
    }
  }