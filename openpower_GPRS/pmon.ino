/*
RunningAverage vrmsAve(60); // average for the last minute of the interested parameters
RunningAverage powerAve(60);
RunningAverage tempAve(60);
RunningAverage freqAve(60)

vrmsAve.add(pmon.vrms());
powerAve.add(pmon.power());
tempAve.add(pmon.Temperature());
freqAve.add(pmon.lineFrequency());

.avg() // the average

freqAve.clr(); // clears the running average
tempAve.clr();
powerAve.clr();
freqAve.clr();
*/

void getvalues () // eget a set of values from powermonitor
{
  //vrmsAve.add(pmon.vrms(1));// Vrms
  powerAve.add(pmon.power(1));// P
  //tempAve.add(pmon.Temperature(1));// Temp
 // freqAve.add(pmon.lineFrequency(1));// line freq
  sample++;
  if (sample==59)
  {
   // freqAve.clr(); // clears the running average
   // tempAve.clr();
   
   // freqAve.clr();
    sample =0; // resets the sample number
    update_now = true; // we have 60 samples, lets update the server 
  }
     updatetime= smillis()+INTERVAL;  // "reset" the timer to update again in the next interval
}
