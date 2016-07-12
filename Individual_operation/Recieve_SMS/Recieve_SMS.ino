/*
 Author: Devvrat Arya

 The program is designed to recieve a SMS through a SIM 900A module. It works in the follwing way
 1. Sets up GSM module into Recieve incoming message
 2. Wait for message for infinte time to come
 3. If message arrives: show the caller Id and Text
*/





#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX

/*
 * Sets up soft serail and hard serail at 9600
 * Shows a serial message for indicating the arduino serial is functional
 * Increase the timeout time for Serials
*/
void setup() 
{
  Serial.begin(9600);
  Serial.println("GSM startrd");
  mySerial.begin(9600);
  mySerial.setTimeout(2000);
  Serial.setTimeout(2000);  
}


char quote= 0x22;
 

 bool GSM_operational(void);

 String sms ="";
 String Caller_id;
 String Text;

 char character;
 int indexOfQuotes[10];
 
 double time_start;
 double time_current;
 
 bool recieve_mode=false;
 bool  msg_recieve=false;
 bool print_msg= false;
 bool time_registered=false;
 bool msg_fetched=false;
 bool start_recieve=false;
 
void loop() 
 {
   // Start operating in recieving mode
   while(!recieve_mode)
    {  
       mySerial.print("ATE0");       
       delay(500);
         
         if(GSM_operational())
            mySerial.print("AT");
          delay(500);  
       
         if(GSM_operational())
            mySerial.print("AT+CMGF=1");
         delay(500);
    
         if (GSM_operational())
           {
             mySerial.print("AT+CNMI=2,2,0,0,0\r" );
             delay(500);
             recieve_mode= true;
            
              delay(500);
             while(mySerial.available())
              {
                char response =mySerial.read();
                Serial.println(response);
              }

               Serial.println("Recieve mode On");
                msg_fetched=true;  //turn it on off
                   
           }
    }
   
    
   if(recieve_mode)                                     // Wait for msgs to recieve<Check recieve mode through a AT command>
                {
                   while(!halt_fetch())                 
                      {

                    while(mySerial.available())
                       {
                        if(!time_registered)            //Capture the time of start of message recieving
                        {
                          time_start=millis(); 
                          time_registered=true; //turn it on later
                        }
                        
                          char character=mySerial.read();
                          sms.concat(character);
                        }
                     
                      } 
                      
                    if(msg_fetched)
                        {
                          Serial.println("Msg Recieved");
                          print_msg=true;
                          msg_fetched=false;
                        }
                }

    while(print_msg)                                          // As message is recieved, time to get the caller id and text
               {
                
                  extract_msg();
                
                  Serial.println("SMS from : ");
                  Serial.println(Caller_id);
                  Serial.println("Text: ");
                  Serial.println(Text);
                 
                  print_msg=false;                             // Task complete, Turn of all Flag
                  time_registered=false;
                  msg_fetched=true;
                  
                  sms="";                                      //Clear string
               } 

    
              

}

double operational_time;

/*
 *The function is created to halt or to indiacte the end of receiving
 *It does that by a timout of 5sec or text msg limit of 500 characters
 *Input: none
 *Output: Boolean, T= halt fetching F= Wait for message 

*/
bool halt_fetch (void)
  { 
    bool halt=false;

    if(time_registered)
    {
      time_current=millis();
      operational_time=time_current-time_start;
    }

    if(operational_time>5000 || sms.length()==500 )                        // Halt condition
     { halt=true; 
       operational_time=0;
     }

   return halt;
  }

  
/*
 * It extracts the message and caller id
 * It does that by quotes postion.
 * Caller id is between first and second qoutes
 * While, Text message is after last quotes
*/
void extract_msg (void)
 { 
    int Length,i,index=0;
    
    Length=sms.length();
    for(i=0;i<=Length;i++)
    {
          if(sms.charAt(i)==quote)
          {
            indexOfQuotes[index]=i;
            index++;
          }
    }

    Caller_id=sms.substring(indexOfQuotes[0]+1,indexOfQuotes[1]);
    Text=sms.substring(indexOfQuotes[5]+3);
 }


/*
 * This fuction whether GSM is operational
 * Input: none
 * Output: bool, T= its functional , F= check serial for error
*/ 

bool GSM_operational(void)
  {
    int count =0;
    bool Status=false;
    mySerial.println();
    while(1)
    { 
      if (mySerial.available()>0)
       { 
        char data = mySerial.read();
        if( data == 'O' || data == 'K')
          {
            Serial.println("OK");
            Status=true;
            break;
            }
        if( data == 'E' || data == 'R' || data== 'O')
          {
            Serial.println("GSM not functional");
             Status=false;
             break;
          }   
       }
      count++;
      delay(10);
      if (count == 100)
      {
        Serial.println("GSM not connected");
       Status=false;
       break;
      }
     }
      return Status;
    }
