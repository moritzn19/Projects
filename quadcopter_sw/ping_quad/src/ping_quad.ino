/*
Hi, i'm glad you downloaded my arducode, please remember to take a look to my website http://giuseppeurso.net
where you will find updates and troubleshooting tips.

*/

#include "C:\Users\moritz\Documents\PlatformIO\Projects\quadcopter_sw\ping_quad\lib\Ultrasonic-HC-SR04-master\Ultrasonic\Ultrasonic.h"
#include <Servo.h>
#include <Wire.h>
#include <NewPing.h>

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on  the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 400 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.


Servo myThrottleChOut;



int throttlePin1 = 9; //output
int pinReadCh6 = 0; //Reads channel 6 from RX
int pinReadCh3 = 3; //Reads channel 3 from RX
int led = 10;
unsigned long throttle_read;
unsigned long alt_hold_switch;
unsigned long throttle_output; //duration to use inside the altitude hold loop

void setup() {
Serial.begin(9600);
pinMode(led, OUTPUT);
pinMode(pinReadCh3, INPUT);
pinMode(pinReadCh6, INPUT);
myThrottleChOut.attach(throttlePin1); //attach the "servo" to digital pin 7, this is the digital pin which is going to write to the KKboards throttle channel.

}

void loop()
{

  throttle_read = pulseIn(pinReadCh3, HIGH); //Reads the throttle level, returns 1150 - 1839 microseconds. 1 millisecond = 1000 microseconds -> throttle_read/1000: ex 1,736ms
  alt_hold_switch = analogRead(pinReadCh6); //Reads the ch6 level
  alt_hold_switch= map(alt_hold_switch, 0, 1024, 0, 1800);
  Serial.print("alt_hold_switch=");
  Serial.println(alt_hold_switch);
  unsigned int uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
        Serial.print("Ping: ");
        Serial.print(uS / US_ROUNDTRIP_CM); // Convert ping time to distance in cm and print result (0 = outside set distance range)
        Serial.println("cm");
        int distance = (uS / US_ROUNDTRIP_CM); //Gets the altitude when entering altitude hold mode, this will be a reference
        throttle_output = throttle_read;

  if (alt_hold_switch_neu > 1700) //If the ch6 knob is turned almost to max, arduino enter altitude hold mode
  {



        while (alt_hold_switch_neu > 1700) //If the ch6 knob is turned almost to max, arduino enters altitude control loop.
        {
            digitalWrite(led, HIGH);
            unsigned int uS2 = sonar.ping(); // Send ping, get ping time in microseconds (uS).
            int newDistance = ((uS2 / US_ROUNDTRIP_CM)); //Gets the current altitude, this will be compared to the reference altitude
            if (newDistance > distance + 3) //+ 10 will give an intervall between desired altitude +- 10 cm in the upper and lower bounds.
            {
                Serial.print("Reference Distance");
                Serial.print(distance);
                Serial.print("New Distance:");
                Serial.println(newDistance);
                if(throttle_output > 1350) //this is to prevent the tricopter to throttling down to no power and fall from the sky
                {
                throttle_output -= 1;//autolanding feature to be added here
                Serial.print("throttle output:");
                Serial.println(throttle_output);
                }
                else
                {
                 //unarm the altitude holding sysstem or BETTER simply LAND...
                }

            }
            else if(newDistance < distance - 3) // - 10 will give an intervall between desired altitude +- 10 cm in the upper and lower bounds.
            {


              if(throttle_output < 1680) //this is to prevent the tricopter from throttling up to max throttle
              {
                throttle_output += 1;


              }

            }
            else
            {
               //do nothing
               throttle_output = throttle_output;
               Serial.print("DISTANCE NOT CHANGED");
               delay(200);

            }
             myThrottleChOut.write(throttle_output);
             Serial.print("trottle output:");
                Serial.println(throttle_output);
                alt_hold_switch = pulseIn(pinReadCh6, HIGH); //Reads the ch6 level, this is needed to be able to exit the while-iteration
        delay(20);
        }
  }
  else
  {


          digitalWrite(led, LOW);
          Serial.println("bypass");
          myThrottleChOut.write(throttle_read);
          alt_hold_switch = pulseIn(pinReadCh6, HIGH); //Reads the ch6 level, this is needed to be able to exit the while-iteration

  }

  delay(20);
}
