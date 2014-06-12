#include <Wire.h>
#include <Servo.h>
#include "nunchuck_funcs.h"

Servo servo;
int servo_pos = 0;

Servo alt;
int alt_pos = 0;

const int servo_d = 30;
boolean servo_on = true;
const int joyx_min = 35;
const int joyx_dead_min = 132;
const int joyx_dead_max = 136;
const int joyx_max = 236;
const int joyy_min = 27;
const int joyy_dead_min = 125;
const int joyy_dead_max = 130;
const int joyy_max = 222;

int cbut_last = 0;

int loop_cnt = 0;

byte joyx, joyy, cbut;

void setup()
{
  Serial.begin(19200);

  nunchuck_set_powerpins();
  nunchuck_init();
  Serial.println("WiiChuckDemo ready");

  if (!servo.attach(2)) {
    Serial.println("Servo on 2 attached");
  }
  if (!alt.attach(3)) {
    Serial.println("Servo on 3 attached");
  }
}

static int clamp(int x, int lo, int hi)
{
  if (x < lo)
  {
    x = lo;
  }
  else if (hi < x)
  {
    x = hi;
  }

  return x;
}

static float percentage(float x, float lo, float hi)
{
  float range = hi - lo;
  return (x - lo) / range;
}

static float calc_percentage(int x, int lo, int hi)
{
  float p = percentage(x, lo, hi);
  return p;
}

void loop()
{
  if (loop_cnt < 100) { // every 100 ms, get new data
    return;
  }
  loop_cnt = 0;

  nunchuck_get_data();

  joyx = nunchuck_joyx();
  joyy = nunchuck_joyy();
  cbut = nunchuck_cbutton();

  if (cbut && (cbut != cbut_last)) {
    servo_on = !servo_on;
  }
  cbut_last = cbut;

  if (!servo_on) { return; }

  if (joyx < joyx_dead_min) {
    float p = 1.0 - calc_percentage(joyx, joyx_min, joyx_dead_min);
    float new_d = servo_d * pow(p, 4);
  }
  else if (joyx_dead_max < joyx) {
    float p = calc_percentage(joyx, joyx_dead_max, joyx_max);
    float new_d = -servo_d * pow(p, 4);
  }
  servo_pos += round(new_d);


  if (joyy < joyy_dead_min) {
    float p = 1.0 - calc_percentage(joyy, joyy_min, joyy_dead_min);
    float new_d = -servo_d * pow(p, 4);
  }
  else if (joyy_dead_max < joyy) {
    float p = calc_percentage(joyy, joyy_dead_max, joyy_max);
    float new_d = servo_d * pow(p, 4);
  }
  alt_pos += round(new_d);


  servo_pos = clamp(servo_pos, 0, 180);
  alt_pos = clamp(alt_pos, 0, 180);

  servo.write(servo_pos);
  alt.write(alt_pos);

}

loop_cnt++;
delay(1);
}
