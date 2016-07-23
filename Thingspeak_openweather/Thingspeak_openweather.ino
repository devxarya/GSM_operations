/*********************************************************************
Authors: Devvrat arya, Deva harsha, Agam tyagi

Function: 
1. It receives a weather data from Openweather.com     (Interval: 12 min )
2. Prints a description of weather on left side of LCD  
3. Secondly, It sense the room temperature using TM35 (Interval: 15 sec )
4. Post sensor temperature to Thingspeak server 

Requires: SIM 900A GSM shield, 16x2 LCD ,TM35

 * Connections:
 * 
 * GSM:  10: Tx, 11 Rx
 * LCD:  Mentioned below
 * Tempreature sensor: A0
 *********************************************************************/

#include <SoftwareSerial.h>
#include <LiquidCrystal.h>


  SoftwareSerial mySerial(10, 11); // RX, TX        // can be replaced to any other PWM pin
  LiquidCrystal lcd(12, 13, 5, 4, 6, 7);           // These are connected to LCD pins (RS, EN, D4 ,D5, D6, D7 ) respectively, Vdd-5V, Vss & R/W -GND
  
  
  String apiKey;
  String postStr;
  String OW_description;
  String OW_temp;
  String OW_data;
  long initial_time;

  bool time_captured=false;
  bool Time2post_data=true;
  bool Time2get_data=true; 
  
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);
  delay(500);
  analogReference(INTERNAL);                  //Change the reference to 1.1v
  apiKey = "ELUQT7K2WQB0LCRJ";                //You can replace your write ThingSpeak write API key 
  lcd.begin(16, 2);
}

bool flag = true;
int SE_temp_int;                              // Tempreature  SE: sensor input

void loop(void)
{    float SE_temp;         
        
     SE_temp=temp();
     delay(800);
     print_content_r(SE_temp);
     time_function();
     
     if(Time2get_data)   
    { 
      OW_data=openWeatherRequest();
      extract(OW_data);
      print_head(OW_description);
      print_content_l(OW_temp);
       Serial.println("GET");
       Time2get_data=false;
     }
     
     if(Time2post_data)
    {
      SE_temp_int =(int)SE_temp;
      Post2thingspeak(SE_temp_int);
       Serial.println("POST");
       Time2post_data=false;
    }

}

/*
 * Fuction for successful implement of AT command
 * Input command: (an ATcommand, expected output)
 * 
*/
void AT_input (String command, String check)
{ 
  String response;
  bool success = false;
  int count = 0;

  while (!success)
  {
    if (count == 0)
    { 
      mySerial.println(command);
      Serial.println(command);
    }

     if(mySerial.available()>0)
      {  
        while (mySerial.available())
           {
              char character;
              character = mySerial.read();
              response.concat(character);
              Serial.print(character);
              if (response.indexOf(check))
              {
                success = true;
              }
          }
      }
    
    if (!success)
    {
      count++;
      delay(1500);
    }

    if (count > 4) // After 3sec
    {
      Serial.println("GSM not functional");
      count = 0;
    }
  }
 }

/*
 * It sink the soft serial data to hard serial
*/
void Serial_sink()
{
  delay(2000);
  while (mySerial.available())
  { 
    char character ;
    character = mySerial.read();
    Serial.print(character);
  }
}


/*
 * It return the open weather data
 * available at Softserial
*/
String Serial_data_capture(void)
 {  
    String data;
    delay(2000);
    while (mySerial.available())
    { 
      char character ;
      character = mySerial.read();
      Serial.print(character);
      data.concat(character);
    }
    return(data);
 }

// This prints on multiple serial ports
void Multi_print(String str)
{
  Serial.print(str);
  mySerial.print(str);
}

void Multi_println(String str)
{
  Serial.println(str);
  mySerial.println(str);
}

void Multi_parse_int (int value)
 {
   Serial.println(value);
   mySerial.println(value);
 }

/*
 * This first creates a TCP connenction with thingspeak server
 * Then post data in the field
 * 
*/
void Post2thingspeak (int data)
{String trans = String(data);
  delay(500);
  AT_input("AT", "OK");
  AT_input("ATE0", "OK");
  AT_input("AT+CIPSHUT", "OK");
  AT_input("AT+CGATT=1", "OK");
  AT_input("AT+CSTT= \"www\",\"\",\"\" ", "OK");

  AT_input("AT+CIICR", "OK");
  Serial_sink();

  mySerial.println("AT+CIFSR");

  Serial.println("AT+CIFSR");
  delay(1000);
  Serial_sink();
  AT_input("AT+CIPSTART=\"TCP\",\"184.106.153.149\",\"80\"", "T");
  delay(2000);
  AT_input("AT+CIPSEND", "OK");

  Multi_print("POST /update?api_key=");
  Multi_print(apiKey);
  Multi_print("&field1=");
  Multi_print(trans);
  Multi_print("&headers=false");
  Multi_println(" HTTP/1.0\n\n\x1A");
  Serial_sink();
 }
 
 String openWeatherRequest (void)
    {String data;
      AT_input("AT","OK");
     AT_input("ATE0","OK");

    AT_input("AT+CGATT=1","OK");

    AT_input("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"","OK"); //3= bearer setting, 1=network selection, the connection type is using gprs

    AT_input("AT+SAPBR=3,1,\"APN\",\"airtelgprs.com\"","OK");
    
     Serial_sink();
     delay(4000);
     Multi_println("AT+SAPBR=1,1");//open bearer for network 1
     delay(5000);
       Serial_sink();
    
     
    AT_input("AT+HTTPINIT","OK");// init the http request

    AT_input("AT+HTTPPARA=\"CID\",\"1\"","OK");//setting HTTp bearer profile identifier

    AT_input("AT+HTTPPARA=\"URL\",\"http://api.openweathermap.org/data/2.5/weather?id=1262453&units=metric&appid=a5dfc6160f64d18135b392af04f31b50\"","OK");

    AT_input("AT+HTTPACTION=0","OK");

    Serial_sink();
    delay(2000);
    Multi_println("AT+HTTPREAD");
    delay(2000);
      data= Serial_data_capture();
     delay(2000);
     Serial_sink();
     delay(500);
     Serial_sink();
      AT_input("AT+HTTPTERM","OK");
    
      AT_input("AT+SAPBR=0,1","OK");
      
      return(data);
    }


/*
 * Return the tempreature measured using TM 35
*/
 float temp (void)
   {
    float tempC;
    int reading;
    int tempPin = 0;
    reading = analogRead(tempPin);
    tempC = reading / 9.31;
    Serial.println("Temp: ");
    Serial.print(tempC);
    return(tempC);
   }

/*
 * It is used to extract a description, temperature from info recived from OpenWeather
*/
   void extract (String data)
   {
    Serial.println(data);
    OW_description =data.substring(data.indexOf("ion")+6,data.indexOf("icon")-3);
    OW_temp = data.substring(data.indexOf("temp")+6,data.indexOf("pres")-2);
    Serial.println(OW_description);
    Serial.println(OW_temp);
   }

   
// Print a heading on LCD 
   void print_head (String str)
     { 
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(str);
       }
       
    //Print secondary content on LCD
    // On left
    void print_content_l (String str)
     {
      lcd.setCursor(0,1);
      lcd.print(str);
      //Put degree cel here
     }

    // Print content on right side
    void print_content_r (float str)
    {
          lcd.setCursor(11,1);
          lcd.print(str);
    }

/*
 * Its responsible for all timings in the program 
 * Uses an internal clock of arduino (better than using delay)
 * Two interval are required, 15 sec and 12 min
*/
void time_function (void) 
  {
  
    long operational_time;
    long current_time;
    int quarter_minute;
    
    if(!time_captured)    // /time reset func
     {
       initial_time=millis();
       time_captured=true;
     }

     current_time=millis();
     operational_time=current_time-initial_time;
     //Serial.println(operational_time);

    
    if(operational_time>15000)  // Change here for 15 seconds
      { 
         quarter_minute+=1;
         Time2post_data=true;
         time_captured=false;
      }
     
     if(quarter_minute>48)   //15*48sec =12 minutes
       {
         Time2get_data=true;
         quarter_minute=0;
       }
  }

