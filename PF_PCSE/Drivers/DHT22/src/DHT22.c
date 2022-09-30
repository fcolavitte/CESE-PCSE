/*
 * DHT22.c
 *
 *  Created on: 27 sep. 2022
 *      Author: Facundo Colavitte
 */
#include "DHT22.h"

static const uint16_t T_Low = 50;	//us
static const uint16_t T_1_High = 70;	//us
//static const uint16_t T_0_High = 25;	//us
static const uint16_t T_1 = T_Low + T_1_High;	//us
//static const uint16_t T_0 = T_Low + T_0_High;	//us



/*Functions ---------------------------------------------------------------------*/
extern uint32_t tiempo_actual(void);
extern void GPIO_set_config(uint8_t GPIO_port, uint8_t GPIO_num);
extern void delay_ms(uint32_t delay);
extern void GPIO_write(uint8_t GPIO_port, uint8_t GPIO_num, bool_t GPIO_state);
static void tomar_lectura(DHT22_sensor * DHT22_struct);
static void decodificar(DHT22_sensor * DHT22_struct);
static uint8_t *uint_to_string(uint32_t numero);

/*Variables Globales ------------------------------------------------------------*/
extern DHT22_sensor _DHT22;
static uint8_t string_uint[11];


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

		/*
		 *
		 * Ver de hacer por interrupción
		 *
		 */

		DHT22_struct->time_last_call = tiempo_actual();
		GPIO_write(DHT22_struct->Port, DHT22_struct->Pin, 0);
		delay_ms(1);	/*Delay 1ms*/
		GPIO_write(DHT22_struct->Port, DHT22_struct->Pin, 1);
		//Activar Timer y poner a cero
		delay_ms(7);	/*Delay 1ms para que se realice la transmisión*/
		//Apagar Timer
		for(uint8_t i=0;i<85;i++){
			DHT22_struct->T_Array[i] = _DHT22.T_Array[i];
		}
		decodificar(DHT22_struct);
	}
}

/*
 * @brief	Traduce los tiempos guardados en el array T_Array[85] y guarda los datos en la sub-estructura data
 * @param	Puntero a estructura del DHT22
 * @return	None
 */
static void decodificar(DHT22_sensor * DHT22_struct){
	uint32_t cadena_dato=0;
	uint8_t inicio_bit_0=0;
	for(uint8_t i=inicio_bit_0;i<85;i++){/*Ver como hago la interrupción, puede que no arranque desde cero*/
		if(DHT22_struct->T_Array[i-inicio_bit_0]>(T_1-10)&&DHT22_struct->T_Array[i-inicio_bit_0]<(T_1+10)){
			cadena_dato += 1<<(31-(i-inicio_bit_0));
		}
	}
	float humidity = (float)(cadena_dato/(1<<16));
	humidity = humidity/10;
	float temperature = (float)(cadena_dato%(1<<15));
	temperature = temperature/10;
	DHT22_struct->data.hum = humidity;
	DHT22_struct->data.temp = temperature;

	uint8_t * data_string = uint_to_string(cadena_dato/(1<<16));
	for(uint8_t i=0;i<6;i++){
		DHT22_struct->data.hum_string[i]=data_string[i];
	}
	DHT22_struct->data.hum_string[6]='\0';

	data_string = uint_to_string(cadena_dato%(1<<15));
	for(uint8_t i=0;i<6;i++){
		DHT22_struct->data.temp_string[i]=data_string[i];
	}
	DHT22_struct->data.temp_string[6]='\0';
}


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
