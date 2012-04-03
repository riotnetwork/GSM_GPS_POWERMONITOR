#include <PMStreaming.h>
#include <PowerMonitor.h>
#include <SoftwareSerial.h>
#include <uBlox.h>
#include <TextFinder.h>

#define INTERVAL 1000                    // update interval in milliseconds
#define smillis() ((long)millis())       // This is used to calculate timeouts
long updatetime;                         // used to store the time till next upodate

long resettime;                         // used to store the time till next upodate
#define RESETINTERVAL 2700000                // re register modem every 45 minutes(27) // 36 for 1 hour
PowerMonitor pmon;            // instance of a powermonitor


int socket; //create a socket to work in // this will be changed to an available socket inside the gsm library
boolean update_now= false;
int message_length; // used to hold the message lentgh
int turnedOn =0;

#define ledpin 13
int sample =0;
int time = 30;

void setup()  
{
  pinMode(ledpin,OUTPUT);
  digitalWrite(ledpin,HIGH);
 // Serial.begin(9600);
  pmon.begin(220,50);
  gsm.begin(9600);    // u-blox gsm on here.
  digitalWrite(ledpin,LOW);
  updatetime= smillis(); // immediatly update after startup
  resettime=  smillis() +RESETINTERVAL;
  //update_now = true; // we have waited long enough, lets update
 // delay(1);
}

void loop() // run over and over
{
  
  if(after(resettime)) // // re register with network every hour to keep gprs profile hihj enough proirotuy on th network
  {
   // Serial << "Modem Re-Register"<<endl;
    gsm.poweroff();
    gsm.begin(9600);
    pmon.begin(220,50);
    delay(20000);
    updatetime= smillis()+INTERVAL;  // "reset" the timer to update again in the next interval
    resettime=  smillis()+RESETINTERVAL; // re register with network agin in 1 hour
  }

  if(after(updatetime)) // 1 second has elapsed
  {
    digitalWrite(ledpin,HIGH);
    sample++;
  //  Serial<<time-sample<<" ";
    if (sample>=time)
    {
      sample =0;         // resets the counter
      update_now = true; // we have waited long enough, lets update
    }
    delay(50);         // waist some time
    /*
    double temp;
  temp=   pmon.powerFactor(1);
  temp=   pmon.Temperature(1);
  temp=   pmon.lineFrequency(1);
  temp=   pmon.power(1);
  temp=   pmon.vrms(1);
    */
    updatetime= smillis()+INTERVAL;  // "reset" the timer to update again in the next interval
    digitalWrite(ledpin,LOW);
  }//updatetime if

  if(update_now) //time to send things to server
  {
    update_now=false; // reset our update indicator
   
  //  Serial<<endl;
    // send the update string
    String myDataString = ""; //allocate for actual data sent
     myDataString += "pfac";
     myDataString += "=";
     myDataString += String(pmon.powerFactor(1));
     myDataString += "&temp";
     myDataString += "=";
     myDataString += String(pmon.Temperature(1));
     myDataString += "&freq";
     myDataString += "=";
     myDataString += String(pmon.lineFrequency(1));
     myDataString += "&power";
     myDataString += "=";
     myDataString += String(pmon.power(1));
     myDataString += "&vrms";
     myDataString += "=";
     myDataString += String(pmon.vrms(1));
  
  if (gsm.attachGPRS(0,"internet", "guest", "guest"))
    {
   //   Serial<<"Signal: " <<gsm.signal(0)<<"/31"<<endl;
      digitalWrite(ledpin,HIGH);
      if(gsm.connectTCP("m2.exosite.com",80,socket))
      { // we have an open tunnel to the server... stream some data

        gsm.SimpleWrite(F("POST /api:v1/stack/alias HTTP/1.1\r\n"));
        gsm.SimpleWrite(F("Host: m2.exosite.com\r\n"));
        gsm.SimpleWrite(F("X-Exosite-CIK: "));
        gsm.SimpleWrite(F("a4426624b5071da75bcab201c2fb27597ed7af95")); // key goes here
        gsm.SimpleWrite(F("\r\nContent-Type: application/x-www-form-urlencoded; charset=utf-8\r\n"));
        gsm.SimpleWrite(F("Accept: application/xhtml+xml\r\n"));
        gsm.SimpleWrite(F("Content-Length: "));
        gsm.SimpleWrite( myDataString.length());
        gsm.SimpleWrite(F("\r\n\r\n"));
        gsm.SimpleWrite(myDataString);
        gsm.SimpleWrite(F("\r\n"));
        delay(200);
       // gsm.SimpleRead();
        
      //  Serial <<  myDataString << endl << "datalength: "<< myDataString.length()<<endl;
      //  delay(200);
        gsm.disconnectTCP(socket); //disconnect after sending
        
      }//tcp if
    }//gprs attach if 
    digitalWrite(ledpin,LOW);

  }//update if
  delay(100); // waste some time

}// loop


boolean after(long timeout)                    // Returns true if a timeout has happened
{
  return smillis()-timeout>0;
}


