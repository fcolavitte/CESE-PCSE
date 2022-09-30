/*
 * DHT22.c
 *
 *  Created on: 27 sep. 2022
 *      Author: Facundo Colavitte
 */
#include "DHT22.h"

/*Functions ---------------------------------------------------------------------*/
extern uint32_t tiempo_actual(void);
extern void GPIO_set_config(uint8_t GPIO_port, uint8_t GPIO_num);
extern void delay_ms(uint32_t delay);
extern void GPIO_write(uint8_t GPIO_port, uint8_t GPIO_num, bool_t GPIO_state);
static void tomar_lectura(DHT22_sensor * DHT22_struct);
static void decodificar(DHT22_sensor * DHT22_struct);


/*
 * @brief	Verifica si el pin del DHT es válido en la placa
 * @param	Puntero a estructura del DHT22
 * @return	None
 */
static bool_t is_pin(DHT22_sensor * DHT22_struct){
	return 1;
}

/*
 * @brief	Inicializa el DHT22 con los valores de la estructura cargada
 * @param	Puntero a estructura del DHT22
 * @return	None
 */
void DHT22_init(DHT22_sensor * DHT22_struct){
	if(is_pin(DHT22_struct)){	/*Si el PIN es válido*/
		DHT22_struct->data.temp = 0;
		DHT22_struct->data.temp_string[0] = '\0';
		DHT22_struct->data.hum = 0;
		DHT22_struct->data.hum_string[0] = '\0';
		DHT22_struct->time_last_call = 0;
		GPIO_set_config(DHT22_struct->Port, DHT22_struct->Pin);
		GPIO_write(DHT22_struct->Port, DHT22_struct->Pin, 1);
	}
}

/*
 * @brief	Solicita el valor de temperatura
 * @param	Puntero a estructura del DHT22
 * @return	Temperatura en grados celcius
 */
float DHT22_get_temp(DHT22_sensor * DHT22_struct){
	float temp=0;
	if(is_pin(DHT22_struct)){
		if(tiempo_actual() - DHT22_struct->time_last_call > 2000){
			tomar_lectura(DHT22_struct);
		}
		temp = DHT22_struct->data.temp;
	}
	return temp;
}

/*
 * @brief	Solicita el valor de temperatura
 * @param	Puntero a estructura del DHT22
 * @return	Temperatura en grados celcius en String
 */
uint8_t * DHT22_get_temp_string(DHT22_sensor * DHT22_struct){
	DHT22_get_temp(DHT22_struct);
	return DHT22_struct->data.temp_string;
}

/*
 * @brief	Solicita el valor de humedad
 * @param	Puntero a estructura del DHT22
 * @return	Humedad
 */
float DHT22_get_hum(DHT22_sensor * DHT22_struct){
	float hum=0;
	if(is_pin(DHT22_struct)){
		if(tiempo_actual() - DHT22_struct->time_last_call > 2000){
			tomar_lectura(DHT22_struct);
		}
		hum = DHT22_struct->data.temp;
	}
	return hum;
}

/*
 * @brief	Solicita el valor de humedad
 * @param	Puntero a estructura del DHT22
 * @return	Humedad en String
 */
uint8_t * DHT22_get_hum_string(DHT22_sensor * DHT22_struct){
	DHT22_get_hum(DHT22_struct);
	return DHT22_struct->data.hum_string;
}

/*
 * @brief	Solicita valores al DHT22 y guarda los tiempos en el array T_Array[85]
 * @param	Puntero a estructura del DHT22
 * @return	None
 */
static void tomar_lectura(DHT22_sensor * DHT22_struct){
	if(is_pin(DHT22_struct)){
		DHT22_struct->time_last_call = tiempo_actual();
		GPIO_write(DHT22_struct->Port, DHT22_struct->Pin, 0);
		delay_ms(1);	/*Delay 1ms*/
		GPIO_write(DHT22_struct->Port, DHT22_struct->Pin, 1);
		/*
		 *
		 * COMPLETAR protocolo de comunicación
		 *
		 */

		decodificar(DHT22_struct);
	}
}

/*
 * @brief	Traduce los tiempos guardados en el array T_Array[85] y guarda los datos en la sub-estructura data
 * @param	Puntero a estructura del DHT22
 * @return	None
 */
static void decodificar(DHT22_sensor * DHT22_struct){

	/*
	*
	* COMPLETAR protocolo de comunicación
	*
	*/

}
