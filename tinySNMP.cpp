#include "tinySNMP.h"

#define DEBUG

void packetSNMPprint(byte _packet[],int _packetSize)
{
	//print packet
	Serial.print("Packet len=");
	Serial.print(_packetSize,DEC);
	Serial.print(" | Sequense len=");
	Serial.println(_packet[1],DEC);
	Serial.print("Packet ID= ");
	Serial.println(_packet[0],HEX);
	
	Serial.println("Packet = byte - int - char");
	for(int n = 0;n < _packetSize;n++)
	{
		Serial.print("p[");
		Serial.print(n); 
		Serial.print("]= 0x"); 
		Serial.print(byte(_packet[n]),HEX);
		Serial.print('\t');
		Serial.print(int(_packet[n]),DEC);
		Serial.print('\t');
		Serial.println(char(_packet[n]));
	}	
}

int packetSNMPcheck(byte _packet[],int _packetSize) //check SNMP
{
	//packet check max size
	if(_packetSize > SNMP_MAX_PACKET_LEN)
	{
#ifdef DEBUG
		Serial.println("SNMP big size");
#endif
		return SNMP_ERR_TOO_BIG;
	}
	
	//packet check to SNMP and size
	if( (byte(_packet[0]) != 0x30 ) || ((_packetSize-2)!= int(_packet[1]) ) )
	{
#ifdef DEBUG
		Serial.println("SNMP size invalid");
#endif
		return SNMP_ERR_GEN_ERROR;
	}

	//check version SNMP
	if(int(_packet[4]) > SNMP_VERSION) 
	{
#ifdef DEBUG
		Serial.println("SNMP version invalid");
#endif
		return SNMP_ERR_GEN_ERROR;
	}
#ifdef DEBUG
	Serial.println("SNMP packet OK");
#endif
	return SNMP_ERR_NO_ERROR;
}

int packetSNMPcommunity(byte _packet[],int _packetSize,char _community[],int _communitySize)
{	
	//community length
	int _comLen = _packet[6];
	//check community	
	if(_communitySize != _comLen)
	{
		return SNMP_ERR_NO_SUCH_NAME;
	}
	else
	{	
		for (int i=0;i < _comLen;i++)
		{
			if(_community[i] != _packet[7+i]) 
			{
#ifdef DEBUG
				Serial.println("SNMP community invalid");
#endif			
				return SNMP_ERR_NO_SUCH_NAME;
			}
#ifdef DEBUG
			Serial.print("com= ");
			Serial.print(char(_community[i]));
			Serial.print(" pkg= ");
			Serial.println(char(_packet[7+i]));
#endif
		}
	}
	return SNMP_ERR_NO_ERROR;
}

int packetSNMPoid(byte _packet[],int _packetSize, struct OID *_oid)
{
	//length community
	int _comLen = _packet[6];
	//check pdu
	byte _pduType = _packet[6 + _comLen + 1];
	int _pduLen = _packet[6 + _comLen + 2];
	//request id
	byte _ridType = _packet[6 + _comLen + 3];
	int _ridLen = _packet[6 + _comLen + 4];
	byte _ridValue = _packet[6 + _comLen + 4 + _ridLen];
	//error
	byte _errType = _packet[6 + _comLen + 4 + _ridLen + 1];
	int _errLen = _packet[6 + _comLen + 4 + _ridLen + 2];
	byte _errValue = _packet[6 + _comLen + 4 + _ridLen + 2 + _errLen];
	//error index
	byte _eriType = _packet[6 + _comLen + 4 + _ridLen + 2 + _errLen + 1];
	int _eriLen = _packet[6 + _comLen + 4 + _ridLen + 2 + _errLen + 2];
	byte _eriValue = _packet[6 + _comLen + 4 + _ridLen + 2 + _errLen + 2 + _eriLen];	
	//varbind list
	byte _vblType = _packet[6 + _comLen + 4 + _ridLen + 2 + _errLen + 2 + _eriLen + 1];
	int _vblLen = _packet[6 + _comLen + 4 + _ridLen + 2 + _errLen + 2 + _eriLen + 2];
	//varbind
	byte _vbiType = _packet[6 + _comLen + 4 + _ridLen + 2 + _errLen + 2 + _eriLen + 3];
	int _vbiLen = _packet[6 + _comLen + 4 + _ridLen + 2 + _errLen + 2 + _eriLen + 4];
	//object id
	byte _oidType = _packet[6 + _comLen + 4 + _ridLen + 2 + _errLen + 2 + _eriLen + 5];
	int _oidLen = _packet[6 + _comLen + 4 + _ridLen + 2 + _errLen + 2 + _eriLen + 6];
	//value oid
	byte _valueType = _packet[6 + _comLen + 4 + _ridLen + 2 + _errLen + 2 + _eriLen + 6 + _oidLen + 1];
	int _valueLen = _packet[6 + _comLen + 4 + _ridLen + 2 + _errLen + 2 + _eriLen + 6 + _oidLen + 2];

	//_oid.commLen=_comLen;
	_oid->SNMPpduType = _pduType;
	_oid->SNMPreqID = _ridValue;
	_oid->SNMPerr = _errValue;
	_oid->SNMPerrID = _eriValue;
	_oid->SNMPoidLen = _oidLen;
	_oid->SNMPvalType = _valueType;
	_oid->SNMPvalLen = _valueLen;
	
	//read oid
	if(_oidLen > 0 && _oidLen <= SNMP_MAX_OID_SIZE)
	{
		for (int i=0;i <= _oidLen;i++)
		{
			_oid->SNMPoid[i] = byte(_packet[6 + _comLen + 4 + _ridLen + 2 + _errLen + 2 + _eriLen + 7 + i]);
			//Serial.println(byte(_oid.oid[i]),HEX);
		}

		//read value
		if(_valueType != SNMP_TYPE_NULL && _valueLen > 0) //NULL
		{
			for (int i=0;i <= _valueLen;i++)
			{
				_oid->SNMPval[i] = byte(_packet[6 + _comLen + 4 + _ridLen + 2 + _errLen + 2 + _eriLen + 6 + _oidLen + 3 + i]);
			}
		}
		return 0;
	}
	else
	{
		return 1;
	}

}

unsigned int checkOID( struct OID *_oid)
{
	//general
	//{0x2B,0x6,0x1,0x2,0x1,0x1,x(1-6),0x0}
	if(_oid->SNMPoid[0] == 0x2B &&
		_oid->SNMPoid[1] == 0x6 &&
		_oid->SNMPoid[2] == 0x1 &&
		_oid->SNMPoid[3] == 0x2 &&
		_oid->SNMPoid[4] == 0x1 &&
		_oid->SNMPoid[5] == 0x1 &&
		_oid->SNMPoid[7] == 0x0)
	{
		for(unsigned int i=1;i<=6;i++)
		{
			if (i == (unsigned int)_oid->SNMPoid[6])
				return i;
			else return 0;
		}
	}
	//sensors
	if(_oid->SNMPoid[0] == 0x2B &&
		_oid->SNMPoid[1] == 0x6 &&
		_oid->SNMPoid[2] == 0x1 &&
		_oid->SNMPoid[3] == 0x4 &&
		_oid->SNMPoid[4] == 0x1 &&
		_oid->SNMPoid[5] == 0x82 &&
		_oid->SNMPoid[6] == 0x9D &&
		_oid->SNMPoid[7] == 0x66 &&
		_oid->SNMPoid[10] == 0x0)
	{	
		//analog sensors (.1.3.6.1.4.1.36582.1.x91-16).0)
		//{0x2B,0x6,0x1,0x4,0x1,0x82,0x9D,0x66,0x1,0x1,0x0}
		if(_oid->SNMPoid[8] == 0x1)
		{	
			for(unsigned int i=1;i<=16;i++)
			{
				if (i == (unsigned int)_oid->SNMPoid[9])
					return i+16;
				else return 0;
			}
		}
		//digital sensors (.1.3.6.1.4.1.36582.2.y(1-16).0)
		//{0x2B,0x6,0x1,0x4,0x1,0x82,0x9D,0x66,0x2,0x1,0x0}
		if(_oid->SNMPoid[8] == 0x2)
		{
			for(unsigned int i=1;i<=16;i++)
			{
				if (i == (unsigned int)_oid->SNMPoid[9])
					return i+64;
				else return 0;
			}
		}
	}
	return 0;
}

int packetSNMPsend()
{
	
}
