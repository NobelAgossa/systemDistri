#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MY_ADDR "127.0.0.1"
#define MY_PORT 6875
#define LISTEN_BACKLOG 50

int main(int argc, char *argv[]){
   int socket_serv; // socket du serveur
   int socket_cli; // socket du client
   struct sockaddr_in my_addr; // socket addr du serveur 
   struct sockaddr_in peer_addr; // socket addr d'un client
   FILE* fichier = NULL; //Le nom du fichier à recevoir

   socklen_t peer_addr_size; // la taille du socket
   pid_t fils;

   char buffer[10]; // pour recevoir les fichiers du client
   char buf[60000];

   socket_serv = socket(AF_INET, SOCK_STREAM, 0); // création du socket
   if (socket_serv < 0){
      printf("Le système n'a pas pu créer la socket %d\n", socket_serv);
      exit(-1);
   }

   memset(&my_addr, 0, sizeof(struct sockaddr_in));
   
   my_addr.sin_family = AF_INET;
   my_addr.sin_port = htons (MY_PORT);
   inet_aton(MY_ADDR, (struct in_addr *)&my_addr.sin_addr.s_addr);
  
   if (bind(socket_serv, (struct sockaddr *) &my_addr,
                   sizeof(struct sockaddr_in)) < 0){
       printf("bind error\n"); //échec de la connexion
       exit(-1);
  }

   if (listen(socket_serv, LISTEN_BACKLOG) < -1)
               perror("listen\n");

   //attente des connexion clientes
   peer_addr_size = sizeof(struct sockaddr_in);
   while(1){
         socket_cli = accept(socket_serv, (struct sockaddr *) &peer_addr,
                   &peer_addr_size);  
        if (socket_cli < 0){
            perror("accept\n");
            exit(-1);
        }
        
        fils = fork();
        if(fils < 0){ // le fils n'a pas pu être créé
           perror("errreur de création du fils\n");
        }
        if(fils==0){
             printf("indentité du client %d\n", peer_addr.sin_port);
             
             read(socket_cli, buffer, 10);
             fichier = fopen(buffer,"ab");
             sleep(3);
             while(read(socket_cli, buf, 10)){
                fwrite(buf,sizeof(char),sizeof(buf),fichier );
              }
             fclose(fichier);
             close(socket_serv);
             break;
        }
        else{
          close(socket_cli);
        }
  }
}
