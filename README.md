# SomfyHomeControl
Node Red commands to Arduino which will send data to Aurel transmitter 433.42mhz, for communication with Somfy receivers (e.g. blinds,...).

Note that Somfy works with frequency 433.42Mhz.  Most transceivers work on 433.92Mhz, so order the right module and the correct antenna. (the datasheet is the one of 433.92Mz, but connections and functionality are similar to the 433.42Mhz version).

Arduino has Ethernet shield and additional shield with Aurel transciever and antenna for 433.42Mhz.

Node Red is sending commands over UDP to the Arduino.


![Arduino setup](https://github.com/pddpauw/SomfyHomeControl/assets/148589775/f6d6f1bb-5619-47e1-8f4a-da227e737340)
