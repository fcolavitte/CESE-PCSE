/*
 * DHT22.c
 *
 *  Created on: 27 sep. 2022
 *      Author: Facundo Colavitte
 *
 *  Driver documentation:
 *  	https://github.com/fcolavitte/CESE-PCSE.git
 */
#include "DHT22.h"


/*Extern functions -------------------------------------------------------------*/
extern uint32_t tiempo_actual(void);
extern void GPIO_set_config(uint8_t GPIO_port, uint8_t GPIO_num);
extern void port_delay_ms(uint32_t delay);
extern void GPIO_write(uint8_t GPIO_port, uint8_t GPIO_num, bool_t GPIO_state);
extern void reset_timer();
extern bool_t is_pin(DHT22_sensor * DHT22_struct);


/*Functions --------------------------------------------------------------------*/
static void decodificar(void);
uint8_t *uint_to_string(uint32_t numero);


/*Variables Globales ------------------------------------------------------------*/
DHT22_sensor _DHT22;
static uint8_t string_uint[11];


/*
 * @brief	Inicializa el DHT22 con los valores de la estructura cargada
 * @param	Número de PIN del port. de 0x0000 a 0xffff, un único bit en 1.
 * @return	None
 */
bool_t DHT22_init(uint16_t _pin, uint8_t _port){
	if(is_pin(_pin)){

		_DHT22.Pin=_pin;
		_DHT22.Port=_port;

		_DHT22.data.temp = 0;
		_DHT22.data.temp_string[0] = '\0';
		_DHT22.data.hum  = 0;
		_DHT22.data.hum_string[0]  = '\0';
		_DHT22.data.crude = 0;
		_DHT22.data.validation = 0;
		_DHT22.time_last_call = 0;
		_DHT22.status = DHT_OK;

		GPIO_set_config(_DHT22.Port, _DHT22.Pin);

		GPIO_write(_DHT22.Port, _DHT22.Pin, 1);

		return 1;
	}
	return 0;
}


/*
 * @brief	Solicita el valor de temperatura
 * @return	Temperatura en grados celcius
 */
float DHT22_get_temp(void){
	if(_DHT22.status == DHT_OK){
		decodificar();
		return _DHT22.data.temp;
	} else {
		return 0;
	}
}


/*
 * @brief	Solicita el valor de temperatura
 * @return	Temperatura en grados celcius en formato String
 */
uint8_t * DHT22_get_temp_string(void){
	if(_DHT22.status == DHT_OK){
		decodificar();
		return _DHT22.data.temp_string;
	} else {
		return (uint8_t *)"Null";
	}
}


/*
 * @brief	Solicita el valor de humedad
 * @return	Humedad
 */
float DHT22_get_hum(void){
	if(_DHT22.status == DHT_OK){
		decodificar();
		return _DHT22.data.hum;
	} else {
		return 0;
	}
}


/*
 * @brief	Solicita el valor de humedad
 * @return	Humedad en formato String
 */
uint8_t * DHT22_get_hum_string(void){
	if(_DHT22.status == DHT_OK){
		decodificar();
		return _DHT22.data.hum_string;
	} else {
		return (uint8_t *)"Null";
	}
}


/*
 * @brief	Traduce los valores guardados en _DHT22.data.crude
 * @Note	No se contempla la verificación de la comunicación
 */
static void decodificar(void){

	float humidity = (float)(_DHT22.data.crude/(1<<16));
	humidity = humidity/10;
	float temperature = (float)(_DHT22.data.crude%(1<<15));
	temperature = temperature/10;
	_DHT22.data.hum = humidity;
	_DHT22.data.temp = temperature;

	/*variables tipo string*/
	uint8_t * data_string = uint_to_string((uint32_t)(humidity*10));
	if(humidity<10){
		_DHT22.data.hum_string[0]=data_string[0];
		_DHT22.data.hum_string[1]=',';
		_DHT22.data.hum_string[2]=data_string[1];
		_DHT22.data.hum_string[3]='%';
		_DHT22.data.hum_string[4]='\0';
	} else {
		_DHT22.data.hum_string[0]=data_string[0];
		_DHT22.data.hum_string[1]=data_string[1];
		_DHT22.data.hum_string[2]=',';
		_DHT22.data.hum_string[3]=data_string[2];
		_DHT22.data.hum_string[4]='%';
		_DHT22.data.hum_string[5]='\0';
	}

	data_string = uint_to_string((uint32_t)(temperature*10));
	if(temperature<10){
		_DHT22.data.temp_string[0]=data_string[0];
		_DHT22.data.temp_string[1]=',';
		_DHT22.data.temp_string[2]=data_string[1];
		_DHT22.data.temp_string[3]='C';	/*No toma el '°'*/
		_DHT22.data.temp_string[4]='\0';
	} else {
		_DHT22.data.temp_string[0]=data_string[0];
		_DHT22.data.temp_string[1]=data_string[1];
		_DHT22.data.temp_string[2]=',';
		_DHT22.data.temp_string[3]=data_string[2];
		_DHT22.data.temp_string[4]='C';	/*No toma el '°'*/
		_DHT22.data.temp_string[5]='\0';
	}
}


/*
 * @brief	Convierte entero sin signo a String
 * @param	Número a convertir
 * @return	Número en formato String
 */
uint8_t * uint_to_string(uint32_t numero){
	uint32_t divisor;
	uint8_t Exp_divisor;
	uint32_t digito;
	uint8_t i = 0;	/*Posición en el string*/
	bool_t  start_num = 0;	/*Var aux para eliminar 0's a izquierda del dígito significativo*/
	string_uint[0]='\0';
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
	if(string_uint[0]=='\0'){
		string_uint[0]='0';
		i=1;
	}
	string_uint[i] = '\0';
	return string_uint;
}

void delay_ms(uint32_t delay) {
	port_delay_ms(delay);
}

