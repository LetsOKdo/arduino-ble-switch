# Arduino BLE Switch

An Arduino sketch acting as a switch when a BLE peripheral is in close proximity.

MIT License

Copyright (c) 2020 Peter Milne

Set the UUID of the peripheral service to be scanned for:

```
const char* UUID = "FD6F"; // Exposure service
```

Set minimum RSSI value in dB to adhjust a suitable range for the switch:

```
const int MIN_RSSI = -85;
```

Set the number of bounces and timeout values to adjust sensitivity:

```
const int NUMBER_OF_BOUNCES = 3;
const int TIMEOUT = 1000; // Timeout in ms
``` 

  
