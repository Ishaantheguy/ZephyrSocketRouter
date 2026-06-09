#include <zephyr/kernel.h>
#include <zephyr/net/socket.h>
#include <zephyr/sys/printk.h>
#include <zephyr/net/net_ip.h>
#include <stdio.h>

#define PORT_A 9000
#define PORT_B 9001
#define MSG_LEN 128

struct chat_msg {
	int sender;
	char text[MSG_LEN];
};

K_MSGQ_DEFINE(chat_q,sizeof(struct chat_msg),10,1);

static int client_a=-1;
static int client_b=-1;

static int create_server(int port)
{
	int sock;
	struct sockaddr_in addr;

	sock=zsock_socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	printk("socket=%d \n", sock);

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	zsock_bind(sock,(const struct net_sockaddr *)&addr,sizeof(addr));

	zsock_listen(sock,1);
	return sock;
}

void user_a_thread()
{
	int server;
	char buf[MSG_LEN];
	
	server=create_server(PORT_A);
	while(1)
	{
		printk("Waiting for User A on %d\n",PORT_A);
	
		client_a=zsock_accept(server,NULL,NULL);
	
		printk("User A online\n");
	
		while(1)
		{
			int len;
			len=zsock_recv(client_a,buf,MSG_LEN-1,0);
		
			if(len<=0)
				break;
			buf[len]='\0';
			struct chat_msg msg;
			msg.sender=0;
			strcpy(msg.text,buf);
			printk("Putting into queue: %s\n", msg.text);
			k_msgq_put(&chat_q,&msg,K_FOREVER);
		}
		zsock_close(client_a);
		client_a = -1;
	}
}

void user_b_thread()
{
	int server;
	char buf[MSG_LEN];
	server=create_server(PORT_B);
	while(1)
	{
		printk("Waiting for User B on %d\n",PORT_B);
		
		client_b=zsock_accept(server,NULL,NULL);
	
		printk("User B online\n");
	
		while(1)
		{
			int len;
			len=zsock_recv(client_b,buf,MSG_LEN-1,0);
		
			if(len<=0)
				break;
			buf[len]='\0';
			struct chat_msg msg;
			msg.sender=1;
			strcpy(msg.text,buf);
			printk("Putting into queue: %s\n", msg.text);
			k_msgq_put(&chat_q,&msg,K_FOREVER);
		}
		zsock_close(client_b);
		client_b = -1;
	}
}

void router_thread()
{
	struct chat_msg msg;
	char out[MSG_LEN+10];

	while(1)
	{
		k_msgq_get(&chat_q,&msg,K_FOREVER);
		snprintf(out,sizeof(out),"[%c] %s",msg.sender ? 'B' : 'A',msg.text);
	
		if(msg.sender==0)
		{
			printk("Message being sent from a to b\n");
			while(client_b<0)
				k_sleep(K_MSEC(100));
			if(client_b>=0)
			{
				int ret=zsock_send(client_b,out,strlen(out),0);
				printk("Ret value:%d\n",ret);
			}
		}
		else if(msg.sender==1)
		{
			printk("Message being sent from b to a\n");
			while(client_a<0)
				k_sleep(K_MSEC(100));
			if(client_a>=0)
			{
				int ret2=zsock_send(client_a,out,strlen(out),0);
				printk("Ret value:%d\n",ret2);
			}
		}
	}
}

K_THREAD_DEFINE(a_tid,
                2048,
                user_a_thread,
                NULL,NULL,NULL,
                5,0,0);

K_THREAD_DEFINE(b_tid,
                2048,
                user_b_thread,
                NULL,NULL,NULL,
                5,0,0);

K_THREAD_DEFINE(router_tid,
                2048,
                router_thread,
                NULL,NULL,NULL,
                5,0,0);
