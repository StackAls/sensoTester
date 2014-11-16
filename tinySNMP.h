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

struct MIB
{
	byte numMIB[11];
	char setMIB[20];
};
//check udp packet to buffer
//buffer,community,mib
int packetSNMPcheck(byte [],char [],struct MIB[]);



#endif