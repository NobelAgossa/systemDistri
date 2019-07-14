#include <sys/shm.h>
#include <sys/wait.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>
#include <math.h>


#define N 4

typedef struct matrice {
    int C[N][N];
} strmatrice;

strmatrice *Cms;
int Cid;
int base_pid, status, pid;

int main()
{
    int i,j;
    int niveau, taille, u, t;

    taille = N / sqrt(4);
    //Créer un segment partagé
    if ((Cid = shmget(IPC_PRIVATE, sizeof(strmatrice), IPC_CREAT | 0666)) < 0)
    {
        perror("smget returned -1\n");
        error(-1, errno, " ");
        exit(-1);
    }
    printf("Allocated %d, at id %d\n", (int) sizeof(strmatrice), Cid);

    //Créer les processus enfants
    base_pid = getpid();
    for(i=1; i<N; i++) {
        if (getpid()==base_pid)
            pid = fork();
        else
            break;
    }

    // Attacher les processus au segment
    if ((Cms = (strmatrice *) shmat(Cid, NULL, 0)) == (strmatrice *) -1){
        perror("Process shmat returned NULL\n");
        error(-1, errno, " ");
    }
    else
        printf("Processus %d attacher au segment %d\n", getpid(), Cid);

    //Effectuer les opérations
    niveau = getpid()-base_pid;

    u = (niveau % (N/taille)) * taille;
    t = floor((niveau * taille) / N) * taille;

    for (i=u;i<(u+taille);i++) {
        for (j=t;j<(t+taille);j++) {
            Cms->C[i][j] = niveau;
        }
    }

    //Déttacher les processus du segment
    if (shmdt(Cms) == -1){
        perror("shmdt returned -1\n");
        error(-1, errno, " ");
    }else
        printf("Processus %d detacher du segment %d\n", getpid(), Cid);

    if (getpid()==base_pid)
        for(i=1; i<N; i++) {
            wait(NULL);
        }
    else
        exit(0);

    Cms = (strmatrice *) shmat(Cid, NULL, 0);
    for (i=0;i<N;i++) {
            printf("[");
        for (j=0;j<N;j++) {
            printf("%d ", Cms->C[i][j]);
        }
        printf("]\n");
    }
    printf("\n\n");

    //Supprimer le segment
    if (shmctl(Cid,IPC_RMID,NULL) == -1){
        perror("shmctl returned -1\n");
        error(-1, errno, " ");
    }

    return 0;
}
