/*
 *Author: Agam tyagi
 *The following code can call to any number using GSM shield
*/



#include <SoftwareSerial.h>                      //defining header file for software serial

SoftwareSerial mySerial(9, 10);                 // RX, TX pin

void setup()
{
      Serial.begin(9600);                       //defining braudrate
      mySerial.begin(9600);                     // set the data rate for the SoftwareSerial port 
}

void loop()                                      // run over and over
{
  Serial.println("Calling through GSM Modem");

                                

delay(2000);
mySerial.println("ATD+91xxxxxxxxxx;");            // REPLACE 'xxx' here the number to be called (must ; at the end ).
                                                  //ATD is used for make calls

Serial.println("Called ATD8791251952");
// print response over serial port
if (mySerial.available())
Serial.write(mySerial.read());
}

