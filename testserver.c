#include <stdio.h>
#include <sys/types.h>	/* system type defintions */
#include <sys/socket.h>	/* network system functions */
#include <netinet/in.h>	/* protocol & struct definitions */
#include <stdlib.h>	/* exit() warnings */
#include <string.h>	/* memset warnings */
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>


#define BUF_SIZE	1024
#define LISTEN_PORT	60000

typedef struct Client{
	int id;
	char name[20];
	char pass[30];
}Client;

typedef struct Connect{
	int id;
	char name[20];
	struct sockaddr_in socketC;
	bool online;
}Connect;


void userWrite(Client person){ //save users to file
  FILE *fp;

  if((fp = fopen("users.txt","a"))== NULL){
    printf("FILE COULD NOT BE OPENED.\n");
  }
  else{
    fwrite(&person, sizeof(struct Client), 1, fp);
  }

  fclose(fp);
}

void newClient(Client *c, char name[], char ipv4[], int clientCount){
  c[clientCount].id = clientCount;
  strcpy(c[clientCount].name,name);
  userWrite(c[clientCount]);
}

// alter print client
void printClient(Client c){
  printf("Name: %s\n",c.name);
  printf("ID Number: %d\n\n",c.id);
}

// Maybe get this to display potential friends to talk to
void displayClients(Client c[], int clientCount){
  int x;

  for(x=0 ; x< clientCount;x++){
    printClient(c[x]);
  }
}

void messageWrite(char message[]){
  FILE *fptr;

  if((fptr = fopen("log.txt","a"))== NULL){
    printf("FILE COULD NOT BE OPENED.\n");
  }
  else{
    fprintf(fptr,"%s",message);
  }

  fclose(fptr);
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
    int			select_ret, bytes_sent, conn_accept;
    int len, clientCount = 0, idCount = 0;
    char from[15];

    Client clients[20];
    Connect live[20];

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


        /* bind socket to the local address */
    i=bind(sock_recv, (struct sockaddr *) &my_addr, sizeof (my_addr));
    if (i < 0){
        printf("bind() failed\n");
        exit(0);
    }
       FD_ZERO(&readfds);		/* zero out socket set */
       FD_SET(sock_recv,&readfds);	/* add socket to listen to */
        /* listen ... */

    getchar();
    printf("Server running.\n");

    conn_accept=accept(sock_recv,(struct sockaddr *)&remote_addr,&incoming_len);
    while (1){
        read_fd_set = active_fd_set;
        select_ret=select(sock_recv+1,&readfds,NULL,NULL,NULL);
        /*select_ret=select(sock_recv+1,&readfds,NULL,NULL,&timeout);*/
        if (select_ret > 0){/* anything arrive on any socket? */
            incoming_len=sizeof(remote_addr);	/* who sent to us? */

            recv_msg_size=recvfrom(sock_recv,buf,BUF_SIZE,0,(struct sockaddr *)&remote_addr,&incoming_len);
            if (recv_msg_size > 0){	/* what was sent? */
                buf[recv_msg_size]='\0';

                char * token = strtok(buf, "|");
                len = strlen(token);
                strcpy(from,(token + len + 1));

                printf("%s",buf);

                getchar();

                if(strcmp("#-connect-#",token)==0){
                  //printf("%s",buf);
                  newClient(clients,from,inet_ntoa(remote_addr.sin_addr),clientCount);
                  clientCount++;
                  strcpy(live[clientCount-1].name, from);
                  live[clientCount-1].id = clientCount;
                  live[clientCount-1].socketC = remote_addr;

                  if(clientCount == 3){
                    displayClients(clients,clientCount);
                  }

                }
                else{
                  for (i =0; i< clientCount; i++) {
                    if (remote_addr.sin_port == live[i].socketC.sin_port) {
                      strcpy(from, live[i].name);
                      printf("Socket: %d\n",live[i].socketC.sin_port);
                      break;
                    }
                  }

                  if(strstr(buf, "<sendTo>") != NULL) {
                    //begin
                    //get name of receiver

                    const char *p1 = strstr(buf, "<sendTo>")+8;
                    const char *p2 = strstr(p1, "</sendTo>");
                    size_t len = p2-p1;
                    char *nem = (char*)malloc(sizeof(char)*(len+1));
                    strncpy(nem, p1, len);
                    nem[len] = '\0';

                    const char *a = strstr(buf, "</sendTo>")+9;
                    const char *b = strchr(buf, '\0');
                    len = b-a;
                    char *res = (char*)malloc(sizeof(char)*(len+1));
                    strncpy(res, a, len);
                    char temp[30] = "\nfrom ";
                    strcat(temp, from);
                    strcat(res, temp);

                    for (i =0; i< clientCount; i++) {

                      if (strcmp(nem, live[i].name) == 0) {



                        bytes_sent = sendto(sock_recv, res, strlen(buf), 0,(struct sockaddr *) &live[i].socketC, sizeof(live[i].socketC));
                        bytes_sent = sendto(sock_recv, "message sent", 13, 0,(struct sockaddr *) &remote_addr, sizeof(remote_addr));
                        messageWrite(res);
                        break;
                      }
                    }
                  }



                  else {
                    if(strstr(buf, "<sendAll>") != NULL){

                      const char *a = strstr(buf, "<sendAll>")+9;
                      const char *b = strchr(buf, '\0');
                      len = b-a;
                      char *res = (char*)malloc(sizeof(char)*(len+1));
                      strncpy(res, a, len);
                      char temp[30] = "\nfrom ";
                      strcat(temp, from);
                      strcat(res, temp);

                      for (i =0; i< clientCount; i++) {

                        bytes_sent = sendto(sock_recv, res, strlen(buf), 0,(struct sockaddr *) &live[i].socketC, sizeof(live[i].socketC));
                        messageWrite(res);

                      }
                    }
                    else{


                    printf("From %s received: %s\n",from,buf);

                    messageWrite(buf);

                    // Sending back confirmation message.
                    bzero(buf,sizeof(buf));
                    strcpy(buf, "Message Received.\n");
                    send_len = strlen(buf);

                    bytes_sent = sendto(sock_recv, buf, send_len, 0,(struct sockaddr *) &remote_addr, sizeof(remote_addr));
                  }
                }
                }

              }
            }

    }

    close(sock_recv);
}
