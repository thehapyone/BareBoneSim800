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
 *        Created on: Sep 28, 2018
 *        Author: Ayo Ayibiowu
 *        Email: charlesayibiowu@hotmail.com
 *        Version: v1.2
 *		
 *		This Example demonstrate how to use the Sim800l to push data to the PubNub Data Stream Network
 *		The code is an abstract from an Sim800 based IoT Energy Meter 
 *        
 *
*/

#include <BareBoneSim800.h>

BareBoneSim800 sim800("your APN network");  //to declare the library with an APN
//BareBoneSim800 sim800("gloworld");


/******************GSM/GPRS Config************************************/
// Connecting to the Internet and Acting as an HTTP Web Client
// username and password has been set to "" in the Library Code

const char networkAPN[] = "web.gprs.mtnnigeria.net"; 

const char networkServer[] = "pubsub.pubnub.com"; // this is the server address
const int port = 80;

const char publishKey[] = "demo"; // secret key for publishing
const char subscribeKey[] = "demo"; // secret key for subscribing

// here we are creating an instance of the SIM800 Class with the network provider APN
BareBoneSim800 gsmClient(networkAPN); 

/*
	This Initialize the GSM Device and Ensures the Modem connects to the network with issues. 
*/
bool initializeGSMDevice()
{
  gsmClient.begin();
  delay(8000); // Here we are going to wait for about 10 secs for the GSM device to connect to a network
  if (!gsmClient.isAttached())
  {
    return false;
  }
  return true;
}

/*******************END of GSM Config*****************************************/

void setup()
{
	Serial.begin(9600);
	sim800.begin();
	while(!Serial);

	Serial.println("Testing GSM module With The PUBNUB DataStream Network");
	Serial.println("Don't Forget to Change the Default Network APN to Yours");

	delay(8000); // this delay is necessary, it helps the device to be ready and connect to a network
	
	bool deviceCheck = initializeGSMDevice();
	if(!deviceCheck)// means an error has occurred
	{
		// device should be restarted, at this point - error is coming from gsm device module
		// do something here   
		Serial.println("Something has gone wrong");
		while(true); // here the device won't continue, until this error has been fixed
	  }
	  Serial.println("Now Ready");
	  
	delay(5000); 
	
	float value1 = random(1,100);
	float value2 = random(45, 1000);
	Serial.println(value1);
	Serial.println(value2);
	// This function sends the data to pubnub. 
	bool result = cloudSend(value1, value2, "hello_world");
	if (result)
		Serial.println("Sent");
	else
		Serial.println("Error Sending");
	
	// That's all. Enjoy
	// You can view the result in your PubNub channel. 
	
	/*
	To subscribe and receive messages, issue a GET request in the following format:
	http://pubsub.pubnub.com
	/subscribe
	/sub-key
	/channel
	/callback
	/timetoken
	
	EXAMPLE -
	http://pubsub.pubnub.com
	/subscribe
	/demo
	/hello_world
	/0
	/0
	
	You will receive a response as an array. The first element of the array will be an empty array 
	(at first) and the second is a timetoken. You must use this timetoken response from the server 
	at every following request. Each response will have a brand new timetoken. So you must replace the old timetoken each time.
	
	// Initial Response
	[[],"9718304464409"]

	Take the "timetoken" value and place it in the following request like this and open it in your browser, You will see your posted info -
	
	http://pubsub.pubnub.com
	/subscribe
	/demo
	/hello_world
	/0
	/9718304464409
	
	More Information about the API is available here - https://www.pubnub.com/http-rest-push-api/
	
	*/
	
}

void loop()
{
	//zzZZZzz
	
}


bool sendToCloud(float value1, float value2, String channelName)
{
  // This function sends out the readings to the cloud server
  // returns success or fail
  String dataInput = String(value1)+"%2C" + String(value2);
  bool stage1 = gsmClient.isAttached();
  if(!stage1)
  { 
    return false;
  }
  if(!gsmClient.gprsConnect())
  { 
    return false;
  }
  String ResourceData = String(networkServer)+"/publish/"+String(publishKey)+"/"+String(subscribeKey)
              + "/0/" + channelName + "/0/%22" + dataInput + "%22"; // Send the data out
              
  //pubsub.pubnub.com/publish/demo/demo/0/hello_world/0/%22Joker24%22
  String result = gsmClient.sendHTTPData(strdup(ResourceData.c_str()));
  
  gsmClient.closeHTTP();
  gsmClient.gprsDisconnect();
      if(result.indexOf("Sent") != -1)
    {
      // means an ack is found
      return true;
    }
    else
      return false;
}

bool cloudSend(float value1, float value2, String channelName)
{
  /* this send data to the cloud */
  uint8_t count = 0;
  bool sent = false;
  // Due to some potential error, we will attempt to resend the data to cloud 
  // for 3 times at most if it fails
  while(count < 3)
  {
    // first we flush the serial, just in case some unwanted data are wanting
    gsmClient.flushSerial(1000);
    if (sendToCloud(value1, value2, channelName))
    {
      sent = true;
      break;
    }
    count++;
    delay(500);    
  }
  return sent;
}



