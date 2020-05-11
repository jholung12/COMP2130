#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>	/* exit() warnings */
#include <string.h>	/* memset warnings */
#include <unistd.h>
#include <arpa/inet.h>


#define PORT 60000
#define BUF_SIZE 1024

int main(){
  int client_sock, conn, send_len;
  struct sockaddr_in server_addr;
  char buf[BUF_SIZE], name[25], text[300];

  client_sock = socket(AF_INET, SOCK_STREAM,0);
  if(client_sock < 0){
    printf("Error!!!\n");
    exit(1);
  }
  printf("Socket Created.\n");
  memset(&server_addr, '\0', sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  conn = connect(client_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(conn < 0){
    printf("ERROR.");
    exit(1);
  }

  printf("Enter your name\t\n");
  fgets(name,25,stdin);
  fflush(stdin);

  printf("Connected to Server.\n");

  while(1){
    printf("Client: \t");
    fgets(text,300,stdin);
    fflush(stdin);
    //strcat(buf,text);

    //send_len=strlen(strcat((strcat(buf,"|")),name));

    send(client_sock,text, strlen(text),0);

    if(strcmp(buf,"exit")==0){
      printf("Disconnected from server.\n");
      exit(1);
    }

    memset(buf, '\0', sizeof(buf));

    if(recv(client_sock,buf,BUF_SIZE,0) < 0){
      printf("Errorrrrrrrrr.\n");
      exit(1);
    }else{
      printf("Server: \t%s\n",buf);
    }
  }

  return 0;
}
