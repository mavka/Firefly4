/*
 * board.h
 *
 *  Created on: 12 сент. 2015 г.
 *      Author: Kreyl
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <inttypes.h>
#include "CRS_defins.h"

// ==== General ====
#define BOARD_NAME          "Firefly4"
// MCU type as defined in the ST header.
#define STM32F072xB

#define CRYSTAL_FREQ_HZ     12000000    // Freq of external crystal

#define SYS_TIM_CLK         (Clk.APBFreqHz * Clk.TimerClkMulti)

#define LED_CNT         3   // Number of RGB LEDs

#if 1 // ========================== GPIO =======================================
// UART
#define UART_GPIO       GPIOA
#define UART_TX_PIN     9
#define UART_RX_PIN     10
#define UART_AF         AF1

// LEDs
//#define LEDWS_GPIO      GPIOB
//#define LEDWS_PIN       1

// USB
#define USB_GPIO		GPIOA
#define USB_DM_PIN		11
#define USB_DP_PIN		12
#define SNS_5V_GPIO     GPIOB
#define SNS_5V_PIN      6

// LightSns
#define LTSNS_GPIO      GPIOA
#define LTSNS_PIN       1

// Button
#define BTN_GPIO        GPIOA
#define BTN_PIN         0

#define SNS_GPIO        GPIOA
#define SNS_PIN0        3
#define SNS_PIN1        4

#endif

#if 1 // ========================= Timer =======================================
//#define LEDWS_TMR       TIM3
//#define LEDWS_TMR_CH    4
//#define LEDWS_TMR_IRQ        TIM9_IRQn
//#define SAMPLING_TMR_IRQHandler STM32_TIM9_HANDLER
#endif

#if 1 // =========================== SPI =======================================
//#define SPI_ADC         SPI1

#endif

#if 1 // ========================== USART ======================================
#define UART            USART1
#define UART_TX_REG     UART->TDR
#define UART_RX_REG     UART->RDR
#endif

#if 1 // ========================= Inner ADC ===================================
#define ADC_REQUIRED        TRUE

// ADC channels
#define SNS_CHNL0 	        3
#define SNS_CHNL1 	        4

// Inner ADC channels.  Do not change.
#define ADC_TEMPERATUR_CHNL 16
#define ADC_VREFINT_CHNL    17
#define ADC_VBAT_CHNL       18

static const uint8_t AdcChannels[] = { SNS_CHNL0, SNS_CHNL1 };
#define ADC_CHANNEL_CNT     2   // Do not use countof(AdcChannels) as preprocessor does not know what is countof => cannot check
#define ADC_SAMPLE_TIME     ast55d5Cycles
#define ADC_SAMPLE_CNT      8   // How many times to measure every channel
#define ADC_SEQ_LEN         (ADC_SAMPLE_CNT * ADC_CHANNEL_CNT)

#if defined STM32F4XX
// Clock divider
#define ADC_CLK_DIVIDER     adcDiv2 // 24MHz / 2 = 12MHz
#define ADC_MAX_SEQ_LEN     16      // 1...16; Const, see ref man
#if (ADC_SEQ_LEN > ADC_MAX_SEQ_LEN) || (ADC_SEQ_LEN == 0)
#error "Wrong ADC channel count and sample count"
#endif
#endif // #if defined STM32F4XX
#endif

#if 1 // ========================== USB ========================================
#define USBDrv          USBD1   // USB driver to use

// CRS
#define CRS_PRESCALER   RCC_CRS_SYNC_DIV1
#define CRS_SOURCE      RCC_CRS_SYNC_SOURCE_USB
#define CRS_POLARITY    RCC_CRS_SYNC_POLARITY_RISING
#define CRS_RELOAD_VAL  ((48000000 / 1000) - 1) // Ftarget / Fsync - 1
#define CRS_ERROR_LIMIT 34
#define HSI48_CALIBRATN 32
#endif

#if 1 // =========================== DMA =======================================
#define STM32_DMA_REQUIRED  TRUE
// Uart
#define UART_DMA_TX     STM32_DMA1_STREAM4
#define UART_DMA_RX     STM32_DMA1_STREAM5
#define UART_DMA_CHNL   0   // Dummy

//#define LEDWS_DMA       STM32_DMA1_STREAM3

#if ADC_REQUIRED
/* DMA request mapped on this DMA channel only if the corresponding remapping bit is cleared in the SYSCFG_CFGR1
 * register. For more details, please refer to Section10.1.1: SYSCFG configuration register 1 (SYSCFG_CFGR1) on
 * page173 */
#define ADC_DMA         STM32_DMA1_STREAM1
#define ADC_DMA_MODE    STM32_DMA_CR_CHSEL(0) |   /* Ignored for F0xx */ \
                        DMA_PRIORITY_LOW | \
                        STM32_DMA_CR_MSIZE_HWORD | \
                        STM32_DMA_CR_PSIZE_HWORD | \
                        STM32_DMA_CR_MINC |       /* Memory pointer increase */ \
                        STM32_DMA_CR_DIR_P2M |    /* Direction is peripheral to memory */ \
                        STM32_DMA_CR_TCIE         /* Enable Transmission Complete IRQ */
#endif // ADC

#endif

#endif /* BOARD_H_ */
