/*
 * DHT22.h
 *
 *  Created on: 27 sep. 2022
 *      Author: Facundo Colavitte
 */

#ifndef DHT22_INC_DHT22_H_
#define DHT22_INC_DHT22_H_

#include "stm32f429xx.h"
#include <stdbool.h>
#include <stdio.h>


/*----------- Estructura para almacenar los datos de cada DHT22 -------------------------------------------*/
typedef struct {
	uint16_t Pin;
	GPIO_TypeDef * Port;
	uint16_t T_Array[43];/*Se requieren 83 valores de tiempo entre flancos para decodificar*/
	struct {
		float temp;
		uint8_t temp_string[6];
		float hum;
		uint8_t hum_string[6];
		uint32_t crude;			/*Primeros 8 bytes que contienen hum y temp*/
		uint8_t validation;		/*Byte de comprobación*/
	} data;
	uint32_t time_last_call;	/*Ultima vez que se llamó en ms*/
	enum{
		DHT_OK = 0,
		ERR_COMMUNICATION = 1,
		ERR_DISCONECT = 2,
		DHT_READING = 3
	} status;
} DHT22_sensor;

typedef bool bool_t;

/*--------------------------------------- PUBLIC FUNCTIONS ------------------------------------------------*/

/*
 * @brief	Inicializa el DHT22 con los valores de la estructura cargada
 * @param	Puntero a estructura del DHT22
 * @return	None
 */
void DHT22_init(DHT22_sensor * DHT22_struct);

/*
 * @brief	Solicita el valor de temperatura
 * @param	Puntero a estructura del DHT22
 * @return	Temperatura en grados celcius
 */
float DHT22_get_temp(DHT22_sensor * DHT22_struct);

/*
 * @brief	Solicita el valor de temperatura
 * @param	Puntero a estructura del DHT22
 * @return	Temperatura en grados celcius en String
 */
uint8_t * DHT22_get_temp_string(DHT22_sensor * DHT22_struct);

/*
 * @brief	Solicita el valor de humedad
 * @param	Puntero a estructura del DHT22
 * @return	Humedad
 */
float DHT22_get_hum(DHT22_sensor * DHT22_struct);

/*
 * @brief	Solicita el valor de humedad
 * @param	Puntero a estructura del DHT22
 * @return	Humedad en String
 */
uint8_t * DHT22_get_hum_string(DHT22_sensor * DHT22_struct);

#endif /* DHT22_INC_DHT22_H_ */
