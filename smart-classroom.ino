

#include<DS3231.h>
#include<String.h>
byte relayPin=9;
byte relayFan=12;
byte fanState=LOW;
byte state=LOW;
byte inPin=2;
byte reading;
byte previous=LOW;
long samay=0;
long debounce=200;
byte ldrSensor=A0;
byte flag=0;
int calibrationTime = 15;
byte pirPin = 7; 

long unsigned int lowIn;         

long unsigned int pause = 5000;  

long stime=0;;

boolean lockLow = true;
boolean takeLowTime;


DS3231  rtc(SDA, SCL);
void setup() {
   

  digitalWrite(relayPin,HIGH);
  digitalWrite(relayFan,HIGH);
  pinMode(relayPin,OUTPUT);
  pinMode(ldrSensor,INPUT);
  pinMode(relayFan,OUTPUT);
 rtc.begin();
 Serial.begin(9600);
   Serial.print("calibrating sensors ");
    for(int i = 0; i < calibrationTime; i++){
      Serial.print(".");
      delay(500);
      }
    Serial.println(" done");
    Serial.println("SENSORS ACTIVE");
          delay(50);
          digitalWrite(relayPin,HIGH);
  digitalWrite(relayFan,HIGH);
 rtc.setDOW(TUESDAY);   
 rtc.setTime(18,28,00);
 rtc.setDate(24,10,2017);

 
}

void loop()
{
  //Serial.println(rtc.getDOWStr());
 // Serial.print("---- ");
 // Serial.print(rtc.getDateStr());
 // Serial.println(" -- ");
  //Serial.println(rtc.getTimeStr());
    Serial.print("Temperature: ");
  Serial.print(rtc.getTemp());
  Serial.println(" C");
  clock_relay();
  push_relay();
  ldr_sensor();
  temp_relay();
  check_pir();
}
void clock_relay()
{
if((!strcmp(rtc.getDOWStr(),"Monday"))||(!strcmp(rtc.getDOWStr(),"Tuesday"))||(!strcmp(rtc.getDOWStr(),"Wednesday"))||(!strcmp(rtc.getDOWStr(),"Thursday"))||(!strcmp(rtc.getDOWStr(),"Friday")))
 {
  if(!strcmp(rtc.getTimeStr(),"18:28:10"))
  {
    fanState=state=HIGH;
  }
  if(!strcmp(rtc.getTimeStr(),"18:28:20"))
  {
    fanState=state=HIGH;
  }
  if(!strcmp(rtc.getTimeStr(),"18:28:30"))
  {
    fanState=state=HIGH;
  }
 }
 digitalWrite(relayPin,state);
}
void push_relay()
{
  reading=digitalRead(inPin);
  if(reading==HIGH&&previous==LOW&&millis()-samay>debounce)
  {
    if(state==LOW)
    state=HIGH;
    else
    state=LOW;
    samay=millis();
    fanState=state; 
  }
  digitalWrite(relayPin,state);
  digitalWrite(relayFan,fanState);
  previous=reading;
}
void ldr_sensor()
{
  unsigned int sun;
  sun= analogRead(ldrSensor);
  Serial.print(sun);
  if(sun<100)
  {
    state=HIGH;
  }
  digitalWrite(relayPin,state);
}
void temp_relay()
{
   //Serial.print("Temperature: ");
  //Serial.print(rtc.getTemp());
  //Serial.println(" C");
  if(rtc.getTemp()>35)
  {
    fanState=HIGH;
  }
  digitalWrite(relayFan,fanState);
}
void check_pir()
{
    if(digitalRead(pirPin) == HIGH){
     //  digitalWrite(relayPin,HIGH);   //the led visualizes the sensors output pin state
       if(lockLow){  
         //makes sure we wait for a transition to LOW before any further output is made:
         lockLow = false;            
         Serial.println("---");
         Serial.print("motion detected at ");
         Serial.print(millis()/1000);
         stime=(millis()/1000);
         Serial.println(" sec"); 
         delay(50);
         }         
         takeLowTime = true;
       }
  Serial.println(rtc.getTimeStr());
     if(digitalRead(pirPin) == LOW){       
     //  digitalWrite(ledPin, LOW);  //the led visualizes the sensors output pin state

       if(takeLowTime){
        lowIn = millis();          //save the time of the transition from high to LOW
        takeLowTime = false;       //make sure this is only done at the start of a LOW phase
        }
       //if the sensor is low for more than the given pause, 
       //we assume that no more motion is going to happen
       if(!lockLow && millis() - lowIn > pause){  
           //makes sure this block of code is only executed again after 
           //a new motion sequence has been detected
           lockLow = true;                        
           Serial.print("motion ended at ");      //output
           Serial.print((millis() - pause)/1000);
           Serial.println(" sec");
           delay(50);
           }
       }
       if(((millis()/1000)-stime)>30)
       {
       state=HIGH;
       fanState=HIGH;
       }
       digitalWrite(relayPin, state);
       digitalWrite(relayFan,fanState);
}

