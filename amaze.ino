#include "motor.h"

#define ML1 5
#define ML2 6
#define MLP 7
#define MR1 3
#define MR2 4
#define MRP 2
#define spdPWM 100

#define DEADEND 32

const byte IR[] = {A0, A1, A2, A3, A4, A5, A6, A7};
const byte IRN = 8;
const int W[8] = { 0, -3, -2, -1, 1, 2, 3, 0};
static byte sensor = 0;

String path = "";

motor m1(ML1, ML2, MLP);
motor m2(MR1, MR2, MRP);

int base = 100;
int right, front, left;
float line = 0;
float pline = 0;
float eline = 0;
float Kp = 100;
float Kd = 5;
float Ki = 0.01;

void readSensor() {
  sensor = 0;
  for (byte i = 0; i < IRN; i ++) {
    int reading = digitalRead(IR[i]);
    if (reading)
      sensor |= (1 << i);
  }
}

void readLine() {
  pline = line;
  line = 0;
  int tot = 0;
  for (int i = 1; i < IRN - 1; i ++) {
    int r = (sensor >> i) & 1;
    line += W[i] * r;
    tot += r;
  }
  line = line / tot;
  eline += line;
  if (tot == 0)
    line = 0;
  
}

void setup() {
  Serial.begin(115200);

  for (byte i = 0; i < IRN; i ++)
    pinMode(IR[i], INPUT);

  pinMode(DEADEND, OUTPUT);
  pinMode(DEADEND + 1, OUTPUT);
}

void turnLeft() {
  m1.setSpeed(spdPWM);
  m2.setSpeed(-spdPWM);
}

void straight() {
  m1.setSpeed(spdPWM);
  m2.setSpeed(-spdPWM);
}

void turnRight() {
  m1.setSpeed(-spdPWM);
  m2.setSpeed(spdPWM);
}

void hardTurnLeft() {
  m1.setSpeed(spdPWM);
  m2.setSpeed(-spdPWM);
}

void linefollow(int t=30) {
  digitalWrite(DEADEND, HIGH);
  int cur = millis();
  while (millis() - cur < t) {
    readSensor();
    readLine();

    int diff = Kp * line + Kd * (line - pline) + Ki * (eline);
    int sped = map(abs(diff), 0, 255, 100, 0);

    Serial.println(line);

    m1.setSpeed(constrain(sped - diff, -spdPWM, spdPWM));
    m2.setSpeed(constrain(sped + diff, -spdPWM, spdPWM));
  }
  digitalWrite(DEADEND, LOW);
}

void loop() {

  readSensor();

  front = (sensor & 0b00111100);
  right = (sensor & 0b11111000) == 0b11111000;
  left = (sensor & 0b00011111) == 0b00011111;

  if (left || right) {
    m1.setSpeed(spdPWM);
    m2.setSpeed(spdPWM);
    delay(200+);

    front = (sensor & 0b00111100);
  }

  //    linefollow();

  if (left) {
    Serial.println("L");
    while(front)
    {
      turnLeft();
      readSensor();
      front = (sensor & 0b00011000);
    }

    while(!front)
    {
      turnLeft();
      readSensor();
      front = (sensor & 0b00011000);
    }
    
  } else if (front) {
    Serial.println("Line Follow");
    linefollow();
  } else if (right) {
    Serial.println("R");
    while(front)
    {
      turnRight();
      readSensor();
      front = (sensor & 0b00011000);
    }

    while(!front)
    {
      turnRight();
      readSensor();
      front = (sensor & 0b00011000);
    }
  } else {
    Serial.println("U");
    while(front)
    {
      readSensor();
      turnRight();
      front = (sensor & 0b00011000);
    }
    while(!front)
    {
      readSensor();
      turnRight();
      front = (sensor & 0b00011000);
    }
  }
}
