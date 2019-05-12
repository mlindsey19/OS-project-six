
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
//    initUserParams();
    printf("hi %i\n", getpid());
    termCond =1;
    int checkTerm = 3;
    while(termCond) {
        sendMsg();
        checkMsg();
        if (!checkTerm--){
            termCond = terminateMaybe();
            checkTerm = 2;
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
            usleep(5000);
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


//static void incrAlive(){
//    struct timespec ts;
//    clock_gettime(CLOCK_MONOTONIC, &ts);
//    srand( (time_t)ts.tv_nsec );    //simClock->sec++;
//    minTimeAlive.ns += BILLION / 4;
//    if (minTimeAlive.ns > BILLION ){
//        minTimeAlive.sec++;
//        minTimeAlive.ns -= BILLION;
//        assert( simClock->ns <= BILLION && "too many nanoseconds");
//    }
//}
//void sigHandle(int cc) {
//    if (cc == SIGUSR1) {
//        printf("c - sig -1\n");
//        getMSG();
//    }
//    if (cc == SIGUSR2) {
//        printf("c - sig -2\n");
//        giveAllBack();
//    }
//}
//static void adjustSharable(){
//    int i;
//    for ( i = 0; i < 20; i++  ) {
//        if (resDesc[i].sharable)
//            acquiredVector[i] = ( resDesc[i].total < acquiredVector[ i] )? resDesc[i].total : acquiredVector[ i];
//        acquiredVector[i] = ( 0 > acquiredVector[ i] )? 0 : acquiredVector[ i ];
//
//    }
//}
//
//static void appendAcquiredVector(char * buf){
//    int temp;
//    int i;
//    for ( i = 0;i < 20; i++ ){
//        sscanf(buf, "%d %[^\n]", &temp, buf);
//        acquiredVector[ i ] += temp;
//        requestVector [ i] = 0;
//        //    printf("%i-\n s %s", temp, buf);
//    }
//    adjustSharable();
//}
//static void reduceAcquiredVector(char * buf){
//    int temp;
//    int i;
//    for ( i = 0;i < 20; i++ ){
//        sscanf(buf, "%d %[^\n]", &temp, buf);
//        acquiredVector[ i ] -= temp;
//        //    printf("%i-\n s %s", temp, buf);
//    }
//    adjustSharable();
//}
//static void getMSG() {
//    //enter critical
//
////       printf("c - try enter crit to get\n");
//
//    if( !sem_trywait(semMsgA)) {
//
//        int i;
//        char buf[BUFF_sz];
//        memset(buf, 0, BUFF_sz);
//        pid_t pid = getpid();
//
////                      printf("c - enter crit to get\n");
//        for (i = 0; i < MAX_MSGS; i++) {
//            //         printf("c - rra: %i - mpid %i hsb %i %s\n", msgQue[i].rra,
//            //                msgQue[i].pid, msgQue[i].hasBeenRead, buf);
//
//            if (msgQueA[i].rra < 1 && msgQueA[i].pid == pid && !msgQueA[i].hasBeenRead) {
//                strncpy(buf, msgQueA[i].buf, (BUFF_sz - 1));
//      //          printf("child: Received message: %s\n", buf);
//                msgQueA[i].hasBeenRead = 1;//true
//                if( msgQueA[i].rra < 0 ) {
//                  giveAllBack();
//
//                } else{
//                    appendAcquiredVector(buf);
//                    isWaitingForResources = 0;
//                  //  printf("c %i - done waiting  \n", getpid());
//                }
//
//                break;
//            }
//        }
//        sem_post(semMsgA);
//
//        //   printf("c - leave crit to get\n");
//        //leave critical
//    }
//}

//static void sendMSG( int fl ){   // fl 1-> release 0->request
//    //enter critical
//
//    //   printf("c - try enter crit to send\n");
//
//
//    if ( !sem_trywait( semMsgG ) ) {
//
//        char buf[BUFF_sz];
//        memset(buf, 0, BUFF_sz);
//
//        int i;
//        for (i = 0; i < 20; i++) {
//            if (fl)
//                sprintf(buf, "%s%d ", buf, releaseVector[i]);
//            else
//                sprintf(buf, "%s%d ", buf, requestVector[i]);
//        }
//        //        printf("c - enter crit to send\n");
//
//        for (i = 0; i < MAX_MSGS; i++) {
//            if (msgQueG[i].hasBeenRead) {
//                memset(msgQueG[i].buf, 0, BUFF_sz);
//                strcpy(msgQueG[i].buf, buf);
//                msgQueG[i].hasBeenRead = 0; // has not been read
//                msgQueG[i].rra = 1 + fl;
//                msgQueG[i].pid = getpid();
//     //           printf("child %i: Send message..  %s - fl %i\n", getpid(), buf, fl);
//                if (!fl) {
//                    isWaitingForResources = 1;
//          //          printf("c %i - is waiting  \n", getpid());
//                } else
//                    reduceAcquiredVector(buf);
//                break;
//            }
//        }
//
//        sem_post(semMsgG);
//
//        //       printf("c - leave crit to send\n");
//
//        //leave critical
//
//
//    }
//}
//void askForMore(){
//    struct timespec ts;
//    clock_gettime(CLOCK_MONOTONIC, &ts);
//    srand( (time_t ) ts.tv_nsec ^ getpid()  );
//    int max,i,t ;
//    t =0;
//    adjustSharable();
//    for ( i = 0; i <20 ; i++){
//        assert(acquiredVector[i]>= 0 );
//        assert(requestVector[i]>= 0 );
//        max = maxRequestVector[ i ] - acquiredVector[ i ] - requestVector[ i ];
//     //  max = ( resDesc[i].sharable );
//        assert(maxRequestVector[i] >= max);
//        if(max < 1)
//            requestVector[ i ] = 0;
//        else if (max == 1)
//            requestVector[ i ] =  ( rand() % 2);
//        else
//            requestVector[ i ] =  ( rand() % ( max + 1 ) ) ;
//        t += requestVector[i];
//    }
//    if(t)
//        sendMSG( 0 );
//}
//void giveUpSome(){
//    struct timespec ts;
//    clock_gettime(CLOCK_MONOTONIC, &ts);
//    srand( (time_t)ts.tv_nsec ^ getpid()  );
//    int max,i,t;
//    adjustSharable();
//    for ( i= t = 0; i <20 ; i++){
//        max = acquiredVector[ i ];
//        if(max < 1)
//            releaseVector[ i ] = 0;
//        else if (max == 1)
//            releaseVector[ i ] =  ( rand() % 2);
//        else
//            releaseVector[ i ] =  ( rand() % ( max + 1 ) ) ;
//        t+= releaseVector [ i ] ;
//    }
//    if(t)
//        sendMSG( 1 );
//}
//static void nextResTime(){
//    struct timespec ts;
//    clock_gettime(CLOCK_MONOTONIC, &ts);
//    srand( (time_t)ts.tv_nsec  ^ getpid() );
//    int ss = requestOrReleaseRate;
//    SimClock x;
//    int dx = (rand() % 50) + 50 ;
//    dx = dx * ( ss / 100 );
//    x.sec = ( dx / BILLION ) + nextRes.sec;
//    x.ns = ( dx % BILLION ) + nextRes.ns;
//    if (x.ns > BILLION ){
//        x.sec++;
//        x.ns -= BILLION;
//        assert( x.ns <= BILLION && "x - too many nanoseconds");
//    }
//    //  printf("child %i give or ask at  %is %ins\n", getpid(), x.sec, x.ns);
//
//    nextRes.sec =  x.sec;
//    nextRes.ns =  x.ns;
//}
//static void giveAllBack() {
//    //enter critical
//
//    //   printf("c - try enter crit to send all\n");
//    int notSent = 1;
//    while (notSent) {
//        usleep(100);
//        if (!sem_trywait(semMsgG)) {
//            char buf[BUFF_sz];
//            memset(buf, 0, BUFF_sz);
//
//            int i;
//            for (i = 0; i < 20; i++)
//                sprintf(buf, "%s%d ", buf, acquiredVector[i]);
//
//
//            //      printf("c - enter crit to send all\n");
//
//            for (i = 0; i < MAX_MSGS; i++) {
//                if (msgQueG[i].hasBeenRead) {
//                    memset(msgQueG[i].buf, 0, BUFF_sz);
//                    strncpy(msgQueG[i].buf, buf, (BUFF_sz - 1));
//                    //           msgQue[i].buf[ BUFF_sz -1 ] = 0;
//                    msgQueG[i].hasBeenRead = 0; // has not been read
//                    msgQueG[i].rra = 2;
//                    msgQueG[i].pid = getpid();
//                    //     printf("child %i: Send all back..  %s \n", getpid(), buf);
//                    reduceAcquiredVector(buf);
//                    isWaitingForResources = 0;
//             //       printf("c %i - done waiting  \n", getpid());
//                    notSent = 0;
//                    break;
//                }
//            }
//
//            sem_post(semMsgG);
//            //       printf("c - leave crit to send all\n");
//
//            //leave critical
//        }
//    }
//
//}