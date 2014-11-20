#ifndef TINYSNMP_H
#define TINYSNMP_H

#include "UIPEthernet.h"
#include "UIPUdp.h"

#define SNMP_MAX_PACKET_LEN 127

#define SNMP_ERR_NO_ERROR 		0
#define SNMP_ERR_TOO_BIG 		1
#define SNMP_ERR_NO_SUCH_NAME	2
#define SNMP_ERR_BAD_VALUE		3
#define SNMP_ERR_READ_ONLY		4
#define SNMP_ERR_GEN_ERROR		5

#define SNMP_PDU_GET			0xA0
#define SNMP_PDU_GET_NEXT	0xA1
#define SNMP_PDU_RESPONSE	0xA2
#define SNMP_PDU_SET			0xA3
#define SNMP_PDU_TRAP		0xA4

struct OID
{
	char oid[20];
	char val[20];
};

struct MIB
{
	char numMIB[16];
	char setMIB[21];
};

//print packet
void packetSNMPprint(char[],int);
//check udp packet
//buffer,size,community,mib
int packetSNMPcheck(char packet[],int sizepacket);
//return community name
int packetSNMPcommunity(char[],int,char[],int);
//return oid & value
int packetSNMPoid(char[],int,struct OID);


#endif