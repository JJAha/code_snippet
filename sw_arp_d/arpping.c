/*
 * arpping.c
 * arp demo
 */

#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/if_ether.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cutils/log.h>
#include <cutils/properties.h>
#include <errno.h>
#include "arpping.h"

int get_hwaddr(char *device, char *macaddr);
int get_net_status(char *interface, char *ipaddr);

void send_arp_pkg( int s, unsigned char * tar_mac, u_int32_t ip, char * interface, char* sou_mac)
{
	struct sockaddr addr;		/* for interface name */
	struct arpMsg   arp;

	/* send arp request */
	memset(&arp, 0, sizeof(arp));
	memcpy(arp.ethhdr.h_dest, tar_mac, 6);	/* MAC DA */
	//memset(arp.ethhdr.h_dest, 0xff, 6);     /* MAC DA */
	memcpy(arp.ethhdr.h_source, sou_mac, 6);		/* MAC SA */
	arp.ethhdr.h_proto = htons(ETH_P_ARP);		/* protocol type (Ethernet) */
	arp.htype = htons(ARPHRD_ETHER);		/* hardware type */
	arp.ptype = htons(ETH_P_IP);			/* protocol type (ARP message) */
	arp.hlen = 6;					/* hardware address length */
	arp.plen = 4;					/* protocol address length */
	arp.operation = htons(ARPOP_REPLY);		/* ARP op code */
	memcpy(arp.sInaddr,&ip,4);			/* source IP address */
	memcpy(arp.sHaddr, sou_mac, 6);			/* source hardware address */
	memcpy(arp.tHaddr, tar_mac, 6);			/* source hardware address */
	memcpy(arp.tInaddr, &ip,4);		/* target IP address */
	
	memset(&addr, 0, sizeof(addr));
	strcpy(addr.sa_data, interface);
	if (sendto(s, &arp, sizeof(arp), 0, &addr, sizeof(addr)) < 0)
		LOGE("Reply arp package failed\n");
	else
		LOGE("Reply arp package success\n");
}

int main()
{
	int 	optval = 1;
	int	s;			/* socket */
	struct arpMsg	arp;
	fd_set		fdset;
	struct timeval	tm;
	int i = 0;
	char ipaddr[50];
	char interface[10]; 
	struct in_addr dst;
	char sou_mac[7];   /*source hardware address*/

	if ((s = socket (PF_PACKET, SOCK_PACKET, htons(ETH_P_ARP))) == -1) {
		printf("Could not open raw socket\n");
		return -1;
	}
	
	if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) == -1) {
		printf( "Could not setsocketopt on raw socket\n");
		close(s);
		return -1;
	}

	while (1) {
		//wait network ok.
		while(get_net_status(interface, ipaddr)){
			sleep(2);
		}

		inet_aton(ipaddr, &dst);
		get_hwaddr(interface, sou_mac);

		/* wait arp reply, and check it */
		FD_ZERO(&fdset);
		FD_SET(s, &fdset);
		if (select(s + 1, &fdset, (fd_set *) NULL, (fd_set *) NULL, NULL) < 0) {
			LOGE("Error on ARPING request: %s\n", strerror(errno));
		} else if (FD_ISSET(s, &fdset)) {
			if (recv(s, &arp, sizeof(arp), 0) < 0 )
				LOGE("recv error:%s", strerror(errno));

			if(memcmp(arp.sHaddr, sou_mac, 6) == 0)
			{
				LOGE("sw_arprecv: IS MYself arp package.");
				break;
			}

			if (arp.operation == htons(ARPOP_REQUEST) && 
					(memcmp(arp.ethhdr.h_dest, "\xff\xff\xff\xff\xff\xff", 6) == 0 || 
					 memcmp(arp.ethhdr.h_dest, "\x00\x00\x00\x00\x00\x00", 6) == 0) && 
					memcmp(arp.sInaddr, &dst.s_addr, sizeof(dst.s_addr)) == 0) {
					LOGD( "Valid arp REQUAST receved from this address,MACaddris:");
					LOGE("%02x:%02x:%02x:%02x:%02x:%02x",arp.sHaddr[0],arp.sHaddr[1],arp.sHaddr[2],arp.sHaddr[3],arp.sHaddr[4],arp.sHaddr[5]);
					send_arp_pkg(s, arp.sHaddr, dst.s_addr, interface, sou_mac);
			}
			LOGD("----------------------------------->");
		}
	}
	close(s);
	return 0;
}

