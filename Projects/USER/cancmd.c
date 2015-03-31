//#include "step.h"
#include "can.h"
#include "main.h"
#include "io_input.h"

#include "cancmd.h"
#include "anniuled.h"
#include "timer.h"
#include "algorithm.h"
#include "can_dmp.h"
#include "can_wp.h"




RINGBUF canCmdRcvBuf;

void CANCMD_buf_init() {
    static CanRxMsg __msg[8];
    ringBufInit(&canCmdRcvBuf, __msg, sizeof(CanRxMsg), lenthof (__msg));
}





void ScanCmd(void) {
    CanRxMsg msg;
    if (ringBufPop(&canCmdRcvBuf, &msg)) {
        if(IS_CANMSG_DMPCMD_RCV(&msg)){
            CAN_DMP dmp;
            CANRxMSG_TO_CANDMP(&dmp,&msg);
            doCmdDmp(&dmp);
        }
    }
}


