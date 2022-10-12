#ifndef _motor_h
#define _motor_h

struct motor {
  int p;
  int l;
  int r;

  motor(int ll, int rr, int pp) {
    l = ll;
    r = rr;
    p = pp;
    pinMode(l, OUTPUT);
    pinMode(r, OUTPUT);
    pinMode(p, OUTPUT);
  }

  void setSpeed(int pwm) {
    analogWrite(p, abs(pwm));
    if (pwm > 0) {
      digitalWrite(l, HIGH);
      digitalWrite(r, LOW);
    } else {
      digitalWrite(l, LOW);
      digitalWrite(r, HIGH);
    }
  }
};

#endif
