#include "tinySNMP.h"

int packetSNMPread(byte _packet[])
{
	int _packetSize = sizeof (_packet[])
	//print packet
	for(int n = 0;n < _packetSize;n++)
	{
		Serial.print("p[");
		Serial.print(n); 
		Serial.print("]= "); 
		Serial.print(_packet[n],HEX);
		Serial.print(" ");
		Serial.println(_packet[n]);
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