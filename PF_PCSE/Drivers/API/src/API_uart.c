/*
 * API_uart.c
 *
 *  Created on: 21 sep. 2022
 *      Author: Facu
 */

/* Includes ------------------------------------------------------------------*/
#include "API_uart.h"


#include <stdio.h>


// UART handler declaration
static UART_HandleTypeDef UartHandle;

//Global variable
static uint8_t string_uint[11];

//Private function
static uint8_t *uint_to_string(uint32_t numero);
static void send_init_data(void);


/*
* @brief  Inicializa la UART. Envía parametros de inicialización por UART
* @param  None
* @retval true:UART se inicializó correctamente, false:falla al inicializar UART
*/
bool_t uartInit(void){

	UartHandle.Instance        = USARTx;

	UartHandle.Init.BaudRate   		= BAUDRATE;
	UartHandle.Init.WordLength 		= WORD_LENGTH;
	UartHandle.Init.StopBits   		= STOP_BITS;
	UartHandle.Init.Parity     		= PARITY;
	UartHandle.Init.HwFlowCtl  		= HW_FLOW_CTL;
	UartHandle.Init.Mode       		= UART_MODE_TX_RX;
	UartHandle.Init.OverSampling 	= 16;

	/*Inicializar UART --------------------------------------------*/
	if (HAL_UART_Init(&UartHandle) != HAL_OK) {
	  return 0;
	}
	send_init_data();
	return 1;
}


/*
* @brief  Envia parámetros de configuración por UART
* @param  None
* @retval None
*/
void send_init_data(void){
	uartSendString("\r\n>> UART inicializado correctamente\r\n");
	uartSendString(">> Parametros de configuracion UART:\r\n");
	uartSendString("    > BAUDRATE:  ");
	uartSendString(uint_to_string(BAUDRATE));
	uartSendString("\r\n    > WORD_LENGTH:  ");
	if(WORD_LENGTH==UART_WORDLENGTH_8B){
		uartSendString("8bits\r\n");
	}else{
		uartSendString("9bits\r\n");
	}
	uartSendString("    > STOP_BITS:  ");
	if(STOP_BITS==UART_STOPBITS_1){
		uartSendString("1\r\n");
	}else{
		uartSendString("2\r\n");
	}
	uartSendString("    > PARITY:  ");
	if(PARITY==UART_PARITY_ODD){
		uartSendString("ODD\r\n");
	}else if(PARITY==UART_PARITY_EVEN){
		uartSendString("EVEN\r\n");
	}else {
		uartSendString("NONE\r\n");
	}
	uartSendString("    > HW_FLOW_CTL:  ");
	switch(HW_FLOW_CTL){
		case UART_HWCONTROL_NONE:
			uartSendString("NONE\r\n");
		break;
		case UART_HWCONTROL_RTS:
			uartSendString("RTS\r\n");
		break;
		case UART_HWCONTROL_CTS:
			uartSendString("CTS\r\n");
		break;
		case UART_HWCONTROL_RTS_CTS:
			uartSendString("RTS-CTS\r\n");
		break;
		default:
			uartSendString("Error en reconocer HW_Control");
		break;
	}
	uartSendString("    > UART_MODE:  UART_MODE_TX_RX\r\n");
	uartSendString("    > OVER_SAMPLING:  16\r\n");
}

/*
* @brief  Envía un String por UART. Longitud máxima del array definida por UART_LONG_MAX (incluyendo el '\0')
* @param  Puntero a cadena de caracteres a enviar
* @retval None
*/
void uartSendString(uint8_t * pstring){
	if(pstring!=0){
		uint16_t size=0;
		bool_t is_string=0;
		for(int i=0;i<UART_LONG_MAX;i++){
			if(pstring[i]=='\0'){
				is_string=1;
				size=i;
				i=UART_LONG_MAX;
			}
		}
		if(is_string==0){	//Convertir en String y recortar
			pstring[UART_LONG_MAX-1]='\0';
			size=UART_LONG_MAX-1;
		}

		uartSendStringSize(pstring, size);
	} else {
		/* Control de Error*/
	}
}


/*
* @brief  Envía String por UART. Longitud máxima del array definida por UART_LONG_MAX (incluyendo el '\0')
* @param  puntero al string a enviar
* @param  cantidad de caracteres a enviar
* @retval None
*/
void uartSendStringSize(uint8_t * pstring, uint16_t size){
	if(pstring!=0 && size>0){
		HAL_UART_Transmit(&UartHandle, pstring, size, 0xffff);
	} else {
		/*Control de Errror*/
	}
}



/*
* @brief  Lee puerto UART
* @param  puntero al string donde se recibirá la lectura
* @param  cantidad de caracteres a recibir
* @retval None
*/
void uartReceiveStringSize(uint8_t * pstring, uint16_t size){
	if(pstring!=0 && size>0){
		HAL_UART_Receive(&UartHandle, pstring, size, 0xffff);
	} else {
		/*Control de Error*/
	}
}


/*
* @brief  Función para convertir uint a string
* @param  Número a convertir
* @retval Número en forma de String (con '\0' al final)
*/
static uint8_t *uint_to_string(uint32_t numero){
	uint32_t divisor;
	uint8_t Exp_divisor;
	uint32_t digito;
	uint8_t i = 0;	/*Posición en el string*/
	bool_t  start_num = 0;	/*Var aux para eliminar 0's a izquierda del dígito significativo*/
	for(uint8_t posicion=0 ; posicion<10 ; posicion++){
		Exp_divisor = 9 - posicion;

		/*divisor=10^Exp_divisor*/
		divisor = 1;
		for(uint8_t j=0 ; j<Exp_divisor ; j++){
			divisor = divisor * 10;
		}

		digito = (numero/divisor)%10;
		if(digito!=0||start_num!=0){
			string_uint[i++] = digito + '0';
			start_num = 1;
		}
	}
	string_uint[i] = '\0';
	return string_uint;
}

