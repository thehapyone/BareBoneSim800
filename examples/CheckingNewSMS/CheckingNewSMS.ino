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

int previousSMSIndex = 0;
int currentSMSIndex = 0;
String message = "";

void setup() {
  Serial.begin(9600);
  sim800.begin();
  while(!Serial);

  Serial.println("Testing GSM module For New SMS Checking");
  delay(8000); // this delay is necessary, it helps the device to be ready and connect to a network

  Serial.println("Should be ready by now");
    bool deviceAttached = sim800.isAttached();
  if (deviceAttached)
    Serial.println("Device is Attached");
  else
    Serial.println("Not Attached");

    // Save the last sms
    currentSMSIndex = sim800.currentMessageIndex; //reads the last saved sms index
    previousSMSIndex = currentSMSIndex;

   Serial.println("Ready, Send your new SMS");
   
   /*//Receiving and Viewing SMS
 String message = "";
 int messageIndex = 1;
 message = sim800.readSMS(messageIndex);
 Serial.print("Received message is: ");
 Serial.println(message);
     */

}

void loop() {
  //zZZzz

  bool checkSMS = sim800.checkNewSMS();
  if (checkSMS)
  {
    Serial.println("New SMS receieved");
    currentSMSIndex = sim800.currentMessageIndex;
    //lets read the sms
     message = sim800.readSMS(currentSMSIndex);
    Serial.print("Received message is: ");
    Serial.println(message);
    previousSMSIndex = currentSMSIndex; // update your sms index
      
  }
  
  delay(5000);
}
