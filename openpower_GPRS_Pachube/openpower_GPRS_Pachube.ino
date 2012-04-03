#include <SoftwareSerial.h>
#include <uBlox.h>
#include <PMStreaming.h>
#include <Streaming.h>
#include <PowerMonitor.h>
#include <TextFinder.h>
#include <RunningAverage.h>

//RunningAverage vrmsAve(60); // average for the last minute of the interested parameters
//RunningAverage powerAve(60);
//RunningAverage tempAve(60);
//RunningAverage freqAve(60);
//SoftwareSerial debug(3,4);
int sample = 0; // seconds of samples to average

#define INTERVAL 1000                    // update interval in milliseconds
#define smillis() ((long)millis())       // This is used to calculate timeouts
long updatetime;                         // used to store the time till next upodate
long resettime;                         // used to store the time till next upodate
#define RESETINTERVAL 3600000                // reset modem every hour


PowerMonitor pmon;            // instance of a powermonitor

boolean update_now= false;
int message_length; // used to hold the message lentgh
int turnedOn =0;
const int fixedmassagelength = 27; // the length of th eabove without any values31/34
#define ledpin 13
void setup()  
{
  pinMode(ledpin,OUTPUT);
  digitalWrite(ledpin,HIGH);
  //  debug.begin(9600);
  //  debug<<F("gsm start");
 turnedOn= gsm.begin(9600);    // u-blox gsm on here // used to set th e9600 baud, but now the library does that...
  // debug<<F("gsm started");
  pmon.begin(220,50); // 220V and 50Hz are nominal parameters for south africa
  // debug<<F("pmon started");
  digitalWrite(ledpin,LOW);
  updatetime= smillis(); // immediatly update after startup
  resettime=  smillis() +RESETINTERVAL;
  delay(1);
  // gsm.poweroff();
}

void loop() // run over and over
{
  if(after(resettime)) // // re register with network every hour to keep gprs profile alive
  {
    gsm.softReset();
    updatetime= smillis()+INTERVAL;  // "reset" the timer to update again in the next interval
    resettime=  smillis() +RESETINTERVAL; // re register with network agin in 1 hour
  }

  if(after(updatetime)) // 1 second has elapsed, we want to get a new set of pmon values
  {
    digitalWrite(ledpin,HIGH);
    sample++;
    if (sample>=40)
    {
      sample =0; // resets the sample number
      update_now = true; // we have 60 samples, lets update the server 
    }
    delay(100);
    updatetime= smillis()+INTERVAL;  // "reset" the timer to update again in the next interval
    digitalWrite(ledpin,LOW);
  }//updatetime if

  if(update_now) // do we have 1 minutes wothr of average?
  {//send things to server
    int socket = 0;
    update_now=false; // reset our updae indicator
    // make the evalues strings and find out how long they are
    //debug<<F("update started\r\n");
    String vrmsstring=String(pmon.vrms(1),2);
    String powerstring=String(pmon.power(1),2);
    String tempstring=String(pmon.Temperature(1));
    String freqstring=String(pmon.lineFrequency(1),2);

    message_length = fixedmassagelength+ vrmsstring.length()+powerstring.length()+tempstring.length()+freqstring.length(); // length of all the values


    if (gsm.attachGPRS(0,"internet", "guest", "guest"))
    {
      digitalWrite(ledpin,HIGH);
      if(gsm.connectTCP("api.pachube.com",80,socket))
      { // we have an open tunnel to the server... stream some data
        gsm.SimpleWrite(F("PUT /v2/feeds/52053.csv HTTP/1.1\nHost: api.pachube.com\nAccept: */*\nX-PachubeApiKey: "));
        gsm.SimpleWrite(F("WNl5Bhl8g-gRtkdyP20ZXp0ggf-SAKxWWUxwLzJoeWduQT0g"));
        gsm.SimpleWrite(F("\nContent-Length: "));
        gsm.SimpleWrite(message_length);
        gsm.SimpleWrite(F("\n\n"));
        gsm.SimpleWrite(F("voltage,")); // vrmsAve.avg()
        gsm.SimpleWrite(vrmsstring);
        gsm.SimpleWrite(F("\npower,"));// powerAver.avg()
        gsm.SimpleWrite(powerstring);
        gsm.SimpleWrite(F("\ntemp,"));// tempAve.avg().
        gsm.SimpleWrite(tempstring);
        gsm.SimpleWrite(F("\nfreq,"));// freqAve.avg()
        gsm.SimpleWrite(freqstring);
        gsm.SimpleWrite(F("\n"));
        delay(100);
        gsm.disconnectTCP(socket); //disconnect after sending
        //  gsm.detachGPRS(0);
      }//tcp if
    }//gprs attache if 
    digitalWrite(ledpin,LOW);
    //  gsm.poweroff();
    // powerAve.clr();

  }//update if
  delay(100); // waste some time

// u-blox gsm on here // used to set th e9600 baud, but now the library does that..} // not turned on
}// loop


boolean after(long timeout)                    // Returns true if a timeout has happened
{
  return smillis()-timeout>0;
}



