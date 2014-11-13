#include "tinySNMP.h"

int _packetSNMPread(EthernetUDP *udp, byte *_packet)
{
	//Serial.print("Initialize UDP: ");
	//Serial.println(success ? "success" : "failed");
	int sizePacket = udp.parsePacket();
	byte _packet[SNMP_MAX_PACKET_LEN];
	
	Serial.print("sec ");
	Serial.println(millis() / 1000);
	
	Serial.print("Packet size = "); 
	Serial.println(sizePacket);
	
	IPAddress remote = udp.remoteIP();
	//print remote ip
	Serial.print("From ");
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
	
		//TODO max packet size
	if(sizePacket) 
	{
		if(sizePacket > SNMP_MAX_PACKET_LEN)
		{
			return SNMP_ERR_TOO_BIG;
		}
		else
		{
			do
			  {
				int len = udp.read(_packet,sizePacket);
			  }
			while (udp.available());
			//finish reading this packet:
			udp.flush();
			
			for(int n = 0;n < sizePacket;n++)
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
		
		char *community;
        // sequence length
        byte seqLen;
        // version
        byte verLen, verEnd;
        // community string
        byte comLen, comEnd;
        // pdu
        byte pduTyp, pduLen;
        byte ridLen, ridEnd;
        byte errLen, errEnd;
        byte eriLen, eriEnd;
        byte vblTyp, vblLen;
        byte vbiTyp, vbiLen;
        byte obiLen, obiEnd;
        byte valTyp, valLen, valEnd;

        // validate packet
        if ( _packetSize != 0 && _packetSize > SNMP_MAX_PACKET_LEN ) {
                //
                //SNMP_FREE(_packet);
				Serial.println("SNMP_API_STAT_PACKET_TOO_BIG");
                return SNMP_API_STAT_PACKET_TOO_BIG;
        }
		
		// packet check to SNMP
        if ( _packet[0] != 0x30 ) {
                //
                //SNMP_FREE(_packet);
				Serial.println("SNMP_API_STAT_PACKET_INVALID");
                return SNMP_API_STAT_PACKET_INVALID;
        }
        // sequence length
        seqLen = _packet[1];
        // version
        verLen = _packet[3];
        verEnd = 3 + verLen;
        // community string
        comLen = _packet[verEnd + 2];
        comEnd = verEnd + 2 + comLen;
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
        //
	
}