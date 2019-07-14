#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 
#include <sys/sem.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>

#define ITER 10

union semun {
   int  val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;  /* Buffer for IPC_INF
                              (Linux-specific) */
};

//fonction d'écriture qui prend en parametre 
//le pointeur vers la mémoire

static void write(char *str,struct sembuf sb,int semid){
  int a,i;
  for(i=0; i<100; i++){
  	
  	// tentative de verrou du semaphore
    sb.sem_op = -1;
    if(semop(semid, &sb,1) == -1){
      perror("sempo");
      exit(1);
    }
  	
    // lecture de la mémoire
    a = atoi(str);
    printf("Processus %d lit %d\n", getpid(), a);
    // modification de la valeur mémoire
    a = a + 1;
    //sleep(1);
    // ecriture de la valeur en mémoire
   sprintf(str, "%d", a);
   
   // déverrouillage du semaphore
    sb.sem_op = 1;
    if(semop(semid, &sb,1) == -1){
      perror("sempo");
      exit(1);
    } 
  }
}

int main() 
{
  pid_t pid1, pid2, pid3;
  int status;
  int a=0;
  int i;
  struct sembuf sb = {0, 0, 0}; 
  union semun sem_arg;

  // clé pour le semaphore 
  key_t sem_key = ftok("semfile",75);
  // on demande au system de nour créer le semaphore
  int semid = semget(sem_key, 1, 0666);
  
  // génération de la clé unique 
  key_t memo_key = ftok("shmfile",65); 
  
  // shmget retourne un identifiant dans memo_id 
  int memo_id = shmget(memo_key,1024,0666|IPC_CREAT); 

  // shmat permet d'attacher à la mémoie partagée 
  char *str = (char*) shmat(memo_id,(void*)0,0); 

  // création des trois processus et leurs actions
  	  
  pid1 = fork();
  if(pid1 < 0){
        perror("Erreur de création du premier processus\n");
        exit(EXIT_FAILURE);
  }
  else if(pid1==0){
  	write(str,sb,semid);
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
       	  write(str,sb,semid);
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
              write(str,sb,semid);
    		}
            else{
            	printf("Processu fils 3 créé avec succès et son pid = %d \n",pid3);
                
                pid1 = wait(&status);
                pid2 = wait(&status);
                pid3 = wait(&status);
                
                // on lit la dernière valeur de la variable partagée
                a = atoi(str);
                printf("Valeur Finale de la mémoire = %d\n", a);
                //le processus détache str de la mémoire partagée
                shmdt(str);
                // destruction de la mémoire
                shmctl(memo_id,IPC_RMID,NULL);
                printf("Arrêt des processus \n");
            }
        }
    }  
	
  return 0; 
} 

