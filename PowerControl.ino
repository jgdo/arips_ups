
#define RELAY_CHARGE_PIN PB0
#define RELAY_POWER_PIN PB7
#define CURRENT_PIN PA2
#define VCC_PIN PA0

#define BATTERY_OUT_VOLTAGE_PIN PA2
#define VCC_IN_VOLTAGE_PIN A4

void myprint() {
  if(Serial) {
    Serial.println();
  }  
}

template<typename T, typename ...Args>
void myprint(T first, Args... rest) {
  if(Serial) {
    Serial.print(first);
    myprint(rest...);
  }
}


float analogReadN(int pin, unsigned n) {
  int sum = 0;
  for(unsigned i =0; i < n; i++) {
    sum += analogRead(pin);
  }

  const float val = ((float)sum / n) * 3.3f / 4096;
  return val;
}

void digitalToggle(int pin) {
  digitalWrite(pin, !digitalRead(pin));
}

float readVccIn() {
  return analogReadN(VCC_IN_VOLTAGE_PIN, 4) * 6;
}

float readBatteryOut() {
  return analogReadN(BATTERY_OUT_VOLTAGE_PIN, 4) * 6;
}


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RELAY_CHARGE_PIN, OUTPUT);
  pinMode(RELAY_POWER_PIN, OUTPUT);
  
  analogReadResolution(12);
}

float readCurrentSensor() {
   const float currentUncorrected = analogReadN(CURRENT_PIN, 8);
   const auto vcc = analogReadN(VCC_PIN, 8) * 2.0f;

   const auto correctionFactor = 5.0f / vcc;
   const auto currentCorrected = currentUncorrected * correctionFactor;

   return currentCorrected; 
}

void setRelayCharge(int on) {
  digitalWrite(RELAY_CHARGE_PIN, on);
  digitalWrite(LED_BUILTIN, !on);
}

void setRelayPower(int on) {
  digitalWrite(RELAY_POWER_PIN, on);
}

// the loop function runs over and over again forever
void loop() {
  /*
  static unsigned long lastTime;

  const auto currentRaw = analogReadN(CURRENT_PIN, 4);
  const auto vcc = analogReadN(VCC_PIN, 4);
  const auto currentCorrected = readCurrentSensor();
  // myprint("Relay:", digitalRead(RELAY_PIN),", currentRaw:", currentRaw, ", vcc:", vcc, ", currentCorrected:", currentCorrected);
  myprint("Relay:", digitalRead(RELAY_PIN), ", currentCorrected:", currentCorrected);
 
  auto currentTime = millis();
  if(currentTime >= lastTime + 1000) {
    digitalToggle(RELAY_PIN);
    lastTime = currentTime;
  }
  
   */

  float vccIn = readVccIn();
  float batteryOut = readBatteryOut();
  int relay = vccIn > 16.0? 1 : 0;
  setRelayCharge(relay);

  myprint("charge:",vccIn, ",battery:", batteryOut);

  int lastCmd = -1;
  while(Serial.available() > 0) {
    lastCmd = Serial.read();
  }

  if(lastCmd == '1') {
    setRelayPower(1);
  } else if(lastCmd == '0') {
    setRelayPower(0);
  } // ignore others

  delay(20);
}
