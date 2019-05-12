
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include "procComm.h"


//static void getMSG();
//static void sendMSG( int );
static void initUserParams();
static void sendMsg();

static int terminateMaybe();

//void askForMore();
//void giveUpSome();
//static void nextResTime();
//static void incrAlive();
//static void giveAllBack( );


char * clockaddr;
char * pcbpaddr;
char * msgqueaddr;

//communication
static void communication();
static MsgQue * msgQue;

static SimClock * simClock;
static PCB * pcb;
static int virtualPid;
static int myPid;
static void checkMsg();


static sem_t * sem[PLIMIT];
static sem_t * sem2[PLIMIT];

int termCond;


//// resource vectors
//static int maxRequestVector[ 20 ];
//static int requestVector[ 20 ];
//static int acquiredVector[ 20 ];
//static int releaseVector [ 20 ];
//static int isWaitingForResources;

//other params
//static int requestOrReleaseRate;
//SimClock nextRes;
//SimClock minTimeAlive;
//void sigHandle(int );
//sigset_t sigset;
//int sig;


int main() {
    //   signal(SIGUSR1, sigHandle);
    //signal(SIGUSR2, sigHandle);
    communication();
    initUserParams();
    printf("hi %i\n", getpid());
    termCond =1;
    int checkTerm = 1000;
    while( termCond ) {
        usleep(10);
        sendMsg();
        sem_wait( sem2[ virtualPid ] );
        // checkMsg();
        if (!checkTerm--){
            termCond = terminateMaybe();
            checkTerm = 50;
        }


    }
    printf("bye %i - %i\n", myPid, virtualPid);
    exit(19);
}
static void initUserParams(){
    myPid = getpid();

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    srand( (time_t )ts.tv_nsec ^ myPid  );
    int i;

    for ( i =0 ; i < PLIMIT; i ++){
        if ( pcb[i].pid == myPid){
            virtualPid = pcb[i].vpid;
            break;
        }
    }


//    for ( i = 0; i <20 ; i++ ) {
//        max = ( resDesc[ i ].total * 66 ) / 100;
//        if(max <= 0)
//            maxRequestVector[i] = 0;
//        else if (max == 1)
//            maxRequestVector[ i ] =  ( rand() % 2);
//        else
//            maxRequestVector[ i ] =  ( rand() % ( max + 1 ) ) ;
//        maxRequestVector [ i ] = ( resDesc[i].sharable ) ? resDesc[i].total : maxRequestVector[i];
//        acquiredVector[ i ] = 0;
//        releaseVector [ i ] = 0;
//        requestVector [ i ] = 0;
//        assert( maxRequestVector[i] >= 0 && maxRequestVector[i] <= 10 && "created too many resources");
//
//    }
//    requestOrReleaseRate = BILLION ;

//    minTimeAlive.sec = 5 +  simClock->sec;
//    minTimeAlive.ns = 0;// ( rand() % BILLION )  + simClock->ns;
//    //printf("%i - %is %ins \n ",getpid(), minTimeAlive.sec, minTimeAlive.ns  );
//
//    nextRes.sec = simClock->sec;
//    nextRes.ns = simClock->ns;
//    isWaitingForResources =0 ; //false
}
static void communication(){
    clockaddr = getClockMem();
    simClock = ( SimClock * ) clockaddr;

    pcbpaddr = getPCBMem();
    pcb = ( PCB * ) pcbpaddr;

    sem[0] = openSem_0();
    sem[1]= openSem_1();
    sem[2]= openSem_2();
    sem[3]= openSem_3();
    sem[4]= openSem_4();
    sem[5]= openSem_5();
    sem[6]= openSem_6();
    sem[7]= openSem_7();
    sem[8]= openSem_8();
    sem[9]= openSem_9();
    sem[10] = openSem_10();
    sem[11] = openSem_11();
    sem[12] = openSem_12();
    sem[13] = openSem_13();
    sem[14] = openSem_14();
    sem[15] = openSem_15();
    sem[16] = openSem_16();
    sem[17] = openSem_17();

    msgqueaddr = getMsgQueMem();
    msgQue = ( MsgQue *) msgqueaddr;


    sem2[0] = openSem2_0();
    sem2[1]= openSem2_1();
    sem2[2]= openSem2_2();
    sem2[3]= openSem2_3();
    sem2[4]= openSem2_4();
    sem2[5]= openSem2_5();
    sem2[6]= openSem2_6();
    sem2[7]= openSem2_7();
    sem2[8]= openSem2_8();
    sem2[9]= openSem2_9();
    sem2[10] = openSem2_10();
    sem2[11] = openSem2_11();
    sem2[12] = openSem2_12();
    sem2[13] = openSem2_13();
    sem2[14] = openSem2_14();
    sem2[15] = openSem2_15();
    sem2[16] = openSem2_16();
    sem2[17] = openSem2_17();

//    sigemptyset(&sigset);
//    sigaddset(&sigset, SIGUSR1);
//    sigaddset(&sigset, SIGUSR2);
}

static void checkMsg() {
    MsgQue * msg = &msgQue[ virtualPid ];
    int pageValid = 0;
    while( !pageValid )
        if( !sem_trywait( sem[ virtualPid ] ) ) {
            //           printf("c - enter crit to check\n");

            char buf[ BUFF_sz ];
            memset( buf, 0, BUFF_sz );

            if ( msg->mail.toFrom == parent && msg->mail.hasBeenRead == false) {

                strncpy( buf, msg->mail.buf, ( BUFF_sz - 1 ) );

                msg->mail.hasBeenRead = true;
                pageValid = 1;

            }
            if ( strlen( buf ) )
                printf("child: Received message: %s\n", buf);
     //       printf("c %i - done waiting  \n", getpid());
            sem_post( sem[ virtualPid ] );
           // usleep(5000);
        }

//    printf("c - leave crit to get\n");
//leave critical

}
static void sendMsg() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    srand( (time_t ) ts.tv_nsec ^ getpid()  );
    MsgQue * msg = &msgQue[virtualPid];
    int sent = 0;
    while( !sent )
        if( !sem_trywait( sem[ virtualPid ] ) ) {
            //  printf("c - enter crit to send\n");

            int request = rand()%32;
            int rw = rand()%2; // 1->dirty
            char buf[BUFF_sz - 1];
            memset(buf, 0, BUFF_sz - 1);

            sprintf(buf, "%i %i", request, rw);

            memset(msg->mail.buf, 0, BUFF_sz );
            msg->mail.hasBeenRead = false;
            msg->mail.toFrom = child; //sending
            strncpy(msg->mail.buf, buf, BUFF_sz );
            printf("c %i - sent: %s \n", myPid,buf);

            sent = 1;
            sem_post( sem[ virtualPid ] );
        }

    //  printf("c - leave crit to send\n");
//leave critical

}

static int terminateMaybe(){
    myPid = getpid();

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    srand( (time_t )ts.tv_nsec ^ myPid  );
    return rand()%2;
}


