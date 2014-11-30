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


#include <Arduino.h>
#include <SPI.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include <Wire.h>

#include "UIPEthernet.h"
#include "UIPUdp.h"

#include "DHT.h"
#include "thermistor.h"
#include "Adafruit_BMP085.h"


#ifdef SNMP_ON
	#include "tinySNMP.h"
	EthernetUDP udp;
#endif
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

int counter = 0;

struct cfgStruct 
{
	//General settings
	char cfgVersion[6];	//config version
	//Network settings
	 byte mac[7]; 
	 byte ip[5],subnet[5],gateway[5],dnsServer[5];
	//char c;
	//long d;
	//float e[6];
	//SNMP settings
	char communitySNMP[SNMP_COMM_MAX_SIZE];
	//byte ipTrapServer[5];
	//int ipTrapPort;
} cfg =
	{
		"v.1.0",
		// The default values
		{0x00,0x99,0x99,0x3D,0x30,0x42},
		{10,62,64,249},{255,255,255,0},{10,62,64,8},{10,62,0,235},
		//'c',
		//10000,
		//{4.5, 5.5, 7, 8.5, 10, 12}
		"public"
	};

	

/*
static byte mymac[] = { 0x00,0x99,0x99,0x3D,0x30,0x42 };  // ethernet interface mac address
IPAddress myip(10,62,64,249);  // default ip address
IPAddress subnet(255,255,255,0);  // default subnet
IPAddress gateway(10,62,64,8);   // default gateway
IPAddress dnsServer(10,62,0,235);  // default dns
*/

/*
struct MIB mibSNMP[] =
{	
	// RFC1213-MIB OIDs
	// .iso (.1)
	// .iso.org (.1.3)
	// .iso.org.dod (.1.3.6)
	// .iso.org.dod.internet (.1.3.6.1)
	// .iso.org.dod.internet.mgmt (.1.3.6.1.2)
	// .iso.org.dod.internet.mgmt.mib-2 (.1.3.6.1.2.1)
	// .iso.org.dod.internet.mgmt.mib-2.system (.1.3.6.1.2.1.1)
	// .iso.org.dod.internet.mgmt.mib-2.system.sysDescr (.1.3.6.1.2.1.1.1)
	{{0x2B,0x6,0x1,0x2,0x1,0x1,0x1,0x0},"Arduino tinySNMP"},
	// .iso.org.dod.internet.mgmt.mib-2.system.sysObjectID (.1.3.6.1.2.1.1.2)
	{{0x2B,0x6,0x1,0x2,0x1,0x1,0x2,0x0},"1.3.6.1.3.36582.0"},
	// .iso.org.dod.internet.mgmt.mib-2.system.sysUpTime (.1.3.6.1.2.1.1.3)
	{{0x2B,0x6,0x1,0x2,0x1,0x1,0x3,0x0},"0"},
	// .iso.org.dod.internet.mgmt.mib-2.system.sysContact (.1.3.6.1.2.1.1.4)
	{{0x2B,0x6,0x1,0x2,0x1,0x1,0x4,0x0},"Alexander Bugrov"},
	// .iso.org.dod.internet.mgmt.mib-2.system.sysName (.1.3.6.1.2.1.1.5)
	{{0x2B,0x6,0x1,0x2,0x1,0x1,0x5,0x0},"tinySNMP"},
	// .iso.org.dod.internet.mgmt.mib-2.system.sysLocation (.1.3.6.1.2.1.1.6)
	{{0x2B,0x6,0x1,0x2,0x1,0x1,0x6,0x0},"Russia NNov"},
	// Arduino defined OIDs
	// .iso.org.dod.internet.private (.1.3.6.1.4)
	// .iso.org.dod.internet.private.enterprises (.1.3.6.1.4.1)
	// .iso.org.dod.internet.private.enterprises.arduino (.1.3.6.1.4.1.36582)
	//
	//analog sensors (.1.3.6.1.4.1.36582.1.x.0)
	{{0x2B,0x6,0x1,0x4,0x1,0x82,0x9D,0x66,0x1,0x1,0x0},"ASensor 1"},
	{{0x2B,0x6,0x1,0x4,0x1,0x82,0x9D,0x66,0x1,0x2,0x0},"ASensor 2"},
	//...
	//digital sensors (.1.3.6.1.4.1.36582.2.y.0)
	{{0x2B,0x6,0x1,0x4,0x1,0x82,0x9D,0x66,0x2,0x1,0x0},"DSensor 1"},
	{{0x2B,0x6,0x1,0x4,0x1,0x82,0x9D,0x66,0x2,0x2,0x0},"DSensor 2"}
	//...
	
};
*/


//-------------SETUP------------------
void setup ()
{

#ifdef DEBUG
	Serial.begin(57600);
	Serial.println("[My Server]");
#endif

//DHCP-init
	if (!Ethernet.begin(cfg.mac))
	{
		#ifdef DEBUG
		Serial.println("DHCP failed");
		#endif

		//initialize default eth
		Ethernet.begin(cfg.mac, cfg.ip, cfg.dnsServer, cfg.gateway, cfg.subnet);
	}
	else
	{
		#ifdef DEBUG
		Serial.println("DHCP OK");
		#endif
	}

//DHT sensor init
	dht.begin();
//BMP sensor init	
	bmp.begin();	
	delay(1000);
	
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

	//start udp server
	//int success = udp.begin(161);
	//Serial.print("Initialize UDP: ");
	//Serial.println(success ? "success" : "failed");
	
}

//------------------------------------------------------------
void loop ()
{
	int error = udp.begin(161);
	//read sensors
	//read analog
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
	//read DHT
	float t = dht.readTemperature(0);
	float h = dht.readHumidity();
	//read BMP
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
	//Serial.print("sec ");
	//Serial.println(millis() / 1000);
		
#ifdef SNMP_ON
	//start udp server
	//int success = udp.begin(161);
	//Serial.print("Initialize UDP: ");
	//Serial.println(success ? "success" : "failed");
	//byte packet[SNMP_MAX_PACKET_LEN];
	
	int packetSize = udp.parsePacket();
	
	//packet exist
	if(packetSize > 0 && packetSize <= SNMP_MAX_PACKET_LEN)
	//if(packetSize > 0)
	{
		struct OID oid = {0,0,0,0,0,0,0,0,0};
		//UDP sender IP and port 
		IPAddress remoteIP = udp.remoteIP();
		unsigned int remotePort = udp.remotePort();

		//print remote ip and port
		Serial.print("From ");
		for (int i =0; i < 4; i++)
		{
		  Serial.print(remoteIP[i], DEC);
		  if (i < 3)
		  {
			Serial.print(".");
		  }
		}
		Serial.print(", port ");
		Serial.println(remotePort);
		
		//read packet
		byte packet[packetSize];
		do //read
		{
			int len = udp.read(packet,packetSize); 
		}
		while (udp.available());
		udp.flush(); //finish reading this packet
/*
		Serial.print("cfg.communitySNMP = ");
		Serial.println(cfg.communitySNMP);
*/		
		//print packet
		//packetSNMPprint(packet,packetSize);
		
		//check packet for SNMP
		error = packetSNMPcheck(packet,packetSize); 

		if(!error)
		{
			//error = packetSNMPcommunity(packet,packetSize,cfg.communitySNMP,sizeof(cfg.communitySNMP)-1);
			error = packetSNMPcommunity(packet,packetSize,cfg.communitySNMP,strlen(cfg.communitySNMP));
			//Serial.print("strlen(cfg.communitySNMP)=");
			//Serial.println(strlen(cfg.communitySNMP));
			
			if(!error)
			{
				//print packet
				packetSNMPprint(packet,packetSize);
				//TODO read oid
				error = packetSNMPoid(packet,packetSize,&oid);
				if(!error)
				{
					switch(unsigned int checkOID( struct OID *oid))
					{
						case 1:
							
							break;
						case 2:
							
							break;
						case 3:
							
							break;
						case 5:
							
							break;
						case 6:
							
							break;
						case 17:
							
							break;
						case 18:
							
							break;
						case 19:
							
							break;
						case 20:
							
							break;
						case 4:
							
							break;
						
						default:
							
							break;
					}
				}
				Serial.print("error = ");
				Serial.println(error);
			//	for(int i=0;i<strlen(oid.oid);i++)
			/*
				for(int i=0;i<error;i++)
				{
					Serial.print("oid.oid[");
					Serial.print(i);
					Serial.print("]=");
					Serial.println(byte(oid.oid[i]),HEX);
				}
			*/	
				
				Serial.print("byte SNMPpduType =");
				Serial.println(oid.SNMPpduType,HEX);
				Serial.print("int SNMPreqID =");
				Serial.println(oid.SNMPreqID,DEC);
				Serial.print("byte SNMPerr=");
				Serial.println(oid.SNMPerr,HEX);
				Serial.print("byte SNMPerrID =");
				Serial.println(oid.SNMPerrID,HEX);
				Serial.print("int SNMPoidLen =");
				Serial.println(oid.SNMPoidLen,DEC);
				
				for(int i=0;i<oid.SNMPoidLen;i++)
				{
					Serial.print("SNMPoid[");
					Serial.print(i);
					Serial.print("]=");
					Serial.println(byte(oid.SNMPoid[i]),HEX);
				}
				

				
				Serial.print("byte SNMPvalType =");
				Serial.println(oid.SNMPvalType,HEX);
				
			}
		}
		
		//TODO error
		//Serial.print("error = ");
		//Serial.println(error);
/*		for(int i=0;i<10;i++)
		{
			Serial.print("size numMIB[");
			Serial.print(i);
			Serial.print("]=");
			Serial.print(strlen(mibSNMP[i].numMIB));
			Serial.print(" valMIB[");
			Serial.print(i);
			Serial.print("]=");
			Serial.println(mibSNMP[i].valMIB);			
		}
*/
	}
#endif
	
	//delay(5000);
	
	#ifdef DEBUG
	/*	
	Serial.print("TA ");
	Serial.println(Thermistor(sensorA[15]));
	Serial.print("DT ");
	Serial.println(t);
	Serial.print("DH ");
	Serial.println(h);
	Serial.print("BT ");
	Serial.println(tb);
	Serial.print("BP ");
	Serial.println(pb);	
	*/
	//Serial.print("sec ");
	//Serial.println(millis() / 1000);
	#endif
	
	if(counter < 500) 
	{
		counter++;
	}
	else
	{
		counter = 0;
		//udp.stop(); //stop udp server
		Ethernet.maintain(); //reinit eth DHCP
		delay(1000);
		//udp.begin(161);
		Serial.print("Maintain sec ");
		Serial.println(millis() / 1000);
		Serial.println("==================");
	}
	udp.stop(); //stop udp server
	//udp.begin(161);
	//delay(500);
}