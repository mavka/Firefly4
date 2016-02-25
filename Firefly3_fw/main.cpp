/*
 * File:   main.cpp
 * Author: Kreyl
 * Project: Armlet2South
 *
 * Created on Feb 05, 2013, 20:27
 */

#include "kl_lib.h"
#include "ch.h"
#include "hal.h"
#include "adc_f100.h"
#include "main.h"
#include "uart.h"
#include "shell.h"
#include "led_rgb_f100.h"
#include "LedSequence.h"

//#define LED_EMPTY_COLOR     ((Color_t){4, 4, 4})

LedSequence_t Seq;

App_t App;

void SaveToFlash();
void LoadFromFlash();

#if 1 // ==== Save after a while ====
#define SAVE_TIMEOUT_MS     9999    // Save to flash after this
VirtualTimer TmrSave;

void OnSaveTime(void *p) {
    chSysLockFromIsr();
    SaveToFlash();
    chSysUnlockFromIsr();
}
void ResetSaveTimer() { chVTReset(&TmrSave);}
void StartSaveTimer() { chVTSet(&TmrSave, MS2ST(SAVE_TIMEOUT_MS), &OnSaveTime, nullptr); }
#endif

#define AUTO_OFF    FALSE

#if AUTO_OFF
Adc_t Adc;
IWDG_t Iwdg;
#define ADC_VALUE_TO_OFF    540
#define ADC_VALUE_TO_ON     198
static inline void GoSleep();
#endif

#if 1 // ========================== Flash ======================================
#define FLASH_PAGE_SIZE     1024
#define FLASH_KEY1          ((uint32_t)0x45670123)
#define FLASH_KEY2          ((uint32_t)0xCDEF89AB)
#define CR_LOCK_Set         ((uint32_t)0x00000080)
#define CR_PER_Set          ((uint32_t)0x00000002)
#define CR_PER_Reset        ((uint32_t)0x00001FFD)
#define CR_STRT_Set         ((uint32_t)0x00000040)
#define CR_PG_Set           ((uint32_t)0x00000001)
#define CR_PG_Reset         ((uint32_t)0x00001FFE)
#define EraseTimeout        ((uint32_t)0x000B0000)
#define ProgramTimeout      ((uint32_t)0x00002000)

__attribute__ ((section("MyFlash"), aligned(FLASH_PAGE_SIZE)))
const LedSequence_t FlashSeq = {
        7,
        {
                {csSetColor, 0, {4,0,0}},
                {csWait, 450},
                {csSetColor, 0, {0,4,0}},
                {csWait, 450},
                {csSetColor, 0, {0,0,4}},
                {csWait, 450},
                {csGoto, 0}
        }
};
#endif

#if 1 // =========================== Main ======================================
int main(void) {
    // ==== Init clock system ====
    Clk.SetupBusDividers(ahbDiv4, apbDiv1, apbDiv1);
    Clk.UpdateFreqValues();
    // ==== Init OS ====
    halInit();
    chSysInit();
    // ==== Init Hard & Soft ====
    JtagDisable();

    App.InitThread();
    Uart.Init(115200, UART_GPIO, UART_TX_PIN, UART_GPIO, UART_RX_PIN);
    Led.Init();
#if AUTO_OFF
    // Set white and print info only when switch on, not after watcdog reset.
    if(!Iwdg.ResetOccured()) {
#endif
        Uart.Printf("\r%S %S\r", APP_NAME, APP_VERSION);
    LoadFromFlash();
    Led.StartSequence(&Seq.Chunk[0]);
#if AUTO_OFF
    }
    Adc.Init();
    while(true) {
        chThdSleepMilliseconds(450);

        uint32_t rslt = 0;
        Adc.Enable();
        Adc.Calibrate();
        for(uint8_t i=0; i<8; i++) {
            Adc.StartConversion();
            while(!Adc.ConversionCompleted()) chThdSleepMilliseconds(20);
            rslt += Adc.Result();
        }
        Adc.Disable();
        rslt >>= 3;
        Uart.Printf("Adc: %u\r", rslt);
        if(rslt > ADC_VALUE_TO_OFF) Led.SetColorSmoothly(clBlack);
        if(rslt < ADC_VALUE_TO_ON)  Led.SetColorSmoothly(LED_COLOR);
        // Check if sleep
        if(Led.IsOff()) GoSleep();
    }
#else
    while(true) {
//        chThdSleepMilliseconds(720);
//        Uart.Printf("aga\r");
        uint32_t EvtMsk = chEvtWaitAny(ALL_EVENTS);
        if(EvtMsk & EVTMSK_UART_NEW_CMD) {
            App.OnCmd((Shell_t*)&Uart);
            Uart.SignalCmdProcessed();
        }
    }
#endif
}
#endif

#if 1 // ========================== Uart cmd ===================================
void App_t::OnCmd(Shell_t *PShell) {
    Cmd_t *PCmd = &PShell->Cmd;
    __attribute__((unused)) int32_t dw32 = 0;  // May be unused in some configurations
//    Uart.Printf("\r%S\r", PCmd->Name);
    // Handle command
    if(PCmd->NameIs("Ping")) Uart.Ack(OK);

    else if(PCmd->NameIs("Get")) Seq.Print();

    else if(PCmd->NameIs("Set")) {
        ResetSaveTimer();
        Led.Stop();
        Seq.Reset();
        LedChunk_t *PChunk;
        while(PCmd->GetNextTokenString() == OK) {
            if(PCmd->TokenIs("RGB")) {  // Read R, G, B
//                Uart.Printf("\rRGB");
                PChunk = &Seq.Chunk[Seq.Cnt++];
                PChunk->ChunkSort = csSetColor;
                // Color
                if(PCmd->GetNextNumber(&PChunk->Color.R) != OK) { Uart.Ack(CMD_ERROR); return; }
                if(PCmd->GetNextNumber(&PChunk->Color.G) != OK) { Uart.Ack(CMD_ERROR); return; }
                if(PCmd->GetNextNumber(&PChunk->Color.B) != OK) { Uart.Ack(CMD_ERROR); return; }
                // MorphMS
                if(PCmd->GetNextNumber(&PChunk->MorphMS) != OK) { Uart.Ack(CMD_ERROR); return; }
            }

            if(PCmd->TokenIs("Wait")) {
//                Uart.Printf("\rW");
                PChunk = &Seq.Chunk[Seq.Cnt++];
                PChunk->ChunkSort = csWait;
                if(PCmd->GetNextNumber(&PChunk->Time_ms) != OK) { Uart.Ack(CMD_ERROR); return; }
            }

            if(PCmd->TokenIs("Goto")) {
//                Uart.Printf("\rG");
                PChunk = &Seq.Chunk[Seq.Cnt++];
                PChunk->ChunkSort = csGoto;
                if(PCmd->GetNextNumber(&PChunk->ChunkToJumpTo) != OK) { Uart.Ack(CMD_ERROR); return; }
            }

            if(Seq.Cnt >= LED_CHUNK_CNT-2) { Uart.Ack(CMD_ERROR); return; } // Last token always added is of type End
        } // while GetNextToken
//        Seq.Print(); chThdSleepMilliseconds(99);
        // Add final End token
        Seq.Chunk[Seq.Cnt++].ChunkSort = csEnd;
        Led.StartSequence(&Seq.Chunk[0]);
        Uart.Ack(OK);
        StartSaveTimer();
    }

    else Uart.Ack(CMD_UNKNOWN);
}
#endif

#if AUTO_OFF
void GoSleep() {
    // Start LSI
    Clk.LsiEnable();
    // Start IWDG
    Iwdg.SetTimeout(4500);
    Iwdg.Enable();
    // Enter standby mode
    SCB->SCR |= SCB_SCR_SLEEPDEEP;
    PWR->CR = PWR_CR_PDDS;
    PWR->CR |= PWR_CR_CWUF;
    __WFI();
}
#endif

#if 1 // ======================== Flash load/save ==============================
void LoadFromFlash() {
    memcpy(&Seq, &FlashSeq, LED_SEQ_SZ);
}

void FLASH_Unlock() {
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
}
void FLASH_Lock(void) { FLASH->CR |= CR_LOCK_Set; }
void FLASH_ClearFlag(uint32_t FLASH_FLAG) { FLASH->SR = FLASH_FLAG; }

uint8_t FLASH_GetBank1Status(void) {
    if(FLASH->SR & FLASH_SR_BSY) return BUSY;
    else if(FLASH->SR & FLASH_SR_PGERR) return FAILURE;
    else if(FLASH->SR & FLASH_SR_WRPRTERR) return FAILURE;
    else return OK;
}
uint8_t FLASH_WaitForLastOperation(uint32_t Timeout) {
    uint8_t status = OK;
    // Wait for a Flash operation to complete or a TIMEOUT to occur
    do {
        status = FLASH_GetBank1Status();
        Timeout--;
    } while((status == BUSY) and (Timeout != 0x00));
    if(Timeout == 0x00) status = TIMEOUT;
    return status;
}

uint8_t FLASH_ErasePage(uint32_t PageAddress) {
    uint8_t status = FLASH_WaitForLastOperation(EraseTimeout);
    if(status == OK) {
        FLASH->CR |= CR_PER_Set;
        FLASH->AR = PageAddress;
        FLASH->CR |= CR_STRT_Set;
        // Wait for last operation to be completed
        status = FLASH_WaitForLastOperation(EraseTimeout);
        // Disable the PER Bit
        FLASH->CR &= CR_PER_Reset;
    }
    return status;
}

uint8_t FLASH_ProgramWord(uint32_t Address, uint32_t Data) {
    uint8_t status = FLASH_WaitForLastOperation(ProgramTimeout);
    if(status == OK) {
        FLASH->CR |= CR_PG_Set; // program the new first half word
        *(__IO uint16_t*)Address = (uint16_t)Data;
        /* Wait for last operation to be completed */
        status = FLASH_WaitForLastOperation(ProgramTimeout);
        if(status == OK) {
            // program the new second half word
            uint32_t tmp = Address + 2;
            *(__IO uint16_t*)tmp = Data >> 16;
            /* Wait for last operation to be completed */
            status = FLASH_WaitForLastOperation(ProgramTimeout);
            /* Disable the PG Bit */
            FLASH->CR &= CR_PG_Reset;
        }
        else FLASH->CR &= CR_PG_Reset;  // Disable the PG Bit
    }
    return status;
}

void SaveToFlash() {
    uint8_t status = OK;
    uint32_t FAddr = (uint32_t)&FlashSeq;
    FLASH_Unlock();
    // Erase flash
    FLASH_ClearFlag(FLASH_SR_EOP | FLASH_SR_PGERR | FLASH_SR_WRPRTERR);   // Clear all pending flags
    status = FLASH_ErasePage(FAddr);
    //Uart.Printf("  Flash erase %u: %u\r", i, FLASHStatus);
    if(status != OK) {
        Uart.Printf("  Flash erase error\r");
        return;
    }
    uint32_t *PRAM = (uint32_t*)&Seq;    // What to write
    uint32_t DataWordCount = (LED_SEQ_SZ + 3) / 4;
    for(uint32_t i=0; i<DataWordCount; i++) {
        status = FLASH_ProgramWord(FAddr, *PRAM);
        if(status != OK) {
            Uart.Printf("  Flash write error\r");
            return;
        }
        FAddr += 4;
        PRAM++;
    }
    FLASH_Lock();
    Uart.PrintfI("\r\nSaved");
}
#endif
