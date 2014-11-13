#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Standard headers */
#include <ti/syslink/Std.h>

/* OSAL & Utils headers */
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/OsalPrint.h>
#include <ti/syslink/SysLink.h>
#include <ti/syslink/ProcMgr.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/String.h>
#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/ProcMgr.h>


#include <ti/ipc/MultiProc.h>
#include <ti/ipc/HeapMemMP.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/ipc/MessageQ.h>
#include <ti/ipc/Notify.h>
#include <ti/ipc/Ipc.h>
#include <ti/syslink/IpcHost.h>

/* Sample app headers */
#include <ti/syslink/samples/hlos/common/SysLinkSamples.h>

#include <ti/syslink/samples/hlos/slaveLoader/SlaveLoader.h>
#include <ti/syslink/samples/hlos/common/SysLinkSamples.h>

//#define DEBUG_SYSLINK
#define APP_INFO_TAG        0xBABA0000

#define PATTERNKEY               1u
#define APP_SHAREDREGION_ENTRY_ID   0u
typedef struct App_Info {
    Char   rMsgqName [80];
    Char   lMsgqName [80];
    Char   heapName  [80];
    UInt32 heapAlign;
    UInt32 heapMsgSize;
    UInt32 heapNumMsg;
    UInt32 heapId;
    Char   buffer  [80];
} App_Info;



typedef struct ReCATSyslink {
    UInt16 dspProcId;
    UInt32 selfProcId;
    String           mapFile;

    MessageQ_Msg     msg;
    MessageQ_Handle  messageQ;
    MessageQ_QueueId remoteQueueId;
    IHeap_Handle     srHeap;
    Ptr              ptr;
    SizeT            heapSize;
    HeapMemMP_Handle heapHandle;

    App_Info        aInfo;
    Int32           isRight;
}ReCATSyslink;

ReCATSyslink syslink;

void* ReCATSyslinkRecv(size_t* len)
{
    /* Get a message */
    Int status = MessageQ_get(syslink.messageQ, &syslink.msg, MessageQ_FOREVER);

    /*Read the data pattern in the message to check the data integrity*/
    if (SharedRegion_isCacheEnabled(SharedRegion_getId((Ptr)syslink.msg))) {
        Cache_inv((Ptr)syslink.msg, MessageQ_getMsgSize(syslink.msg), Cache_Type_ALL, TRUE);
    }

    UInt8* dest = (char*) syslink.msg;
    dest += sizeof(MessageQ_MsgHeader);

    UInt32 size;
    size = ( dest[0]<<24 ) | (dest[1] << 16) | (dest[2] << 8) | (dest[3]);
    dest += 4;

    *len = size;
    char *data = malloc(*len);
    memcpy(data, dest, *len);

#ifdef DEBUG_SYSLINK
    printf("[INFO] recv %d data: %s!\n", *len, data);
#endif

    return data;
}
void ReCATSyslinkSend(void *data, size_t len)
{
    static int msgId = 0;
    MessageQ_setMsgId(syslink.msg, msgId ++);


    UInt8* buffer = (UInt8*) syslink.msg;
    buffer += sizeof(MessageQ_MsgHeader);

    buffer[0] = len >> 24 & 0x00FF;
    buffer[1] = len >> 16 & 0x00FF;
    buffer[2] = len >>  8 & 0x00FF;
    buffer[3] = len       & 0x00FF;
    buffer += 4;
    memcpy( buffer, data, len);

    if (SharedRegion_isCacheEnabled(SharedRegion_getId((Ptr)syslink.msg))) {
        Cache_wb((Ptr)syslink.msg, MessageQ_getMsgSize(syslink.msg), Cache_Type_ALL, TRUE);
    }

    Int status = MessageQ_put(syslink.remoteQueueId, syslink.msg);
    if (status != MessageQ_S_SUCCESS) {
        Osal_printf ("MessageQ_put had a failure/error\n");
        return;
    }
#ifdef DEBUG_SYSLINK
    printf("send %d data success!\n", len);
#endif
}

static int ReCATSyslinkAlloc ()
{
    Int                 status  = 0;

    /* Read the config info written by the slave. */
    do {
        status = Ipc_readConfig(syslink.dspProcId, APP_INFO_TAG, &syslink.aInfo, sizeof(App_Info) );
    } while (status == Ipc_E_FAIL);

#ifdef DEBUG_SYSLINK
    printf("[INFO] dspID = %d self = %d\n", syslink.dspProcId, syslink.selfProcId);
    printf("[INFO] rMsgqName = %s, lMsgqName = %s, heapName = %s\n",
           syslink.aInfo.rMsgqName, syslink.aInfo.lMsgqName, syslink.aInfo.heapName);
    printf ("[INFO] heapMsgSize = %d, heapNumMsg = %d, heapAlign = %d, heapId = %d\n",
            syslink.aInfo.heapMsgSize ,
            syslink.aInfo.heapNumMsg  ,
            syslink.aInfo.heapAlign   ,
            syslink.aInfo.heapId   );
#endif

    /* Create & Register this heap */
    HeapMemMP_Params    heapMemParams;
    HeapMemMP_Params_init(&heapMemParams);
    heapMemParams.sharedAddr = NULL;
    syslink.heapSize = (HeapMemMP_sharedMemReq(&heapMemParams) + (syslink.aInfo.heapNumMsg * syslink.aInfo.heapMsgSize));
    syslink.srHeap  = SharedRegion_getHeap(APP_SHAREDREGION_ENTRY_ID);
    if (syslink.srHeap == NULL) {
        Osal_printf("SharedRegion_getHeap failed for %d processor."
                    " srHeap: [0x%x]\n", syslink.dspProcId, syslink.srHeap);
        goto FreeNone;
    }

    syslink.ptr = Memory_alloc(syslink.srHeap, syslink.heapSize, 0, NULL);
    if (syslink.ptr == NULL) {
        Osal_printf ("Memory_alloc failed!\n");
        goto FreeNone;
    }

    heapMemParams.name = syslink.aInfo.heapName;
    heapMemParams.sharedAddr = syslink.ptr;
    heapMemParams.sharedBufSize = syslink.aInfo.heapNumMsg * syslink.aInfo.heapMsgSize;
    syslink.heapHandle = HeapMemMP_create(&heapMemParams);
    if (syslink.heapHandle == NULL) {
        Osal_printf("HeapMemMP_create failed!\n");
        goto FreeStrHeap;
    }

    MessageQ_registerHeap(syslink.heapHandle, syslink.aInfo.heapId);

#ifdef DEBUG_SYSLINK
    printf("[INFO] HeapName = %s, ID = %d, size = %d\n",
           syslink.aInfo.heapName, syslink.aInfo.heapId, syslink.heapSize);
#endif


    /* Construct the messageQ name for the master */
    syslink.messageQ = MessageQ_create(syslink.aInfo.lMsgqName, NULL);
    if (syslink.messageQ == NULL) {
        Osal_printf("Error in MessageQ_create [0x%x]",status);
        goto FreeHeap;
    }
    
#ifdef DEBUG_SYSLINK
    printf("[INFO] arm's messageQ = %s\n", syslink.aInfo.lMsgqName);
#endif


    /* Construct the messageQ name for the slave */
    do {
        status = MessageQ_open(syslink.aInfo.rMsgqName, &syslink.remoteQueueId);
        usleep(10000);
    } while (status == MessageQ_E_NOTFOUND);
    if (status < 0) {
        Osal_printf("Error in MessageQ_open [0x%x] : procId [%d]\n",
                    status, syslink.dspProcId);
        goto FreeMessage;
    }
#ifdef DEBUG_SYSLINK
    printf("[INFO] dsp's messageQ = %s\n", syslink.aInfo.rMsgqName);
#endif


    syslink.msg = MessageQ_alloc(syslink.aInfo.heapId, syslink.aInfo.heapMsgSize);
    if (syslink.msg == NULL) {
        Osal_printf ("Error in MessageQ_alloc\n");
        goto FreeMessage;
    }
    syslink.isRight = 0;
    return 0;


FreeMsg:
    MessageQ_free(syslink.msg);
FreeMessage:
    MessageQ_delete(&syslink.messageQ);
FreeQueue:
    MessageQ_close(&syslink.remoteQueueId);

FreeHeap:
    MessageQ_unregisterHeap(syslink.aInfo.heapId);
    HeapMemMP_delete(&syslink.heapHandle);
FreeStrHeap:
    Memory_free(syslink.srHeap, syslink.ptr, syslink.heapSize);
FreeNone:
    return -1;
}


int ReCATSyslinkInit()
{
    fprintf(stderr, "Syslink setup...");
    SysLink_setup ();
    SysLinkSamples_startup ();
    fprintf(stderr, "ok.\n");


    String  dspFile = "./dsp.xe674";
    syslink.dspProcId = MultiProc_getId ("DSP");
    syslink.selfProcId = MultiProc_self();
    syslink.msg    = NULL;
    syslink.messageQ = NULL;
    syslink.mapFile = NULL;
    syslink.isRight = -1;

    /* Set the slaves to run the application with. */
    Int status = SysLinkSamples_setToRunProcIds(1, &syslink.dspProcId, FALSE);

    if (SysLinkSamples_toRunProcId(syslink.dspProcId))
    {
        status = SlaveLoader_startup(syslink.dspProcId, "startup", dspFile, syslink.mapFile);

        Osal_printf("Loading and starting procId [%d] with %s and mapFile: %s . Ret %d\n",
                    syslink.dspProcId, dspFile, syslink.mapFile, status);

        status = Ipc_control(syslink.dspProcId, Ipc_CONTROLCMD_LOADCALLBACK, NULL);
        if (status < 0) {
            Osal_printf("Error in Ipc_control "
                        "Ipc_CONTROLCMD_LOADCALLBACK [0x%x]\n", status);
            return -1;
        }

        status = Ipc_control(syslink.dspProcId, Ipc_CONTROLCMD_STARTCALLBACK, NULL);
        if (status < 0) {
            Osal_printf ("Error in Ipc_control "
                         "Ipc_CONTROLCMD_STARTCALLBACK[0x%x]\n", status);
            return -1;
        }
    }
    else
        return -1;


    return ReCATSyslinkAlloc();
}

void ReCATSyslinkClear()
{
    if(syslink.isRight == 0)
    {
        MessageQ_free(syslink.msg);
        MessageQ_delete(&syslink.messageQ);
        MessageQ_close(&syslink.remoteQueueId);
        MessageQ_unregisterHeap(syslink.aInfo.heapId);
        HeapMemMP_delete(&syslink.heapHandle);
        Memory_free(syslink.srHeap, syslink.ptr, syslink.heapSize);
    }
    Int32 status = Ipc_control(syslink.dspProcId, Ipc_CONTROLCMD_STOPCALLBACK, NULL);
    Osal_printf("Leaving ReCATSyslinkClear() (0x%x)\n", status);


    if (SysLinkSamples_toRunProcId(syslink.dspProcId))
    {
            status = SlaveLoader_shutdown(syslink.dspProcId, "shutdown",  syslink.mapFile);
            Osal_printf("SlaveLoader_shutdown status [%d]\n", status);
    }


    return;
}

char* ReCATSyslinkReadBuffer()
{
    Ipc_readConfig(syslink.dspProcId, APP_INFO_TAG, &syslink.aInfo, sizeof(App_Info) );
    return syslink.aInfo.buffer;
}

void ReCATSyslinkSendEvent(UInt32 id)
{
    Notify_sendEvent(syslink.dspProcId, 0, id, 0, TRUE);
}
