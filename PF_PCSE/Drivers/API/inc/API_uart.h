/*
 * API_uart.h
 *
 *  Created on: 21 sep. 2022
 *      Author: Facu
 */

#ifndef API_INC_API_UART_H_
#define API_INC_API_UART_H_

#include "stm32f4xx_hal.h"  		/* <- HAL include */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "main.h"


#define UART_LONG_MAX 100	/*Longitud máxima de cadena de caracteres a enviar por UART*/

/* ----------- Parámetros para comunicación UART --------------------*/
static const uint32_t BAUDRATE		= 9600;
static const uint32_t WORD_LENGTH	= UART_WORDLENGTH_8B;
static const uint32_t STOP_BITS		= UART_STOPBITS_1;
static const uint32_t PARITY		= UART_PARITY_ODD;
static const uint32_t HW_FLOW_CTL	= UART_HWCONTROL_NONE;



typedef bool bool_t;	  // stdbool.h


/*------------------------ Funciones ---------------------------------*/
bool_t uartInit(void);
void uartSendString(uint8_t * pstring);
void uartSendStringSize(uint8_t * pstring, uint16_t size);
void uartReceiveStringSize(uint8_t * pstring, uint16_t size);

#endif /* API_INC_API_UART_H_ */
