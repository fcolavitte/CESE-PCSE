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
extern void reset_timer(void);
extern bool_t is_pin(uint16_t GPIO_num);
extern void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

/*Functions --------------------------------------------------------------------*/
static void decodificar(void);
static uint8_t *uint_to_string(uint32_t numero);
static bool_t comrpobar_datos(void);

/*Variables Globales ------------------------------------------------------------*/
/** Variable en la que se almacenan los parámetros y lecturas del DHT22	 		 */
DHT22_sensor _DHT22;
/** Variable usada para almacenar en formato String números uint32_t			 */
static uint8_t string_uint[11];
/**
 * Variable donde se guardan los bits en el proceso de lectura durante las interrupciones no se
 * guardan directamente en _DHT22.data.crude para no modificar el valor mientras se lo decodifica
 */
uint32_t _data_crude;
/**
 * Variable donde se guardan los bits en el proceso de lectura durante las interrupciones no se
 * guardan directamente en _DHT22.data.validation para no modificar el valor mientras se lo decodifica
 */
uint8_t _data_validation;

/**
 * @brief	Inicializa las funciones de lectura del DHT22 en el GPIO asignado
 * @param	Número de GPIO del Port. Revisar HAL o datsheet para obtener el número de cada pin.
 * @param	Valores válidos PORT_A a PORT_G
 * @return	True si se inició correctamente. Sino regresa False
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
		_DHT22.status = DHT_CONNECTING;

		GPIO_set_config(_DHT22.Port, _DHT22.Pin);

		GPIO_write(_DHT22.Port, _DHT22.Pin, 1);

		return 1;
	}
	return 0;
}


/**
 * @brief	Solicita el valor de temperatura
 * @return	Temperatura en grados celcius. En caso de error devuelve valores inferiores a -900.
 * @note 	-911 ERR_COMMUNICATION, -992 DISCONECTED, -993 Tomando primer lectura, -994 CONNECTING, -999 Otros.
 */
float DHT22_get_temp(void){
	if(_DHT22.status==DHT_READED){
		_DHT22.data.crude=_data_crude;
		_DHT22.data.validation=_data_validation;
		decodificar();
	}
	switch (_DHT22.status){
		case DHT_OK:
			;
			break;
		case ERR_COMMUNICATION:
			_DHT22.data.temp = -991;
			break;
		case ERR_DISCONECT:
			_DHT22.data.temp = -992;
			break;
		case DHT_READING:
			{if(_DHT22.data.validation==0){
				_DHT22.data.temp = -993;	/*Obteniendo primera lectura*/
			}}
			break;
		case DHT_CONNECTING:
			_DHT22.data.temp = -994;		/*Todavía no se realizó primer lectura*/
			break;
		default:
			_DHT22.data.temp = -999;
			break;
	}
	return _DHT22.data.temp;
}


/**
 * @brief	Solicita el valor de temperatura
 * @return	Temperatura en grados celcius en formato String
 * @note 	E1 ERR_COMMUNICATION, E2 DISCONECTED, E3 Tomando primer lectura, E4 CONNECTING, E9 Otros.
 */
uint8_t * DHT22_get_temp_string(void){
	if(_DHT22.status==DHT_READED){
		_DHT22.data.crude=_data_crude;
		_DHT22.data.validation=_data_validation;
		decodificar();
	}
	switch (_DHT22.status){
		case DHT_OK:
			;
			break;
		case ERR_COMMUNICATION:
			_DHT22.data.temp_string[0] = 'E';
			_DHT22.data.temp_string[1] = '1';
			_DHT22.data.temp_string[2] = '\0';
			break;
		case ERR_DISCONECT:
			_DHT22.data.temp_string[0] = 'E';
			_DHT22.data.temp_string[1] = '2';
			_DHT22.data.temp_string[2] = '\0';
			break;
		case DHT_READING:
			{if(_DHT22.data.validation==0){
				_DHT22.data.temp_string[0] = 'E';
				_DHT22.data.temp_string[1] = '3';
				_DHT22.data.temp_string[2] = '\0';	/*Obteniendo primera lectura*/
			}}
			break;
		case DHT_CONNECTING:
			_DHT22.data.temp_string[0] = 'E';
			_DHT22.data.temp_string[1] = '4';
			_DHT22.data.temp_string[2] = '\0';		/*Todavía no se realizó primer lectura*/
			break;
		default:
			_DHT22.data.temp_string[0] = 'E';
			_DHT22.data.temp_string[1] = '9';
			_DHT22.data.temp_string[2] = '\0';
			break;
	}
	return _DHT22.data.temp_string;
}


/**
 * @brief	Solicita el valor de humedad
 * @return	Humedad. En caso de error devuelve valores inferiores a -900.
 * @note 	-911 ERR_COMMUNICATION, -992 DISCONECTED, -993 Tomando primer lectura, -994 CONNECTING, -999 Otros.
 */
float DHT22_get_hum(void){
	if(_DHT22.status==DHT_READED){
		_DHT22.data.crude=_data_crude;
		_DHT22.data.validation=_data_validation;
		decodificar();
	}
	switch (_DHT22.status){
		case DHT_OK:
			;
			break;
		case ERR_COMMUNICATION:
			_DHT22.data.hum = -991;
			break;
		case ERR_DISCONECT:
			_DHT22.data.hum = -992;
			break;
		case DHT_READING:
			{if(_DHT22.data.validation==0){
				_DHT22.data.hum = -993;	/*Obteniendo primera lectura*/
			}}
			break;
		case DHT_CONNECTING:
			_DHT22.data.hum = -994;		/*Todavía no se realizó primer lectura*/
			break;
		default:
			_DHT22.data.hum = -999;
			break;
	}
	return _DHT22.data.hum;
}


/**
 * @brief	Solicita el valor de humedad
 * @return	Humedad en formato String
 * @note 	E1 ERR_COMMUNICATION, E2 DISCONECTED, E3 Tomando primer lectura, E4 CONNECTING, E9 Otros.
 */
uint8_t * DHT22_get_hum_string(void){
	if(_DHT22.status==DHT_READED){
		_DHT22.data.crude=_data_crude;
		_DHT22.data.validation=_data_validation;
		decodificar();
	}
	switch (_DHT22.status){
		case DHT_OK:
			;
			break;
		case ERR_COMMUNICATION:
			_DHT22.data.hum_string[0] = 'E';
			_DHT22.data.hum_string[1] = '1';
			_DHT22.data.hum_string[2] = '\0';
			break;
		case ERR_DISCONECT:
			_DHT22.data.hum_string[0] = 'E';
			_DHT22.data.hum_string[1] = '2';
			_DHT22.data.hum_string[2] = '\0';
			break;
		case DHT_READING:
			{if(_DHT22.data.validation==0){
				_DHT22.data.hum_string[0] = 'E';
				_DHT22.data.hum_string[1] = '3';
				_DHT22.data.hum_string[2] = '\0';	/*Obteniendo primera lectura*/
			}}
			break;
		case DHT_CONNECTING:
			_DHT22.data.hum_string[0] = 'E';
			_DHT22.data.hum_string[1] = '4';
			_DHT22.data.hum_string[2] = '\0';		/*Todavía no se realizó primer lectura*/
			break;
		default:
			_DHT22.data.hum_string[0] = 'E';
			_DHT22.data.hum_string[1] = '9';
			_DHT22.data.hum_string[2] = '\0';
			break;
	}
	return _DHT22.data.hum_string;
}


/**
 * @brief	Traduce los valores guardados en _DHT22.data.crude
 */
static void decodificar(void){
	if(comrpobar_datos()){
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
		/*Si el primer bit de los 2bytes de temp es 1, la temperatura es negativa*/
		if((_DHT22.data.crude&0x8000) > 0){
			temperature=-temperature;
			for(int i=0;i<6;i++){
				_DHT22.data.temp_string[i+1]=_DHT22.data.temp_string[i];
			}
			_DHT22.data.temp_string[0]='-';
		}
	} else {
		_DHT22.data.hum=999;
		_DHT22.data.temp=999;
		_DHT22.data.hum_string[0] = 'E';
		_DHT22.data.hum_string[1] = '\0';
		_DHT22.data.temp_string[0] = 'E';
		_DHT22.data.temp_string[1] = '\0';
	}
}


/**
 * @brief	Verifica los bits recibidos en comparación con el byte de paridad. Modifica _DHT22.status
 * @return	True si byte de paridad coincide, sino False.
 */
static bool_t comrpobar_datos(void){
	uint32_t suma_bytes = _DHT22.data.crude % 256;
	suma_bytes += (_DHT22.data.crude/256) % 256;
	suma_bytes += (_DHT22.data.crude/65536) % 256;
	suma_bytes += (_DHT22.data.crude/16777216) % 256;
	if((uint8_t)suma_bytes==_DHT22.data.validation){
		_DHT22.status = DHT_OK;
		return 1;
	} else {
		_DHT22.status = ERR_COMMUNICATION;
		/*Control de Error*/
		return 0;
	}
}


/**
 * @brief	Convierte entero sin signo a String
 * @param	Número a convertir
 * @return	Número en formato String
 */
static uint8_t * uint_to_string(uint32_t numero){
	uint32_t divisor;		/* Divisor potencia de 10 para "desplazar la coma" en sistema decimal	*/
	uint8_t Exp_divisor;	/* Exponente del divisor, define la potencia de 10						*/
	uint32_t digito;		/* Dígito del 0 al 9 sin codificación ASCII								*/
	uint8_t i = 0;			/* Posición en el string												*/
	bool_t  start_num = 0;	/* Var aux para eliminar 0's a izquierda del dígito significativo		*/
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
			string_uint[i++] = digito + '0';	/* Agregar dígito al String en código ASCII			*/
			start_num = 1;
		}
	}

	/*Si el número original fue 0*/
	if(string_uint[0]=='\0'){
		string_uint[0]='0';
		i=1;
	}

	string_uint[i] = '\0';
	return string_uint;
}


/**
 * @brief	Delay en milisegundos
 * @param	Tiempo en ms a esperar
 */
void delay_ms(uint32_t delay) {
	port_delay_ms(delay);
}


/**
 * @brief	Verificar status del DHT22
 * @return	Ver "struct DHT22_sensor.status"
 */
uint8_t DHT22_get_status(void){
	return _DHT22.status;
}
