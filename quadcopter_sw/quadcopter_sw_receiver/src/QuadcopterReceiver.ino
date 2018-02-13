#include <Servo.h>
#include <SPI.h>
//Download and import the RF24 library from https://github.com/maniacbug/RF24 to make the following inclusions work:
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
//Download and import the naza decoder library from http://www.rcgroups.com/forums/showthread.php?t=1995704
#include <NazaDecoderLib.h>

//DJI Naza accepts input in the form of commands from the Arduino Servo Library

Servo throttle;
Servo rudder;
Servo aileron;
Servo elevator;
Servo gear;

int echoPin = 30;
int triggerPin = 31;
int abstand=0;
int dauer=0;
double entfernung=0;
bool sendTrigger=true;
int valueRed;
int valueBlue;
int valueGreen;
double time;
int redLed[4];
int blueLed[4];
int greenLed[4];
int csnPin = 10;
int cePin = 9;
int throttlePin = 5;
int rudderPin = 6;
int elePin = 4;
int ailPin = 3;
int gearPin = 7;
int blueLedPin = 8;
int yellowLedPin = 11;
//int redLedPin = 12;
int megaLedPin = 13;
int throttleSetting = 1220; //1220 us correspnds to 0% thrust on Naza. We want to always know what the throttle setting is for safety reasons.
int gearSetting = 1855;
int lostRadioCount = 0;
int periode = 50;
int displace = 500;
// int LEDG = 45;
// int LEDR = 44;
uint32_t currTime, attiTime; //Left over from Naza Decoder test code. Probably useless, but keeping out of superstition
boolean armed = false;

const uint64_t pipe = 0xE8E8F0F0E1LL; // Define the transmit pipe
RF24 radio(cePin,csnPin); // Create a Radio
int flightControls[4];  // 4 element array holding Joystick readings

//********************TARGET LOCATION*******************
double targetLat = 35.0;
double targetLon = -86.0;

void setup()
{
  time=0;
  //Attach servos to appropriate pins


  throttle.attach(throttlePin);
  rudder.attach(rudderPin);
  aileron.attach(ailPin);
  elevator.attach(elePin);
  gear.attach(gearPin);
  pinMode(megaLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  //pinMode(redLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);
  valueRed=0;
  valueGreen=0;
  valueBlue=0;
  redLed[0]=2;
  greenLed[0]=45;
  blueLed[0]=46;
  redLed[1]=44;
  greenLed[1]=45;
  blueLed[1]=46;
  redLed[2]=44;
  greenLed[2]=45;
  blueLed[2]=46;
  redLed[3]=44;
  greenLed[3]=45;
  blueLed[3]=46;
  // pinMode(redLed[0],OUTPUT);
  // pinMode(greenLed[0],OUTPUT);
  // pinMode(blueLed[0],OUTPUT);
  // pinMode(LEDG,OUTPUT);
  // pinMode(LEDR,OUTPUT);
  //Initial throttle controls: Zero thrust, all other controls neutral.
  //Imperfections in system necessitate using 1470 as opposed to 1500 in some cases
  flightControls[0] = 1220;
  flightControls[1] = 1470;
  flightControls[2] = 1500;
  flightControls[3] = 1470;
  throttleSetting = flightControls[0]; //Redundant, but done mostly for code clarity.
  throttle.writeMicroseconds(throttleSetting);
  aileron.writeMicroseconds(flightControls[1]);
  rudder.writeMicroseconds(flightControls[2]);
  elevator.writeMicroseconds(flightControls[3]);
  gear.writeMicroseconds(gearSetting); //1525 sets dji naza to "Attitude Hold" mode
                                       //1185 sets to "Manual" Mode
                                       //1855 sets to "GPS" mode
  //RADIO INITIALIZATION
  Serial.begin(9600); //???? Does this need to be 115200? Does the RF Transmitter library depend on Serial? (Guess no)
  Serial3.begin(115200);
  delay(100);
  radio.begin();
  radio.openReadingPipe(1,pipe);
  radio.startListening();
  radio.setPALevel(RF24_PA_MAX);
  //radio.setChannel(0x34);
  //radio.setDataRate(RF24_1MBPS);
  //Serial.println("Setup starting. Waiting for gear command.");

  //Ultraschallsensoren
  pinMode(triggerPin,OUTPUT);
  pinMode(echoPin,INPUT);
  digitalWrite(triggerPin,LOW);
  sendTrigger=false;
  //GROUND SETUP SEQUENCE
  while (armed == false)
  {
    time=time+5;
    // if(time>270)
    // {
    //   time=0;
    // }
    valueRed=127+128*cos(2*3.1415/1.2*time);
    valueGreen=128+127*cos(2*3.1415/1.2*(200-time));
    //   if(time<0)
    //  {
    //    valueRed=255-(time*(-1));
    //    valueGreen=255;
    //  }
    //   else
    // {
    //     valueRed=255;
    //    valueGreen=255-time;
    //   }
    //    valueBlue=255;
    //led(1,valueRed,valueGreen,valueBlue);
    analogWrite(2,valueRed);
    analogWrite(12,valueGreen);
    //valueRed=analogRead(44);
    //Serial.println("analog="+(String)valueRed);
    //Serial.println("time="+ (String)time);
    //Serial.println("Sin="+ (String)valueRed);
    //Serial.println("value="+(String)valueBlue);
    Serial.println("Disarmed." + (String) flightControls[0] + " " + (String) flightControls[1] + " " + (String) flightControls[2] + " " + (String) flightControls[3]);
    //FLASHING LED TO SHOW DISARMED STATE
    delay(50);
    digitalWrite(megaLedPin, HIGH);
    delay(50);
    digitalWrite(megaLedPin, LOW);

    //LISTENING FOR ARMING SIGNAL AND NAZA MODE SELECTION
    if (radio.available())
    {
      bool done = false;
      while (!done)
      {
        // Fetch the data payload
        done = radio.read(flightControls, sizeof(flightControls));
      }
    }

  ////// Serial.println("Disarmed");


    //****************************ACQUIRE & REPORT GPS DATA***********************************
    if(Serial3.available())
    {
      uint8_t decodedMessage = NazaDecoder.decode(Serial3.read());
      switch (decodedMessage)
      {
        case NAZA_MESSAGE_GPS:
          Serial.print("Lat: "); Serial.print(NazaDecoder.getLat(), 7);
          Serial.print(", Lon: "); Serial.print(NazaDecoder.getLon(), 7);
          Serial.print(", Alt: "); Serial.print(NazaDecoder.getGpsAlt(), 7);
          Serial.print(", Fix: "); Serial.print(NazaDecoder.getFixType());
          Serial.print(", Sat: "); Serial.println(NazaDecoder.getNumSat());
          break;
        case NAZA_MESSAGE_COMPASS:
          Serial.print("Heading: "); Serial.println(NazaDecoder.getHeadingNc(), 2);
          break;
      }
    }
    //*******************************************************************************


    if (flightControls[3] == 1185) //If we are in Manual mode, switch lights to GPS mode
       {
         gearSetting  = 1185;
         //digitalWrite(redLedPin, LOW);
         digitalWrite(blueLedPin, HIGH);
         Serial.println("Gear set to 1185.");
       }
    else if (flightControls[3] == 1855) //If we are in GPS mode, switch lights to Manual mode
       {
         gearSetting = 1855;
         digitalWrite(blueLedPin, LOW);
        // digitalWrite(redLedPin, HIGH);
         Serial.println("Gear set to 1855.");
       }
    //ARMING SIGNAL RECEIVED
    if ((flightControls[0] == 1220) && (flightControls[1] == 1220) && (flightControls[2] == 1220) && (flightControls[3] == 1220))
    {
      for (int i = 0; i < 50; i++)
      {
        throttle.writeMicroseconds(flightControls[0]);
        aileron.writeMicroseconds(1850);
        rudder.writeMicroseconds(flightControls[2]);
        elevator.writeMicroseconds(flightControls[3]);
        delay(20);
      }
      digitalWrite(megaLedPin, LOW);
      armed = true;
      Serial.println("Armed.");
    }

    //Keep Naza thinking everything is OK, but do not send actual controller values.
    throttle.writeMicroseconds(1220);
    aileron.writeMicroseconds(1470);
    rudder.writeMicroseconds(1500);
    elevator.writeMicroseconds(1470);
    gear.writeMicroseconds(gearSetting);
  }
}
void loop()
{
  dauer++;
  if(sendTrigger==false)
  {
    digitalWrite(triggerPin,HIGH);
    sendTrigger=true;
    dauer=0;
  }
  else
  {
    if(digitalRead(echoPin)==HIGH)
    {
      entfernung=dauer/2;
      sendTrigger=false;
    }
  }
  Serial.println("Entfernung="+(String) entfernung);
  time=time+5;
  valueGreen=128+127*cos(2*3.1414/1.2*time);            ////////////////////////////////////SINFADE
  analogWrite(12,valueGreen);
  analogWrite(2,255);
  //led(1);
  //Serial.println("Armed." + (String) flightControls[0] + " " + (String) flightControls[1] + " " + (String) flightControls[2] + " " + (String) flightControls[3]);
  if (radio.available())
  {
    lostRadioCount = 0;
    bool done = false;
    while (!done)
    {
      // Fetch the data payload
      done = radio.read(flightControls, sizeof(flightControls));
     }
  }
  else
  {
    lostRadioCount = lostRadioCount + 1;
    if (lostRadioCount > 15000)
    {
      //Serial.println("EMERGENCY");
      emergencyLanding();
    }
  }
  throttleSetting = flightControls[0]; //We store the value of the throttle for safety during emergencyLanding()
  //Serial.println("Armed. t =" + (String) throttleSetting + " a = " + (String) flightControls[1] + " r = " + (String) flightControls[2] + " e = " + (String) flightControls[3]);
  throttle.writeMicroseconds(flightControls[0]);
  aileron.writeMicroseconds(flightControls[1]);
  rudder.writeMicroseconds(flightControls[2]);
  elevator.writeMicroseconds(flightControls[3]);
  digitalWrite(triggerPin,LOW);
}

void emergencyLanding()
{
//  Serial.println("in function");
  armed = false;
  valueRed=0;
  valueGreen=255;
  valueBlue=255;
  //led(1);
  aileron.writeMicroseconds(1470);
  rudder.writeMicroseconds(1500);
  elevator.writeMicroseconds(1470);
  //Serial.println("neutralized flightControls");

  int throttleDiff = throttleSetting - 1200;
  //Serial.println("throttleDiff calculated = " + (String) throttleDiff);

  for (throttleDiff; throttleDiff > 0; throttleDiff--)
  {
    throttleSetting = 1200 + throttleDiff;
    throttle.writeMicroseconds(throttleSetting);
  //  Serial.println("Landing. t = " + (String) throttleSetting);
    delay(10);
  }

  //Serial.println("throttle off");

  while (armed == false)
  {
    time=millis();
    valueRed=128+127*cos(2*PI/2000*time);
    valueGreen=128+127*cos(2*PI/2000*(500-time));
    valueBlue=255;
    //led(1);

    delay(250);
    digitalWrite(megaLedPin, HIGH);
    delay(250);
    digitalWrite(megaLedPin, LOW);
    if (radio.available())
    {
      bool done = false;
      while (!done)
      {
        // Fetch the data payload
        done = radio.read(flightControls, sizeof(flightControls));
      }
    }
    throttleSetting = flightControls[0];
  //  Serial.println("Crash landed. Waiting for arming sequence. t = " + (String) throttleSetting);
    if ((throttleSetting == 1220) && (flightControls[1] == 1220) && (flightControls[2] == 1220) && (flightControls[3] == 1220))
    {
      throttle.writeMicroseconds(throttleSetting);
      aileron.writeMicroseconds(flightControls[1]);
      rudder.writeMicroseconds(flightControls[2]);
      elevator.writeMicroseconds(flightControls[3]);
      lostRadioCount = 0;
      digitalWrite(megaLedPin, LOW);
      armed = true;
    }
  }

}
void led(int led, int red, int green, int blue) // faded in ms
{
  analogWrite(44,0);
  analogWrite(45,254);
  analogWrite(46,255);
  Serial.println("red = "+ (String)red +"green="+(String)green+"blue="+(String)blue);
}
