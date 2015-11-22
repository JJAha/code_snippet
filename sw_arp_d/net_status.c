#include <netinet/if_ether.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <linux/sockios.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cutils/log.h>

#include <stdlib.h>

#define wireless "wlan0"
#define wired    "eth0"

int get_net_status(char *interface, char *ipaddr)
{
	int s=socket(AF_INET,SOCK_DGRAM,0); //建立套接口
	struct ifreq req;
	int err;
	unsigned int re_value = 1;
	
	strcpy(req.ifr_name,wireless); //将无线设备名作为输入参数传入
	if (!(ioctl(s, SIOCGIFFLAGS, &req))) //get net link status.
	{
		if(req.ifr_flags & IFF_UP) {  //the wireless netlink status is UP
			if (!(ioctl(s, SIOCGIFADDR, &req))) //get ipaddr.
			{
				re_value = 0;
				strcpy(interface, "wlan0");
				strcpy(ipaddr, inet_ntoa(((struct sockaddr_in*)(&req.ifr_addr))->sin_addr));
				LOGD("STB-->Interface: wlan0 ,Ipaddr is:%s\n", ipaddr);
			}
		}
	}

	strcpy(req.ifr_name,wired); //将有线设备名作为输入参数传入
	if (!(ioctl(s, SIOCGIFFLAGS, &req))) //get net link status.
	{
		if(req.ifr_flags & IFF_UP) {   //the wired netlink status is UP
			if (!(ioctl(s, SIOCGIFADDR, &req))) //get ipaddr.
			{
				re_value = 0;
				strcpy(interface, "eth0");
				strcpy(ipaddr, inet_ntoa(((struct sockaddr_in*)(&req.ifr_addr))->sin_addr));
				LOGD("STB-->Interface: eth0 ,Ipaddr is:%s\n", ipaddr);
			}
		}
	}
	
	close(s);
	return re_value;
}
