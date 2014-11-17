#include "tinySNMP.h"

int packetSNMPcheck(byte _packet[],int _packetSize,char _community[],struct MIB _mibSNMP[])
{
	//String _pkt = _packet;
	//int _packetSize = strlen(_packet);
	//print packet
	Serial.print("_packetSize= ");
	Serial.println(_packetSize);
/*	
	for(int n = 0;n < _packetSize;n++)
	{
		Serial.print("p[");
		Serial.print(n); 
		Serial.print("]= "); 
		Serial.print(_packet[n],HEX);
		Serial.print(" ");
		Serial.println(_packet[n]);
	}
*/
	
	// packet check to SNMP and size
	if( _packet[0] != 0x30 || _packet[1] != _packetSize-2) 
	{
		//Serial.println("SNMP PACKET INVALID");
		return SNMP_ERR_GEN_ERROR;
	}
	
	//sequence length
	int _seqLen = _packet[1];
	//check version
	int _versionSNMP = _packet[4]; //version SNMP
	if(_versionSNMP!=0) return SNMP_ERR_GEN_ERROR;
	//community length
	int _comLen = _packet[6];
	//check community
	int _communityLen = strlen(_community - 1);
	
	Serial.print("_communityLen=");
	Serial.println(_communityLen,DEC);
	
	if(_communityLen != _comLen)
	{
		return SNMP_ERR_NO_SUCH_NAME;
	}
	else
	{	
		for (int i=0;i < _comLen;i++)
		{
			if(byte(_community[i]) != byte(_packet[7+i])) return SNMP_ERR_NO_SUCH_NAME;
			Serial.print("com= ");
			Serial.print(char(_community[i]));
			Serial.print(" pkg= ");
			Serial.println(char(_packet[7+i]));
		}
	}
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
	char _oidSNMP[_oidLen + 1];
	if(_oidLen > 0)
	{
		for (int i=0;i < _oidLen;i++)
		{
			//TODO parse multibyte
			_oidSNMP[i] = _packet[6 + _comLen + 4 + _ridLen + 2 + _errLen + 2 + _eriLen + 7 + i];
		}
	}
	//value oid
	byte _valueType = _packet[6 + _comLen + 4 + _ridLen + 2 + _errLen + 2 + _eriLen + 6 + _oidLen + 1];
	int _valueLen = _packet[6 + _comLen + 4 + _ridLen + 2 + _errLen + 2 + _eriLen + 6 + _oidLen + 2];
	char _oidValue [_valueLen];
	if(_valueType != 0x05 && _valueLen > 0) //NULL
	{
		for (int i=0;i < _valueLen;i++)
		{
			_oidValue[i] = _packet[6 + _comLen + 4 + _ridLen + 2 + _errLen + 2 + _eriLen + 6 + _oidLen + 3 + i];
		}
	}
	return SNMP_ERR_NO_ERROR;
}