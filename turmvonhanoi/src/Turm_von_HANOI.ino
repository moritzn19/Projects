#include<Servo.h>

#define OUT1 2
#define OUT2 3
#define OUT3 4
#define OUT4 5
#define OUT5 6
#define OUT6 7
#define OUT7 8
#define OUT8 9
#define OUT9 10
#define OUT0 11

const int phases1[] = {0, 0, 0, 0, 0, 1, 1, 1};
const int phases2[] = {0, 0, 0, 1, 1, 1, 0, 0};
const int phases3[] = {0, 1, 1, 1, 0, 0, 0, 0};
const int phases4[] = {1, 1, 0, 0, 0, 0, 0, 1};

int h1=50;
int h2=65;
int h3=80;
int auf=35;
int zu=90;
int s1=60; //s: Stange
int s2=90;
int s3=120;
int PhaseL = 0;
int PhaseR = 0;
int Speed = 30; //MUST BE 1 - 100
Servo g; //Greifarm
Servo m; //Mitte

///////////////////////////////////////void setup()////////////////////////////////

void setup()
{
pinMode(OUT1, OUTPUT);
pinMode(OUT2, OUTPUT);
pinMode(OUT3, OUTPUT);
pinMode(OUT4, OUTPUT);
pinMode(OUT5, OUTPUT);
pinMode(OUT6, OUTPUT);
pinMode(OUT7, OUTPUT);
pinMode(OUT8, OUTPUT);
g.attach(OUT0);
m.attach(OUT9);

Serial.begin(9600);
}

//////////////////////////////////////////////void loop()//////////////////////////

void loop()
{
  moveG(auf);//1
  moveM(s1);
  delay(20000);
  moveG(zu);//2
  moveBoth(h1);
  moveM(s3);
  moveBoth(-h3);
  //nextStick(3);
  moveG(auf);//3
  moveBoth(h3);
  moveM(s1);
  moveBoth(-h2);
  //nextStick(1);
  moveG(zu);//3
  moveBoth(h2);
  moveM(s2);
  moveBoth(-h3);
  //nextStick(2);
  moveG(auf);//4
  moveBoth(h3);
  moveM(s3);
  moveBoth(-h3);
  //nextStick(3);
  moveG(zu);//5
  moveBoth(h3);
  moveM(s2);
  moveBoth(-h2);
  //nextStick(2);
  moveG(auf);//6
  moveBoth(h2);
  moveM(s1);
  moveBoth(-h3);
  //nextStick(1);
  moveG(zu);//7
  moveBoth(h3);
  moveM(s3);
  moveBoth(-h3);
  //nextStick(3);
  moveG(auf);//8
  moveBoth(h3);
  moveM(s2);
  moveBoth(-h2);
  //nextStick(2);
  moveG(zu);//9
  moveBoth(h2);
  moveM(s1);
  moveBoth(-h3);
  //nextStick(1);
  moveG(auf);//10
  moveBoth(h3);
  moveM(s2);
  moveBoth(-h3);
  //nextStick(2);
  moveG(zu);//11
  moveBoth(h3);
  moveM(s3);
  moveBoth(-h2);
  //nextStick(3);
  moveG(auf);//12
  moveBoth(h2);
  moveM(s1);
  moveBoth(-h3);
  //nextStick(1);
  moveG(zu);//13
  moveBoth(h3);
  moveM(s3);
  moveBoth(-h1);
  //nextStick(3);
  moveG(auf);//14
  moveBoth(h1);
  moveM(s1);
  moveBoth(-h1);
}

/////////////////////////Methoden////////////////////////////////

void nextStick(int stick)
{
  moveBoth(90);
  if(stick==1);
  {
    moveM(60);
  }
  if(stick==2);
  {
    moveM(90);
  }
  if(stick==3);
  {
    moveM(120);
  }
  moveBoth(-90);
}


void moveBoth(int grad)
{
      if(grad<0)
      {
        grad=grad*-1;
        for(int i=0;i<grad;i++)
        {
          moveR(1);
          moveL(-1);
        }
      }
      else
      {
         for(int i=0;i<grad;i++)
         {
          moveR(-1);
          moveL(1);
         }
      }
}


void moveL(int countGrad)
{
  int count;
  count=degreeToSteps(countGrad);
  int rotationDirection = count < 1 ? -1 : 1;
  count *= rotationDirection;
  for(int x = 0; x < count; x++)
  {
    digitalWrite(OUT1, phases1[PhaseL]);
    digitalWrite(OUT2, phases2[PhaseL]);
    digitalWrite(OUT3, phases3[PhaseL]);
    digitalWrite(OUT4, phases4[PhaseL]);
    IncrementPhaseL(rotationDirection);
    delay(100/Speed);
  }
}


void moveR(int countGrad)
{
  int count;
  count=degreeToSteps(countGrad);
  int rotationDirection = count < 1 ? -1 : 1;
  count *= rotationDirection;
  for (int x = 0; x < count; x++)
  {
    digitalWrite(OUT5, phases1[PhaseR]);
    digitalWrite(OUT6, phases2[PhaseR]);
    digitalWrite(OUT7, phases3[PhaseR]);
    digitalWrite(OUT8, phases4[PhaseR]);
    IncrementPhaseR(rotationDirection);
    delay(100/Speed);
  }
}


void moveM(int grad)
{
  int gradAlt=m.read();
  if(grad<gradAlt)
  {
    for (int i=gradAlt;i>grad;i--)
    {
       m.write(i);
       delay(15);
    }
  }
  else
  {
     for (int i=gradAlt;i<grad;i++)
    {
       m.write(i);
       delay(15);
    }
  }
}


void moveG(int grad)
{
    int gradAlt=g.read();
  if(grad<gradAlt)
  {
    for (int i=gradAlt;i>grad;i--)
    {
       g.write(i);
       delay(15);
    }
  }
  else
  {
     for (int i=gradAlt;i<grad;i++)
    {
       g.write(i);
       delay(15);
    }
  }
}


void IncrementPhaseL(int rotationDirection)
{
PhaseL += 8;
PhaseL += rotationDirection;
PhaseL %= 8;
}
void IncrementPhaseR(int rotationDirection)
{
PhaseR += 8;
PhaseR += rotationDirection;
PhaseR %= 8;
}
int degreeToSteps(int grad)
{
    int i=4096/360*grad;
    return i;
}
