#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>	/* exit() warnings */
#include <string.h>	/* memset warnings */
#include <unistd.h>
#include <arpa/inet.h>

#define PORT  60000
#define BUF_SIZE  1024

typedef struct Client{
  int id;
  char name[15];
  char ipv4[16]; // change data type
  int status;
  //Friend friends[10];
} Client;

void newClient(Client *c, char name[], char ipv4[], int clientCount){
  c[clientCount].id = clientCount;
  strcpy(c[clientCount].name,name);
  strcpy(c[clientCount].ipv4,ipv4);
}

void messageWrite(char message[]){
  FILE *fptr;

  if((fptr = fopen("log.txt","a"))== NULL){
    fprintf(stderr,"FILE COULD NOT BE OPENED.\n");
  }
  else{
    fprintf(fptr,"%s",message);
  }

  fclose(fptr);
}

int main(){
  int sock, ret;
  struct sockaddr_in server_addr;

  int new_sock;
  struct sockaddr_in new_addr;

  socklen_t addr_size;

  char buf[BUF_SIZE], receive[20];
  strcpy(receive,"Message Received.");
  pid_t child_pid;

  sock = socket(AF_INET,SOCK_STREAM,0);
  if(sock < 0){
    printf("Error!!!\n");
    exit(1);
  }

  printf("Socket Created.\n");
  memset(&server_addr, '\0', sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  ret = bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(ret < 0){
    printf("ERROR.\n");
    exit(1);
  }

  printf("Bind to port %i\n", PORT);

  if(listen(sock,10)==0){
    printf("Listening....\n");
  }
  else{
    printf("Error for bind.");
  }

  while(1){
    new_sock = accept(sock, (struct sockaddr*)&new_addr, &addr_size);
    if (new_sock < 0){
      exit(1);
    }
    printf("Connection from %s: %d\n",inet_ntoa(new_addr.sin_addr), ntohs(new_addr.sin_port));

    if((child_pid = fork()) == 0){
      close(sock);

      printf("%d",child_pid);

      while(1){
        recv(new_sock,buf,1024,0);
        if(strcmp(buf,"exit")==0){
           printf("Disconnected from server.");/*%s %d \n", inet_ntoa(new_addr.sin_addr), ntohs(new_addr.sin_port));*/
           break;
        }
        else{
          printf("Client: %s\n",buf);
          printf("%d\n",child_pid);
          messageWrite(buf);
          //memset(receive, '\0', sizeof(receive));
          //strcpy(buf, "Message Received.");
          send(new_sock,receive,strlen(receive),0);
          //bzero(receive,sizeof(receive));
        }
      }
      //send(new_sock,receive,strlen(receive),0);
    }
  }

  close(new_sock);

  return 0;
}
