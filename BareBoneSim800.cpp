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
 *        Version: v1.1
 *        
 *
*/
 

 #include "Arduino.h"
 #include "AltSoftSerial.h"
 #include "BareBoneSim800.h"
 
 // Initialize the constructors
 BareBoneSim800::BareBoneSim800()
 {
 }
 
 BareBoneSim800::BareBoneSim800(const char* networkAPN){
	 _networkAPN = networkAPN;
	 _userName = "";
	 _passWord = "";
 }
 
 BareBoneSim800::BareBoneSim800(const char* networkAPN, const char* userName, const char* passWord){
	 _networkAPN = networkAPN;
	 _userName = userName;
	 _passWord = passWord;
 }

AltSoftSerial gsmSerial;
 
 
 // 
 // PRIVATE METHODS
 //
 byte BareBoneSim800::_checkResponse(uint16_t timeout)
 {
	 // This function handles the response from the radio and returns a status response
	 uint8_t Status = 99; // the defualt stat and it means a timeout
	 unsigned long t = millis();
	 //unsigned long count = 0;
	 
	 // loop through until their is a timeout or a response from the device
	 while(millis()<t+timeout)
	 {
		 //count++;
		 if(gsmSerial.available()) //check if the device is sending a message
		 {
			 String tempData = gsmSerial.readString(); // reads the response
			 if(DEBUG)
				 Serial.println(tempData);
			 char *mydataIn = strdup(tempData.c_str()); // convertss to char data from
			 
		 /*
		   * Checks for the status response
		   * Response are - OK, ERROR, READY, >, CONNECT OK
		   * SEND OK, DATA ACCEPT, SEND FAIL, CLOSE, CLOSED
		   * note_ LOCAL iP COMMANDS HAS NO ENDING RESPONSE 
		   * ERROR - 0
		   * READY - 1
		   * CONNECT OK - 2
		   * SEND OK - 3
		   * SEND FAIL - 4
		   * CLOSED - 5
		   * > - 6
		   * OK - 7
		   * 
		   * 
		   */
		   for (byte i=0; i<_responseInfoSize; i++)
		   {
			if((strstr(mydataIn, _responseInfo[i])) != NULL)
			{
			  Status = i;
			  return Status;
			  }
		   }
		 }
		 //Serial.println(count);
	 }
	 return Status;
 }
 
 String BareBoneSim800::_readData(){
	 // this function just reads the raw data
	 uint16_t timeout=0;
	 while (!gsmSerial.available() && timeout<10000)
	 {
		 delay(10);
		 timeout++;
	 }
	 if(gsmSerial.available())
	 {
		 String output = gsmSerial.readString();
		 if(DEBUG)
			 Serial.println(output);
		 return output;
	 }
 }
 /*
  String BareBoneSim800::_readData(uint16_t timeout){
	 // this function just reads the raw data
	 uint16_t t=0;
	 while (!gsmSerial.available() && t<timeout)
	 {
		 delay(10);
		 timeout++;
	 }
	 if(gsmSerial.available())
	 {
		 String output = gsmSerial.readString();
		 if(DEBUG)
			 Serial.println(output);
		 return output;
	 }
 }
 */
 /*
 void BareBoneSim800::_flushSerial(uint16_t timeout){ 
	unsigned long t = millis();	 
	 // loop through until their is a timeout or a response from the device
	 String output;
	 while(millis()<t+timeout)
	 {
	  if(gsmSerial.available())
	 {
		 output = gsmSerial.readString();
		 output= "";
		 break;
	 }
	 }
 }
 */
 
 int BareBoneSim800::_getLatestMessageIndex()
{
	// this function checks the message for the latest message index.
	// returns 0 if nothing is found, possible for an empty simcard
	String buffer = "";
	String bufferIndex = "";
	int messageIndex = 0;
	int tempIndex = 0;
	gsmSerial.print(F("AT+CMGL=\"ALL\",0"));
	gsmSerial.print("\r");
	buffer = _readData(); //reads the result	
	tempIndex = buffer.lastIndexOf("+CMGL:");
	if(tempIndex != -1)
	{
		// means message is found
		tempIndex = tempIndex + 6;
		bufferIndex = buffer.substring(tempIndex);
		bufferIndex = bufferIndex.substring(1,(bufferIndex.indexOf(",")));
		messageIndex = bufferIndex.toInt();
		return messageIndex;
	}
	else
		return messageIndex;	
	
}

 void BareBoneSim800::_setUp(){
	 // here we setup some important parameters
	 gsmSerial.print(F("AT+CSCS=\"GSM\"\r\n"));
	 byte someBuffer = _checkResponse(10000); // just to clear the buffer
	 delay(100);
	 gsmSerial.print(F("AT+CMGF=1\r"));
	 byte result = _checkResponse(10000);
	 // here we save the latest message index here
	 currentMessageIndex = _getLatestMessageIndex();	 
	 previousMessageIndex = currentMessageIndex;
	 
 }
 
 void BareBoneSim800::_enableBearerProfile()
{
	// This function enable and set the bearer profile for time, location and GPRS service
	String buffer;
	gsmSerial.print(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n" ));buffer=_readData();  // set bearer parameter 
	delay(100);
	gsmSerial.print(F("AT+SAPBR=3,1,\"APN\",\""));  
	gsmSerial.print(_networkAPN);
	gsmSerial.print(F("\"\r\n")); // set apn 
	buffer=_readData();
	delay(20);
	gsmSerial.print(F("AT+SAPBR=3,1,\"USER\",\""));  
	gsmSerial.print(_userName);
	gsmSerial.print(F("\"\r\n")); // set username
	buffer=_readData();
	delay(20);
	gsmSerial.print(F("AT+SAPBR=3,1,\"PWD\",\""));  
	gsmSerial.print(_passWord);
	gsmSerial.print(F("\"\r\n")); // set password  
	buffer=_readData();
	delay(20);
	gsmSerial.print(F("AT+SAPBR=1,1\r\n")); // activate bearer context
	buffer=_readData();
	delay(100);
	gsmSerial.print(F("AT+SAPBR=2,1\r\n ")); // get context ip address
	buffer=_readData(); 
	delay(1000);

}

void BareBoneSim800::_disableBearerProfile(){
	// Disables the bearer profile
	gsmSerial.print(F("AT+SAPBR=0,1\r\n"));
	
	byte result = _checkResponse(20000);
	//String _buffer = _readData();
	delay(100);
	
}

 	 
 
 
 //PUBLIC METHODS
  void BareBoneSim800::begin(){
	 gsmSerial.begin(9600);	 	 
	 // I should clear the buffer just incase
	 //_flushSerial(1000);
	 byte result = _checkResponse(1000); // this will flush the serial
 }
 
 //*******CHecks if the GSM Module is attached **********/
 bool BareBoneSim800::isAttached()
 {
	 byte result;
	 // sends a test AT command, if attached we should get an OK response
	 // if attached we should get an OK response
	 gsmSerial.print(F("AT\r\n"));
	 result = _checkResponse(1000); // timeout of 1 secs
	 if (result != OK)
		 return false;
	 _setUp(); //set this ready for sms mode
	 return true;
 }
 
 void BareBoneSim800::flushSerial(uint16_t timeout){
	 // flush serial out
	  unsigned long t = millis();
	  // loop through until their is a timeout or a response from the device
	  String output;
	  while(millis()<t+timeout)
	  {
		  if(gsmSerial.available())
		  {
			  output = gsmSerial.readString();
			  output= "";
			  break;
		  }
	  }
  }

 
 bool BareBoneSim800::setFullMode(){
	 //This set the device to full funcionality - AT+CFUN
	 bool nowReady = false;
	 gsmSerial.print(F("AT+CFUN=1\r\n"));
	 // Let's confirm if this was valid
	 byte result = _checkResponse(20000); // timeout 10s
	 if (result == OK || result == READY)
		 nowReady = true;
	 delay(10);
	 // sometimes some excess data usually come out
	 result = _checkResponse(5000); // timeout 10s 
	 if (nowReady)	 
		 return true;
	 else
		 return false;
 }
 
 bool BareBoneSim800::enterSleepMode(){
	 // This set the device into a good sleep mode - AT+CFUN=0 and AT+CSCLK
	  gsmSerial.print(F("AT+CFUN=0\r\n"));
	 byte result = _checkResponse(10000);
	 if (result != NOT_READY)
		 return false;
	 
	 delay(50); // just chill small	
	 // The GSM will send an OK response again
	 result = _checkResponse(20000);
	 if (result != OK)
		 return false;
	 delay(50);
	 gsmSerial.print(F("AT+CSCLK=2\r\n")); // enable automatic sleep
	  result = _checkResponse(20000);
	 if(result != OK)
		 return false;
	 	 
	 return true;
 }
 
 bool BareBoneSim800::disableSleep(){
	 //This mode disable sleep mode - AT+CSLK=0
	 // first we need to send something random for as long as 100ms
	 gsmSerial.print(F("FF\r"));
	 delay(120); // this is between waking charaters and next AT commands
	 byte result = _checkResponse(1000); // just incase something pops up
	 gsmSerial.print(F("AT+CSCLK=0\r\n"));
	 result = _checkResponse(20000);
	 if(result != OK)
		 return false;
	 delay(100); // just chill for 100ms for things to stablize
	 if(!setFullMode())
		 return false;
	 //NOTE - After disabling sleep a good idea is to make the device to flush the serial
	 return true;
	 
 }
 
 
bool BareBoneSim800::sendSMS(const char* number, char* text){
	/* This sends an sms out 
	*	First send out AT+CMGF=1 - activate text mode
	* The AT+CMGS=\number\
	AT+CMGS=<number><CR><message><CTRL-Z>	+CMGS:<mr>
	OK
	*/
	byte result;
	gsmSerial.print(F("AT+CMGF=1\r\n")); // set sms to text mode
	result = _checkResponse(10000);
	if(result == ERROR)
		return false; // this just end the function here 
	delay(1000);
	gsmSerial.print(F("AT+CMGS=\"")); // command to send sms
	gsmSerial.print(number);
	gsmSerial.print(F("\"\r\n"));
	result=_checkResponse(60000); // to clear buffer and see if successful
	if(result == READY_TO_RECEIVE){
		gsmSerial.print(text);
		gsmSerial.print("\r");
		result=_checkResponse(1000);
		gsmSerial.print((char)26);
		result = _checkResponse(20000);
		// if successfully sent we should get CMGS:xxx ending with OK
		if(result == OK)
			return true;
		else
			return false;
	}
}

String BareBoneSim800::readSMS(uint8_t index){
	String buffer = "";
	gsmSerial.print(F("AT+CMGF=1\r"));
	byte result = _checkResponse(10000);
	if(result == OK)
	{
		gsmSerial.print(F("AT+CMGR="));
		gsmSerial.print(index);
		gsmSerial.print("\r");
		buffer = _readData(); //reads the result
		if(buffer.indexOf("CMGR:") != -1)
		{
			// means message is found
			return buffer;
		}
		else
			return "";
	}
	else
		return "";
}

String BareBoneSim800::readSIMNumber(){
	// This function reads the simcard registered number
	String buffer = "";
	gsmSerial.print(F("AT+CNUM"));
	gsmSerial.print("\r");
	buffer = _readData(); //reads the result
	if(buffer.indexOf("+CNUM:") != -1)
	{
		// means message is found
		return buffer;
	}
	else
		return "";
}

bool BareBoneSim800::checkNewSMS(){
	// This function checks if a new sms is available
	int messageIndex = 0;
	currentMessageIndex = _getLatestMessageIndex();
	if (currentMessageIndex > previousMessageIndex)
	{
		previousMessageIndex = currentMessageIndex;
		return true;
	}
	else
		return false;
}

bool BareBoneSim800::dellAllSMS(){
	/* This deletes all sms in memory  
	
	*/
	byte result;
	gsmSerial.print(F("AT+CMGDA=\"DEL ALL\"\r\n")); // set sms to text mode
	result = _checkResponse(25000); // max time to wait is 25secs
	if(result == OK)
	{
		previousMessageIndex = 0;
		currentMessageIndex = 0;
		return true;
	}
	else
		return false;
}

String BareBoneSim800::getTime(){
	// This function is for get time & date from the network
	// first enable the bearer profile
	_enableBearerProfile();
	gsmSerial.print(F("AT+CIPGSMLOC=2,1\r\n"));
	String _buffer = _readData();
	//Serial.println("sasd");
	delay(10);
	_buffer = _readData(); // This second read should work out
	// here we disbale the bearer profile back
	delay(100);
	_disableBearerProfile();
	if (_buffer.indexOf("0,") != -1){ // here we should fetch the result only if we get 0 and not error or some other numbers
		return _buffer.substring(_buffer.indexOf("0,"),(_buffer.indexOf("OK")-4));
	}
	else
		return "0";
}

String BareBoneSim800::getLocation(){
	// this function is used for approximating the location for the device
		// first enable the bearer profile
	_enableBearerProfile();
	gsmSerial.print(F("AT+CIPGSMLOC=1,1\r\n"));
	String _buffer = _readData();
	delay(10);
	_buffer = _readData(); // This second read should work out
	delay(100);
	// here we disbale the bearer profile back
	_disableBearerProfile();
	if (_buffer.indexOf("0,") != -1){ // here we should fetch the result only if we get 0 and not error or some other numbers
		return _buffer.substring(_buffer.indexOf("0,"),(_buffer.indexOf("OK")-4));
	}
	else
		return "0";
}

byte BareBoneSim800::getBattPercent(){
	//This function is use for getting battery percentage
	//It will return a value between 0 - 100
	//+CBC: 0,100,4208

	String buffer;
	gsmSerial.print(F("AT+CBC\r\n"));
	buffer = _readData();
	String buffer2 = buffer.substring(buffer.indexOf(",")+1);
	return buffer2.substring(0, buffer2.indexOf(",")).toInt(); // converts the result to interger
}

/*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
This Section Focuses on the GPRS connectivitiy
*/

bool BareBoneSim800::gprsDisconnect()
{
	// This function disconnect the GPRS 
	gsmSerial.print(F("AT+CIPSHUT\r\n"));
	byte result = _checkResponse(65000);
	if(result != OK)
		return false;
	delay(30);
	gsmSerial.print(F("AT+CGATT=0\r\n"));
	result = _checkResponse(65000);
	if(result != OK)
		return false;
	// disable bearer profile
	_disableBearerProfile();
	return true;
}

bool BareBoneSim800::gprsConnect(){
	// This function connects to the internet 
	gprsDisconnect();
	String buffer;
	_enableBearerProfile(); // Activate the GPRS connectivity 
	// attach the GPRS service
	gsmSerial.print(F("AT+CGATT=1\r\n"));
	byte result = _checkResponse(20000);
	delay(20);
	if (result != OK)
		return false;
		
	gsmSerial.print(F("AT+CIPMUX=0\r\n"));
	result = _checkResponse(10000);
	if (result != OK)
		return false;
	delay(10);
	gsmSerial.print(F("AT+CSTT=\""));  
	gsmSerial.print(_networkAPN);
	gsmSerial.print(F("\",\""));
	gsmSerial.print(_userName);
	gsmSerial.print(F("\",\""));
	gsmSerial.print(_passWord);
	gsmSerial.print(F("\"\r\n")); //  

	result = _checkResponse(60000);
	if(result != OK)
		return false;
	delay(10);
	gsmSerial.print(F("AT+CIICR\r\n"));
	result = _checkResponse(85000); 
	if(result != OK)
		return false;
	delay(10);
	gsmSerial.print(F("AT+CIFSR\r\n"));
	result = _checkResponse(5000); // this basically change to IP STATUS	
	// but at the stage I believe IP will already be available
	return true;	
	
}
/*
bool BareBoneSim800::serverConnect(const char *host, const uint16_t port){
	// this function connects to the host/serverConnect
	delay(10);
	//byte result = _checkResponse(2000);
	gsmSerial.print(F("AT+CIPSTART=\"TCP\",\""));
	gsmSerial.print(host);
	gsmSerial.print("\",");
	gsmSerial.print(port);
	gsmSerial.print("\r\n");
	byte result = _checkResponse(10000);
	delay(10);
	result = _checkResponse(75000);
	if(result == CONNECT_OK || result == ALREADY_CONNECT)
		return true;
	else 
		return false;
}
*/

/*
String BareBoneSim800::sendTCPData(char *data)
{
	// This function sends the TCP data 
	byte result;
	int len = strlen(data); // get the length
	gsmSerial.print(F("AT+CIPSEND=")); // 
	gsmSerial.print(len);
	gsmSerial.print(F("\r"));
	result=_checkResponse(5000); // to clear buffer and see if successful
	if(result != READY_TO_RECEIVE){
		return "";
	}
	Serial.println("Now Ready to Receive");
	gsmSerial.print(data);
	gsmSerial.print(F("\r\n"));
	//result=_checkResponse(1000);
	gsmSerial.print((char)26);
	result = _checkResponse(20000);
	delay(10);
	String buffer = _readData();
	if(buffer.indexOf("SEND OK") != -1) // means it was successfull sent and data is available to be read
		return buffer.substring(buffer.indexOf("SEND OK")+1);
	delay(50);	
	return "";
	}
	*/
	
String BareBoneSim800::sendHTTPData(char *data)
{
	// This function performs HTTP Client 
	byte result;
	gsmSerial.print(F("AT+HTTPINIT\r\n"));
	result = _checkResponse(10000);
	if(result != OK)
		return "";
	delay(5);
	gsmSerial.print(F("AT+HTTPPARA=\"CID\",1\r\n"));
	result = _checkResponse(10000);
	if(result != OK)
		return "";
	delay(5);
	gsmSerial.print(F("AT+HTTPPARA=\"URL\","));
	gsmSerial.print(data);
	gsmSerial.print(F("\r\n"));
	result = _checkResponse(10000);
	if(result != OK)
		return "";
	delay(5);
	gsmSerial.print(F("AT+HTTPACTION=0\r\n"));
	result= _checkResponse(100000);
	//if(result != OK)
	//	return "";
	delay(10);
	result = _checkResponse(20000);
	gsmSerial.print(F("AT+HTTPREAD\r\n"));
	String buffer = _readData();
	delay(50);
	return buffer;
	
}

void BareBoneSim800::closeHTTP()
{
	// This close the TCP connection
	gsmSerial.print("AT+HTTPTERM\r\n");
	byte result = _checkResponse(10000);
	delay(30);
}

	
