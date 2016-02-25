/*
 * board.h
 *
 *  Created on: 12 сент. 2015 г.
 *      Author: Kreyl
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <inttypes.h>

// ==== General ====
#define BOARD_NAME          "MicUsb"
// MCU type as defined in the ST header.
#define STM32L1XX_MDP

#define SYS_TIM_CLK         (Clk.APB1FreqHz)

// USB
//#define USBDrv      	USBD1   // USB driver to use

#if 1 // ========================== GPIO =======================================
// UART
#define UART_GPIO       GPIOA
#define UART_TX_PIN     9
#define UART_RX_PIN     10
#define UART_AF         AF7

#define LED0_GPIO       GPIOB
#define LED0_PIN        5
#define LED1_GPIO       GPIOB
#define LED1_PIN        0
#define LED2_GPIO       GPIOB
#define LED2_PIN        4
#endif

#if 1 // ========================== USART ======================================
#define UART            USART1
#define UART_TX_REG     UART->DR
#define UART_RX_REG     UART->DR
#endif

#if 1 // =========================== DMA =======================================
//#define STM32_DMA_REQUIRED  TRUE
// Uart
#define UART_DMA_TX     STM32_DMA1_STREAM4
#define UART_DMA_RX     STM32_DMA1_STREAM5
#define UART_DMA_CHNL   0   // Dummy
#endif

#endif /* BOARD_H_ */
