# SomfyHomeControl
Node Red commands to Arduino which will send data to Aurel transmitter 433.42mhz, for communication with Somfy receivers (e.g. blinds,...).

Note that Somfy works with frequency 433.42Mhz.  Most transceivers work on 433.92Mhz, so order the right module and the correct antenna.

Arduino has Ethernet shield and additional shield with Aurel transciever and antenna for 433.42Mhz.

Node Red is sending commands over UDP to the Arduino.
