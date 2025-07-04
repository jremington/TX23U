# TX23U
La Crosse wind speed and direction Arduino interface library

Based on https://www.john.geek.nz/2012/08/la-crosse-tx23u-anemometer-communication-protocol/

Forked from https://github.com/egzumer/Arduino-LaCrosse-TX23-Library/blob/master/LaCrosse_TX23.cpp

This library reads both first **and second** datagrams, the first contained exponential or moving average wind speed values, with 10 to 20 second time constant, the second containing wind gust peak values over the last 10 or 20 seconds.

LaCrosse TX23 is a wind speed and direction sensor. It uses 3 wires for communication and power:

Pin1 - Brown(Black) - DATA
Pin2 - Red - Vcc (3.3V works, not sure if 5V OK)
Pin3 - Green - N/C
Pin4 - Yellow - GND

DATA pin is to be connected directly to one of Arduino ports.
