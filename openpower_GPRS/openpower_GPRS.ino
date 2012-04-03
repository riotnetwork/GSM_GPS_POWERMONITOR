#include <SoftwareSerial.h>
#include <uBlox.h>
#include <PMStreaming.h>
#include <Streaming.h>
#include <PowerMonitor.h>
#include <TextFinder.h>
#include <RunningAverage.h>

//RunningAverage vrmsAve(60); // average for the last minute of the interested parameters
  RunningAverage powerAve(60);
//RunningAverage tempAve(60);
//RunningAverage freqAve(60);
SoftwareSerial debug(3,4);
int sample = 30; // seconds of samples to average

#define INTERVAL 1000                    // update interval in milliseconds
#define smillis() ((long)millis())       // This is used to calculate timeouts
long updatetime;                         // used to store the time till next upodate

PowerMonitor pmon;            // instance of a powermonitor
int socket;
boolean update_now= false;
int message_length; // used to hold the message lentgh

const int fixedmassagelength = 80; // the length of th eabove without any values
#define ledpin 13
void setup()  
{
  pinMode(ledpin,OUTPUT);
  digitalWrite(ledpin,HIGH);
  debug.begin(9600);
  debug<<F("gsm start");
  gsm.begin(9600);    // u-blox gsm on here // used to set th e9600 baud, but now the library does that...
  debug<<F("gsm started");
  pmon.begin(220,50); // 220V and 50Hz are nominal parameters for south africa
  debug<<F("pmon started");
  digitalWrite(ledpin,LOW);
  updatetime= smillis(); // immediatly update after startup
}

void loop() // run over and over
{
  if(after(updatetime)) // 1 second has elapsed, we want to get a new set of pmon values
  {
    digitalWrite(ledpin,HIGH);
    getvalues ();
    digitalWrite(ledpin,LOW);
  }//updatetime if

  if(update_now) // do we have 1 minutes wothr of average?
  {//send things to server
    // make the evalues strings and find out how long they are
debug<<F("update started\r\n");
    String vrmsstring=String(pmon.vrms(1),2);
    String powerstring=String(powerAve.avg(),2);
    String tempstring=String(pmon.Temperature(1));
    String freqstring=String(pmon.lineFrequency(1),2);
  
    message_length = fixedmassagelength+ vrmsstring.length()+powerstring.length()+tempstring.length()+freqstring.length(); // length of all the values

    update_now=false; // reset our updae indicator
    if (gsm.attachGPRS(0,"internet", "guest", "guest"))
    {
      digitalWrite(ledpin,HIGH);
      if(gsm.connectTCP("ec2-75-101-172-70.compute-1.amazonaws.com",8081,socket))
      { // we have an open tunnel to the server... stream some data
      gsm.SimpleWrite(F("POST /v1/device/6/obs?username=mikkel&password=quizzy HTTP/1.1\nHost: ec2-75-101-172-70.compute-1.amazonaws.com:8081:\nContent-Length: "));
      gsm.SimpleWrite(message_length);
      gsm.SimpleWrite(F("\nContent-Type: application/json\n\n"));
      gsm.SimpleWrite(F("{\"doctype\":\"1\",\"voltage\":\"")); // vrmsAve.avg()
      gsm.SimpleWrite(vrmsstring);
      gsm.SimpleWrite(F("\",\"power\":\""));// powerAver.avg()
      gsm.SimpleWrite(powerstring);
      gsm.SimpleWrite(F("\",\"temperature\":\""));// tempAve.avg().
      gsm.SimpleWrite(tempstring);
      gsm.SimpleWrite(F("\",\"freq\":\""));// freqAve.avg()
      gsm.SimpleWrite(freqstring);
      gsm.SimpleWrite(F("\",\"device\":\"6\"}"));
        delay(2000);
      gsm.disconnectTCP(socket); //disconnect after sending
      gsm.detachGPRS(0);
      }//tcp if
    }//gprs attache if 
    digitalWrite(ledpin,LOW);
    powerAve.clr();
  }//update if
}// loop


boolean after(long timeout)                    // Returns true if a timeout has happened
{
  return smillis()-timeout>0;
}


