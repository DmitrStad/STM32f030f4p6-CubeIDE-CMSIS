/*
 * main.h
 *
 *  Created on: Nov 29, 2023
 *      Author: User
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "stm32f0xx.h"
#include "stm32f030x6.h"
#include "system_stm32f0xx.h"
#include "stdbool.h"
#include "string.h"
#include "stdio.h"

#define USE_DMA

/* Размеры буферов приёма и передачи */
#define	RX_BUFF_SIZE	256
#define TX_BUFF_SIZE	256

void ExecuteCommand(void);

#define DELAY_VALUE 1000

void delay(volatile uint32_t time);

#define LED1_ON() GPIOA->ODR |= (1 << 9); // Включаем диод
#define LED1_OFF() GPIOA->ODR &= ~(1 << 9); //Выключаеи диод
#define LED1_TOGGLE GPIOA->ODR ^= GPIO_ODR_9; //Меняем состояние диода на противоположное

#define LED2_ON() GPIOA->BSRR =GPIO_BSRR_BS_10; //Включаем диод
#define LED2_OFF() GPIOA->BSRR =GPIO_BSRR_BR_10; //Выключаеи диод
#define LED2_TOGGLE GPIOA->ODR ^= GPIO_ODR_10; //Меняем состояние диода на противоположное

#endif /* MAIN_H_ */

