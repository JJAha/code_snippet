#include <netinet/if_ether.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/if_ether.h>
#include <net/if.h>
#include <linux/sockios.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int get_hwaddr(char *device, char *macaddr)
{
	//char *device="eth0";    //eh0是网卡设备名
	//unsigned char macaddr[ETH_ALEN]; //ETH_ALEN（6）是MAC地址长度
	int s=socket(AF_INET,SOCK_DGRAM,0); //建立套接口
	struct ifreq req;
	int err;
	strcpy(req.ifr_name,device); //将设备名作为输入参数传入
	err=ioctl(s,SIOCGIFHWADDR,&req); //执行取MAC地址操作
	close(s);
	
	if(err!= -1)
	{ 
		memcpy(macaddr,req.ifr_hwaddr.sa_data,ETH_ALEN); //取输出的MAC地址
		int i;
	//	for(i=0;i<ETH_ALEN;i++)
	//		printf("%02x:",macaddr[i]);
	//	printf("\n");
	}

	return 0;
}
