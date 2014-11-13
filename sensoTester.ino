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
	EthernetUDP udp;
	#define SNMP_MAX_PACKET_LEN UIP_UDP_MAXPACKETSIZE
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


static byte mymac[] = { 0x00,0x99,0x99,0x3D,0x30,0x42 };  // ethernet interface mac address
IPAddress myip(10,62,64,249);  // default ip address
IPAddress subnet(255,255,255,0);  // default subnet
IPAddress gateway(10,62,64,8);   // default gateway
IPAddress dnsServer(10,62,0,235);  // default dns

//-------------SETUP------------------
void setup ()
{

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

		//initialize default eth
		Ethernet.begin(mymac, myip, dnsServer, gateway, subnet);
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
	
}

//------------------------------------------------------------
void loop ()
{
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

		
	#ifdef SNMP_ON
	//start udp server
	int success = udp.begin(161);
	Serial.print("Initialize UDP: ");
	Serial.println(success ? "success" : "failed");
	
	int size = udp.parsePacket();
	byte _packet[SNMP_MAX_PACKET_LEN];
	
	if (size) 
	{
		Serial.print("sec ");
		Serial.println(millis() / 1000);
		
		Serial.print("Packet size = "); 
		Serial.println(size);
		
		Serial.print("From ");
		IPAddress remote = udp.remoteIP();
		for (int i =0; i < 4; i++)
		{
		  Serial.print(remote[i], DEC);
		  if (i < 3)
		  {
			Serial.print(".");
		  }
		}
		Serial.print(", port ");
		Serial.println(udp.remotePort());
		
		do
		  {
			int len = udp.read(_packet,size);
		  }
		while (udp.available());
		//finish reading this packet:
		udp.flush();
		
		for(int n = 0;n < size;n++)
		{
			Serial.print("[");
			Serial.print(n); 
			Serial.print("]= "); 
			Serial.print(_packet[n],HEX);
			Serial.print(" ");
			Serial.println(_packet[n]);
		}
		free(_packet);
	}
		
	//stop udp server
		udp.stop();
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
	Serial.print("~");
	#endif
}