/*
 * port.c
 *
 *  Created on: 27 sep. 2022
 *      Author: Facu
 */
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdbool.h>
#include <stdio.h>

#define PIN_DHT22		GPIO_PIN_2 	/*Número de Pin del puerto al que se conectó DHT22. De 0 a 15*/
#define PORT_PIN_DHT22	GPIOA		/*Puerto al que pertenece el pin que conecta con el DHT22*/
typedef bool bool_t;






//Configurar GPIO para comunicación con DHT22
void GPIO_set_config(uint8_t GPIO_num){

	//Preparar configuración PIN
	GPIO_InitTypeDef PIN_DHT22_config={
			.Pin = PIN_DHT22,
			.Mode = MODE_OUTPUT | OUTPUT_OD | EXTI_IT | TRIGGER_RISING | TRIGGER_FALLING,
			/*https://www.st.com/resource/en/reference_manual/dm00093941-stm32f334xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
			 *Página 145. Modo OUTPUT tiene el registro input conectado a I/O PIN
			* OUTPUT_OD: Open-Drain
			* EXIT_IT habilita interrupciones
			* TRIGGER_RISING Interrupción por flanco ascendente
			* TRIGGER_FALLING Interrupción por flanco descendente
			*/
			.Pull = GPIO_PULLUP,      /*Specifies the Pull-up or Pull-Down resistence, GPIO_NOPULL, GPIO_PULLUP, GPIO_PULLDOWN*/
			.Speed = GPIO_SPEED_FREQ_MEDIUM
	};


	HAL_GPIO_Init(PORT_PIN_DHT22, &PIN_DHT22_config);
}




//poner pin en bajo - liberar pin (open)
void GPIO_write(uint8_t GPIO_num, bool_t GPIO_state){
	if(GPIO_num<15){
		uint16_t _GPIO_num = 2^(GPIO_num);
		if(GPIO_state) {
			HAL_GPIO_WritePin(PORT_PIN_DHT22, _GPIO_num, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(PORT_PIN_DHT22, _GPIO_num, GPIO_PIN_RESET);
		}
	} else {
		/*Error en el número de PIN*/
	}
}

//leer pin
bool_t GPIO_read(uint8_t GPIO_num){
	if(GPIO_num<15){
		return HAL_GPIO_ReadPin(PORT_PIN_DHT22, PIN_DHT22);
	} else {
		/*Error en el número de PIN*/
		return 0;
	}
}


//void  HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin);
//__weak void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
/***************************************
 Características:
   Hal_gpio_exti_irqhandler: la bandera de solicitud de interrupción se borra y entra en la función de devolución de llamada
   Hal_gpio_exti_callback: función de servicio de interrupción
   Exti Line Detección de devolución de llamada.
 __weak: __weak Función de modificador, los usuarios pueden redefinir una función en el archivo de usuario, cuando se compila el compilador final, se seleccionará la función definida por el usuario. Si el usuario no redefinir esta función, entonces el compilador realizará la función de Declare __weak y El compilador no informará un error.
****************************************/
