
#include <time.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include "procComm.h"


static void sigHandle(int);
//static void createResources();
//static void giveResources();
//static void deadlock();

static pid_t pids[ PLIMIT ];
static int processLimit = 12;
static int activeLimit = 6;
static int active = 0;
static int total = 0;
static int maxTimeBetweenNewProcsSecs = 0;
static int maxTimeBetweenNewProcsNS = 1000;
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
static sem_t * sem2[PLIMIT];
static void initVars();

//table
#define TABLESZ 256
Frame frameTable[ TABLESZ ];
PageTable pageTable[PLIMIT];



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
    signal( SIGALRM, sigHandle );
    // ualarm(900000,0);
    alarm(100);
    communication();
    initVars();


    int mvRef = 1000;
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
        if( !mvRef-- )
            for (int i =0; i < TABLESZ; i++){
                frameTable[i].refByte /=2 ;
            }

        childrenStatus();

    }

    cleanSHM();
    return 0;
}
static void incre(int fl){
int x = fl ? 10:1500 ;
    simClock->ns += x;
    if (simClock->ns > BILLION ){
        simClock->sec++;
        simClock->ns -= BILLION;
        assert( simClock->ns <= BILLION && "too many nanoseconds");
    }
}

static void initVars(){
    simClock->sec = 0;
    simClock->ns = 0;
    total = 0;
    int i,j;
    for (i=0;i< processLimit; i++){
        msgQue[i].pid = 0;
        //mail
        msgQue[i].mail.hasBeenRead = true;
        msgQue[i].mail.toFrom = parent;
        memset( msgQue[i].mail.buf, 0, BUFF_sz);

        //PCB
        pcb[i].pid = -1;
        pcb[i].vpid = -1;

        //pagetable
        for ( j = 0; j < BUFF_sz; j++ ) {
            pageTable[i].pages[j].dirty = false;
            pageTable[i].pages[j].present = false;
            pageTable[i].frames[j] =  -1;
        }
    }
    for (i = 0; i < TABLESZ; i++){
        frameTable[i].dirty = false;
        frameTable[i].occupied = false;
        frameTable[i].refByte = 0; //add 128, shift >> 1
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


}
static int getIndexFromPid(int pid){
    int i;
    for ( i = 0; i < PLIMIT; i++)
        if ( pid == pids[ i ] )
            return i;
    return 0;
}

static void freeFrames(int idx){
    int i;
    for( i=0; i<32; i++ ) {
        if( pageTable[idx].frames[i] <= 0 )
            frameTable[ pageTable[idx].frames[i] ].occupied = false;
        if( frameTable[ pageTable[idx].frames[i] ].dirty  == true){
            incre(0);
        }
    }
}
static int findOldest(){
    int i, mi;
    int min = frameTable[0].refByte;
    for( mi = 0, i = 1; i < TABLESZ; i++ ){
        if ( frameTable[i].refByte < min ){
            min = frameTable[i].refByte;
            mi=i;
        }

    }
    return mi;
}
static void frameCheck( char * buf, int idx ){
    int page, rw;
    sscanf(buf,"%i %i", &page, &rw);

    if (page<0){
        freeFrames(idx);
        return;
    }
    PageTable * pTab = &pageTable[idx];
    Page * pg = &pageTable[idx].pages[page];
    int * fr = &pTab->frames[page];
    //is page present
    if( pg->present == true){
        frameTable[ *fr ].refByte += 128;
        pg->dirty = ( rw ) ? 1 : pg->dirty;
        incre(1); // +10ns

        sem_post( sem2[ idx ] );
    } else {


     //   printf("Page fault   ------    -----   -----\n");
        // find unoccupied
        int i,o;
        for (i = o = 0; i < TABLESZ; i++) {
            if (frameTable[i].occupied == false) {
                frameTable[i].occupied = true;
                frameTable[i].refByte += 128;

                pg->present = true;
                *fr = i;
                o = 1;

                incre(0); // add time to get from disk
                sem_post( sem2[ idx ] );

                break;
            }
        }

        if(!o){
            i = findOldest();
            if (frameTable[i].dirty ==true){
                incre(0); //+1500 write to disk
            }
            frameTable[i].occupied = true;
            frameTable[i].refByte += 128;
            incre(0);// get from disk

            sem_post( sem2[ idx ] );
        }



    }

    //add page table reference



}

static void checkMSG(){
    int aRequest = 0;
    //enter critical
    //   printf("p - try enter crit to check\n");

    int i,pid;
    for (i =0; i < processLimit ;i++){
        pid = pids[i];
        if ( !sem_trywait( sem[ i ] ) ) {
            //           printf("p    -    enter crit to check to %i\n", pid);

            char buf[BUFF_sz - 1];
            memset(buf, 0, BUFF_sz - 1);

            MsgQue * msg = &msgQue[ i ];

            if (msg->mail.toFrom == child && msg->mail.hasBeenRead == false) {

                strncpy(buf, msg->mail.buf, (BUFF_sz - 1));
                msg->mail.hasBeenRead = true;
                aRequest = 1;

             //   printf("parent: Received message: %s - %i\n", buf, pid);
            }
            sem_post( sem[ i ] );
            //         printf("p - leave crit after check\n");
            if(aRequest){
                frameCheck(buf, i);
                aRequest=0;
            }


            //leave critical
        }
    }
}




static void sigHandle( int cc ){
    cleanSHM();
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
    simClock->ns += 1;
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
