/**
 * @file DHT22.h
 * @version v1.0
 * @date 05/10/2022
 * @author Facundo Colavitte
 *
 *  Driver documentation:
 *  	https://github.com/fcolavitte/CESE-PCSE.git
 */

#ifndef DHT22_INC_DHT22_H_
#define DHT22_INC_DHT22_H_

#include "stm32f429xx.h"
#include <stdbool.h>
#include <stdio.h>

/**
 * @brief Puertos admisibles para conectar DHT22
 */
#define PORT_A 1
#define PORT_B 2
#define PORT_C 3
#define PORT_D 4
#define PORT_E 5
#define PORT_F 6
#define PORT_G 7

/**
 * @brief Estructura para almacenar variables del sensor DHT22
 */
typedef struct {
	/** Número de pin del GPIO segun numeración dentro del Port*/
	uint16_t Pin;
	/** Puerto del pin que se conectará al DHT22*/
	uint8_t  Port;
	/** Estructura de datos recibidos del DHT22*/
	struct {
		/** Temperatura en °C. El DHT22 tiene una precisión de 0,1°C*/
		float 	 temp;
		/** Temperatura en formato String en grados centigrados*/
		uint8_t  temp_string[6];
		/** Humedad en %*/
		float 	 hum;
		/** Humedad en formato String*/
		uint8_t  hum_string[6];
		/** Primeros 4 bytes de la comunicación que contienen hum y temp*/
		uint32_t crude;				
		/** Byte de comprobación correspondiente al 5to byte de la comunicación*/
		uint8_t  validation;		
	} data;
	/** Última vez que se realizó una lectura en milisegundos*/
	uint32_t time_last_call;
	/** Estados del sensor DHT22*/
	enum{
		DHT_OK = 0,
		ERR_COMMUNICATION = 1,
		ERR_DISCONECT = 2,
		DHT_READING = 3
	} status;
} DHT22_sensor;

typedef bool bool_t;

/*--------------------------------------- PUBLIC FUNCTIONS ------------------------------------------------*/

/**
 * @brief	Inicializa las funciones de lectura del DHT22 en el GPIO asignado
 * @param	Número de GPIO del Port. Revisar HAL o datsheet para obtener el número de cada pin.
 * @param	Valores válidos PORT_A a PORT_G
 * @return	True si se inició correctamente. Sino regresa False
 */
bool_t DHT22_init(uint16_t _pin, uint8_t _port);

/**
 * @brief	Solicita el valor de temperatura
 * @return	Temperatura en grados celcius
 */
float DHT22_get_temp(void);

/**
 * @brief	Solicita el valor de temperatura
 * @return	Temperatura en grados celcius en formato String
 */
uint8_t * DHT22_get_temp_string(void);

/**
 * @brief	Solicita el valor de humedad
 * @return	Humedad
 */
float DHT22_get_hum(void);

/**
 * @brief	Solicita el valor de humedad
 * @return	Humedad en formato String
 */
uint8_t * DHT22_get_hum_string(void);

/**
 * @brief	Delay en milisegundos
 * @param	Tiempo en ms a esperar
 */
void delay_ms(uint32_t delay);

#endif /* DHT22_INC_DHT22_H_ */
