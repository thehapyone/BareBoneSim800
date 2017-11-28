/*    This is a bare bone library for communicating with SIM800
 *    It's barebone in that - it only provides basic functunaitlites while still 
 *    maintaining strong performance and being memory friendly.
 *    It currently supports GSM(sending and reading SMS), 
 *    GPRS connectivity(sending and receiving TCP) with Time and Location
 *     
 *    This library is writing by  Ayo Ayibiowu.
 *    charlesayibiowu@hotmail.com
 *    Designed to work with the GSM Sim800l module,
 *    and possibily other SIMxxx modules(not tested with others)
 *  
 *    This library use AltSoftSerial, and the pins has already be defined
 *    be sure that GND is attached to arduino too. 
 *    The RESET_PIN is not used for this Library, you can leave it un-connected
 *    
 *    To Enable Debugging - Go to <BareBoneSim800.h file and change the
 *    #define DEBUG 0 to #define DEBUG 1
 *         
 *    PINOUT: 
 *        _____________________________
 *       |  ARDUINO UNO >>>   SIM800L  |
 *        -----------------------------
 *            GND      >>>   GND
 *        RX  8       >>>   TX    
 *        TX  9       >>>   RX
 *      
 *                 
 *   POWER SOURCE 4.2V (MAX) >>> VCC
 *
 *        Created on: Oct 24, 2017
 *        Author: Ayo Ayibiowu
 *        Email: charlesayibiowu@hotmail.com
 *        Version: v1.0
 *        
 *
*/

#include <BareBoneSim800.h>

BareBoneSim800 sim800; // 
//BareBoneSim800 sim800("your APN");  //needed for gprs funtionality 



void setup() {
  Serial.begin(9600);
  sim800.begin();
  while(!Serial);

  Serial.println("Testing GSM module For SMS Sending");
  delay(8000); // this delay is necessary, it helps the device to be ready and connect to a network

  Serial.println("Should be ready by now");
    bool deviceAttached = sim800.isAttached();
  if (deviceAttached)
    Serial.println("Device is Attached");
  else
    Serial.println("Not Attached");

   //Testing sending SMS aspect

   const char* number = "+2347038945220";
   char* message = "Hello, This is a text message";
   
   delay(1000);
   
   bool messageSent = sim800.sendSMS(number, message);
   if(messageSent)
      Serial.println("Message Sent");
   else
      Serial.println("Not Sent, Something happened");
     

}

void loop() {
  //zZZzz

}
