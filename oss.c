
#include <time.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include "procComm.h"


static void sigHandle(int);
static void childHandle(int);
//static void createResources();
//static void giveResources();
//static void deadlock();

static pid_t pids[ PLIMIT ];
static int processLimit = PLIMIT;
static int activeLimit = 4;
static int active = 0;
static int total = 0;
static int maxTimeBetweenNewProcsSecs = 5;
static int maxTimeBetweenNewProcsNS = BILLION;
static void cleanSHM();
static void childrenStatus();
static void increment();
static void communication();
static void generateProc();
static void nextProcTime();
static int getIndexFromPid(int);
static void checkMSG();



static char * clockaddr;
static char * pcbpaddr;
static char * msgqueaddr;
//static char output[BUFF_sz] = "input.txt";
static SimClock gentime;
const char user[] = "user";
const char path[] = "./user";

//communication
static SimClock * simClock;
static PCB * pcb;
static MsgQue * msgQue;
static sem_t * sem[PLIMIT];
static void initVars();
static void sendMSG(  int );



// vectors
//static int aboutToAllocate [ 20 ];
//static int availableVector[ 20 ];
//static int maxVector[ 20 ];
//static int requests[ 20 ][ 20 ];
//static int allocatedMatrix[ 20 ][ 20 ];


//slowdown to prevent starvation


int main() {

    signal( SIGINT, sigHandle );
    //   signal( SIGCHLD, childHandle );
//    signal( SIGALRM, sigHandle );
    // ualarm(900000,0);
    alarm(10);
    communication();
    initVars();

    nextProcTime();
    while(1){
        if( total == processLimit && active == 0)
            break;
        increment();
        if ( ( active < activeLimit ) && ( total < processLimit ) &&
             ( simClock->sec >= gentime.sec && simClock->ns > gentime.ns ) ) {

            generateProc();
            printf("p -proc gen\n");
        }
        checkMSG();

        childrenStatus();

    }

    cleanSHM();
    return 0;
}

static void deleteMemory() {
    childrenStatus();
    deletePCBMem(pcbpaddr);
    deleteSem();
    deleteMsgQueMem(msgqueaddr);
    deleteClockMem(clockaddr);
}
static void increment(){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    srand( (time_t)ts.tv_nsec );    //simClock->sec++;
    simClock->ns += rand() % BILLION;
    if (simClock->ns > BILLION ){
        simClock->sec++;
        simClock->ns -= BILLION;
        assert( simClock->ns <= BILLION && "too many nanoseconds");
    }


}


void cleanSHM(){
    int i;
    for (i =0; i < processLimit; i++) {
        if (pids[i] > 0) {

            kill(pids[i], SIGTERM);
        }
    }
    deleteMemory();
    //   fclose(ofpt);

}
void childrenStatus() {
    pid_t pid;
    int i, status;
    for (i = 0; i < 20; i++) {
        if(pids[i] > 0){
            pid = waitpid(pids[i], &status, WNOHANG);
            if (WIFEXITED(status)
                && pid == pids[i]) {
                i = getIndexFromPid(pid);
                pids[i] = 0;
                printf("term pid:%u  status: %i \n", pid, WEXITSTATUS(status));
                active--;
            }
        }
    }
}

static void nextProcTime(){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    srand( (time_t)ts.tv_nsec  );
    int ss = maxTimeBetweenNewProcsSecs * BILLION + maxTimeBetweenNewProcsNS;
    SimClock x;
    int dx = (rand() % 50) + 50 ;
    dx = dx * ( ss / 100 );
    x.sec = ( dx / BILLION ) + gentime.sec;
    x.ns = ( dx % BILLION ) + gentime.ns;
    if (x.ns > BILLION ){
        x.sec++;
        x.ns -= BILLION;
        assert( x.ns <= BILLION && "x - too many nanoseconds");
    }
    printf("OSS: process index %i may generate after %is %ins\n",total, x.sec, x.ns);
    gentime.sec =  x.sec;
    gentime.ns = x.ns;
}

static void generateProc() {
    if ( ( pids[ total ] = fork() ) < 0)
        perror("error forking new process");
    if ( pids[ total ] == 0 )
        execl( path, user, NULL );

    pcb[total].pid = pids[total];
    pcb[total].vpid = total;

    active++;
    total++;
    nextProcTime();


}
static void initVars(){
    simClock->sec = 0;
    simClock->ns = 0;
    total = 0;
    int i;
    for (i=0;i< PLIMIT; i++){
        msgQue[i].pid = 0;
        //mail 1
        msgQue[i].mail.hasBeenRead = true;
        msgQue[i].mail.toFrom = parent;
        memset( msgQue[i].mail.buf, 0, BUFF_sz);


        //PCB
        pcb[i].pid = -1;
        pcb[i].vpid = -1;

    }
}
static void communication(){
    clockaddr = getClockMem();
    simClock = ( SimClock * ) clockaddr;



    pcbpaddr = getPCBMem();
    pcb = ( PCB * ) pcbpaddr;


    msgqueaddr = getMsgQueMem();
    msgQue = ( MsgQue *) msgqueaddr;


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


}
static int getIndexFromPid(int pid){
    int i;
    for ( i = 0; i < PLIMIT; i++)
        if ( pid == pids[ i ] )
            return i;
    return 0;
}
static void sendMSG( int pidIndex ) {
    //enter critical
    pid_t pid = pids[ pidIndex ];
    int trySend = 5;
    while (!trySend--) {
        if (pid) {
            if ( !sem_trywait( sem[ pidIndex ] ) ) {
                printf("p    -    enter crit to send to %i\n", pid);

                char buf[BUFF_sz - 1];
                memset(buf, 0, BUFF_sz - 1);

                sprintf(buf, "%i", 1);

                msgQue[ pidIndex ].mail.hasBeenRead = false;
                msgQue[ pidIndex ].mail.toFrom = parent; //sending
                strncpy( msgQue[ pidIndex ].mail.buf, buf, BUFF_sz );
                trySend = 0;
                sem_post( sem[ pidIndex ] );

                //leave critical
                printf("Parent: Send message...%s :%i \n", buf, pid);
            }
        }
    }
}
static void checkMSG(){
    int aRequest = 0;
    //enter critical
 //   printf("p - try enter crit to check\n");
    int i,pid;
    for (i =0; i < PLIMIT;i++){
        pid = pids[i];
        if ( !sem_trywait( sem[ i ] ) ) {
 //           printf("p    -    enter crit to check to %i\n", pid);

            char buf[BUFF_sz - 1];
            memset(buf, 0, BUFF_sz - 1);

            MsgQue * msg = &msgQue[i];

            if (msg->mail.toFrom == child && msg->mail.hasBeenRead == false) {

                strncpy( buf, msg->mail.buf, ( BUFF_sz - 1 ) );
                msg->mail.hasBeenRead = true;
                aRequest =1;
            }

            printf("parent: Received message: %s - %i\n", buf, pids[i]);

            sem_post( sem[ i ] );
   //         printf("p - leave crit after check\n");
            if(aRequest){
                sendMSG( i );
                aRequest=0;
            }


            //leave critical
        }
    }
}
static void sigHandle( int cc ){
    cleanSHM();
}
static void childHandle( int cc ){
    childrenStatus();
}
//static void adjustSharable(){
//    int i;
//    for ( i = 0; i < 20; i++  ) {
//        if (resDesc[i].sharable)
//            availableVector[i] = maxVector[i];
//    }
//}
//
//static void adjustResVectors(pid_t pid){
//    int i, j;
//    i = getIndexFromPid(pid);
//    for ( j = 0 ; j < 20; j++ ){
//        allocatedMatrix[i][j] += aboutToAllocate[j];
//        availableVector[j] -= aboutToAllocate[j];
//        requests[i][j]  = 0 ;
//        aboutToAllocate[j] = 0;
//
//    }
//    adjustSharable();
//}
//
//
////static void printReqAlloAvail(pid_t pid){
////    int i, j;
////    printf("pid %i\n", pid);
////    i = getIndexFromPid(pid);
////
////    printf("req - ");
////    for ( j = 0; j < 20; j++ ) {
////        printf("%i ", requests[i][j]);
////    }
////    printf("\n");
////
////    printf("has - ");
////    for ( j = 0; j < 20; j++ ) {
////        printf("%i ", allocatedMatrix[i][j]);
////    }
////    printf("\n");
////
////    printf("available - ");
////    for ( j = 0; j < 20; j++ ) {
////        printf("%i ", availableVector[j]);
////    }
////    printf("\n");
////}
//
//static void appendAvailableVector(char * buf, pid_t pid){
//    int temp;
//    int i,j;
//    i = getIndexFromPid(pid);
//    for ( j = 0; j < 20; j++ ){
//        sscanf(buf, "%d %[^\n]", &temp, buf);
//        availableVector[ j ] += temp;
//        allocatedMatrix[ i ][j] -= temp;
//
//    }
//    adjustSharable();
//    //   printReqArray();
//}
//static void appendRequestVector(char * buf, pid_t pid){
//    int temp;
//    int i = getIndexFromPid(pid);
//    int j;
//    for ( j = 0; j < 20; j++ ){
//        sscanf(buf, "%d %[^\n]", &temp, buf);
//        requests[ i ][j] += temp;
//    }
//}
//static void checkMSG(){
//    //enter critical
//
////    printf("p         - try enter crit to check\n");
//    if (!sem_trywait(semMsgG)) {
//        int i;
//        char buf[BUFF_sz];
//        memset(buf, 0, BUFF_sz);
//        adjustSharable();
//        pid_t pid;
//
//
//        //   printf("p- enter crit to check\n");
//        for (i = 0; i < MAX_MSGS; i++) {
//            if (msgQueG[i].rra && !msgQueG[i].hasBeenRead) {
//                strcpy(buf, msgQueG[i].buf);
//                pid = msgQueG[i].pid;
//                if (msgQueG[i].rra - 1) {  //1-> release 0->request
//          //          printf("released : %s\n", buf);
//                    msgQueG[i].hasBeenRead = 1; //true
//                    appendAvailableVector(buf, pid);
//                } else {
//         //           printf("request : %s\n", buf);
//                    msgQueG[i].hasBeenRead = 1; //true
//                    appendRequestVector(buf, pid);
//               //     printReqAlloAvail(pid);
//                }
//            }
//        }
////        printf("p- leave crit to check\n");
//
//        sem_post(semMsgG);
//    }//leave critical
//}
//
//static int isRequestAvailable( int i ){
//    //check requests vs available
////    printf("p- checking if res avail \n");
//    int j, a, q,z;
//    a = 1;
//    q = 0;
//    for( j  = 0 ; j < 20 ; j++ ) {
//        q += requests[i][j];
//        if ( ( requests[i][j] + allocatedMatrix[i][j] ) > availableVector[j]) {
//            a = 0;
//        }
//    }
//
//    a = ( q == 0 ) ? 0  : a ;//if q is 0 a<-0
//    //   a = !( q + z ) ? 1 : a;
//    // if (!a)
//    //      printf("not avail : %i, %i\n", a, q);
//    return a;
//}
//
//static void giveResources(){
//    int i;
//    for ( i =0 ; i < total ; i++){
//        if ( isRequestAvailable( i ) ){
//            int j;
//            for ( j =0; j < 20; j++ )
//                aboutToAllocate[j] = requests[i][j];
//            sendMSG( pids[i] , 0);
//            break;
//        }
//    }
//}
//
//static void deadlock(){
//    int i,j, allReq, availReq,allocated;
//    allReq = availReq =allocated = 0;
//    for (i = 0; i< 20 ; i++){
//        int isAvail = 1;
//        int thisReq =0 ;
//        for(j=0; j < 20; j++) {
//            allReq += requests[i][j];
//            thisReq += requests[i][j];
//            allocated += allocatedMatrix[i][j];
//            if ( ( requests[i][j] + allocatedMatrix[i][j] ) > availableVector[j]) {
//                isAvail = 0;
//            }
//        }
//        isAvail = ( thisReq) ? isAvail: thisReq;
//        availReq += isAvail;
//    }
//    // if sum of request == 0 -> do nothing
//    // if requests avail > 0 -> do nothing
//    // if allocated == 0 -> do nothing
//    // no deadlock
//    int deadlockcheck = 0;
//    if(110 <  deadlockcheck++) {
//        deadlockcheck = 0;
//        printf("deadlock  - ra %i - sumR %i - allo %i\n", availReq, allReq, allocated);
//    }
//    int indexOfMost = 0;
//    int max = 0;
//    if (allReq && !availReq && allocated){
//        // restart proc with most rec
//
//        for (i = 0; i < 20 ; i++) {
//            int numRes = 0;
//            for (j = 0; j < 20; j++) {
//                numRes += allocatedMatrix[i][j];
//            }
//            if (max < numRes) {
//                max = numRes;
//                indexOfMost = i;
//            }
//        }
//      if(max) {
//         // kill(pids[indexOfMost], SIGUSR2);
//          printReqArray();
//          printf("**************************deadlock %i\n", pids[indexOfMost]);
//          sendMSG(pids[ indexOfMost ],1);
//
//        //  sleep(1);
//      }
//    }
//
//
//}
//
//static void createResources(){
//    struct timespec ts;
//    clock_gettime(CLOCK_MONOTONIC, &ts);
//    srand( (time_t)ts.tv_nsec  );
//
//    int i;
//    for ( i =0; i <20 ; i++){
//        resDesc[i].total = (rand() % 9) + 1;
//        maxVector[i] = resDesc[i].total;
//        availableVector[i] = maxVector[i];
//        assert(resDesc[i].total >= 0 && resDesc[i].total <= 10 && "created too many resources");
//        int p = 20;
//        int dart = ( rand() % 99 ) + 1; //20%
//        resDesc[ i ].sharable = ( dart < p ) ? 1 : 0; //1->true
//    }
//    //set all msg to empty
//    for ( i = 0; i < MAX_MSGS; i ++){
//        msgQueA[i].hasBeenRead = 1;
//        msgQueG[i].hasBeenRead = 1;
//    }
//
//
//
//}