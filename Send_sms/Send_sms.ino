   //Author: Devvrat arya
   // The following program is capable of sending SMS to any number
   // WARNING: The program sends SMS countinuosly in a loop. Hence a SMS tariff is is recommended
   
   // Serial setup here
   void setup() 
    {
    Serial.begin(9600); 
    }

  #define Available true
  #define Busy false

   char Task = 0x73;
   int    i=0;
   void loop (void)
    {
    // This part verify the availabilty of Module  
 
    delay(1300);
    Serial.print("AT");
    
    delay(1300);
   bool Status = Busy;
    while (Serial.available() > 0)
    {
    char inChar = (char)Serial.read();
    Status = Available;
    }

    if(Status==Available)
    {
    // Turn on the Messaging mode
    Serial.println();
    Serial.println("AT+CMGF=1"); 
    
    delay(1300);
    
    Status = Busy;
    while (Serial.available() > 0) {
    char inChar = (char)Serial.read();
    Status = Available;
    }
    
    
    if(Status==Available )
    {
    
    Serial.println();
    Serial.print("AT+CMGS=\"");                         // Enter the SMS number
    Serial.print("+91xxxxxxxxx");                       // REPLACE xxx with a mobile number
    Serial.println("\"");
    delay(1000);
    Serial.print("GSM bot functonal iteration=" + i ); // SMS body with i = loop counter
 
    delay(500);
 
    Serial.write(0x1A);
    Serial.write(0x0D);
    Serial.write(0x0A);
    Task=0x72;
 
    }
    }
    
  }

 

 

   
