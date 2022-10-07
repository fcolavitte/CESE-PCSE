/*
 * port.c
 *
 *  Created on: 27 sep. 2022
 *      Author: Facundo Colavitte
 *
 *  Driver documentation:
 *  	https://github.com/fcolavitte/CESE-PCSE.git
 */

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f429xx.h"
#include "DHT22.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

/**
 * @brief Tiempo en us que diferencia un 1 de un 0 en la comunicación.
 * 		  Incluye tanto tiempo en bajo como en alto para cada bit.
 */
#define T_corte 90

/**
 * @brief Número de bit que detecta la interrupción en el cual comienza los datos.
 * @note  Primera interrupción pertenece al inicio de la confirmación del
 * 		  DHT22 y la segunda es el final de la confirmación.
 */
#define _inicio_bit_0 2


/*Functions GPIO ---------------------------------------------------------------------------*/
void GPIO_set_config(uint8_t GPIO_port, uint16_t GPIO_num);
void GPIO_write(uint8_t GPIO_port, uint16_t GPIO_num, bool_t GPIO_state);
bool_t is_pin(uint16_t GPIO_num);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
static void EXTI9_5_IRQHandler(void);


/*Functions time & timer -------------------------------------------------------------------*/
void reset_timer(void);
uint32_t tiempo_actual(void);
void port_delay_ms(uint32_t delay);
void Timer_Init(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

/*Variables --------------------------------------------------------------------------------*/
/** Handler para Timer2											*/
TIM_HandleTypeDef hTim2;
/** Sensor DHT22 en que se está realizando la lectura			*/
extern DHT22_sensor _DHT22;
/** Contador para definir la posición de bits de la comunicación*/
uint8_t T_Array_counter = 0;
/** Cuenta los ms con interrupciones del Timer2					*/
uint32_t cont_timer=0;
/**
 * Variable donde se guardan los bits en el proceso de lectura durante las interrupciones no se
 * guardan directamente en _DHT22.data.crude para no modificar el valor mientras se lo decodifica
 */
extern uint32_t _data_crude;
/**
 * Variable donde se guardan los bits en el proceso de lectura durante las interrupciones no se
 * guardan directamente en _DHT22.data.validation para no modificar el valor mientras se lo decodifica
 */
extern uint8_t _data_validation;



/*----------------------------------------------- GPIO -------------------------------------------------------*/


/**
 * @brief	Convierte variable PORT definida en DHT22.h a variable utilizada por la HAL para dicho puerto
 * @param	PORT_A a PORT_G
 * @param	Puntero a estructura PORT
 */
GPIO_TypeDef  * _HAL_PORT_DECODE(uint8_t Port){
	GPIO_TypeDef  * _HAL_PORT;
	switch (Port){
		case PORT_A:
			_HAL_PORT = GPIOA;
		break;
		case PORT_B:
			_HAL_PORT = GPIOB;
		break;
		case PORT_C:
			_HAL_PORT = GPIOC;
		break;
		case PORT_D:
			_HAL_PORT = GPIOD;
		break;
		case PORT_E:
			_HAL_PORT = GPIOE;
		break;
		case PORT_F:
			_HAL_PORT = GPIOF;
		break;
		default:
			_HAL_PORT = GPIOG;
		break;
	}
	return _HAL_PORT;
}


/**
 * @brief	Configurar GPIO para comunicación con DHT22
 * @param	PORT_A a PORTG
 * @param	Pin, en caso de STM32 va de GPIO_PIN_0 a GPIO_PIN_15
 * @note	GPIO modo salida, open-drain con interrupciones por flanco descendente
 */
void GPIO_set_config(uint8_t GPIO_port, uint16_t GPIO_num){
	if(is_pin(GPIO_num)){
		//Preparar configuración PIN
		GPIO_InitTypeDef PIN_DHT22_config;

		if(PORT_A == GPIO_port){__HAL_RCC_GPIOA_CLK_ENABLE();}
		if(PORT_B == GPIO_port){__HAL_RCC_GPIOB_CLK_ENABLE();}
		if(PORT_C == GPIO_port){__HAL_RCC_GPIOC_CLK_ENABLE();}
		if(PORT_D == GPIO_port){__HAL_RCC_GPIOD_CLK_ENABLE();}
		if(PORT_E == GPIO_port){__HAL_RCC_GPIOE_CLK_ENABLE();}
		if(PORT_F == GPIO_port){__HAL_RCC_GPIOF_CLK_ENABLE();}
		if(PORT_G == GPIO_port){__HAL_RCC_GPIOG_CLK_ENABLE();}

		PIN_DHT22_config.Pin = GPIO_num;
		PIN_DHT22_config.Mode = MODE_OUTPUT | OUTPUT_OD | EXTI_IT | TRIGGER_FALLING;
		PIN_DHT22_config.Pull = GPIO_PULLUP;
		PIN_DHT22_config.Speed = GPIO_SPEED_FREQ_HIGH;

		GPIO_TypeDef  * _HAL_PORT;
		_HAL_PORT=_HAL_PORT_DECODE(GPIO_port);

		//Cargar configuración PIN
		HAL_GPIO_Init(_HAL_PORT, &PIN_DHT22_config);

		//Interrupción
		IRQn_Type IRQn;
		switch (GPIO_num){
			case GPIO_PIN_0:
				IRQn = EXTI0_IRQn;
			break;
			case GPIO_PIN_1:
				IRQn = EXTI1_IRQn;
			break;
			case GPIO_PIN_2:
				IRQn = EXTI2_IRQn;
			break;
			case GPIO_PIN_3:
				IRQn = EXTI3_IRQn;
			break;
			case GPIO_PIN_4:
				IRQn = EXTI4_IRQn;
			break;
			default:
				if(GPIO_num<GPIO_PIN_10){
					IRQn = EXTI9_5_IRQn;
				} else {
					IRQn = EXTI15_10_IRQn;
				}
			break;
		}

		HAL_NVIC_SetPriority(IRQn,0,0);
		NVIC_EnableIRQ(IRQn);

		Timer_Init();
	}
}



/**
 * @brief	Poner en bajo o liberar PIN
 * @param	Port. Valores válidos PORT_A a PORT_G
 * @param	Número de PIN
 * @param	Estado a setear. 0=>Bajo. 1=>Open.
 * @note	GPIO en modo open-drain
 */
void GPIO_write(uint8_t _GPIO_port, uint16_t GPIO_num, bool_t GPIO_state){

	if(is_pin(GPIO_num)){

		GPIO_TypeDef * GPIO_port;
		GPIO_port = _HAL_PORT_DECODE(_GPIO_port);

		if(GPIO_state) {
			HAL_GPIO_WritePin(GPIO_port, GPIO_num, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(GPIO_port, GPIO_num, GPIO_PIN_RESET);
		}

	} else {
		/*Error en el número de PIN*/
	}
}



/**
 * @brief	Verifica si el valor del pin es válido
 * @param	Número de PIN
 * @return	1 si es válido, sino devuelve 0
 */
bool_t is_pin(uint16_t GPIO_num){
	if(IS_GPIO_PIN(GPIO_num) != 1){
		return 0;
	}
	return 1;
}


/*-------------------------------------------- Interrupciones --------------------------------------------*/

/**
 * @brief	Manejador de interrupción EXIT0
 */
void EXTI0_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

/**
 * @brief	Manejador de interrupción EXIT1
 */
void EXTI1_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

/**
 * @brief	Manejador de interrupción EXIT2
 */
void EXTI2_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}

/**
 * @brief	Manejador de interrupción EXIT3
 */
void EXTI3_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}

/**
 * @brief	Manejador de interrupción EXIT4
 */
void EXTI4_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}

/**
 * @brief	Manejador de interrupción EXIT9_5
 */
void EXTI9_5_IRQHandler(void){
	if(_DHT22.Pin<=GPIO_PIN_9 && _DHT22.Pin>=GPIO_PIN_5){
		HAL_GPIO_EXTI_IRQHandler(_DHT22.Pin);
	} else {
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
	}
}

/**
 * @brief	Manejador de interrupción EXIT15_10
 */
void EXTI15_10_IRQHandler (void){
	if(_DHT22.Pin<=GPIO_PIN_15 && _DHT22.Pin>=GPIO_PIN_10){
		HAL_GPIO_EXTI_IRQHandler(_DHT22.Pin);
	} else {
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
	}
}


/**
 * @brief	Control de interrupción por PIN
 * @param	Número de PIN
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin==_DHT22.Pin){
		uint32_t new_bit = __HAL_TIM_GET_COUNTER(&hTim2);
		_DHT22.status = DHT_READING;
		/*Define si el pulso corresponde a un 0 o 1*/
		if(new_bit>T_corte){new_bit=1;}else{new_bit=0;}
		/*Lectura de los bits de humedad y temperatura*/
		if(T_Array_counter>=_inicio_bit_0 && T_Array_counter<32+_inicio_bit_0){
			new_bit = new_bit<<(31+_inicio_bit_0-T_Array_counter);
			_data_crude |= new_bit;
		}
		/*Lectura de los bits de paridad*/
		if(T_Array_counter>=32+_inicio_bit_0 && T_Array_counter<40+_inicio_bit_0){
			new_bit = new_bit<<(39+_inicio_bit_0-T_Array_counter);
			_data_validation |= new_bit;
			/*Si se completó la lectura*/
			if(T_Array_counter==39+_inicio_bit_0){
				cont_timer=0;
				_DHT22.status = DHT_READED;
			}
		}
		T_Array_counter++;
		if(T_Array_counter> 39+_inicio_bit_0){
			T_Array_counter=0;
		}

		__HAL_TIM_SET_COUNTER(&hTim2,0);
	}
}



/*------------------------------------------------ Time --------------------------------------------------*/
/**
 * @brief	Devuelve el tiempo actual desde que está corriendo el programa
 * @return	Tiempo desde que el uC está encendido en milisegundos
 */
uint32_t tiempo_actual(void){
	return HAL_GetTick();
}

/**
 * @brief	CGenerar delay bloqueante en milisegundos
 * @param	Tiempo en milisegundos a esperar
 */
void port_delay_ms(uint32_t delay){
	HAL_Delay(delay);
}


/*------------------------------------------------ TIMER -------------------------------------------------*/

/**
 * @brief	Resetea el tiempo del timer 2
 */
void reset_timer(void){
	T_Array_counter=0;
	__HAL_TIM_SET_COUNTER(&hTim2,0);
}


/**
 * @brief	Inicializa el Timer 2
 * @note	Cuenta cada 1us e interrupción cada 1ms
 */
void Timer_Init(void){
	__HAL_RCC_TIM2_CLK_ENABLE();
    hTim2.Instance = TIM2;
    hTim2.Init.Prescaler = 80-1;							/*Valor del contador del timer en us*/
    hTim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    hTim2.Init.Period = 1000-1;								/*Interrupción cada 1ms*/
    hTim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    hTim2.Init.RepetitionCounter = 0;
    hTim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&hTim2);

    //Selección del reloj interno
    TIM_ClockConfigTypeDef TimClock = {0};
    TimClock.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&hTim2, &TimClock);

	//Configurar reloj como master
	TIM_MasterConfigTypeDef TimMaster = {0};
	TimMaster.MasterSlaveMode = TIM_SLAVEMODE_DISABLE;
	TimMaster.MasterOutputTrigger = TIM_TRGO_RESET;
	HAL_TIMEx_MasterConfigSynchronization(&hTim2, &TimMaster);

	//Habilitar interrupción del timer
	HAL_NVIC_SetPriority(TIM2_IRQn,5,5);
	NVIC_EnableIRQ(TIM2_IRQn);

	HAL_TIM_Base_Start_IT(&hTim2);
}


/**
 * @brief	Manejador de interrupción por Timer2
 */
void TIM2_IRQHandler (void){
	HAL_TIM_IRQHandler(&hTim2);
}


/**
 * @brief	Control de interrupción por Timer2
 * @param	Manejador del Timer2
 * @Note	Ocurre una interrupcion cada 1ms
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	cont_timer++;
	if(cont_timer>9000){	/*Desbordamiento en caso de que la lectura no se haya completado*/
		cont_timer=0;
	}
	if(cont_timer>2501){	/*Liberar pin para esperar respuesta del DHT22*/
		T_Array_counter=0;
		GPIO_write(_DHT22.Port, _DHT22.Pin, 1);
	}else if(cont_timer>=2500){	/*Iniciar lectura poniendo pin en bajo*/
		_data_crude=0;
		_data_validation=0;
		GPIO_write(_DHT22.Port, _DHT22.Pin, 0);
		T_Array_counter=0;
	}
	/*Si no hubo flancos descendentes durante 1ms tras pedir el dato*/
	if(_DHT22.status == DHT_READING){_DHT22.status = ERR_DISCONECT;}
}
