//
// Created by mlind on 4/23/2019.
//

#ifndef PROJECT_FIVE_PROCCOMM_H
#define PROJECT_FIVE_PROCCOMM_H
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <stdio.h>

#define BUFF_sz 32

typedef struct {
    unsigned int sec;
    unsigned int ns;
}SimClock;
#define BUFF_clock sizeof( SimClock )

typedef struct {
 pid_t pid;
 pid_t vpid;

}PCB;
#define BUFF_pcb sizeof( PCB )

typedef struct {
    int hasBeenRead; // 1 -> yes and ready to be replaced
    char toFrom; // 0-> to child 1-> from child
    char buf[BUFF_sz];
}Msg;

typedef struct {
    int dirty;
    int refByte;
    int occupied;
}Frame;
typedef struct {
    int dirty;
    int present;
}Page;
typedef struct {
    Page pages[BUFF_sz];
    int frames[BUFF_sz];
}PageTable;


typedef struct {
    pid_t pid; // for send to specific child
    Msg mail;
}MsgQue;

enum mailBox{
    parent = 'p',
    child = 'c',
    true = 1,
    false = 0
};

#define BUFF_msgque sizeof( MsgQue )

#define PLIMIT 18
#define BILLION 1000000000

#define KEY_PATH "/bin"

#define SEM_0 "/sem_0"
#define SEM_1 "/sem_1"
#define SEM_2 "/sem_2"
#define SEM_3 "/sem_3"
#define SEM_4 "/sem_4"
#define SEM_5 "/sem_5"
#define SEM_6 "/sem_6"
#define SEM_7 "/sem_7"
#define SEM_8 "/sem_8"
#define SEM_9 "/sem_9"
#define SEM_10 "/sem_10"
#define SEM_11 "/sem_11"
#define SEM_12 "/sem_12"
#define SEM_13 "/sem_13"
#define SEM_14 "/sem_14"
#define SEM_15 "/sem_15"
#define SEM_16 "/sem_16"
#define SEM_17 "/sem_17"





#define PERM 0755




void deleteMsgQueMem( char * paddr );
char * getMsgQueMem();


char * getClockMem();
void deleteClockMem( char * paddr );

char * getPCBMem();
void deletePCBMem( char * paddr );
void deleteSem() ;



sem_t * openSem();
sem_t * openSem_0();
sem_t * openSem_1();
sem_t * openSem_2();
sem_t * openSem_3();
sem_t * openSem_4();
sem_t * openSem_5();
sem_t * openSem_6();
sem_t * openSem_7();
sem_t * openSem_8();
sem_t * openSem_9();
sem_t * openSem_10();
sem_t * openSem_11();
sem_t * openSem_12();
sem_t * openSem_13();
sem_t * openSem_14();
sem_t * openSem_15();
sem_t * openSem_16();
sem_t * openSem_17();

#endif //PROJECT_FIVE_PROCCOMM_H
