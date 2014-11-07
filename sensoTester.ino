/*

 */

#define DEBUG
//#undef DEBUG
#define LCDB
//#undef LCDB
//#define UIPETHERNET_DEBUG
#define SNMP_ON
//#undef SNMP_ON
//#define HTTP_ON
#undef HTTP_ON

// ID of the settings block
#define cfg_VERSION "001"
// Tell it where to store your cfg data in EEPROM
#define cfg_START 32
// max characters in page name
#define MAX_PAGENAME_LEN 8 
// declare a static string
#define P(name) static const prog_uchar name[] PROGMEM 

#include <Arduino.h>
#include <SPI.h>
#include <EEPROM.h>
#include <UIPEthernet.h>
#include <UIPUdp.h>
#include <avr/pgmspace.h>
#include <Wire.h>

#include "DHT.h"
#include "thermistor.h"
#include "Adafruit_BMP085.h"
#include "Agentuino.h"

#ifdef LCDB
	#include <LiquidCrystal.h>
	#include "lcdb.h"
	LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
#endif LCDB

//Analog sensor
//int AnalogChannel = 15;
//int DigitalChannel = 8;
int sensorA [16];
//-------------
//DHT sensor
#define DHTPIN 30
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
//-------------
//Pressure sensor
Adafruit_BMP085 bmp;
//-------------
#ifdef SNMP_ON
//SNMP settings
// RFC1213-MIB OIDs
// .iso (.1)
// .iso.org (.1.3)
// .iso.org.dod (.1.3.6)
// .iso.org.dod.internet (.1.3.6.1)
// .iso.org.dod.internet.mgmt (.1.3.6.1.2)
// .iso.org.dod.internet.mgmt.mib-2 (.1.3.6.1.2.1)
// .iso.org.dod.internet.mgmt.mib-2.system (.1.3.6.1.2.1.1)
// .iso.org.dod.internet.mgmt.mib-2.system.sysDescr (.1.3.6.1.2.1.1.1)
static char sysDescr[] = "1.3.6.1.2.1.1.1.0";  // read-only  (DisplayString)
// .iso.org.dod.internet.mgmt.mib-2.system.sysObjectID (.1.3.6.1.2.1.1.2)
static char sysObjectID[] = "1.3.6.1.2.1.1.2.0";  // read-only  (ObjectIdentifier)
// .iso.org.dod.internet.mgmt.mib-2.system.sysUpTime (.1.3.6.1.2.1.1.3)
//static char sysUpTime[] = "1.3.6.1.2.1.1.3.0";  // read-only  (TimeTicks)
// .iso.org.dod.internet.mgmt.mib-2.system.sysContact (.1.3.6.1.2.1.1.4)
static char sysContact[] = "1.3.6.1.2.1.1.4.0";  // read-write (DisplayString)
// .iso.org.dod.internet.mgmt.mib-2.system.sysName (.1.3.6.1.2.1.1.5)
static char sysName[] = "1.3.6.1.2.1.1.5.0";  // read-write (DisplayString)
// .iso.org.dod.internet.mgmt.mib-2.system.sysLocation (.1.3.6.1.2.1.1.6)
static char sysLocation[] = "1.3.6.1.2.1.1.6.0";  // read-write (DisplayString)
// .iso.org.dod.internet.mgmt.mib-2.system.sysServices (.1.3.6.1.2.1.1.7)
static char sysServices[] = "1.3.6.1.2.1.1.7.0";  // read-only  (Integer)
// Arduino defined OIDs
// .iso.org.dod.internet.private (.1.3.6.1.4)
// .iso.org.dod.internet.private.enterprises (.1.3.6.1.4.1)
// .iso.org.dod.internet.private.enterprises.arduino (.1.3.6.1.4.1.36582)
//
//
// RFC1213 local values
static char locDescr[]              = "Agentuintuino";		// read-only (static)
static char locObjectID[]           = "1.3.6.1.3.2009.0";	// read-only (static)
static uint32_t locUpTime           = 0;					// read-only (static)
char locContact[20]          = "Eric Gionet";				// from EEPROM - read/write (not done for simplicity)
char locName[20]             = "Agentuino";					// from EEPROM - read/write (not done for simplicity)
char locLocation[20]         = "Nova Scotia, CA";			// from EEPROM - read/write (not done for simplicity)
static int32_t locServices          = 7;					// read-only (static)

uint32_t prevMillis = millis();
char oid[SNMP_MAX_OID_LEN];
SNMP_API_STAT_CODES api_status;
SNMP_ERR_CODES status;

void pduReceived()
{
	Serial.println("my pduReceived");
	SNMP_PDU pdu;
	//
	//#ifdef DEBUG
	Serial.println("UDP Packet Received Start..");
	//Serial.println(" RAM:"); 
	//Serial.println(freeMemory());
	//#endif
	//
	api_status = Agentuino.requestPdu(&pdu);
	Serial.print("api_status pdu = ");
	Serial.println(api_status);	
	//
	if ( pdu.type == SNMP_PDU_GET || pdu.type == SNMP_PDU_GET_NEXT || pdu.type == SNMP_PDU_SET
    && pdu.error == SNMP_ERR_NO_ERROR && api_status == SNMP_API_STAT_SUCCESS ) 
	{
		//
		pdu.OID.toString(oid);
		//
		//#ifdef DEBUG
		Serial.println("OID: "); 
		Serial.println(oid);
		//#endif
		//Serial << "OID: " << oid << endl;
		//
		
		if ( strcmp(oid, sysDescr ) == 0 ) 
		{
			// handle sysDescr (set/get) requests
			if ( pdu.type == SNMP_PDU_SET ) 
			{
				// response packet from set-request - object is read-only
				pdu.type = SNMP_PDU_RESPONSE;
				pdu.error = SNMP_ERR_READ_ONLY;
			} 
			else 
			{
				// response packet from get-request - locDescr
				status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locDescr);
				pdu.type = SNMP_PDU_RESPONSE;
				pdu.error = status;
			}
			//
			//#ifdef DEBUG
			Serial.println("sysDescr..."); 
			Serial.print(locDescr);
			Serial.print(" "); 
			Serial.print(pdu.VALUE.size);
			//Serial << "sysDescr..." << locDescr << " " << pdu.VALUE.size << endl;
			//#endif
		} 
/*		else if ( strcmp_P(oid, sysUpTime ) == 0 ) 
		{
			// handle sysName (set/get) requests
			if ( pdu.type == SNMP_PDU_SET ) 
			{
				// response packet from set-request - object is read-only
				pdu.type = SNMP_PDU_RESPONSE;
				pdu.error = SNMP_ERR_READ_ONLY;
			} 
			else 
			{
				// response packet from get-request - locUpTime
				status = pdu.VALUE.encode(SNMP_SYNTAX_TIME_TICKS, locUpTime);
				pdu.type = SNMP_PDU_RESPONSE;
				pdu.error = status;
			}
			//
			//#ifdef DEBUG
			Serial.println("sysUpTime..."); 
			Serial.print(locUpTime);
			Serial.print(" "); 
			Serial.print(pdu.VALUE.size);
			//Serial << "sysUpTime..." << locUpTime << " " << pdu.VALUE.size << endl;
			//#endif
		} 
*/		else if ( strcmp(oid, sysName ) == 0 ) 
		{
		  // handle sysName (set/get) requests
		  if ( pdu.type == SNMP_PDU_SET ) 
		  {
			// response packet from set-request - object is read/write
			status = pdu.VALUE.decode(locName, strlen(locName)); 
			pdu.type = SNMP_PDU_RESPONSE;
			pdu.error = status;
		  } 
		  else 
		  {
			// response packet from get-request - locName
			status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locName);
			pdu.type = SNMP_PDU_RESPONSE;
			pdu.error = status;
		  }
		  //
			//#ifdef DEBUG
			Serial.println("sysName..."); 
			Serial.print(locName);
			Serial.print(" "); 
			Serial.print(pdu.VALUE.size);
			//Serial << "sysName..." << locName << " " << pdu.VALUE.size << endl;
			//#endif
		} 
		else if ( strcmp(oid, sysContact ) == 0 ) 
		{
		  // handle sysContact (set/get) requests
		  if ( pdu.type == SNMP_PDU_SET ) 
		  {
			// response packet from set-request - object is read/write
			status = pdu.VALUE.decode(locContact, strlen(locContact)); 
			pdu.type = SNMP_PDU_RESPONSE;
			pdu.error = status;
		  } 
		  else 
		  {
			// response packet from get-request - locContact
			status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locContact);
			pdu.type = SNMP_PDU_RESPONSE;
			pdu.error = status;
		  }
		  //
		  //#ifdef DEBUG
			Serial.println("sysContact..."); 
			Serial.print(locContact);
			Serial.print(" "); 
			Serial.print(pdu.VALUE.size);
		  //Serial << "sysContact..." << locContact << " " << pdu.VALUE.size << endl;
		  //#endif
		} 
		else if ( strcmp(oid, sysLocation ) == 0 ) 
		{
		  // handle sysLocation (set/get) requests
		  if ( pdu.type == SNMP_PDU_SET ) 
		  {
			// response packet from set-request - object is read/write
			status = pdu.VALUE.decode(locLocation, strlen(locLocation)); 
			pdu.type = SNMP_PDU_RESPONSE;
			pdu.error = status;
		  } 
		  else 
		  {
			// response packet from get-request - locLocation
			status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locLocation);
			pdu.type = SNMP_PDU_RESPONSE;
			pdu.error = status;
		  }
		  //
		  //#ifdef DEBUG
		  	Serial.println("sysLocation..."); 
			Serial.print(locLocation);
			Serial.print(" "); 
			Serial.print(pdu.VALUE.size);
		  //Serial << "sysLocation..." << locLocation << " " << pdu.VALUE.size << endl;
		  //#endif
		} 
		else if ( strcmp(oid, sysServices) == 0 ) 
		{
		  // handle sysServices (set/get) requests
		  if ( pdu.type == SNMP_PDU_SET ) 
		  {
			// response packet from set-request - object is read-only
			pdu.type = SNMP_PDU_RESPONSE;
			pdu.error = SNMP_ERR_READ_ONLY;
		  } 
		  else 
		  {
			// response packet from get-request - locServices
			status = pdu.VALUE.encode(SNMP_SYNTAX_INT, locServices);
			pdu.type = SNMP_PDU_RESPONSE;
			pdu.error = status;
		  }
		  //
		  //#ifdef DEBUG
		  	Serial.println("locServices..."); 
			Serial.print(locServices);
			Serial.print(" "); 
			Serial.print(pdu.VALUE.size);
		  //Serial << "locServices..." << locServices << " " << pdu.VALUE.size << endl;
		  //#endif
		} 
		else 
		{
		  // oid does not exist
		  //
		  // response packet - object not found
		  pdu.type = SNMP_PDU_RESPONSE;
		  pdu.error = SNMP_ERR_NO_SUCH_NAME;
		  Serial.println("oid does not exist..");
		}
		//
		Agentuino.responsePdu(&pdu);
	}
	//
	Agentuino.freePdu(&pdu);
	//
	Serial.println("UDP Packet Received End.."); 
	//Serial.println(locServices);
	//Serial.print(" RAM:"); 
	//Serial.print(freeMemory());
	//Serial << "UDP Packet Received End.." << " RAM:" << freeMemory() << endl;
}
#endif
//-------------
#ifdef HTTP_ON
//Ethernet server
EthernetServer server(80); // port server
EthernetClient client;
#endif
//-------------

char buffer[MAX_PAGENAME_LEN+1];

static byte mymac[] = { 0x00,0x99,0x99,0x3D,0x30,0x42 };  // ethernet interface mac address
IPAddress myip(10,62,64,249);  // default ip address
IPAddress subnet(255,255,255,0);  // default subnet
IPAddress gateway(10,62,64,8);   // default gateway
IPAddress dnsServer(10,62,0,30);  // default dns

// Settings structure
struct cfgStruct 
{
	// This is for mere detection if they are your settings
	char version[4];
	// The variables of your settings
	 byte mac[7]; 
	 byte ip[5],net[5],gw[5],dns[5];
	//char c;
	//long d;
	//float e[6];
} cfg =
	{
		cfg_VERSION,
		// The default values
		{0x00,0x99,0x99,0x3D,0x30,0x39},
		{192,168,0,101},{255,255,255,0},{192,168,0,1},{192,168,0,1}
		//'c',
		//10000,
		//{4.5, 5.5, 7, 8.5, 10, 12}
	};

/*
const char http_OK[] PROGMEM =
"HTTP/1.0 200 OK\r\n"
"Content-Type: text/html\r\n"
"Pragma: no-cache\r\n\r\n";

const char http_Found[] PROGMEM =
"HTTP/1.0 302 Found\r\n"
"Location: /\r\n\r\n";

const char http_Unauthorized[] PROGMEM =
"HTTP/1.0 401 Unauthorized\r\n"
"Content-Type: text/html\r\n\r\n"
"<h1>401 Unauthorized</h1>";

const char http_NotFound[] PROGMEM =
"HTTP/1.0 404 Unauthorized\r\n"
"Content-Type: text/html\r\n\r\n"
"<h1>404 Not found</h1>";
*/

void loadcfg() 
{
  // To make sure there are settings, and they are YOURS!
  // If nothing is found it will use the default settings.
	if (EEPROM.read(cfg_START + 0) == cfg_VERSION[0] &&
		EEPROM.read(cfg_START + 1) == cfg_VERSION[1] &&
		EEPROM.read(cfg_START + 2) == cfg_VERSION[2])
    for (unsigned int t=0; t<sizeof(cfg); t++)
		*((char*)&cfg + t) = EEPROM.read(cfg_START + t);
}

void savecfg()
{
  for (unsigned int t=0; t<sizeof(cfg); t++)
  {
    EEPROM.write(cfg_START + t, *((char*)&cfg + t));
  }
}
/*
#ifdef LCDB
int menuLCD(int lcd_key, int numMENU, int count)
{
	switch(lcd_key)
	{
		case btnNONE:
		{
            if(count)
            break;
		}
		case btnRIGHT:
		{             
            lcd.print("RIGHT ");
            break;
		}
		case btnLEFT:
		{
            lcd.print("LEFT   "); //  push button "LEFT" and show the word on the screen
            break;
		}    
		case btnUP:
		{
            lcd.print("UP    ");  //  push button "UP" and show the word on the screen
            break;
		}
		case btnDOWN:
		{
            lcd.print("DOWN  ");  //  push button "DOWN" and show the word on the screen
            break;
		}
		case btnSELECT:
		{
            lcd.print("SELECT");  //  push button "SELECT" and show the word on the screen
            break;
		}
		

	switch(numMENU)
	{
		case menuNONE:
		{
			if(lcd_key=btnNONE && count > countMENU)
			{
				
			}
			return menuNONE;
			break;
		}
		case menuCONFIG:
		{
			
			break;
		}
		case menuANALOG:
		{
			
			break;
		}
		case menuDIGITAL:
		{
			
			break;
		}
	}
	
	}
}
#endif
*/


//-------------SETUP------------------
void setup ()
{
#ifdef LCDB
	lcd.begin(16, 2); 	// start the library
#endif
#ifdef LCDB
	lcd.setCursor(0,0);             // set the LCD cursor   position 
	lcd.print("Wait for load...");
#endif

	loadcfg(); //load from EEPROM

#ifdef DEBUG
	Serial.begin(57600);
	Serial.println("[My Server]");
#endif

//DHCP-init
	if (!Ethernet.begin(mymac))
	{
		#ifdef DEBUG
		Serial.println("DHCP failed");
		#endif
		
		#ifdef LCDB
		lcd.clear();
		//lcd.setCursor(0,0);
		lcd.print("DHCP failed");
		delay(2000);
		#endif
		
		//initialize default cfg
		Ethernet.begin(mymac, myip, dnsServer, gateway, subnet);
	}
	else
	{
		#ifdef DEBUG
		Serial.println("DHCP OK");
		#endif
		#ifdef LCDB
		lcd.clear();
		//lcd.setCursor(0,0);
		lcd.print("DHCP OK");
		delay (100);
		#endif
	}
  
#ifdef DEBUG  //print out the IP cfg
    Serial.print("My IP = ");
    Serial.println(Ethernet.localIP());
    Serial.print("My GW = ");
    Serial.println(Ethernet.gatewayIP());
    Serial.print("My Mask = ");
    Serial.println(Ethernet.subnetMask());
    Serial.print("My DNS = ");
    Serial.println(Ethernet.dnsServerIP());
#endif

#ifdef DEBUG  //print out the IP cfg
    //Serial.println(String("My cnf IP = "+String(cfg.ip[0])+"."+String(cfg.ip[1])+"."+String(cfg.ip[2])+"."+String(cfg.ip[3])));
	//Serial.print(cfg.ip[1]);
	//Serial.print(cfg.ip[2]);
	//Serial.println(cfg.ip[3]);
	
	//Serial.print("My GW = ");
	//char sh[sizeof(cfg.gw)+1];
	//sh.getBytes(cfg.gw,sizeof(cfg.gw));
	//Serial.print("My Mask = ");
	//Serial.println(cfg.net);
	//Serial.print("My DNS = ");
	//Serial.println(cfg.dns);	
#endif

#ifdef SNMP_ON
	api_status = Agentuino.begin();
	
	Serial.print("begin status = ");
	Serial.println(api_status);	
	  
	if ( api_status == SNMP_API_STAT_SUCCESS ) 
	{
		//
		Agentuino.onPduReceive(pduReceived);
		//
		delay(100);
		//
		Serial.println("SNMP Agent Initalized...");
		//
		#ifdef LCDB
		lcd.clear();
		//lcd.setCursor(0,0);
		lcd.print("SNMP OK");
		delay (5000);
		#endif
		return;
	}
#endif

	
#ifdef LCDB
	lcd.clear();
#endif

//Eth server init
#ifdef HTTP_ON
	server.begin();
#endif
//DHT sensor init
	dht.begin();
//BMP sensor init	
	bmp.begin();
	
	delay(1000);
}

//------------------------------------------------------------
void loop ()
{
	//read sensors
	for (int analogChannel = 1; analogChannel < 16; analogChannel++) 
	{
		sensorA[analogChannel] = analogRead(analogChannel);
		/*
		#ifdef DEBUG
		Serial.print(analogChannel);
		Serial.print(": ");
		Serial.print(Thermistor(sensorA[analogChannel]));
		Serial.print(" - - ");
		Serial.println(sensorA[analogChannel]);
		#endif
		*/
			
	}
	
	float h = dht.readHumidity();
	float t = dht.readTemperature(0);
	
	float tb = bmp.readTemperature();
	float pb = bmp.readPressure()/133.3;

/*
	if ( t == BAD_TEMP || h == BAD_HUM ) 
	{ // if error conditions          
		Serial.println("Failed to read from DHT");
		t=0.0;
		h=0.0;
	}
*/

	//LCD 
	#ifdef LCDB
	//lcd.clear();
	lcd.setCursor(0,0);
	//lcd.print("IP:");
	//lcd.print(Ethernet.localIP());
	lcd.print(t);
	lcd.print(" ");
	lcd.print(tb);
	lcd.print(" ");
	lcd.print(Thermistor(sensorA[15]));

	lcd.setCursor(0,1);
	//lcd.print("Temp: ");
	//lcd.print(Thermistor(sensorA[15]));
	lcd.print("H:");
	lcd.print(h);
	lcd.print("%");
	lcd.print(" p:");
	lcd.print(pb);
	//delay(100);
	#endif
 
	#ifdef SNMP_ON
	//SNMP
	for (int i=millis();millis()<(i+5000);)
	{
		Agentuino.listen();
	}
	Serial.println("sec ");
	//Serial.println(millis() / 1000);
	#endif
	
	#ifdef HTTP_ON
	// listen for incoming clients
	client = server.available();
	if(client)
	{
		//#ifdef DEBUG
		//Serial.println("new client");
		//#endif DEBUG
		// type
		int type=0;
		// an http request ends with a blank line
		boolean currentLineIsBlank = true;
		while(client.connected())
		{
			if(client.available())
			{
				//GET, POST, HEAD
				
				char c = client.read();
				Serial.write(c);
				// if you've gotten to the end of the line (received a newline
				// character) and the line is blank, the http request has ended,
				// so you can send a reply
				if (c == '\n' && currentLineIsBlank)
				{
					// send a standard http response header
					client.println("HTTP/1.1 200 OK");
					client.println("Content-Type: text/html");
					client.println("Connection: close");  // the connection will be closed after completion of the response
					//client.println(F(http_OK));
					client.println("Refresh: 5");  // refresh the page automatically every 5 sec
					client.println();
					client.println("<!DOCTYPE HTML>");
					client.println("<html>");
					// output the value of each analog input pin
					for (int analogChannel = 15; analogChannel < 16; analogChannel++) 
					{
						//int sensorReading = analogRead(analogChannel);
						client.print("analog input ");
						client.print(analogChannel);
						client.print(" is ");
						//client.print(Thermistor(sensorReading));
						client.print(Thermistor(sensorA[analogChannel]));
						client.println("<br />");
				   
					}
					client.println("</html>");
					break;
				}
				if (c == '\n') 
				{
					// you're starting a new line
					currentLineIsBlank = true;
				} 
				else if (c != '\r') 
				{
					// you've gotten a character on the current line
					currentLineIsBlank = false;
				}
			}
		}
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
	//#endif
	
	
	//#ifdef DEBUG
    Serial.println("client disonnected");
	//#endif
	}
	#endif
}