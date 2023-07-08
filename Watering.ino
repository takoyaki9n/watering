#define SERIAL_ON

int pinPumps[2] = { 10, 16 };
int pinSensors[2] = { A0, A1 };

int pinLED = 9;
int cntLED;

void setupModules(int id) {
  pinMode(pinPumps[id], OUTPUT);
  pinMode(pinSensors[id], INPUT);
  digitalWrite(pinPumps[id], HIGH);
}

void setup() {
#ifdef SERIAL_ON
  Serial.begin(9600);
#endif

  setupModules(0);
  setupModules(1);

  pinMode(pinLED, OUTPUT);
  digitalWrite(pinLED, HIGH);
  cntLED = 0;

  delay(500);
}

void operateModules(int id, float threshold) {
  int value = analogRead(pinSensors[id]);

#ifdef SERIAL_ON
  Serial.print("MOISTURE LEVEL #");
  Serial.print(id);
  Serial.print(": ");
  Serial.println(value);
#endif

  digitalWrite(pinPumps[id], value < threshold ? HIGH : LOW);
}

void loop() {
  operateModules(0, 500);
  operateModules(1, 500);

  digitalWrite(pinLED, cntLED == 0 ? HIGH : LOW);
  cntLED = (cntLED + 1) % 10;

  delay(1000);
}