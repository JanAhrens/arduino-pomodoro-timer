#include <SerialLCD.h>
#include <SoftwareSerial.h>
#define BOUNCE_LOCK-OUT
#include <Bounce.h>

// Arduino pin configuration
const int buttonPin = 2;
SerialLCD slcd(11,12);

Bounce debouncer = Bounce();
int backlightState;
long highSince = 0;
long delta = 0;
int flashCount = 0;

long pomodoroTimer = 0;
long previousMillis = 0;

#define TIMER_RUNNING (pomodoroTimer > 0)
#define TIMER_DURATION (25 * 60)

#ifdef DEBUG
#  define DEBUG_PRINTLN(X) Serial.println(X);
#else
#  define DEBUG_PRINTLN(X)
#endif

void setup() {
  slcd.begin();

  pinMode(buttonPin, INPUT);
  debouncer.attach(buttonPin);
  debouncer.interval(20);

  backlightState = HIGH;

#ifdef DEBUG
  Serial.begin(9600);
#endif

  DEBUG_PRINTLN("ready");
}

void loop() {
  if (debouncer.update() == 1) {
    if (debouncer.read() == HIGH) {
      if (highSince == 0) {
        highSince = millis();
      }
    } else {
      delta = millis() - highSince;
      if (delta <= 500) {
        DEBUG_PRINTLN("short press detected");
        backlightState = !backlightState;
      } else {
        DEBUG_PRINTLN("long press deteced");
        pomodoroTimer = TIMER_DURATION;

        DEBUG_PRINTLN("Timer started");
      }

      highSince = 0;
    }
  }

  if (backlightState == HIGH) {
    slcd.backlight();
  } else {
    slcd.noBacklight();
  }

  slcd.setCursor(0, 0);
  slcd.print("Pomodoro Timer");

  slcd.setCursor(0, 1);
  if (TIMER_RUNNING) {
    slcd.print("Busy for ");

    if (pomodoroTimer > 60) {
      slcd.print(pomodoroTimer / 60, DEC);
      slcd.print(":");
      if (pomodoroTimer % 60 < 10) {
        slcd.print(0, DEC);
      }
      slcd.print(pomodoroTimer % 60, DEC);
      slcd.print(" min");
    } else {
      slcd.print(pomodoroTimer, DEC);
      slcd.print(" sec");
    }
    // clean rest of the row by filling it with spaces
    slcd.print("    ");
  } else {
    // no timer running: show a small helptext
    slcd.print("start:long press");
  }

  // couting the pomodoroTimer down
  if ((millis() - previousMillis) > 1000) {
    if (TIMER_RUNNING) {
      pomodoroTimer--;
      if (pomodoroTimer == 0) {
        DEBUG_PRINTLN("Timer reached zero");
        pomodoroTimer = 0;
        flashCount = 30;
      }
    }

    if (flashCount > 0) {
      backlightState = !backlightState;
      flashCount--;
    }

    previousMillis = millis();
  }
}
