#include "uBlox.h"
#include <SoftwareSerial.h>
#include <Streaming.h>
//Simple sketch to start a connection as client.
/*
  GSM board power test
 
 D2 : reset sense
 D3 : MSIO
 D4 : MOSI
 D5 : GSM powerup signal
 D6 : GSM reset signal
 D7 : GSM PSU control
 */
char msg[50];
int numdata;
char inSerial[50];
int i=0;
int socket;
boolean started=false;
long date;
unsigned int time;
double lon,lat;
int alt,cert;

#define smillis() ((long)millis())       // This is used to calculate timeouts
#define INTERVAL 60000
long update;

void setup() 
{
  //Serial connection.
  Serial.begin(9600);
  Serial.println("GSM Shield testing.");
  //Start configuration of shield with baudrate.
  //For http uses is raccomanded to use 4800 or slower.
  if (gsm.begin(9600))
  {
    Serial.println("\nstatus=READY");
    started=true;  
  //  gsm.SimpleWrite("AT+UGAOP=\"eval1-les.services.u-blox.com\",46434,1000,0\r");
  }
  
  else Serial.println("\nstatus=IDLE");
  gsm.gps(5); // mode 5 meand all possible aiding features are used
 // update= smillis()+INTERVAL;  // "reset" the timer to update again in the next interval
   gsm.locate(5,date,time,lon,lat,alt,cert);
  Serial << "date: " <<date<<endl<<"time: " <<time<<endl<<"Lon: " <<lon<<endl<<"lat: " <<lat<<endl<<"alt " <<alt<<endl<<"certainty: " <<cert<<endl;
  Serial <<lon<<", " <<lat;
// Serial << "setup done"<< endl;
}

void loop() 
{
  /*
  if(after(update))
  {
  gsm.locate(5,date,time,lon,lat,alt,cert);
  Serial << "date: " <<date<<endl<<"time: " <<time<<endl<<"Lon: " <<lon<<endl<<"lat: " <<lat<<endl<<"alt " <<alt<<endl<<"certainty: " <<cert<<endl;
  Serial <<lon<<", " <<lat;
  update= smillis()+INTERVAL;
  }
  */
  //Read for new byte on serial hardware,
  //and write them on NewSoftSerial.
  serialhwread();
  //Read for new byte on NewSoftSerial.
  serialswread();
}

void serialhwread(){
  i=0;
  if (Serial.available() > 0){            
    while (Serial.available() > 0) {
      inSerial[i]=(Serial.read());
      delay(10);
      i++;      
    }

    inSerial[i]='\0';
    if(!strcmp(inSerial,"/END")){
      pinMode(6,OUTPUT);
      digitalWrite(6,LOW);
      delay(200);
      pinMode(6,INPUT);

      gsm.SimpleWriteln("AT+CPWROFF");
    }
    //Send a saved AT command using serial port.
    if(!strcmp(inSerial,"TEST")){
      Serial.println("SIGNAL QUALITY");
      gsm.SimpleWriteln("AT+CSQ");
    }

    //Read last message saved.
    if(!strcmp(inSerial,"MSG")){
      Serial.println(msg);
    }
    else{
      Serial.println(inSerial);
      gsm.SimpleWriteln(inSerial);
    }    
    inSerial[0]='\0';
  }
}

void serialswread(){
  gsm.SimpleRead();
}

boolean after(long timeout)                    // Returns true if a timeout has happened
{
  return smillis()-timeout>0;
}


