#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>	/* exit() warnings */
#include <string.h>	/* memset warnings */
#include <unistd.h>
#include <arpa/inet.h>


#define BUF_SIZE	1024
#define SERVER_IP	"127.0.0.1"
#define	SERVER_PORT	60000

int main(int argc, char *argv[]){
    //int server_port = 60000;
    int			sock_send, sock_recv;
    int i;
    struct sockaddr_in	my_addr;
    struct sockaddr_in	addr_send;
    struct sockaddr_in	remote_addr;
    char			text[80],buf[BUF_SIZE] = "", name[15];
    int			recv_msg_size;
    int			send_len,bytes_sent, incoming_len;
    int			select_ret;
    fd_set	readfds,active_fd_set,read_fd_set;

        /* create socket for sending data */
    sock_send=socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    /* create socket for receiving */
    sock_recv=socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock_send < 0){
        printf("socket() failed\n");
        exit(0);
    }

            /* fill the address structure for sending data */
    memset(&addr_send, 0, sizeof(addr_send));  /* zero out structure */
    addr_send.sin_family = AF_INET;  /* address family */
    addr_send.sin_addr.s_addr = inet_addr(SERVER_IP);
    addr_send.sin_port = htons((unsigned short)SERVER_PORT);
    //memset(&addr_send, 0, sizeof(addr_send));  /* zero out structure */
    //addr_send.sin_family = AF_INET;  /* address family */
    //addr_send.sin_addr.s_addr = inet_addr(SERVER_IP);
    //addr_send.sin_port = htons((unsigned short)server_port);

        /* make local address structure */
    memset(&my_addr, 0, sizeof (my_addr));	/* zero out structure */
    my_addr.sin_family = AF_INET;	/* address family */
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);  /* current machine IP */
    my_addr.sin_port = htons((unsigned short)SERVER_PORT);

        /* bind socket to the local address */
    /*i=bind(sock_recv, (struct sockaddr *) &my_addr, sizeof (my_addr));
    if (i < 0){
        printf("bind() failed\n");
        exit(0);
    }*/
     FD_ZERO(&readfds);		/* zero out socket set */
     FD_SET(sock_recv,&readfds);	/* add socket to listen to */

    printf("Enter Name: \n");
    fgets(name,15,stdin);
    fflush(stdin);

    char string[30] = "#-connect-#|";
    strcat(string,name);
    strcpy(buf,string);


    send_len = strlen(buf);
    sendto(sock_send, buf, send_len, 0,(struct sockaddr *) &addr_send, sizeof(addr_send));

    while(1){
        printf("Send? ");
        fgets(text,80,stdin);
        fflush(stdin);
        if (strcmp(text,"quit") == 0)
            break;

        strcpy(buf,text);
        send_len=strlen(strcat((strcat(buf,"|")),name));
        bytes_sent=sendto(sock_send, strcat((strcat(buf,"|")),name), send_len, 0,(struct sockaddr *) &addr_send, sizeof(addr_send));

        // Receive from server.
        read_fd_set = active_fd_set;
        select_ret=select(sock_recv+1,&readfds,NULL,NULL,NULL);
        /*select_ret=select(sock_recv+1,&readfds,NULL,NULL,&timeout);*/
        if (select_ret > 0){/* anything arrive on any socket? */
            incoming_len=sizeof(remote_addr);	/* who sent to us? */
            recv_msg_size=recvfrom(sock_recv,buf,BUF_SIZE,0,(struct sockaddr *)&remote_addr,&incoming_len);
            if (recv_msg_size > 0){
              buf[recv_msg_size]='\0';
              printf("%s",buf);
            }
        }
    }

    close(sock_send);
}
