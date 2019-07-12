/**Ce programme crée trois processus fils
 *et affiche le pid de chaque processus 
 * 11/07/19
 * Auteur: Nobel Agossa nobel.agossa2014@gmail.com
***/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    pid_t pid1, pid2, pid3;
    int status;
    
    pid1 = fork();
    if(pid1 < 0){
          perror("Erreur de création du processus\n");
          exit(EXIT_FAILURE);
    }
    else if(pid1==0){
    }
    else{
    	printf("Processu fils 1 créé avec succès et son pid = %d \n",pid1);
        
        pid2 = fork();
        if(pid2 < 0){
          perror("Erreur de création du second processus\n");
          pid1 = wait(&status);
          exit(EXIT_FAILURE);
        }
        else if(pid2==0){
    	}
        else{
    		printf("Processu fils 2 créé avec succès et son pid = %d \n",pid2);
            
            pid3 = fork();
            if(pid3 < 0){
              perror("Erreur de création du troisieme processus\n");
              pid1 = wait(&status);
              pid2 = wait(&status);
              exit(EXIT_FAILURE);
            }
            else if(pid3==0){
    		}
            else{
            	printf("Processu fils 3 créé avec succès et son pid = %d \n",pid3);
                
                pid1 = wait(&status);
                pid2 = wait(&status);
                pid3 = wait(&status);
                
                printf("Arrêt des processus \n");
            }
        }
    } 
}
