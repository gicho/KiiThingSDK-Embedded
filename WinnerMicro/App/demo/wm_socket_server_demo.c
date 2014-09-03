/***************************************************************************** 
* 
* File Name : wm_socket_demo.c 
* 
* Description: socket demo function 
* 
* Copyright (c) 2014 Winner Micro Electronic Design Co., Ltd. 
* All rights reserved. 
* 
* Author : wanghaifang
* 
* Date : 2014-6-2 
*****************************************************************************/ 
#include "wm_include.h"
#include <string.h>


#if DEMO_STD_SOCKET_SERVER
#define    DEMO_SOCK_S_TASK_SIZE      256
OS_EVENT *demo_sock_s_q = NULL;
void *demo_sock_s_queue[DEMO_QUEUE_SIZE];
static OS_STK DemoSockSTaskStk[DEMO_SOCK_S_TASK_SIZE]; 

extern ST_Demo_Sys gDemoSys;
static OS_EVENT *sock_receive_q;
static void *sock_receive_queue[DEMO_QUEUE_SIZE];
static OS_STK SKRCVTaskStk[DEMO_SOCK_S_TASK_SIZE]; 
//extern OS_EVENT *demo_q;
extern u8 RemoteIp[4];
static void demo_sock_s_task(void *sdata);

int create_server_socket_demo(struct tls_ethif * ethif)
{
	struct sockaddr_in pin;
	
	memset(&pin, 0, sizeof(struct sockaddr));
	pin.sin_family=AF_INET;                 //AF_INET��ʾʹ��IPv4
	gDemoSys.socket_num = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	memcpy((char *)&pin.sin_addr.s_addr, (char *)&ethif->ip_addr, 4);
	//pin.sin_addr.s_addr = htonl(0x00000000UL);//IPADDR_ANY
	pin.sin_port=htons(LocalPort);

	if(bind(gDemoSys.socket_num, (struct sockaddr *)&pin, sizeof(struct sockaddr)) != 0)
	{
		printf("bind error !\n");
		closesocket(gDemoSys.socket_num);
		return -1;
	}

	if(listen(gDemoSys.socket_num, 0) != 0)
	{
		printf("listen error !\n");
		closesocket(gDemoSys.socket_num);
		return -1;
	}
	printf("listen port=%d\n", LocalPort);

	return 0;
}


int socket_server_demo(void)
{
	struct tls_ethif * ethif;

	ethif = tls_netif_get_ethif();
	printf("\nip=%d.%d.%d.%d\n",ip4_addr1(&ethif->ip_addr.addr),ip4_addr2(&ethif->ip_addr.addr),
		ip4_addr3(&ethif->ip_addr.addr),ip4_addr4(&ethif->ip_addr.addr));

	DemoStdSockOneshotSendMac();			

	return create_server_socket_demo(ethif);
}
	
void sock_server_recv_task(void *sdata)
{
	ST_Demo_Sys *sys = (ST_Demo_Sys *)sdata;
	int ret = 0;
	INT8U error;
	bool accepted = false;
	int accept_socket_num = 0;
	struct sockaddr_in pin;
	socklen_t socklen;
	static int serversocketnum = 0xFF;

	OSTimeDly(100);
	OSQPend(sock_receive_q, 0, &error);
	if (serversocketnum == 0xFF){
		serversocketnum = sys->socket_num;
	}
	for(;;) 
	{

		if(!accepted)
		{
			pin.sin_family=AF_INET;                 //AF_INET��ʾʹ��IPv4
			printf("start to accept socket num=%d\n", serversocketnum);
			socklen = sizeof(struct sockaddr);
			accept_socket_num = accept(serversocketnum, (struct sockaddr *)&pin, &socklen);
			if(accept_socket_num<0)
			{
				printf("accept error !\n");
				continue;
			}
			accepted = true;
			sys->socket_ok = true;
			gDemoSys.is_raw = 0;
			sys->socket_num = accept_socket_num;
		//	OSQPost(demo_q,(void *)DEMO_MSG_OPEN_UART);
			continue;
		}

		if(sys->socket_ok)
		{
			ret = 0;
			printf("start to recv data from socket num=%d\n", sys->socket_num);
			ret = recv(sys->socket_num, sys->sock_rx, DEMO_BUF_SIZE, 0);
			sys->sock_data_len = ret;
			sys->recvlen += sys->sock_data_len;
			printf("\n%d\n",sys->recvlen);
			if(ret>0)
			{
				OSQPost(demo_sock_s_q,(void *)DEMO_MSG_SOCKET_RECEIVE_DATA);
			}
			else
			{
				accepted = false;
				sys->socket_ok = false;
				closesocket(sys->socket_num);
				printf("closesocket: %d\n", sys->socket_num);
				sys->socket_num = 0;
			}
			continue;
		}
		OSTimeDly(100);
	}
}

int CreateSockServerDemoTask(char *buf)
{
	demo_sock_s_q = OSQCreate((void **)&demo_sock_s_queue, DEMO_QUEUE_SIZE);
	//�û�����socket��ص���Ϣ
	tls_os_task_create(NULL, NULL,
			demo_sock_s_task,
                    (void *)&gDemoSys,
                    (void *)DemoSockSTaskStk,          /* ����ջ����ʼ��ַ */
                    DEMO_SOCK_S_TASK_SIZE * sizeof(u32), /* ����ջ�Ĵ�С     */
                    DEMO_SOCKET_S_TASK_PRIO,
                    0);

	sock_receive_q = OSQCreate((void **)&sock_receive_queue, DEMO_QUEUE_SIZE);
	//����socket���ݵĽ���
	tls_os_task_create(NULL, NULL,
			sock_server_recv_task,
                    (void *)&gDemoSys,
                    (void *)SKRCVTaskStk,          /* ����ջ����ʼ��ַ */
                    DEMO_SOCK_S_TASK_SIZE * sizeof(u32), /* ����ջ�Ĵ�С     */
                    DEMO_SOCKET_RECEIVE_TASK_PRIO,
                    0);

	return WM_SUCCESS;
}


static void sock_s_net_status_changed_event(u8 status )
{
	switch(status)
	{
		case NETIF_WIFI_JOIN_FAILED:
			OSQPost(demo_sock_s_q,(void *)DEMO_MSG_WJOIN_FAILD);
			break;
		case NETIF_WIFI_JOIN_SUCCESS:
			OSQPost(demo_sock_s_q,(void *)DEMO_MSG_WJOIN_SUCCESS);
			break;
		case NETIF_IP_NET_UP:
			OSQPost(demo_sock_s_q,(void *)DEMO_MSG_SOCKET_CREATE);
			break;
		default:
			break;
	}
}

static void demo_sock_s_task(void *sdata)
{
	ST_Demo_Sys *sys = (ST_Demo_Sys *)sdata;
	void *msg;
	INT8U error;
	struct tls_ethif * ethif = tls_netif_get_ethif();

	printf("\nsock s task\n");

//����socket��������ʹ��
	sys->sock_rx = malloc(DEMO_BUF_SIZE);
	if(NULL == sys->sock_rx)
	{
		printf("\nmalloc socket rx fail\n");
		return;
	}
	memset(sys->sock_rx, 0, DEMO_BUF_SIZE);	
//////	
	if(ethif->status)	//�Ѿ�����
	{
		OSQPost(demo_sock_s_q,(void *)DEMO_MSG_SOCKET_CREATE);
	}
	else
	{
		tls_wifi_set_oneshot_flag(1);		/*һ������ʹ��*/
		printf("\nwait one shot......\n");
	}
	tls_netif_add_status_event(sock_s_net_status_changed_event);

	for(;;) 
	{
    		msg = OSQPend(demo_sock_s_q, 0, &error);
		//printf("\n msg =%d\n",msg);
		switch((u32)msg)
		{
			case DEMO_MSG_WJOIN_SUCCESS:
				printf("\njoin net success\n");
				break;
				
			case DEMO_MSG_SOCKET_CREATE:
				if(0 == socket_server_demo())
					OSQPost(sock_receive_q, (void *)1);
				break;
				
			case DEMO_MSG_WJOIN_FAILD:
				if(sys->socket_num > 0)
				{
					sys->socket_num = 0;
					sys->socket_ok = FALSE;
				}
				printf("join net faild\n");
				break;

			case DEMO_MSG_SOCKET_RECEIVE_DATA:
				/*�յ����ݣ����д���*/
#if	(TLS_CONFIG_UART)
				tls_uart_tx(sys->sock_rx,sys->sock_data_len);	/*������������ʾ*/
#endif				
				break;

			case DEMO_MSG_SOCKET_ERR:
				printf("\nsocket err\n");
				break;

			default:
				break;
		}
	}

}

#endif