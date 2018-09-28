/*    This is a bare bone library for communicating with SIM800
 *    It's barebone in that - it only provides basic functionalities while still 
 *    maintaining strong performance and being memory friendly.
 *    It currently supports GSM(sending and reading SMS), 
 *    GPRS connectivity(sending and receiving TCP) with Time and Location
 *     
 *    This library is written by  Ayo Ayibiowu.
 *    charlesayibiowu@hotmail.com
 *    Designed to work with the GSM Sim800l module,
 *    and possibily other SIMxxx modules(not tested with others)
 *  
 *    This library use SoftwareSerial, you can define the RX and TX pin
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
  *       |  ARDUINO MEGA >>>   SIM800L  |
 *        -----------------------------
 *            GND      >>>   GND
 *        RX  48       >>>   TX    
 *        TX  46       >>>   RX
 *                 
 *   POWER SOURCE 4.2V (MAX) >>> VCC
 *
 *        Created on: Oct 24, 2017
 *        Author: Ayo Ayibiowu
 *        Email: charlesayibiowu@hotmail.com
 *        Version: v1.2
 *        
 *		?????????????????????????????????????????????????
 *		Updates:
 *		SoftwareSerial upgraded to AltSoftSerial to support compartibility with other PinChange Interrupt Library
 *		and SPI based Library. 
 *
*/

 
 #ifndef BareBoneSim800_h
 #define BareBoneSim800_h
 
 #include "AltSoftSerial.h"
 #include "Arduino.h"
 
 #define RX_PIN 8		//not needed since the AltSoftSerial has already define it inside
 #define TX_PIN 9
 #define RESET_PIN 2 // pin to reset not currently used is this version
 
#define TIMEOUT 99
#define ERROR 0
#define NOT_READY 1
#define READY 2
#define CONNECT_OK 3
#define CONNECT_FAIL 4
#define ALREADY_CONNECT 5
#define SEND_OK 6
#define SEND_FAIL 7
#define DATA_ACCEPT 8
#define CLOSED 9
#define READY_TO_RECEIVE 10 // basically SMSGOOD means >
#define OK 11

#define DEBUG 0 //This enables debugging mode, to disable it - set value to 0


 
 class BareBoneSim800
 {
	 private:
	 // some variables to be used
	   const char* _networkAPN;
		const char* _userName;
		const char* _passWord;
	   static const uint8_t _responseInfoSize = 12; 
	   const char* _responseInfo[_responseInfoSize] =
				  {"ERROR",
				  "NOT READY",
				  "READY",
				  "CONNECT OK",
				  "CONNECT FAIL",
				  "ALREADY CONNECT",
				  "SEND OK",
				  "SEND FAIL",
				  "DATA ACCEPT",
				  "CLOSED",
				  ">",
				  "OK"};
	   

	   	 // some private function 
		 	//void _flushSerial(uint16_t timeout);
			byte _checkResponse(uint16_t timeout);
	   String _readData();
	   //String _readData(uint16_t timeout);
	   void _setUp();
	   int _getLatestMessageIndex();
	   void _enableBearerProfile();
	   void _disableBearerProfile();
	   

	 public:
	 
	 BareBoneSim800();
	 BareBoneSim800(const char* networkAPN);
	 BareBoneSim800(const char* networkAPN, const char* userName, const char* passWord);
	 
	 volatile int previousMessageIndex = 0; //stores the last read message index
	 volatile int currentMessageIndex = 0; //stores the latest message index

	 	 // some public function 
	 void begin();
	 bool isAttached();
	 void flushSerial(uint16_t timeout);

	 
	 bool setFullMode();
	 bool enterSleepMode();
	 bool disableSleep();
	 
	 bool sendSMS(const char* number, char* text);
	 String readSMS(uint8_t index); // reads an sms at a particular index
	 String readSIMNumber(); //reads the sim registered number
	 bool checkNewSMS(); //checks for new sms
	 bool dellAllSMS(); // deletes all sms 
	 
	 String getTime(); // return time of the network
	 String getLocation(); //return location of device
	 byte getBattPercent(); //returns the battery level in percentage
	 
	 // GPRS Connectivity
	 bool gprsConnect();
	 bool gprsDisconnect();
	 
	 //HTTP Handling
	 //bool serverConnect(const char* host, const uint16_t port);
	// String sendTCPData(char* data);
	 String sendHTTPData(char* data);
	 void closeHTTP();
	 
 };
 
 #endif
	 

	 
	 
	 
	 
	 