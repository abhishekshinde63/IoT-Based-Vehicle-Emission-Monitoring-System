#include <SoftwareSerial.h>
int gasPin= A0;
int hrgrogePin= A1;
int coPin= A2;
int buzzer=5;

// replace with your channel's thingspeak API key
String apiKey = "5S065NC3XTG6GJSR";

// connect 2 to TX of Esp
// connect 3 to RX of Esp
SoftwareSerial ser(2,3); // TX,RX of Esp
SoftwareSerial mySerial(9, 10);
char msg;

// this runs once
void setup() {    
  mySerial.begin(115200);   // Setting the baud rate of GSM Module  
  Serial.begin(115200);    // Setting the baud rate of Serial Monitor (Arduino)
              
  // enable debug serial
  //sets the baud rate to 115200
  Serial.begin(115200); 
  // enable software serial
  //sets the baud rate to 115200
  ser.begin(115200);
  // reset ESP8266
  ser.println("AT+RST");
  pinMode(buzzer,OUTPUT);
 }
// this runs again and again 
void loop()
{
  
   float gas_val=analogRead(gasPin);
   float hydrogen_val=analogRead(hrgrogePin);
   float co_val=analogRead(coPin);

  // convert to string
  char buf[16],buf1[16],buf2[16];
  String strGas = dtostrf(gas_val,4,1,buf);
  String strhydro = dtostrf(hydrogen_val,4,1,buf1);
  String strco = dtostrf(co_val,4,1,buf2);

  if(analogRead(gasPin)>50 && analogRead(hrgrogePin)>50 && analogRead(coPin)>50)
  {
    digitalWrite(buzzer,HIGH);
    SendMessage();
    
   }
   else 
   {
     digitalWrite(buzzer,LOW);
   }
  Serial.println(strGas);
  Serial.println(strhydro);
  Serial.println(strco);
 
  // TCP connection
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80";
  ser.println(cmd);
  
  if(ser.find("Error"))
  {
    Serial.println("AT+CIPSTART error");
    return;
  }
  
  // prepare GET string
  String getStr = "GET /update?api_key=";
  getStr += apiKey;
  getStr +="&field1=";
  getStr += String(strGas);
   getStr += apiKey;
  getStr +="&field2=";
  getStr += String(strhydro);
   getStr += apiKey;
  getStr +="&field3=";
  getStr += String(strco);
  getStr += "\r\n\r\n";

  // send data length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  ser.println(cmd);
  if(ser.find(">"))
  {
    ser.print(getStr);
  }
  else
  {
    ser.println("AT+CIPCLOSE");
    // alert user
    Serial.println("AT+CIPCLOSE");
  }
  // thingspeak needs 15 sec delay between updates
  delay(20000);  
}
void SendMessage()
{
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\"+917057896847\"\r"); // Replace x with mobile number
  delay(1000);
  mySerial.println("asdfghjklzxcvbnm,.");// The SMS text you want to send
  delay(100);
   mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}
void MakeCall()
{
  mySerial.println("ATD+917304980126;"); // ATDxxxxxxxxxx; -- watch out here for semicolon at the end!!
  Serial.println("Calling  "); // print response over serial port
  delay(1000);
}


 
