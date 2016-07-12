/*
 * Author: Devvrat arya
 * 
 * The code presented here is to list all the SMS saved on the module
 * There are 3 AT commands for doing so
*/


/*
 *The serial communication is done through (1,0) pins of arduino
 *So serial communcation started at 9600 baud rate
*/

void setup() 
{
  Serial.begin(9600);
}


void loop() 
 {
    delay(1300);

    
    Serial.print("ATE0");               //It is to enable echo mode of SIM900A. This means module will respond after every command (such as OK)
    delay(500);
    if(GSM_operational())               //Defined below
       Serial.print("AT");              // It checks weather module is ready for command
    delay(500);  
       
    if(GSM_operational())
        Serial.print("AT+CMGF=1");      // Enable text mode messaging mode
    delay(500);
    
    if (GSM_operational())
     {Serial.print("AT+CMGL=\"ALL\"\r"); //To show all message , here \" is used to  print double quotes on Serial monitor. Replace it with index number(1,2,3) to show individual msg
          delay(500);
      while(1)                           // Print all the Serial data available and Hang in there forever
        {
        if (Serial.available())
       {
        char data =Serial.read();
        Serial.print(data); 
       }
        }
     }
 }

/*
 * This function is to determine is GSM_operational
 * It do so by observing reply from GSM shield
 * There could three states in which GSM can exist which are accouned below
 *  
*/

bool GSM_operational(void)
  {
    int count =0;
    bool Status=false;
    Serial.println();
    while(1)
    { 
      if (Serial.available()>0)
       { 
        char data = Serial.read();
        if( data == 'O' || data == 'K')                     //In this case GSM is functional( print 'OK')
          {
            Serial.println("OK");
            Status=true;
            break;
            }
        if( data == 'E' || data == 'R' || data== 'O')
          {
            Serial.println("GSM not functional");           // GSM reply an erroe msg( print: GSM not functional)
             Status=false;
             break;
          }   
       }
      count++;
      delay(10);
      if (count == 100)
      { 
        Serial.println("GSM not connected");               // No reply in 100 cycles , hence GSM not connected
       Status=false;
       break;
      }
     }
    return Status;
    }
