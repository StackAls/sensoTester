#ifndef TINYSNMP_H
#define TINYSNMP_H

#include "UIPEthernet.h"
#include "UIPUdp.h"

#define SNMP_MAX_PACKET_LEN UIP_UDP_MAXPACKETSIZE

#define SNMP_ERR_NO_ERROR 		0
#define SNMP_ERR_TOO_BIG 		1
#define SNMP_ERR_NO_SUCH_NAME	2
#define SNMP_ERR_BAD_VALUE 		3
#define SNMP_ERR_READ_ONLY		4
#define SNMP_ERR_GEN_ERROR		5

//read udp packet to buffer
int _packetSNMPread(EthernetUDP *udp, byte *buffer);


#endif