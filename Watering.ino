#include "avr/sleep.h"
#include "avr/wdt.h"

// Time definitions for Watch-dog-timer
#define WDT_1S B00000110  // 1s
#define WDT_2S B00000111  // 2s
#define WDT_4S B00100000  // 4s
#define WDT_8S B00100001  // 8s

#define SERIAL_ON
// 2 hours (= 60 * 60 / 8)
#define INTERVAL 900

int pinButton = 8;

int pinPower = 7;
int pinPumps[2] = { 10, 16 };
int pinSensors[2] = { A0, A1 };

// Intrpt svc rtn for WDT ISR (vect)
ISR(WDT_vect) {}
void interrupt() {}

void setupModules(int id) {
  pinMode(pinPumps[id], OUTPUT);
  pinMode(pinSensors[id], INPUT);
  digitalWrite(pinPumps[id], HIGH);
}

void setupWDT(byte sleepT) {
  sleepT += B00010000;  // sleepTime + Enalbe WD-change bit-on
  MCUSR &= B11110111;   // Prepare WDT-reset-flag in MCU-status-Reg
  WDTCSR |= B00011000;  // Enable WD-system-reset + WD-change
  WDTCSR = sleepT;      // Set sleepTime + Enable WD-change
  WDTCSR |= B01000000;  // Finally, enable WDT-interrrupt
}

void setup() {
#ifdef SERIAL_ON
  Serial.begin(9600);
#endif

  pinMode(pinButton, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinButton), interrupt, FALLING);

  pinMode(pinPower, OUTPUT);
  digitalWrite(pinPower, LOW);
  setupModules(0);
  setupModules(1);

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  setupWDT(WDT_8S);
}

void operateModules(int id, float threshold) {
  // The `value` is high when it's dry.
  int value = analogRead(pinSensors[id]);

#ifdef SERIAL_ON
  Serial.print("MOISTURE LEVEL #");
  Serial.print(id);
  Serial.print(": ");
  Serial.println(value);
#endif

  if (value > threshold) {
    // `LOW` to run the pump, `HIGH` to stop it.
    digitalWrite(pinPumps[id], LOW);
    delay(1000);
  }
  digitalWrite(pinPumps[id], HIGH);
}

void deepSleep(void) {
  ADCSRA &= B01111111;  // disable ADC to save power
  sleep_enable();
  sleep_cpu();  // sleep until WDT-interrupt
  sleep_disable();
  ADCSRA |= B10000000;  // enable ADC again
}

void loop() {
  digitalWrite(pinPower, HIGH);
  delay(100);
  operateModules(0, 500);
  operateModules(1, 500);
  digitalWrite(pinPower, LOW);

  for (int i = 0; i < INTERVAL; i++) {
    int button = digitalRead(pinButton);
    if (!button) break;
    deepSleep();
  }
}
