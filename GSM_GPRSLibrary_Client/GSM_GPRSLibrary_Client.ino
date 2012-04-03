#include "uBlox.h"
#include <SoftwareSerial.h>

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
char ip[15];
int i=0;
int socket;
boolean started=false;

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
  }
  else Serial.println("\nstatus=IDLE");

  if(started){
    //GPRS attach, put in order APN, username and password.
    //If no needed auth let them blank.
    if (gsm.attachGPRS(0,"internet", "guest", "guest"))
    {
      Serial.println("status=ATTACHED");
      delay(1000); 
      //Read IP address.
      gsm.ip(ip);
      delay(100);
      Serial.println(ip);
      Serial.println(gsm.signal(0));
      if(gsm.connectTCP("echo.u-blox.com",7,socket))
      {
        delay(2000);
        gsm.WhileSimpleRead();   
        delay(1000);  
        gsm.disconnectTCP(socket);
      }
    }
    else Serial.println("status=ERROR");
  }
}

void loop() 
{
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


