#include <NewPing.h>
#include "C:\Users\moritz\Documents\PlatformIO\Projects\quadcopter_sw\UltrasonicMavlink-master\lib\mavlink\common\mavlink.h"




// NewPing sonar0(3, 4, 250);
// NewPing sonar1(5, 6, 250);
// NewPing sonar2(7, 8, 250);
// NewPing sonar3(9, 10, 250);
// NewPing sonar4(11, 12, 100);

// unsigned long pingTimer;
// unsigned int pingSpeed = 50;

int aileron = 0;
int elevator = 5;

//===============================================================================

void setup()
 {

  Serial.begin(9600);
  //pingTimer = millis();
  pinMode(aileron, INPUT);
  pinMode(elevator, INPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

}




//=======================================================================

void loop() {

  int aileron = pulseIn(aileron, HIGH);
  int elevator = pulseIn(elevator, HIGH);



  analogWrite(12, aileron);
  analogWrite(13, elevator);

  Serial.println(aileron);
  Serial.println(elevator);


}
