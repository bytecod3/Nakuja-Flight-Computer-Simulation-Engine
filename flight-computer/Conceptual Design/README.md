# Flight-Computer-Conceptual-Design 
## Broad outline of FC PCB
### Components
* Microcontroller Unit - ESP32 WROOM
* MPU Sensor - Measures acceleration and gyroscopic data to give orientation
* BMP Sensor - Measures barometric pressure to estimate altitude
* GPS Module - Provides geolocation data
* XBEE Module - For wireless communication
* Flash Memory - Two 16MB to be daisy chained
* Data Dump - Allows data extraction from flash memory
* COTS Module - Proposed interface: Stratologger
* User LED's - Indicates states: Handshake, Receive, Confirm
  
External Interfaces: XBEE, Data Dump, COTS and Power Board
