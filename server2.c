#include <stdio.h>
#include <sys/types.h>	/* system type defintions */
#include <sys/socket.h>	/* network system functions */
#include <netinet/in.h>	/* protocol & struct definitions */
#include <stdlib.h>	/* exit() warnings */
#include <string.h>	/* memset warnings */
#include <arpa/inet.h>
#include <unistd.h>


#define BUF_SIZE	1024
#define LISTEN_PORT	60000

typedef struct Friend{

} Friend;

typedef struct Client{
  int id;
  char name[15];
  char ipv4[16]; // change data type
  int status;
  Friend friends[10];
} Client;

void newClient(Client *c, char name[], char ipv4[], int clientCount){
  c[clientCount].id = clientCount;
  strcpy(c[clientCount].name,name);
  strcpy(c[clientCount].ipv4,ipv4);
}

// alter print client
void printClient(Client c){
  printf("Name: %s\n",c.name);
  printf("IP Address: %s\n",c.ipv4);
  printf("ID Number: %d\n\n",c.id);
}

// Maybe get this to display potential friends to talk to
void displayClients(Client c[], int clientCount){
  int x;

  for(x=0 ; x< clientCount;x++){
    printClient(c[x]);
  }
}

void fileWrite(char message[]){
  FILE *fptr;

  if((fptr = fopen("log.txt","a"))== NULL){
    fprintf(stderr,"FILE COULD NOT BE OPENED.\n");
  }
  else{
    fprintf(fptr,"%s",message);
  }

  fclose(fptr);
}

void registerUser(){

}


int main(int argc, char *argv[]){
    //int listen_port = 60000;
    int			sock_recv, sock_send;
    struct sockaddr_in	my_addr;
    struct sockaddr_in	addr_send;
    int			i;
    fd_set	readfds,active_fd_set,read_fd_set;
    struct timeval		timeout={0,0};
    int			incoming_len, send_len;
    struct sockaddr_in	remote_addr;
    int			recv_msg_size;
    char			buf[BUF_SIZE];
    int			select_ret, bytes_sent;
    int len, clientCount = 0, idCount = 0;
    char from[15];
    Client clients[20];

        /* create socket for sending data */
    sock_send=socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

            /* create socket for receiving */
    sock_recv=socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock_recv < 0){
        printf("socket() failed\n");
        exit(0);
    }
        /* make local address structure */
    memset(&my_addr, 0, sizeof (my_addr));	/* zero out structure */
    my_addr.sin_family = AF_INET;	/* address family */
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);  /* current machine IP */
    my_addr.sin_port = htons((unsigned short)LISTEN_PORT);

        /* fill the address structure for sending data */
    memset(&addr_send, 0, sizeof(addr_send));  /* zero out structure */
    addr_send.sin_family = AF_INET;  /* address family */
    addr_send.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr_send.sin_port = htons((unsigned short)LISTEN_PORT);
        /* make local address structure */
    //memset(&my_addr, 0, sizeof (my_addr));	/* zero out structure */
    //my_addr.sin_family = AF_INET;	/* address family */
    //my_addr.sin_addr.s_addr = htonl(INADDR_ANY);  /* current machine IP */
    //my_addr.sin_port = htons((unsigned short)listen_port);

        /* bind socket to the local address */
    i=bind(sock_recv, (struct sockaddr *) &my_addr, sizeof (my_addr));
    if (i < 0){
        printf("bind() failed\n");
        exit(0);
    }
       FD_ZERO(&readfds);		/* zero out socket set */
       FD_SET(sock_recv,&readfds);	/* add socket to listen to */
        /* listen ... */
    while (1){
        read_fd_set = active_fd_set;
        select_ret=select(sock_recv+1,&readfds,NULL,NULL,NULL);
        /*select_ret=select(sock_recv+1,&readfds,NULL,NULL,&timeout);*/
        if (select_ret > 0){/* anything arrive on any socket? */
            incoming_len=sizeof(remote_addr);	/* who sent to us? */

            printf("REEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE");
            getchar();
            recv_msg_size=recvfrom(sock_recv,buf,BUF_SIZE,0,(struct sockaddr *)&remote_addr,&incoming_len);
            if (recv_msg_size > 0){	/* what was sent? */
                buf[recv_msg_size]='\0';
                char * token = strtok(buf, "|");
                len = strlen(token);
                strcpy(from,(token + len + 1));

                // Do a file check for user if they have been on server before.
                // Must register new users.
                // Prevent unregistered users from accessng chats.

                printf("Sock Recv: %i\n",sock_recv);
                printf("Remote Address: %s\n", inet_ntoa(remote_addr.sin_addr));
                printf("Incoming: %i\n",incoming_len);
                printf("recv_msg_size: %i\n\n",recv_msg_size);

                if(strcmp("#-connect-#",token)==0){
                  newClient(clients,from,inet_ntoa(remote_addr.sin_addr),clientCount);
                  // struct sockaddr_in
                  clientCount++;
                  if(clientCount == 3){
                    displayClients(clients,clientCount);
                  }

                }
                else{
                  printf("From %s received: %s\n",from,buf);

                  // Sending back confirmation message.
                  strcpy(buf, "Message Received.\n");
                  send_len = strlen(buf);

                  bytes_sent = sendto(sock_send, buf, send_len, 0,(struct sockaddr *) &addr_send, sizeof(addr_send));

                  printf("\nBytes Sent: %i\n",bytes_sent);
                  printf("hello\n");
                }
                // printf("%s\n", (token + len + 1)); //printing the token
              }
            }
        /*if (strcmp(buf,"shutdown") == 0)
            break;*/

        //listen_port++;

            /* make local address structure */
        //memset(&my_addr, 0, sizeof (my_addr));	/* zero out structure */
        //my_addr.sin_family = AF_INET;	/* address family */
        //my_addr.sin_addr.s_addr = htonl(INADDR_ANY);  /* current machine IP */
        //my_addr.sin_port = htons((unsigned short)listen_port);
    }

    close(sock_recv);
}
