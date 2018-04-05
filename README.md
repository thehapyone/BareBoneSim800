## A BareBone SIM800l Library Module for Arduino
The Purpose of This library is to use the SIM800 module providing core functionalities while still maintaining strong performance and being memory efficient.

If you like **BareBoneSim800** - give it a star, or fork it and contribute!

This Library uses the AltSoftSerial Library instead of the SoftSerial Library to allow for compatibility with other PinChange Interrupts Library and SPI Libraries

## Applications and Features
+ Send SMS, Read SMS and Delete SMS
+ AT Command Test
+ Send and Receive HTTP data
+ TCP Connection
+ IoT Applications
+ Low power saving mode
+ Location and Time Update

## Getting Started
Please kindly follow the examples in the example folder.

## To download and Install
Go to [Release](https://github.com/thehapyone/BareBoneSim800/releases) and click the " Source Code (zip) " button to download the latest release, then rename the uncompressed folder to **BareBoneSim800**. 
  Check that the *BareBoneSim800* folder contains BareBoneSim800.cpp and BareBoneSim800.h and copy the folder into the Arduino Library directory in your documents folder.
  i.e Place the BareBoneSim800 library folder your arduinosketchfolder/libraries/ folder. 
  You may need to create the libraries subfolder if its your first library. Restart the IDE and you are good to go.

#### OR 
  
Go to arduino IDE Sketch >> Import Library >> add library... Select the folder BareBoneSim800


## connections & pinouts

     PINOUT: 
         _____________________________
        |  ARDUINO UNO >>>   SIM800L  |
         -----------------------------
             GND      >>>   GND
         RX  8       >>>   TX    
         TX  9       >>>   RX
		 
	  |  ARDUINO MEGA >>>   SIM800L  |
        -----------------------------
            GND      >>>   GND
        RX  48       >>>   TX    
        TX  46       >>>   RX
       
                  
    POWER SOURCE (3.8V - 4.2V) >>> VCC
You can regulate the voltage to the desired voltage by using a diode to step down th Arduino 5V to 4.2v or just use a Lipo battery

## Methods and functions

Name|Return|Notes
:-------|:-------:|:-----------------------------------------------:|
begin()|None|Initialize the library
isAttached()|true or false|Checks if the GSM Module is attached or not.
flushSerial(timeout)|None|Flush out any unwanted data coming from the GSM Module.
setFullMode()|true or false|Activates the device to full functionality.
enterSleepMode()|true or false|Puts the GSM module into a power saving and sleep mode.
disableSleep()|true or false|Disable sleep for the device and wakes it up, gives error if device not in sleep before.
sendSms(number,text)|true or false|Sends  sms.
readSms(index)|String|index is the position of the sms in the prefered memory storage
dellAllSMS()|true or false|Deletes all message in the SIM800 module.
getTime()|String|Gets time of the network in GMT+0
getLocation()|String|Gets approximate location of device
readSIMNumber()|String|Gets sim number stored on sim. Returns empty if sim number not found.
checkNewSMS()|true or false|Checks if a SMS has arrived. Needs to be call for sms index update
previousMessageIndex()|Int|Variable for holding the last stored sms index
currentMessageIndex()|Int|Variable for holding the latest sms index
getBattPercent()|Byte|Battery percentage 
gorsConnect()|true or false|Connects to APN network
gprsDisconnect()|true or false|Disconnects from Gprs network
sendHTTPData(data)|String|Make an HTTP Request
closeHTTP()|None|Close connection
____________________________________________________________________________________


## Thanks!
Thank you for downloading and enjoying the BareBoneSim800 library.
I hope you find it useful. Heck, I wrote it for you- yeah, that's right- you.

This Library was inspired from the SIM800 Library from Cristainsteib(https://cristiansteib.github.io/Sim800l) and the Powerful TingGSMLibrary by Volodymyr Shymanskyy.
Special Thanks to Paul Stoffregen that created the AltSoftSerial (http://www.pjrc.com/teensy/td_libs_AltSoftSerial.html) Library, it's library was the backbone of this Library communication

Contributing to this software is warmly welcomed

## License
This project is released under
The GNU Lesser General Public License (LGPL-3.0)

