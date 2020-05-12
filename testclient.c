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
    int i, choice, id;
    struct sockaddr_in	my_addr;
    struct sockaddr_in	addr_send;
    struct sockaddr_in	remote_addr;
    char			text[80],buf[BUF_SIZE] = "", name[15], choice_str[5], id_str[20];
    int			recv_msg_size;
    int			send_len,bytes_sent, incoming_len, conn;
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

     FD_ZERO(&readfds);		/* zero out socket set */
     FD_SET(sock_recv,&readfds);	/* add socket to listen to */

    conn=connect(sock_send,(struct sockaddr *)&addr_send,sizeof(addr_send));

    printf("Enter Name: \n");
    fgets(name,25,stdin);
    fflush(stdin);

    char string[30] = "#-connect-#|";
    strcat(string,name);
    strcpy(buf,string);

    send_len = strlen(buf);
    bytes_sent = sendto(sock_send, buf, send_len, 0,(struct sockaddr *) &addr_send, sizeof(addr_send));

    printf("Do you want to be in Work Group/Fun Group/Private Message?\n");
    printf("Enter 1 2 or 3 respectively.\n");
    scanf("%i",&choice);
    while(choice > 3 || choice < 1){
      printf("Enter a valid option.\n");
      scanf("%i",&choice);
    }
    system("clear");
    if(choice == 1){
      printf("\t// Work Group //\n");
    }
    else{
      if(choice == 2){
        printf("\t// Fun Group //\n");
      }
      else{
        printf("\t// Private Message //\n");
        printf("Enter ID Number of person to chat with.\n");
        scanf("%d",&id);
        sprintf(id_str,"%d",id);
      }
    }
    sprintf(choice_str,"%d",choice);
    //strcpy(choice_str, itoa(choice));

    /* Initialization of what client does. ("Structures" - Kyle) */

    /*send_len = strlen(choice_str);
    bytes_sent = sendto(sock_send, choice_str, send_len, 0,(struct sockaddr *) &addr_send, sizeof(addr_send));*/

    //printf("Bytes Sent: %i\n",bytes_sent);
if(fork()==0){
    while(1){


        printf("Send? ");
        fgets(text,80,stdin);
        fflush(stdin);


        //  THIS IS TO DIFFERENTIATE CHAT MESSAGES LOOKY HEREEEEEEEEEEEEEEEEEEEEEEEEE

      /**  if (strcmp(text,"quit") == 0)
            break;

        if(choice==1){
          strcpy(text,"|wg|");
        }
        else{
          if(choice==2){
            strcpy(text,"|fg|");
          }
          else{
            strcpy(text,"|pr|");
            strcpy(text,id_str);
          }
        }

        strcpy(buf,text);

        printf("%s",buf);*/

        send_len=strlen(strcat((strcat(buf,"|")),name));
        bytes_sent=sendto(sock_send, strcat((strcat(buf,"|")),name), send_len, 0,(struct sockaddr *) &addr_send, sizeof(addr_send));

        bzero(buf, sizeof(buf));
      }
        // Receive from server.
        //read_fd_set = active_fd_set;
        //select_ret=select(sock_recv+1,&readfds,NULL,NULL,NULL);
        //select_ret=select(sock_recv+1,&readfds,NULL,NULL,&timeout);
        /*if (select_ret > 0){/* anything arrive on any socket? */
            //incoming_len=sizeof(remote_addr);	/* who sent to us? */
            /*recv_msg_size=recvfrom(sock_recv,buf,BUF_SIZE,0,(struct sockaddr *)&remote_addr,&incoming_len);
            if (recv_msg_size > 0){
              buf[recv_msg_size]='\0';
              printf("%s",buf);
            }
        }*/
        }
        /* receiving server messages */
        else{
          while(1){
          bzero(buf, sizeof(buf));

          incoming_len = sizeof(addr_send);
          recv_msg_size = recvfrom(sock_send,buf,BUF_SIZE,0,(struct sockaddr *) &addr_send, &incoming_len);
          //printf("%i\n",recv_msg_size);
          if(recv_msg_size < 0){
            printf("Errorrrrrrrrr.\n");
            exit(1);
          }else{

            printf("%s\n",buf);
          }
        }
      }


    close(sock_send);
}
