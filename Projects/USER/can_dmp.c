#include "can.h"
#include "main.h"
#include "io_input.h"
#include "can_wp.h"

#include "cancmd.h"
#include "anniuled.h"
#include "timer.h"
#include "algorithm.h"
#include "can_dmp.h"
#include "crc.h"




typedef void (*pFunction)(void);
pFunction Jump_To_Application;

uint8_t DeviceCanAddr = 0;

#if  defined(DMP_TYPE_INPUT)
const uint8_t Ver[] = { "SoftVer-B.SINA.PB-A.01.00.00&"__DATE__ };
#elif defined(DMP_TYPE_OUTPUT)
const uint8_t Ver[] = { "SoftVer-B.SQFA.PB-A.01.00.00&"__DATE__ };
#elif defined(DMP_TYPE_MOTOR)
const uint8_t Ver[] = { "SoftVer-B.SBJA.PB-A.01.00.00&"__DATE__ };
#endif



#define BOOT_FLAG_APPTOIAP (*(unsigned int *)(0x200000ff))


void dmpGoToApp(void) {
    DEFINE_CAN_DMP_FRAME(dmpt);
    dmpt.dlc = 2;
    dmpt.data[0] = CANCMD_GOTOAPP;
    if (BOOT_FLAG_APPTOIAP != 0x5faff55f
        || *(uint32_t *)APP_ADDR < 0x20000000) {
        dmpt.data[1] = 0;
        dmpSend(&dmpt);
        return;
    }
    //todo check
    dmpt.data[1] = 1;
    dmpSend(&dmpt);
    __disable_irq();
    BOOT_FLAG_APPTOIAP = POWERUP_IAPTOAPP;
    Jump_To_Application = (pFunction)(APPLICATION_ADDRESS + 4);
    __set_MSP(*(__IO uint32_t *)APPLICATION_ADDRESS);
    Jump_To_Application();
}


static uint8 eraseFlash(void) {
    uint32_t NbrOfPage, proaddr, re = 1;
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
    NbrOfPage = (APP_ADDREND - APP_FLAH_ADDR) / FLASH_PAGE_SIZE;
    for (uint32_t i = 0; i < NbrOfPage; i++) {
        proaddr = FLASH_PAGE_SIZE;
        proaddr = proaddr * i;
        proaddr = proaddr + APP_ADDR;
        if (FLASH_ErasePage(proaddr) != FLASH_COMPLETE) {
            re = 0;
            break;
        }
    }
    return re;
}


void dmpEraseFlash(void) {
    uint8 re;
    re = eraseFlash();
    DEFINE_CAN_DMP_FRAME(dmpt);
    dmpt.data[0] = CANCMD_ERASEFLASH;
    dmpt.dlc = 2;
    dmpt.data[2] = re;
    dmpSend(&dmpt);
}


uint32_t apppacknumber = 0;
bool preburnapp = 0;

void dmpPreBurnApp(CAN_DMP *dmp) {
    uint8 re;
    DEFINE_CAN_DMP_FRAME(dmpt);
    apppacknumber = dmp->data[1] | dmp->data[2] << 8 | dmp->data[3] << 16;
    FLASH_Unlock();
    re = eraseFlash();
    dmpt.dlc = 2;
    dmpt.data[0] = CANCMD_PREBURNAPP;
    dmpt.data[1] = re ;
    dmpSend(&dmpt);
    preburnapp = re;
}


#define BOOT_BURN_APP_PACKERROR     -1
#define BOOT_BURN_APP_WRITE_TIMEOUT -2
#define BOOT_BURN_APP_CHECK_ERROR   -3
void dmpBurnFlash(CAN_DMP *dmp) {
    uint8_t re;
    uint32_t addr, data;
    uint32_t ipack;
    static uint32_t nextpack = 0;
    DEFINE_CAN_DMP_FRAME(dmpt);
    ipack = dmp->data[1] | dmp->data[2] << 8 | dmp->data[3] << 16;
    if (dmp->dlc != 8) {
        re = BOOT_BURN_APP_PACKERROR;
             goto ERROR;
    }
    if (ipack != nextpack) {
        re = BOOT_BURN_APP_PACKERROR ;
             goto ERROR;
    } else {
        nextpack++;
    }
    addr = ipack * 4 + APP_ADDR;
    data = dmp->data[4] | dmp->data[5] << 8 | dmp->data[6] << 16 | dmp->data[7] << 24;
    if (FLASH_ProgramWord(addr, data) == FLASH_COMPLETE) {
        dmpt.dlc = 5;
        dmpt.data[0] = CANCMD_BURNAPP;
        dmpt.data[1] = (uint8_t)(ipack);
        dmpt.data[2] = (uint8_t)(ipack >> 8);
        dmpt.data[3] = (uint8_t)(ipack >> 16);
        dmpt.data[4] = (uint8_t)(ipack >> 24);
        dmpSend(&dmpt);
    } else {
        re = BOOT_BURN_APP_PACKERROR;
        goto ERROR;
    }
    if (ipack == apppacknumber - 1) {
        //todo check
        //todo write check;
        //MD5_CTX md5 ;
        //unsigned char digest[16] ;
        //MD5Init(&md5);
        //MD5Update(&md5,(unsigned char *)0x20000000,0x100);
        //MD5Final(&md5,digest);
        FLASH_Lock();
    }
    return;
    ERROR:
    dmpt.dlc = 5;
    dmpt.data[0] = CANCMD_BURNAPP;
    dmpt.data[1] = (uint8_t)(re);
    dmpt.data[2] = (uint8_t)(re >> 8);
    dmpt.data[3] = (uint8_t)(re >> 16);
    dmpt.data[4] = (uint8_t)(re >> 24);
    dmpSend(&dmpt);
    FLASH_Lock();
}



void dmpGoToIap(CAN_DMP *dmp) {
    if (dmp->dlc != 1) {
        return;
    }
    DEFINE_CAN_DMP_FRAME(dmpt);
    dmpt.dlc = 1;
    dmpt.data[0] = CANCMD_GOTOIAP;
    dmpSend(&dmpt);
    __disable_irq();
    BOOT_FLAG_APPTOIAP = POWERUP_APPTOIAP;
    Jump_To_Application = (pFunction)(IAPLICATION_ADDRESS + 4);
    __set_MSP(*(__IO uint32_t *)IAPLICATION_ADDRESS);
    Jump_To_Application();
}


#define Ver_No_Send  8 //  "SoftVer-" should not send

void dmpReadSoftVer(CAN_DMP *dmp) {
    uint16_t i;
    uint8_t j;
    uint8_t length;
    uint8_t num;
    DEFINE_CAN_DMP_FRAME(dmpt);
    dmpt.data[0] = CANCMD_READSOFTVER;

    length = sizeof Ver;
    length = length + 1 - Ver_No_Send + 1;      //实际要发送的数据长度  每包实际数据为6个
    num = DIVUP(length, 6);
    for (j = 0; j < num; j++) {
        dmpt.data[1] = ((num & 0xf) << 4) + j;
        if (j == (num - 1)) {
            //最高一包数据
            dmpt.dlc = (length % 6) + 2;
            dmpt.data[dmpt.dlc - 1] = DeviceCanAddr;
            for (i = 2; i < (dmpt.dlc - 1); i++) {
                dmpt.data[i] = Ver[8 + j * 6 + i - 2];
            }
        } else {
            dmpt.dlc = 8;
            for (i = 2; i < 8; i++) {
                dmpt.data[i] = Ver[8 + (i - 2) + j * 6];
            }
        }
        dmpSend(&dmpt);
    }
}


#define CAN_FILTER_IDX_DATA    0X04
#define CAN_MASK_IDX_DATA      0X06

uint8_t dmpSetAddr(CAN_DMP *dmp) {
    if ((dmp->uid != Flash_Device_ID_STM) || (dmp->dlc != 3)) {
        return 0;
    }

    uint8_t addr;
    addr = dmp->data[2] << 8 | dmp->data[1];
    if ((addr == 0) || (addr == CAN_WP_ID_ALL_BROADCAST) ||
        (CAN_WP_GET_TYPE(addr) != CAN_WP_DEV_TYPE_INPUT)) {
        return 0;
    }

    DeviceCanAddr = addr;
    unsigned int mask = (0x3ff << 18 | 1 << 28) << 3 | CAN_MASK_IDX_DATA;
    unsigned int filter = (CAN_WP_ID(CAN_WP_DEV_TYPE_INPUT, addr) << 18 | 1 << 28) << 3
                          | CAN_FILTER_IDX_DATA;
    canAddfilter(filter, mask, 4);
    ledRunSetState(LED_STAT_RUN);

    DEFINE_CAN_DMP_FRAME(dmpt);
    dmpt.dlc = 3;
    dmpt.data[0] = CANCMD_SETADRR;
    dmpt.data[1] = (uint8)DeviceCanAddr;
    dmpt.data[2] = (uint8)(DeviceCanAddr >> 8);
    dmpSend(&dmpt);
    return 1;
}


void dmpWaitSetAdd(CAN_DMP *dmp) {
    if ((dmp->uid != Flash_Device_ID_STM) || dmp->dlc != 2) {
        return;
    }
    if (dmp->data[1] == 1) {
        ledRunSetState(LED_STAT_WAITADRR);            //让LED灯进入等待配对模式 LED闪加快
    } else {
        if ((DeviceCanAddr != 0) && (DeviceCanAddr != 0xff)) {
            ledRunSetState(LED_STAT_RUN);
        } else {
            ledRunSetState(LED_STAT_NOTREGEST);
        }
    }
    DEFINE_CAN_DMP_FRAME(dmpt);
    dmpt.dlc = 1;
    dmpt.data[0] = CANCMD_WAITSETADD;
    dmpSend(&dmpt);
}




void dmpReadWpId(CAN_DMP *dmp) {
    DEFINE_CAN_DMP_FRAME(dmpt);
    dmpt.dlc = 2;
    dmpt.data[0] = CANCMD_CHECKID;
    dmpt.data[1] = (uint8)DeviceCanAddr;
    dmpt.data[2] = (uint8)(DeviceCanAddr >> 8);
    dmpSend(&dmpt);
}


void doCmdDmp(CAN_DMP *dmp) {
    switch (dmp->data[0]) {
    case CANCMD_READSOFTVER:
        dmpReadSoftVer(dmp);
        break;
    case CANCMD_GOTOIAP:
        dmpGoToIap(dmp);
        break;
    case CANCMD_BURNAPP:
        dmpBurnFlash(dmp);
        break;
    default:
        break;
    }
}


uint8_t dmpSend(CAN_DMP *dmp) {
    CanTxMsg msg;
    msg.ExtId = dmp->uid | 1 << 27 | 0 << 28;
    msg.RTR = CAN_RTR_DATA;
    msg.IDE = CAN_ID_EXT;
    msg.DLC = dmp->dlc;
    memcpy(msg.Data, dmp->data, lenthof(msg.Data));
    return CanSend(&msg);
}




