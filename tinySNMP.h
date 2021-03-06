#ifndef TINYSNMP_H
#define TINYSNMP_H

#include <Arduino.h>

#define SNMP_MAX_PACKET_LEN 	127
#define SNMP_VERSION			1	//2c

#define SNMP_ERR_NO_ERROR 		0
#define SNMP_ERR_TOO_BIG 		1
#define SNMP_ERR_NO_SUCH_NAME	2
#define SNMP_ERR_BAD_VALUE		3
#define SNMP_ERR_READ_ONLY		4
#define SNMP_ERR_GEN_ERROR		5

#define	SNMP_TYPE_BOOL			0x01
#define	SNMP_TYPE_INT			0x02
#define	SNMP_TYPE_BITS			0x03
#define	SNMP_TYPE_OCTETS		0x04
#define	SNMP_TYPE_NULL			0x05
#define	SNMP_TYPE_OID			0x06

#define SNMP_PDU_GET				0xA0
#define SNMP_PDU_GET_NEXT		0xA1
#define SNMP_PDU_RESPONSE		0xA2
#define SNMP_PDU_SET				0xA3
#define SNMP_PDU_TRAP			0xA4

#define SNMP_COMM_MAX_SIZE		7

#define SNMP_MAX_OID_SIZE		20
#define SNMP_MAX_OID_VAL_SIZE	20
#define SNMP_MAX_ID				4

#define SNMP_MAX_MIB_SIZE		SNMP_MAX_OID_SIZE
#define SNMP_MAX_MIB_VAL_SIZE	SNMP_MAX_OID_VAL_SIZE+1

struct OID
{
	//unsigned int SNMPsize;
	//unsigned int SNMPver;
	//community
	unsigned int SNMPcommLen;
	byte SNMPcomm[SNMP_COMM_MAX_SIZE];
	//pdu
	byte SNMPpduType;
	//request id
	byte SNMPridType;
	unsigned int SNMPridLen;
	byte SNMPrid[SNMP_MAX_ID];
	//error
	byte SNMPerrType;
	unsigned int SNMPerrLen;
	byte SNMPerr[SNMP_MAX_ID];
	//error index
	byte SNMPeriType;
	unsigned int SNMPeriLen;
	byte SNMPeri[SNMP_MAX_ID];
	//object id
	byte SNMPoidType;
	unsigned int SNMPoidLen;
	byte SNMPoid[SNMP_MAX_OID_SIZE];
	//value
	byte SNMPvalType;
	unsigned int SNMPvalLen;
	byte SNMPval[SNMP_MAX_OID_VAL_SIZE];
};

//print packet
void packetSNMPprint(byte packet[],int packet_size);
//check udp packet
//buffer,size
int packetSNMPcheck(byte packet[],int packet_size);
//return community name
int packetSNMPcommunity(byte packet[],int packet_size,char community[],int community_size);
//read packet to struct
int packetSNMPread(byte packet[],int packet_size, struct OID *oid);
//check oid
unsigned int checkOID( struct OID *oid);

#endif