#include <sys/socket.h>
#include <netinet/in.h>
 #include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

char static *args[3];
void affiche(int n)
{
    int cfd;
    int success;
    int port;
    int i;
  
   struct sockaddr_in srv_addr; 

   cfd = socket(AF_INET, SOCK_STREAM, 0);
   if (cfd < 0){
      printf("Le SE n'a pas pu créer la socket %d\n", cfd);
      exit(-1);
   }

   port = atoi(args[2]);

   srv_addr.sin_family = AF_INET;
   srv_addr.sin_port = htons (port);
   inet_aton(args[1], (struct in_addr *)&srv_addr.sin_addr.s_addr);

   success = connect(cfd, (struct sockaddr *) &srv_addr,
                 sizeof(struct sockaddr_in));
   if(success < 0){ 
      printf("Impossible de se connecter au serveur %s:%d error %d\n", 
               args[1], port, success);
      exit(-1);
   }

	send(cfd, "Execute", sizeof("Execute"), 0);
	printf("Envoie du signal\n");
	close(cfd);

}

int main(int argc, char *argv[]){
  int i;
   signal(SIGALRM, affiche);
     if(argc!=3)
     {
       printf("USAGE: ./client <serveur_ip_addr> <serveur_port>\n");
       exit(-1);
     }
      args[0] = argv[0];
      args[1] = argv[1];
      args[2] = argv[2];
  	while(1)
  	{
  	  alarm(10);
  	  pause();
  	}

   return(0);
}

