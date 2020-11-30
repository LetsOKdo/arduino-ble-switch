/*
  Arduino BLE switch

  Scans for BLE peripherals advertising specific service UUIDs and switches
  state when peripheral goes in or out of range, using signal strength.

  Compatible boards:
  Arduino MKR WiFi 1010, Arduino Nano 33 IoT,
  Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense

  MIT License

  Copyright (c) 2020 Peter Milne
*/

#include <ArduinoBLE.h>

// RSSI value in dB, higher value = closer proximity
const int MIN_RSSI = -85;  // Adjust this value
const int NUMBER_OF_BOUNCES = 3; // Adjust
const int TIMEOUT = 1000; // Timeout in ms

const char* UUID = "FD6F"; // Exposure service
const byte SWITCH_PIN =  21;  // Switch

enum states {OFF = LOW, ON = HIGH, SWITCHING};
enum ranges {OUT_OF_RANGE, IN_RANGE};

states state_now = OFF, state_before = OFF;
ranges range = OUT_OF_RANGE;
int count = 0;  // Bounce counter

void setup() {
  pinMode(SWITCH_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  //while (!Serial);

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed...");
    while (1);
  }

  // Set discovered event handler
  BLE.setEventHandler(BLEDiscovered, bleDiscoveredHandler);

  Serial.print("Starting scan for UUID: "); Serial.println(UUID);
  BLE.scanForUuid(UUID, true);   // Scan with duplicates
}

void loop() {
  // Poll for events with timeout
  BLE.poll(TIMEOUT);
}

// Discovered a peripheral
void bleDiscoveredHandler(BLEDevice peripheral) {
  int rssi = peripheral.rssi();
  Serial.print("MAC: "); Serial.print(peripheral.address());
  Serial.print(" RSSI: "); Serial.println(rssi);

  // Save range
  range = ( rssi > MIN_RSSI) ? IN_RANGE : OUT_OF_RANGE;

  printDebugInfo();

  switch (state_now) {
    case ON:
      if (state_now != state_before) {
        Serial.println("Turning ON"); Serial.println();
        action(ON);
      }
      state_before = state_now; // Save state
      count = 0; // Reset counter
      if (range == OUT_OF_RANGE) state_now = SWITCHING;
      break;

    case OFF:
      if (state_now != state_before) {
        Serial.println("Turning OFF"); Serial.println();
        action(OFF);
      }
      state_before = state_now;
      count = 0;
      if (range == IN_RANGE) state_now = SWITCHING;
      break;

    case SWITCHING:
      count++;
      // Switch on straight away
      if (range == IN_RANGE) {
        state_now = ON;
      }
      // Only switch off if signal is really, really out of range
      if ((range == OUT_OF_RANGE) && (count >= NUMBER_OF_BOUNCES)) {
        state_now = OFF;
      }
  }
}

void action(states state) {
  digitalWrite(LED_BUILTIN, state);
  digitalWrite(SWITCH_PIN, state);
}

void printDebugInfo() {
  Serial.print("State: ");
  switch (state_now) {
    case OFF: Serial.print("OFF"); break;
    case ON: Serial.print("ON"); break;
    case SWITCHING: Serial.print("SWITCHING");
  }
  Serial.print(" Count: "); Serial.print(count);
  Serial.print(" Range:");
  switch (range) {
    case OUT_OF_RANGE: Serial.println(" OUT_OF_RANGE"); break;
    case IN_RANGE: Serial.println(" IN_RANGE");
  }
  Serial.println();
}
