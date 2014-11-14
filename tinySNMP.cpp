#include "tinySNMP.h"

int _packetSNMPread(EthernetUDP _udpSNMP, byte _packet[])
{
	//Serial.print("Initialize UDP: ");
	//Serial.println(success ? "success" : "failed");
	int _packetSize = _udpSNMP.parsePacket();
	if(_packetSize == 0) return SNMP_PACKET_INVALID;
	
	Serial.print("Packet size = "); 
	Serial.println(_packetSize);
	
	IPAddress _remoteIP = _udpSNMP.remoteIP();
	int _remotePort = _udpSNMP.remotePort();
/*
	//print remote ip
	Serial.print("From ");
	for (int i =0; i < 4; i++)
	{
	  Serial.print(_remoteIP[i], DEC);
	  if (i < 3)
	  {
		Serial.print(".");
	  }
	}
	Serial.print(", port ");
	Serial.println(_remotePort);
*/	
	//copy packet to buffer
	if(_packetSize > 0) 
	{
		if(_packetSize > SNMP_MAX_PACKET_LEN) //max packet size
		{
			return SNMP_ERR_TOO_BIG;
		}
		else
		{
			do //read
			{
				int len = _udpSNMP.read(_packet,_packetSize); 
			}
			while (_udpSNMP.available());
			//_udpSNMP.flush(); //finish reading this packet

			//print packet
			for(int n = 0;n < _packetSize;n++)
			{
				Serial.print("[");
				Serial.print(n); 
				Serial.print("]= "); 
				Serial.print(_packet[n],HEX);
				Serial.print(" ");
				Serial.println(_packet[n]);
			}
		
		}
	}
	
	// packet check to SNMP and size
	if ( _packet[0] != 0x30 || _packet[1] != _packetSize-2) 
	{
		//Serial.println("SNMP_API_STAT_PACKET_INVALID");
		return SNMP_PACKET_INVALID;
	}
	
/*
	// validate packet
	if ( _packetSize != 0 && _packetSize > SNMP_MAX_PACKET_LEN ) 
	{
		Serial.println("SNMP_API_STAT_PACKET_TOO_BIG");
		return SNMP_ERR_TOO_BIG;
	}
*/		

	
	// sequence len
	int _seqLen = _packet[1];
	// version
	int _versionSNMP = _packet[4]; //version SNMP
	// community string
	int _comLen = _packet[6];
/*	for(int i=0,i<comLen)
	{
		community
	}
	// pdu
	pduTyp = _packet[comEnd + 1];
	pduLen = _packet[comEnd + 2];
	ridLen = _packet[comEnd + 4];
	ridEnd = comEnd + 4 + ridLen;
	errLen = _packet[ridEnd + 2];
	errEnd = ridEnd + 2 + errLen;
	eriLen = _packet[errEnd + 2];
	eriEnd = errEnd + 2 + eriLen;
	vblTyp = _packet[eriEnd + 1];
	vblLen = _packet[eriEnd + 2];
	vbiTyp = _packet[eriEnd + 3];
	vbiLen = _packet[eriEnd + 4];
	obiLen = _packet[eriEnd + 6];
	obiEnd = eriEnd + obiLen + 6;
	valTyp = _packet[obiEnd + 1];
	valLen = _packet[obiEnd + 2];
	valEnd = obiEnd + 2 + valLen;
*/	//
	
}