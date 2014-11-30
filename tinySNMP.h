#ifndef TINYSNMP_H
#define TINYSNMP_H

#include "UIPEthernet.h"
#include "UIPUdp.h"

#define SNMP_MAX_PACKET_LEN 	127
#define SNMP_VERSION			0	//1

#define SNMP_ERR_NO_ERROR 		0
#define SNMP_ERR_TOO_BIG 		1
#define SNMP_ERR_NO_SUCH_NAME	2
#define SNMP_ERR_BAD_VALUE		3
#define SNMP_ERR_READ_ONLY		4
#define SNMP_ERR_GEN_ERROR		5

#define SNMP_PDU_GET			0xA0
#define SNMP_PDU_GET_NEXT		0xA1
#define SNMP_PDU_RESPONSE		0xA2
#define SNMP_PDU_SET			0xA3
#define SNMP_PDU_TRAP			0xA4

#define SNMP_COMM_MAX_SIZE		7

#define SNMP_MAX_OID_SIZE		16
#define SNMP_MAX_OID_VAL_SIZE	20

#define SNMP_MAX_MIB_SIZE		SNMP_MAX_OID_SIZE
#define SNMP_MAX_MIB_VAL_SIZE	SNMP_MAX_OID_VAL_SIZE+1

struct OID
{
	//unsigned int SNMPsize;
	//unsigned int SNMPver;
	//char SNMPcomm[SNMP_COMM_MAX_SIZE];
	//unsigned int commLen;
	byte SNMPpduType;
	int SNMPreqID;
	byte SNMPerr;
	byte SNMPerrID;
	unsigned int SNMPoidLen;
	byte SNMPoid[SNMP_MAX_OID_SIZE];
	unsigned int SNMPvalLen;
	byte SNMPvalType;
	byte SNMPval[SNMP_MAX_OID_VAL_SIZE];
};

/*
struct MIB
{
	char numMIB[SNMP_MAX_MIB_SIZE];
	char valMIB[SNMP_MAX_MIB_VAL_SIZE];
};
*/

//print packet
void packetSNMPprint(byte packet[],int packet_size);
//check udp packet
//buffer,size,community,mib
int packetSNMPcheck(byte packet[],int packet_size);
//return community name
int packetSNMPcommunity(byte packet[],int packet_size,char community[],int community_size);
//return oid & value
int packetSNMPoid(byte packet[],int packet_size, struct OID *oid);
//oid, val
//int packetSNMPoid(byte packet[],int packet_size,byte oid[],int oid_size);

#endif