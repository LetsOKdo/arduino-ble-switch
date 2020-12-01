# Arduino BLE Switch

An Arduino sketch acting as a switch when a BLE peripheral is in close proximity.

MIT License

Copyright (c) 2020 Peter Milne

Set the UUID of the peripheral service to be scanned for:

```
const char* UUID = "FD6F"; // Exposure service
```

Set minimum RSSI value to adjust a suitable range for the switch:

```
const int MIN_RSSI = -85;
````

Set timeout for discovery callback in ms

```
const int TIMEOUT = 1000;
```

## ArduinoBLESwitchMode

This version uses a mode calculation to debounce signal strength.

Set the RSSI_COUNT to control switching sensitivity:

```
const int RSSI_COUNT = 4;
```

## ArduinoBLESwitch
This version uses number of bounces to debounce signal strength. 

Set the NUMBER_OF_BOUNCES value to adjust sensitivity:

```
const int NUMBER_OF_BOUNCES = 3;

``` 

  
