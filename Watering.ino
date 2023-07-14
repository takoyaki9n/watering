#define SERIAL_ON
#define PERIOD 10

int counter;

int pinPumps[2] = { 10, 16 };
int pinSensors[2] = { A0, A1 };

int pinLED = 9;

void setupModules(int id) {
  pinMode(pinPumps[id], OUTPUT);
  pinMode(pinSensors[id], INPUT);
  digitalWrite(pinPumps[id], HIGH);
}

void setup() {
  counter = 0;
#ifdef SERIAL_ON
  Serial.begin(9600);
#endif

  setupModules(0);
  setupModules(1);

  pinMode(pinLED, OUTPUT);
  digitalWrite(pinLED, LOW);

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

  // Sensor value is low when it's wet.
  int runPump = (counter % PERIOD == id) && (value > threshold);
  // `LOW` to run the pump, `HIGH` to stop it.
  digitalWrite(pinPumps[id], runPump ? LOW : HIGH);
}

void loop() {
  operateModules(0, 500);
  operateModules(1, 500);

  int blinkLED = counter % PERIOD == 0;
  digitalWrite(pinLED, blinkLED ? HIGH : LOW);

#ifdef SERIAL_ON
  Serial.print("COUNT: ");
  Serial.println(counter);
#endif
  counter++;
  delay(1000);
}