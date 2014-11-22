#ifndef H_SYSLINK
#define H_SYSLINK

#include <xdc/std.h>
#include <string.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/IHeap.h>
#include <ti/ipc/GateMP.h>
#include <ti/ipc/Ipc.h>
#include <ti/sdo/ipc/Ipc.h>
#include <ti/ipc/MessageQ.h>
#include <ti/ipc/HeapMemMP.h>
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/Notify.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/hal/Cache.h>
#include <xdc/cfg/global.h>


#define APP_INFO_TAG        0xBABA0000
#define ARM2DSPNotify            11u
#define DSP2ARMNotify            12u
#define PATTERNKEY               1u

#define MSGQ_NAME            "MSGQ_"
#define HEAP_NAME            "HeapMemMP"
#define HEAP_ALIGN             512
#define HEAP_MSGSIZE           512
#define HEAP_NUMMSGS            10
#define HEAPID                   0



#define DEBUG

/* App info struct passed to HOST */
typedef struct App_Info {
    Char   lMsgqName [80];
    Char   rMsgqName [80];
    Char   heapName  [80];
    UInt32 heapAlign;
    UInt32 heapMsgSize;
    UInt32 heapNumMsg;
    UInt32 heapId;
    Char   buffer  [80];
} App_Info;

typedef struct _ReCATSyslink
{
    App_Info        aInfo;
    Uint16          rProcId;
    UInt16          lProcId;
    MessageQ_Msg    msg;
    MessageQ_Handle messageQ;
    MessageQ_QueueId remoteQueueId;
#ifdef NOTIFY
    Semaphore_Handle   semHandle;
#endif
    HeapMemMP_Handle heapHandle;

    unsigned char buffer[4096];
    Semaphore_Handle ioLock;
	Semaphore_Handle SpiLock;
	Semaphore_Handle SysLock;
}ReCATSyslink;



/***********SYSLINK*********/
extern void ReCATSyslinkClear();
extern void ReCATSyslinkSend(void *data, size_t len);
extern void* ReCATSyslinkRecv(size_t* len);
extern int ReCATSyslinkInit(UInt16 armID);
extern int ReCATPrintBuffer(char *fmt, ...);
#endif
