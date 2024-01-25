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

#define TX_BUFF_SIZE	100

char TxBuffer[TX_BUFF_SIZE];					//Буфер передачи USART
bool ComReceived;								//Флаг приёма строки данных
char Rx;										//Входные данные

void USART1_IRQHandler(void);
void TIM3_IRQHandler(void);
void Init_GPIO(void);
void initTIM3(void);
void initUSART1(void);
void initDMA(void);
void TransmitWithDMA(char *str);
void ExecuteCommand(void);


#endif /* MAIN_H_ */

