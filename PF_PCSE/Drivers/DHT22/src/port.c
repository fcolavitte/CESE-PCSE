/*
 * port.c
 *
 *  Created on: 27 sep. 2022
 *      Author: Facu
 */
#include <stdbool.h>
#include <stdio.h>

typedef bool bool_t;

enum GPIO_MODE{
	INPUT,
	OUTPUT
};

enum GPIO_CONFIG{
	push_pull,
	open_drain
};

//Configurar como open-drain
void GPIO_set_config(uint8_t GPIO_num, enum GPIO_CONFIG GPIO_config){

}

//set pin mode (entrada o salida)
void GPIO_set_mode(uint8_t GPIO_num, enum GPIO_MODE GPIO_mode){

}


//poner pin en bajo - liberar pin (open)
void GPIO_write(uint8_t GPIO_num, bool_t GPIO_state){

}

//leer pin
bool_t GPIO_read(uint8_t GPIO_num){

}
