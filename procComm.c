//
// Created by mlind on 4/23/2019.
//

#include "procComm.h"
 enum shmkeyID{
    clockshmkey = 'a',
    pcbshmkey = 'r',
    msgqueshmkey = 'm',
};

char * getMsgQueMem(){
    key_t shmkey;

    if ((shmkey = ftok( KEY_PATH , msgqueshmkey)) == (key_t) -1) {
        perror("IPC error: ftok");
    }
    char * paddr;
    int shmid = shmget ( shmkey, PLIMIT * BUFF_msgque , PERM | IPC_CREAT );

    if ( shmid == -1 )
        perror( "msgque get mem - error shmid" );

    paddr = ( char * ) ( shmat ( shmid, 0,0));

    return paddr;
}

void deleteMsgQueMem( char * paddr ){
    key_t shmkey;

    if ((shmkey = ftok( KEY_PATH , msgqueshmkey)) == (key_t) -1) {
        perror("IPC error: ftok");
    }

    int shmid = shmget ( shmkey, PLIMIT * BUFF_msgque, PERM );

    shmctl(shmid, IPC_RMID, NULL);

    if(  shmdt( paddr )  == -1 ){
        perror("err shmdt clock");
    }
}


sem_t * openSem_0(){
    sem_t * sem = sem_open(SEM_0, O_CREAT, PERM, 1);
    return sem;
}
sem_t * openSem_1(){
    sem_t * sem = sem_open(SEM_1, O_CREAT, PERM, 1);
    return sem;
}
sem_t * openSem_2(){
    sem_t * sem = sem_open(SEM_2, O_CREAT, PERM, 1);
    return sem;
}
sem_t * openSem_3(){
    sem_t * sem = sem_open(SEM_3, O_CREAT, PERM, 1);
    return sem;
}
sem_t * openSem_4(){
    sem_t * sem = sem_open(SEM_4, O_CREAT, PERM, 1);
    return sem;
}
sem_t * openSem_5(){
    sem_t * sem = sem_open(SEM_5, O_CREAT, PERM, 1);
    return sem;
}
sem_t * openSem_6(){
    sem_t * sem = sem_open(SEM_6, O_CREAT, PERM, 1);
    return sem;
}
sem_t * openSem_7(){
    sem_t * sem = sem_open(SEM_7, O_CREAT, PERM, 1);
    return sem;
}
sem_t * openSem_8(){
    sem_t * sem = sem_open(SEM_8, O_CREAT, PERM, 1);
    return sem;
}
sem_t * openSem_9(){
    sem_t * sem = sem_open(SEM_9, O_CREAT, PERM, 1);
    return sem;
}
sem_t * openSem_10(){
    sem_t * sem = sem_open(SEM_10, O_CREAT, PERM, 1);
    return sem;
}
sem_t * openSem_11(){
    sem_t * sem = sem_open(SEM_11, O_CREAT, PERM, 1);
    return sem;
}
sem_t * openSem_12(){
    sem_t * sem = sem_open(SEM_12, O_CREAT, PERM, 1);
    return sem;
}
sem_t * openSem_13(){
    sem_t * sem = sem_open(SEM_13, O_CREAT, PERM, 1);
    return sem;
}
sem_t * openSem_14(){
    sem_t * sem = sem_open(SEM_14, O_CREAT, PERM, 1);
    return sem;
}
sem_t * openSem_15(){
    sem_t * sem = sem_open(SEM_15, O_CREAT, PERM, 1);
    return sem;
}
sem_t * openSem_16(){
    sem_t * sem = sem_open(SEM_16, O_CREAT, PERM, 1);
    return sem;
}
sem_t * openSem_17(){
    sem_t * sem = sem_open(SEM_17, O_CREAT, PERM, 1);
    return sem;
}


char * getClockMem(){
    key_t shmkey;

    if ((shmkey = ftok( KEY_PATH , clockshmkey)) == (key_t) -1) {
        perror("IPC error: ftok");
    }
    char * paddr;
    int shmid = shmget ( shmkey, BUFF_clock, PERM | IPC_CREAT );

    if ( shmid == -1 )
        perror( "get clock mem - error shmid" );

    paddr = ( char * ) ( shmat ( shmid, 0,0));

    return paddr;
}
void deleteClockMem( char * paddr ){
    key_t shmkey;

    if ((shmkey = ftok( KEY_PATH , clockshmkey)) == (key_t) -1) {
        perror("IPC error: ftok");
    }

    int shmid = shmget ( shmkey, BUFF_clock, PERM );


    if(  shmdt( paddr )  == -1 ){
        perror("err shmdt clock");
    }
    shmctl(shmid, IPC_RMID, NULL);

}
char * getPCBMem(){
    key_t shmkey;

    if ((shmkey = ftok( KEY_PATH , pcbshmkey ) ) == (key_t) -1) {
        perror("IPC error: ftok");
    }
    char * paddr;
    int shmid = shmget ( shmkey,  PLIMIT * BUFF_pcb , PERM | IPC_CREAT );
    if ( shmid == -1 )
        perror( "error shmid  pcb" );

    paddr = ( char * ) ( shmat ( shmid, 0,0));

    return paddr;
}
void deletePCBMem( char * paddr ){
    key_t shmkey;

    if ((shmkey = ftok( KEY_PATH , pcbshmkey ) ) == (key_t) -1) {
        perror("IPC error: ftok");
    }
    int shmid = shmget ( shmkey,  PLIMIT * BUFF_pcb, PERM  );

    shmctl(shmid, IPC_RMID, NULL);

    if(  shmdt( paddr )  == -1 ){
        perror("err shmdt clock");
    }
}
void deleteSem() {

    sem_unlink(SEM_0);
    sem_unlink(SEM_1);
    sem_unlink(SEM_2);
    sem_unlink(SEM_3);
    sem_unlink(SEM_4);
    sem_unlink(SEM_5);
    sem_unlink(SEM_6);
    sem_unlink(SEM_7);
    sem_unlink(SEM_8);
    sem_unlink(SEM_9);
    sem_unlink(SEM_10);
    sem_unlink(SEM_11);
    sem_unlink(SEM_12);
    sem_unlink(SEM_13);
    sem_unlink(SEM_14);
    sem_unlink(SEM_15);
    sem_unlink(SEM_16);
    sem_unlink(SEM_17);
    // unlink message ques

}