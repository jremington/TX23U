/*************************************************************************************************
  LaCrosse_TX23.h - Library for reading LaCrosse TX23 anemometer data.
  
  LaCrosse TX23 is a wind speed and direction sensor. It uses 3 wires for communication and power:
  Pin1	Brown(Black)	DATA
  Pin2	Red				Vcc
  Pin3	Green			N/C
  Pin4	Yellow			GND
  
  DATA pin is to be connected directly to one of Arduino ports.
  
  
  
  Created by Krzysztof Egzmont, 06.11.2016.
  Based on https://www.john.geek.nz/2012/08/la-crosse-tx23u-anemometer-communication-protocol/
  Big thanks for John.
  
  Donations: 
  Bitcoin - 1JrHQwJWt1JNZepT1EXqPSioXcwD227nnU
  Ethereum - 0xFC9713013e78f6F14bA5064E36f875dbbB32B94a
  
  BSD license, all text above must be included in any redistribution
*************************************************************************************************/

#include <LaCrosse_TX23.h>

LaCrosse_TX23::LaCrosse_TX23(int pin, DelayFunc delayFunc) : 
	_pin(pin),
	_delay(delayFunc)
{
	pinMode(pin, INPUT);
}

void LaCrosse_TX23::pullBits(void *dst, bool *src, int count)
{
	uint8_t * d = (uint8_t*) dst;
	for (int i=0; i<count; i++)
	{
		d[i/8] |= src[i]<<(i%8);  //shift in bits from source array to destination variable (1 or 2 bytes)
	}	
}

bool LaCrosse_TX23::readRaw(uint16_t &speed, uint8_t &direction, uint16_t &gustspeed)
{
	speed = 0;
	direction = 0;

	digitalWrite(_pin,LOW); //trigger sensor
	pinMode(_pin,OUTPUT);
	_delay(500);
	pinMode(_pin,INPUT);  //pin pulled up by external resistor
	pulseIn(_pin,LOW);  //wait for LOW, then time to HIGH (start of start frame)

	const unsigned bitLen = 1200;  //estimated bit time in microseconds

	bool data[50];
	bool lastState = 1;
	unsigned long start = micros();
	for(unsigned long t = 0; t<50000; t = micros()-start)
	{
		bool state = digitalRead(_pin);
		unsigned bitNum = t/bitLen;
		if(t%bitLen>bitLen/2) data[bitNum] = state;
		if(state!=lastState)
		{
			unsigned delta = t%bitLen;  //adjust start time to compensate for bit rate
			if(delta<100)
				start -= delta;
			else if(delta>900)
				start += delta;
			lastState = state;
		}
	}

	uint8_t ctr = 0; pullBits(&ctr,data+0,5);
	uint8_t dir = 0; pullBits(&dir,data+5,4);
	uint16_t spd = 0; pullBits(&spd,data+9,12);
	uint8_t sum = 0; pullBits(&sum,data+21,4);
	uint8_t ndir = 0; pullBits(&ndir,data+25,4);  //inverted copy of ndir
	ndir ^= 0x0f;
	uint16_t nspd = 0; pullBits(&nspd,data+29,12); //inverted copy of nspd
	nspd ^= 0x0fff;

	uint8_t csum = 0x0f & (dir + (spd&0x0f) + ((spd>>4)&0x0f) + ((spd>>8)&0x0f));  //checksum

	if(ctr!=27) return false;  //0x1B start frame
	if(csum!=sum) return false; //bit errors in message
	if(spd!=nspd || dir!=ndir) return false; //bit errors in message

	speed = spd;
	direction = dir;

// second copy of message with presumably recent gust speed 
// (returns to lower values or zero after some number of intervals)

	while(digitalRead(_pin)==HIGH);
	while(digitalRead(_pin) == LOW);  //wait for LOW, then time to HIGH (start of start frame)
	lastState = 1;
	start = micros();
	for(unsigned long t = 0; t<50000; t = micros()-start)
	{
		bool state = digitalRead(_pin);
		unsigned bitNum = t/bitLen;
		if(t%bitLen>bitLen/2) data[bitNum] = state;
		if(state!=lastState)
		{
			unsigned delta = t%bitLen;  //adjust start time to compensate for bit rate
			if(delta<100)
				start -= delta;
			else if(delta>900)
				start += delta;
			lastState = state;
		}
	}

	ctr = 0; pullBits(&ctr,data+0,5);
	dir = 0; pullBits(&dir,data+5,4);
	spd = 0; pullBits(&spd,data+9,12);
	sum = 0; pullBits(&sum,data+21,4);
	ndir = 0; pullBits(&ndir,data+25,4);  //inverted copy of ndir
	ndir ^= 0x0f;
	nspd = 0; pullBits(&nspd,data+29,12); //inverted copy of nspd
	nspd ^= 0x0fff;

	csum = 0x0f & (dir + (spd&0x0f) + ((spd>>4)&0x0f) + ((spd>>8)&0x0f));  //checksum

	if(ctr!=27) return false;  //0x1B start frame
	if(csum!=sum) return false; //bit errors in message
	if(spd!=nspd || dir!=ndir) return false; //bit errors in message

	gustspeed = spd;
//	direction = dir;
	
	return true;
}

bool LaCrosse_TX23::read(float &speed, uint16_t &direction, float &gustspeed)
{
	uint16_t s, sg;
	uint8_t d;
	bool ok = readRaw(s,d,sg);
	speed = s*0.1;
	gustspeed = sg*0.1;
	direction = d * 22.5;
	return ok;
}

LaCrosse_TX23::RawData LaCrosse_TX23::readRaw()
{
	LaCrosse_TX23::RawData data;
	data.valid = readRaw(data.speed, data.direction, data.gustspeed);
	return data;
}

LaCrosse_TX23::Data LaCrosse_TX23::read()
{
	LaCrosse_TX23::Data data;
	data.valid = read(data.speed, data.direction, data.gustspeed);
	return data;
}

