#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>	/* exit() warnings */
#include <string.h>	/* memset warnings */
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024
#define SERVER_IP	"127.0.0.1"

int readPortNum(){
  FILE *fp;
  int port_num;

  if((fp = fopen("port.txt","r"))== NULL){
    fprintf(stderr,"FILE COULD NOT BE OPENED.\n");
  }
  else{
    fscanf(fp,"%d",&port_num);
  }

  fclose(fp);

  return port_num;
}

int main(){
  int port_num;
  int			sock_send, sock_recv;
  int i;
  struct sockaddr_in	my_addr;
  struct sockaddr_in	addr_send;
  struct sockaddr_in	remote_addr;
  char			text[80],buf[BUF_SIZE] = "", name[15];
  int			recv_msg_size;
  int			send_len,bytes_sent, incoming_len, conn;
  int			select_ret;
  fd_set	readfds,active_fd_set,read_fd_set;

    /* create socket for sending data */
  sock_send=socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if (sock_send < 0){
      printf("socket() failed\n");
      exit(0);
  }

    /* read port number from file */

  port_num = readPortNum();

  printf("%i\n\n",port_num);
    /* fill the address structure for sending data */
  memset(&addr_send, 0, sizeof(addr_send));  /* zero out structure */
  addr_send.sin_family = AF_INET;  /* address family */
  addr_send.sin_addr.s_addr = inet_addr(SERVER_IP);
  addr_send.sin_port = htons((unsigned short)port_num);

  conn=connect(sock_send,(struct sockaddr *)&addr_send,sizeof(addr_send));

  printf("Enter Name: \n");
  fgets(name,25,stdin);
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
  }

  close(sock_send);

  return 0;
}
