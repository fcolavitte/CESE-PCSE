/*
 * API_delay.c
 *
 *  Created on: 7 sep. 2022
 *      Author: Facundo
 */

/* Includes ------------------------------------------------------------------*/
#include "API_delay.h"


/*
 * @brief	Inicializa una estructura delay
 * @param	Puntero a estructura delay
 * @param	Duración del retardo no bloqueante en ms
 * @return	None
 */
void delayInit(delay_t * delay, tick_t duration) {
	if(delay != 0 && duration > 0 && duration < MAX_DELAY){	//Comprobación de parámetros de entrada
		delay->startTime=HAL_GetTick();
		delay->duration=duration;
		delay->running=0;//false
	} else {
		//Control de error
	}

}


/*
 * @brief	Actualización y lectura del estado del delay. Reinicia el mismo si este ya finalizó
 * @param	Puntero a estructura delay
 * @return	True si terminó retardo, sino False
 */
bool_t delayRead(delay_t * delay) {
	if(delay!=0) {	//Comprobación de parámetro de entrada
		if(delay->running==0) {
			if(HAL_GetTick() - delay->startTime>delay->duration){
				delay->running=1;//true
			}
		} else {
			delay->running=0;
			delay->startTime=HAL_GetTick();
		}
		return delay->running;
	} else {
		//Control de error
		return 0;
	}
}


/*
 * @brief	Cambio del valor de retardo del delay
 * @param	Puntero a estructura delay
 * @param	Nueva duración del retardo no bloqueante en ms
 * @return	None
 */
void delayWrite(delay_t * delay, tick_t duration) {
	if(delay != NULL && duration > 0 && duration < MAX_DELAY) {
		delay->duration=duration;
	} else {
		//Control de error
	}

}
