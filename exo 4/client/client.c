#include <sys/socket.h>
#include <netinet/in.h>
 #include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define GREETING "BONJOUR SERVEUR"

int main(int argc, char *argv[]){
   int socket_cli; // socket client
   int connect_state; // etat de la connexion
   int port;
   char *element;
   long fileSize;
   FILE* fichier = NULL;
   char *buffer;
   size_t result;
   struct sockaddr_in srv_addr; // socket adresse du serveur 

   if(argc!=4){
     printf("USAGE: ./client <serveur_ip_addr> <serveur_port>\n");
     exit(-1);
   }

   socket_cli = socket(AF_INET, SOCK_STREAM, 0); // création du socket
   if (socket_cli < 0){
      printf("Le système n'a pas pu créer la socket %d\n", socket_cli);
      exit(-1);
   }
   
   port = atoi(argv[2]);

   srv_addr.sin_family = AF_INET;
   srv_addr.sin_port = htons (port);
   inet_aton(argv[1], (struct in_addr *)&srv_addr.sin_addr.s_addr);

   connect_state = connect(socket_cli, (struct sockaddr *) &srv_addr,
                 sizeof(struct sockaddr_in));
   if(connect_state < 0){ // la connexion a echouée
      printf("Impossible de se connecter au serveur %s:%d error %d\n", 
               argv[1], port, connect_state);
      exit(-1);
   }
   
   send(socket_cli, argv[3], sizeof(argv[1]),0);
   sleep(2);
   fichier = fopen(argv[3],"rb");
   
  //taille du fichier
  fseek (fichier , 0 , SEEK_END);
  fileSize = ftell (fichier);
  rewind (fichier);
  
  //memoire du fichier
  buffer = (char*) malloc (sizeof(char)*fileSize);
  if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}
  
  //copy du fichir dans le buffer
  result = fread (buffer,1,fileSize,fichier);
  if (result != fileSize) {
    fputs ("Reading error",stderr);
    exit (3);
   }else{
    send(socket_cli, buffer, fileSize,0);
   }
  
   //fermeture du socket
   fclose(fichier);
   close(socket_cli);
   return(0);
}

