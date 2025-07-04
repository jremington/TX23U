# TX23U
La Crosse wind speed and direction Arduino interface library

Based on https://www.john.geek.nz/2012/08/la-crosse-tx23u-anemometer-communication-protocol/

Forked from https://github.com/egzumer/Arduino-LaCrosse-TX23-Library/blob/master/LaCrosse_TX23.cpp

This library reads both first **and second(*)** datagrams, the first contains exponential or moving average wind speed values, with 10 to 20 second time constant, the second contains wind gust peak value during the last 10 to 20 seconds. Also added function getBitTime() to determine the length of a data bit in microseconds from the datagram start frame.

LaCrosse TX23 is a wind speed and direction sensor. It uses 3 wires for communication and power:
```
Pin1 - Brown(Black) - DATA
Pin2 - Red - Vcc (3.3V works, don't know if 5V OK)
Pin3 - Green - N/C
Pin4 - Yellow - GND
```
DATA pin is to be connected directly to one of Arduino digital input pins, with **external 10K pullup** to Vcc.

(*) See reader comments at end of https://www.john.geek.nz/2012/08/la-crosse-tx23u-anemometer-communication-protocol/

Logic analyzer capture of a valid TX23U transmission showing the two back-to-back datagrams. In this case the wind speed and gust speed are both zero, so the two datagrams have identical values. The "double hump" is the start sequence of each data frame.


![TX23U_datagrams](https://github.com/user-attachments/assets/651a8f8d-b067-4187-9e43-20c105912102)
