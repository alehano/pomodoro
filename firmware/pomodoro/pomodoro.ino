#include <EncButton.h> // https://github.com/GyverLibs/EncButton
#include <TimerMs.h> // https://github.com/GyverLibs/TimerMs

//// setup 

const int LED_R_PIN = 10;
const int LED_G_PIN = 11;
const int LED_B_PIN = 12;
const int BUZ_PIN = 5;
const int BTN_PIN = 4;

const int LED_SPEED_MS = 700; // led blinking speed

const uint32_t WORK_T_MS = 1500000; // work time 25 min
const uint32_t REST_T_MS = 300000; // rest time 5 min
const uint32_t LONG_REST_T_MS = 1200000; // long rest 20 min

////

int curLed = LED_R_PIN;
int ledState = 0; // 0 - off, 1 - on, 2 - blinking
bool ledOn = false;

int pomodoros = 0;
int state = 0; // 0 - pause, 1 - work, 2 - work end, 3 - rest, 4 - rest end
int initState = true;

EncButton<EB_TICK, BTN_PIN> btn;

TimerMs ledTmr(LED_SPEED_MS, 1, 0);
TimerMs mainTmr(WORK_T_MS, 0, 1);

void setup() {

  pinMode(LED_R_PIN, OUTPUT);
  pinMode(LED_G_PIN, OUTPUT);
  pinMode(LED_B_PIN, OUTPUT);
  pinMode(BUZ_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT);

}

void loop() {
  btn.tick();

  // state
  switch (state) {
    case 0: // pause
      if (initState) {
        setCurLed(LED_B_PIN);
        setLedState(2);
        initState = false;
      }
      if (btn.release()) {
        beepClick();
        setState(1);
      }
      break;
    case 1: // work
      if (initState) {
        initState = false;

        setCurLed(LED_R_PIN);
        setLedState(1);

        mainTmr.setTime(WORK_T_MS);
        mainTmr.start();
      }
      if (mainTmr.tick()) {
        setState(2);
      }
      if (btn.release()) {
        beepClick();
        setState(0);
      }
      break;
    case 2: // work end
      if (initState) {
        initState = false;

        setCurLed(LED_R_PIN);
        setLedState(2);

        pomodoros = pomodoros+1;
        if (pomodoros >= 4) {
          // long rest
          pomodoros = 0;
          mainTmr.setTime(LONG_REST_T_MS);
          beepLongRest();
        } else {
          mainTmr.setTime(REST_T_MS);
          beepEnd();
        }
      }
      if (btn.release()) {
        beepClick();
        setState(3);
      }
      break;
    case 3: // rest
      if (initState) {
        initState = false;

        setCurLed(LED_G_PIN);
        setLedState(1);
        mainTmr.start();
      }
      if (mainTmr.tick()) {
        setState(4);
      }
      if (btn.release()) {
        beepClick();
        setState(1);
      }
      break;
    case 4: // rest end
      if (initState) {
        initState = false;

        setCurLed(LED_G_PIN);
        setLedState(2);

        beepEnd();
      }
      if (btn.release()) {
        beepClick();
        setState(1);
      }
      break;
  }

  // led control
  if (ledTmr.tick()) {
    switch (ledState) {
      case 0: 
        if (ledOn) {
          ledSwitch(false);
        };
        break;
      case 1: 
        if (!ledOn) {
          ledSwitch(true);
        };
        break;
      case 2: 
        ledSwitch(!ledOn);
        break;
    }
  }

}

void setState(int val) {
  if (val == state) return;
  if (state >= 5) val = 0;
  state = val;
  initState = true;
}

void ledSwitch(bool on) {
  if (on) {
    digitalWrite(curLed, HIGH); 
  } else {
    digitalWrite(curLed, LOW);     
  }
  ledOn = on; 
}

void setCurLed(int val) {
  if (val == curLed) return;
  ledSwitch(false);
  curLed = val;
}

void setLedState(int val) {
  if (val == ledState) return;
  ledState = val;
}

void beepClick() {
  beep(1, 50, 0);  
}

void beepEnd() {
  beep(2, 100, 0);  
}

void beepLongRest() {
  beep(3, 300, 0);  
}

void beep(int n, int durSig, int durPause) {
  if (n>1 && durPause == 0 ) {
    durPause = durSig;
  }
  for (int i=0;i<n;i++) {
    digitalWrite(BUZ_PIN, HIGH);
    delay(durSig);
    digitalWrite(BUZ_PIN, LOW);
    delay(durPause);
  }
}
