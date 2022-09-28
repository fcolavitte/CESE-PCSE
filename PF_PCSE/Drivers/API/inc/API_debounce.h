/*
 * API_debounce.h
 *
 *  Created on: 13 sep. 2022
 *      Author: Facundo
 */

#ifndef API_INC_API_DEBOUNCE_H_
#define API_INC_API_DEBOUNCE_H_

//--------- includes & defines -----------
#include "stm32f4xx_hal.h"  		/* <- HAL include */
#include "stm32f4xx_nucleo_144.h" 	/* <- BSP include */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "API_delay.h"

#define T_ANTIREBOTE 40	//Tiempo de anti-rebote en ms

//--------- Functions -----------
void debounceFSM_init();		// debe cargar el estado inicial
void debounceFSM_update();		// debe leer las entradas, resolver la lógica de
								// transición de estados y actualizar las salidas
bool_t readKey();
bool_t readFlancoAscendente();
bool_t readFlancoDescendente();

#endif /* API_INC_API_DEBOUNCE_H_ */
