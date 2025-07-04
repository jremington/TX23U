#include <Arduino.h>
//new version of library, new example code by SJR
#include <LaCrosse_TX23.h>

//DATA wire connected to arduino pin 2
LaCrosse_TX23 anemometer = LaCrosse_TX23(2);

void setup()
{
  Serial.begin(115200);
  while(!Serial) delay(1); 
  Serial.print("Bit time ");
  Serial.print(anemometer.getBitTime());  //FYI only, supposed to depend on temperature. I got 1216 us at 25C
  Serial.println(" us");
}

void loop()
{
  const char * dirTable[]= {"N","NNE","NE","ENE","E","ESE","SE","SSE","S","SSW","SW","WSW","W","WNW","NW","NNW"};
	float speed, gustspeed;
	uint16_t direction;
  
	if(anemometer.read(speed, direction, gustspeed))
  {
    Serial.print("Speed = ");
    Serial.print(speed,1);
    Serial.print(" Gust = ");
    Serial.print(gustspeed,1);
    Serial.print(" m/s");
    Serial.print(" Dir = ");
    Serial.print(direction);
    Serial.print("° ");
    Serial.println(dirTable[int(direction/22.5)]);    
  }
  else
  {
    Serial.println("Read error");
  }

	
	//delay between successive read requests must be at least 2sec, otherwise wind speed will read 0.
	delay(2000);
}
