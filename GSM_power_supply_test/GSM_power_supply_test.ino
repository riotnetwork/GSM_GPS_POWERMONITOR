#include <SoftwareSerial.h>

/*
  GSM board power test
 
 D2 : reset sense
 D3 : MSIO
 D4 : MOSI
 D5 : GSM powerup signal
 D6 : GSM reset signal
 D7 : GSM PSU control
 */
//SoftwareSerial uBlox(10,9);

#define power_ctrl 15 // 15 // 5
#define reset_ctrl 16 // 16

void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(power_ctrl, OUTPUT); //GSM module control power_ctrl
  pinMode(reset_ctrl, OUTPUT); //GSM module reset control reset_ctrl
  pinMode(13, OUTPUT);  //onboard LED
  digitalWrite(power_ctrl , HIGH);     //GSM module  OFF
  digitalWrite(reset_ctrl , HIGH);     //GSM module reset control

  digitalWrite(reset_ctrl , HIGH);     //GSM module reset control
  delay(100);
  digitalWrite(reset_ctrl , LOW);     //GSM module reset OFF

  delay(100);

  digitalWrite(power_ctrl , HIGH);     //startup sequence
  delay(30);                // 30 ms delay for start pulse
  digitalWrite(power_ctrl , LOW);     //startup sequence end

  /*
  digitalWrite(power_ctrl , LOW);     //startup sequence
   delay(30);                // 30 ms delay for start pulse
   pinMode(power_ctrl, INPUT); //GSM module control power_ctrl
   // Serial.println("Module started, wait 2 s for interfaces to configure ");
   */
  digitalWrite(13,HIGH); // turn LED on
  // delay(1000);
  //uBlox.write("AT\n");
}

void loop() 
{
  /*

   if (uBlox.available())
   Serial.write(uBlox.read());
   if (Serial.available())
   uBlox.write(Serial.read());
   */
}





