#include <SoftwareSerial.h>

#include <TextFinder.h>
#include <PowerMonitor.h>
#include <Streaming.h>

	
SoftwareSerial xbee(2,3);
PowerMonitor pmon;
#define INTERVAL 2000
#define smillis() ((long)millis())       // This is used to calculate timeouts
long updatetime;

double readings[] = {0.00,0.00,0.00,0.00}; // Vrms, RealPower,aqumilated energy, Temperature
void setup()  
 {
   xbee.begin(38400);
   pmon.begin(220,50); // 220V and 50Hz are nominal parameters for south africa
   updatetime= smillis();
 }

void loop() // run over and over
{
  
  //delay(1000);
  if(after(updatetime))
  {
   readings[0] = pmon.vrms(1);
   delay(20);
   readings[1] = pmon.power(1);
   delay(20);
   readings[2] = pmon.energy(1);
   delay(20);
   readings[3] = pmon.Temperature(1);
   delay(20);
   xbee << "Vrms : \t\t"<< readings[0] <<endl<< "Real power : \t"<< readings[1] <<endl<< "Energy : \t\t"<< readings[2] <<endl<< "Temp : \t\t"<< readings[3] <<endl;
    updatetime= smillis()+INTERVAL;  
  }
}

boolean after(long timeout)                    // Returns 1 if a timeout occured, otherwise 0.
{
  return smillis()-timeout>0;
}

