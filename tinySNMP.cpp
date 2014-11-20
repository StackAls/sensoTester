#include "tinySNMP.h"

void packetSNMPprint(char _packet[],int _packetSize)
{
	//print packet
	Serial.print("Packet len=");
	Serial.print(_packetSize,DEC);
	Serial.print(" | Sequense len=");
	Serial.println(_packet[1],DEC);
	Serial.print("Packet ID= ");
	Serial.println(_packet[0],HEX);
	
	Serial.println("Packet = byte-int-char");
	for(int n = 0;n < _packetSize;n++)
	{
		Serial.print("p[");
		Serial.print(n); 
		Serial.print("]="); 
		Serial.print(byte(_packet[n]),HEX);
		Serial.print(" - ");
		Serial.print(int(_packet[n]),DEC);
		Serial.print(" - ");
		Serial.println(char(_packet[n]));
	}	
}

int packetSNMPcheck(char _packet[],int _packetSize) //check SNMP
{

	
	// packet check to SNMP and size
	if( (byte(_packet[0]) != 0x30 ) || ((_packetSize-2)!= int(_packet[1]) ) )
	{
		//Serial.println("SNMP size invalid");
		return SNMP_ERR_GEN_ERROR;
	}
	//sequence length
	int _seqLen = _packet[1];
/*	
	Serial.print("_seqLen= ");
	Serial.println(_seqLen);
*/	
	//check version
	int _versionSNMP = _packet[4]; //version SNMP
/*	
	Serial.print("_versionSNMP= ");
	Serial.println(_versionSNMP);	
*/	
	if(int(_versionSNMP)>1) 
	{
		//Serial.println("SNMP version invalid");
		return SNMP_ERR_GEN_ERROR;
	}
	return SNMP_ERR_NO_ERROR;
}

int packetSNMPcommunity(char _packet[],int _packetSize,char _community[],int _communitySize)
{	
	//community length
	int _comLen = _packet[6];
	//check community
/*	Serial.print("_comLen= ");
	Serial.print(_comLen);
	Serial.print(" _communitySize=");
	Serial.println(_communitySize);
*/	
	if(_communitySize != _comLen)
	{
		return SNMP_ERR_NO_SUCH_NAME;
	}
	else
	{	
		for (int i=0;i < _comLen;i++)
		{
			if(_community[i] != _packet[7+i]) return SNMP_ERR_NO_SUCH_NAME;
/*			Serial.print("com= ");
			Serial.print(char(_community[i]));
			Serial.print(" pkg= ");
			Serial.println(char(_packet[7+i]));
*/		}
	}
	return 0;
}

int packetSNMPoid(char _packet[],int _packetSize, struct OID _oid)
{
	//length
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
	//read oid
	if(_oidLen > 0)
	{

		for (int i=0;i < _oidLen;i++)
		{
			//TODO parse multibyte
			_oid.oid[i] = byte(_packet[6 + _comLen + 4 + _ridLen + 2 + _errLen + 2 + _eriLen + 7 + i]);
			//Serial.println(byte(_oid.oid[i]),HEX);
		}
		//strcpy(_oidSNMP,_oid.oid);
	}
	return 0;
}

int packetSNMPvalue(char _packet[],int _packetSize,struct OID _oid)
{
	//length
	int _comLen = _packet[6];
	int _ridLen = _packet[6 + _comLen + 4];
	int _errLen = _packet[6 + _comLen + 4 + _ridLen + 2];
	int _eriLen = _packet[6 + _comLen + 4 + _ridLen + 2 + _errLen + 2];
	int _oidLen = _packet[6 + _comLen + 4 + _ridLen + 2 + _errLen + 2 + _eriLen + 6];
	//value oid
	byte _valueType = _packet[6 + _comLen + 4 + _ridLen + 2 + _errLen + 2 + _eriLen + 6 + _oidLen + 1];
	int _valueLen = _packet[6 + _comLen + 4 + _ridLen + 2 + _errLen + 2 + _eriLen + 6 + _oidLen + 2];
	//read value
	if(_valueType != 0x05 && _valueLen > 0) //NULL
	{
		for (int i=0;i < _valueLen;i++)
		{
			_oid.val[i] = byte(_packet[6 + _comLen + 4 + _ridLen + 2 + _errLen + 2 + _eriLen + 6 + _oidLen + 3 + i]);
		}
	}
}