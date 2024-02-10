/**
  ******************************************************************************
  * @file    main.c
  * @author  Dmitriy Stadnyuk
  * @version V1.0.0
  * @date    10.11.2023
  * @brief   Тестовое задание 
  *			 Микроконтроллер: STM32f030f4p6 
  *
  *			 Реализованы отправка и приём данных по USART.
  *			 Разработан простейший обработчик команд.
  *
  ******************************************************************************
  */

#include "main.h"

/**
  * @brief  Обработчик прерывания от USART1
  * @param  None
  * @retval None
  */

void USART1_IRQHandler(void)
{
	if ((USART1->ISR & USART_ISR_RXNE)!=0)		//Прерывание по приёму данных
	{
		Rx=(char)USART1->RDR;
		ComReceived=true;
	}
}

/**
  * @brief  Подпрограмма обработчика прерывания
  *			по переполнению тайемера TIM3
  * @param  None
  * @retval None
  */

void TIM3_IRQHandler(void)
{
	TIM3->SR &= ~TIM_SR_UIF;					//Сброс флага переполнения
	if (ComReceived)
							ExecuteCommand();	//Обрабатываем принятый символ
	 GPIOA->ODR ^= GPIO_ODR_9; //Мигаем пином 9 порта A
		  GPIOA->ODR ^= GPIO_ODR_10; //Мигаем пином 10 порта A
}

/**
  * @brief  Инициализация портов ввода-вывода
  * @param  None
  * @retval None
  */


void Init_GPIO(void){

	//Диоды подключены к выходам PA9 и PA10
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; //Включаем тактирование на порте A

			    GPIOA->MODER |= (1 << (9 * 2)); // Настраиваем порт 9 на output
			    GPIOA->MODER |= (1 << (10 * 2)); // Настраиваем порт 10 на output
}

/**
  * @brief  Подпрограмма обработчика прерывания
  *			по переполнению тайемера TIM3
  * @param  None
  * @retval None
  */

void initTIM3(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;			//Включить тактирование TIM3
	TIM3->PSC = 8000-1;						//Предделитель частоты (8Гц/8000 = 1кГц)
	TIM3->ARR = 1000-1;							//Модуль счёта таймера (1кГц/1000 = 1с)
	TIM3->DIER |= TIM_DIER_UIE;					//Разрешить прерывание по переполнению таймера
	TIM3->CR1 |= TIM_CR1_CEN;					//Включить таймер

	NVIC_EnableIRQ(TIM3_IRQn);					//Рарзрешить прерывание от TIM3
	NVIC_SetPriority(TIM3_IRQn, 1);				//Выставляем приоритет
}


/**
  * @brief  Инициализация USART1
  * @param  None
  * @retval None
  */

void initUSART1(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

		    // Включение тактирования USART1
		    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

		    // Настройка пинов PA2 (TX) и PA3 (RX) в режим альтернативной функции
		    GPIOA->MODER |= GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1;

		    // Настройка альтернативной функции для пинов PA2 и PA3
		    GPIOA->AFR[0] |= (1 << GPIO_AFRL_AFRL2_Pos) | (1 << GPIO_AFRL_AFRL3_Pos);

		    // Настройка скорости передачи данных
		    USART1->BRR = 8000000/9600; // Скорость 9600 бит/с при частоте шины 8 МГц

		    // Включение передатчика и приемника USART1 и прерываний при приёме данных
		    USART1->CR1 |= USART_CR1_UE | USART_CR1_RE | USART_CR1_TE | USART_CR1_RXNEIE;

		    USART1->CR3 = USART_CR3_DMAT | USART_CR3_DMAR; // Разрешаем использование DMA для передачи и приема
		        USART1->CR1 |= USART_CR1_UE; // Включаем USART1

		        // Включаем тактирование DMA1
		        RCC->AHBENR |= RCC_AHBENR_DMA1EN;

		    NVIC_EnableIRQ(USART1_IRQn); //Разрешаем прерывания в NVIC



}

/**
  * @brief  Инициализация DMA для передачи данных по USART1
  * @param  None
  * @retval None
  */

void initDMA(void)
{
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;				//разрешить такт. DMA

	DMA1_Channel2->CPAR = (uint32_t)&USART1->TDR;	//указатель на регистр данных для передачи USART1

	DMA1_Channel2->CCR = 0;
	DMA1_Channel2->CCR |= DMA_CCR_DIR;				//направление - из памяти в устройство
	DMA1_Channel2->CCR |= DMA_CCR_MINC;			//инкремент указателя в памяти
	USART1->CR3 |= USART_CR3_DMAT;					//настроить USART1 на работу с DMA
}

/**
  * @brief  Передача строки по USART1 с помощью DMA
  * @param  str - Указатель на строку
  * @retval None
  */

void TransmitWithDMA(char *str)
{
		strcat(str,"\r\n");									//добавляем символ конца строки

	DMA1_Channel2->CCR &= ~DMA_CCR_EN;						//выключаем DMA
	DMA1_Channel2->CMAR = (uint32_t)str;					//указатель на строку, которую нужно передать
	DMA1_Channel2->CNDTR = strlen(str);						//длина строки
	DMA1->IFCR |= DMA_IFCR_CTCIF1;							//сброс флага окончания обмена
	DMA1_Channel2->CCR |= DMA_CCR_EN;    					//включить DMA
}

/**
  * @brief  Обработчик команд
  * @param  None
  * @retval None
  */

void ExecuteCommand(void)
{
	memset(TxBuffer,0,sizeof(TxBuffer));					//Очистка буфера передачи

	/* Обработчик команд */
	switch(Rx){
	case '1':

		TIM3->ARR=100-1;									//Меняем частоту мигания
		TIM3->CNT=0;
			strcpy(TxBuffer,"Toggle Frequency set to 10Hz");
			break;

	case 'S':
		TIM3->CR1 |= TIM_CR1_CEN;
		strcpy(TxBuffer, "Toggle Continue");				//Возобновляем работу таймера
		break;

	case 'P':
		TIM3->CR1 &= ~TIM_CR1_CEN;							//Останавливаем работу таймера
		TIM3->EGR |= TIM_EGR_UG;
		strcpy(TxBuffer, "Toggle stop");
		break;

	case '2':
			TIM3->ARR = 2000-1;
			TIM3->CNT = 0;
			strcpy(TxBuffer, "Toggle frequency set to 0.5 Hz");
			break;

	default:
		strcpy(TxBuffer, "Unknown command");
	break;
	}

	TransmitWithDMA(TxBuffer);								//Передаём строку из памяти в устройство
	ComReceived = false;									//Обнуляем флаг приёма строки
}

/**
  * @brief  Основная программа
  * @param  None
  * @retval None
  */

int main(void)
{

	Init_GPIO();
	initTIM3();
	initUSART1();
	initDMA();

	while(true)
	{

	};
}
