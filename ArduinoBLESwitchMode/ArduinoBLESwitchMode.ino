/*
  Arduino BLE switch

  Scans for BLE peripherals based on service UUID - switches state when
  peripheral goes in or out of range, using signal strength mode calculation.

  Compatible boards:
  Arduino MKR WiFi 1010, Arduino Nano 33 IoT,
  Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense

  MIT License

  Copyright (c) 2020 Peter Milne
*/

#include <ArduinoBLE.h>

const int MIN_RSSI = -85;  // RSSI switching threshold
const int RSSI_COUNT = 4;  // Number of RSSI values for mode calc
const int TIMEOUT = 1000; // Timeout in ms

const char* UUID = "FD6F"; // Exposure service
const byte SWITCH_PIN =  21;  // Switch

enum ranges {OUT_OF_RANGE = LOW, IN_RANGE = HIGH};
ranges range_now, range_before = OUT_OF_RANGE;
static int count = 0;  // Counter
int rssiArray[RSSI_COUNT] = {0}; // Store RSSI values

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

  // Save RSSI values for mode calc
  if (count < RSSI_COUNT) {
    rssiArray[count++] = rssi;
    return;
  }

  Serial.print("Count: "); Serial.print(count);
  count = 0;
  int modeRSSI = mode(rssiArray, RSSI_COUNT);
  Serial.print(" Mode: "); Serial.print(modeRSSI);
  range_now = modeRSSI > MIN_RSSI ? IN_RANGE : OUT_OF_RANGE;
  Serial.print(" Range: "); Serial.println(rangeToStr(range_now));
  Serial.println();

  // Only switch when state changes
  if (range_now != range_before) {
    range_before = range_now;
    if (range_now == IN_RANGE) {
      Serial.println(" Turning ON");
      Serial.println();
      action(IN_RANGE);
    }
    else {
      Serial.println(" Turning OFF");
      Serial.println();
      action(OUT_OF_RANGE);
    }
  }
}

void action(ranges range) {
  digitalWrite(LED_BUILTIN, range);
  digitalWrite(SWITCH_PIN, range);
}

// Statistical mode
int mode(int a[], int n) {
  int maxValue = 0, maxCount = 0, i, j;
  for (i = 0; i < n; ++i) {
    int count = 0;
    for (j = 0; j < n; ++j) {
      if (a[j] == a[i])
        ++count;
    }
    if (count > maxCount) {
      maxCount = count;
      maxValue = a[i];
    }
  }
  return maxValue;
}

String rangeToStr(ranges range) {
  switch (range) {
    case OUT_OF_RANGE: return String("OUT_OF_RANGE"); break;
    case IN_RANGE: return String("IN_RANGE");
  }
}
