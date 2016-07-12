/*
 * Author: Devvrat arya
 * 
 * The code enable user to directly enter AT command in Serial window
 * The response of GSM also printed on the Serial window
*/

#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX    // can be replaced to any other PWM pin


/*
 * GSM module is connected to software serial (declared as mySerial) at 10, 11 Pin of arduino
*/
void setup() 
{
  Serial.begin(9600);
  Serial.println("Enter AT commands");
  
  mySerial.begin(9600);
  delay(500);
}
 
void loop() 
 { 

     if(Serial.available())                     // If data available on hard Serial, pirnt it on soft Serial
       {
         char input=Serial.read();
         mySerial.print(input);
        
       }

     if(mySerial.available())                  // If data available on soft Serial,print it on hard Serial
       {
         char output=mySerial.read();
         Serial.print(output);
       }
 }

