/*
 * port.c
 *
 *  Created on: 27 sep. 2022
 *      Author: Facundo Colavitte
 */
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f429xx.h"
#include "DHT22.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include "API_uart.h"


/*Functions GPIO ------------------------------------------------------------------------*/
void GPIO_set_config(GPIO_TypeDef * GPIO_port, uint16_t GPIO_num);
void GPIO_write(GPIO_TypeDef * GPIO_port, uint16_t GPIO_num, bool_t GPIO_state);
bool_t GPIO_read(GPIO_TypeDef * GPIO_port, uint16_t GPIO_num);
bool_t is_pin(uint16_t GPIO_num);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void EXTI9_5_IRQHandler(void);

/*Functions time & timer ----------------------------------------------------------------*/
void reset_timer(void);
uint32_t tiempo_actual(void);
void delay_ms(uint32_t delay);
void Timer_Init(void);
HAL_StatusTypeDef HAL_TIM_Base_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Start(TIM_HandleTypeDef *htim);
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim);
void TIM_Base_SetConfig(TIM_TypeDef *TIMx, TIM_Base_InitTypeDef *Structure);
void reset_T_Array_counter(void);

/*Variables -----------------------------------------------------------------------*/
TIM_HandleTypeDef hTim2;		/*Handler para Timer2*/
DHT22_sensor _DHT22;			/*Sensor DHT22 en que se está realizando la lectura*/
uint8_t T_Array_counter = 0;	/*Contador para recorrer T_Array de _DHT22*/




/*---------------------------------------------------- GPIO -------------------------------------------------------*/

/*
 * @brief	Configurar GPIO para comunicación con DHT22
 * @param	Puntero a estructura delay
 * @return	None
 * @note	GPIO modo salida (permite leer PIN), open-drain con interrupciones por flanco ascendente y descendente
 */
void GPIO_set_config(GPIO_TypeDef * GPIO_port, uint16_t GPIO_num){
	if(is_pin(GPIO_num)){
		//Preparar configuración PIN
		GPIO_InitTypeDef PIN_DHT22_config;

		if(GPIOA == GPIO_port){__HAL_RCC_GPIOA_CLK_ENABLE();}
		if(GPIOB == GPIO_port){__HAL_RCC_GPIOB_CLK_ENABLE();}
		if(GPIOC == GPIO_port){__HAL_RCC_GPIOC_CLK_ENABLE();}
		if(GPIOD == GPIO_port){__HAL_RCC_GPIOD_CLK_ENABLE();}
		if(GPIOE == GPIO_port){__HAL_RCC_GPIOE_CLK_ENABLE();}
		if(GPIOF == GPIO_port){__HAL_RCC_GPIOF_CLK_ENABLE();}
		if(GPIOG == GPIO_port){__HAL_RCC_GPIOG_CLK_ENABLE();}


		PIN_DHT22_config.Pin = GPIO_num;
		PIN_DHT22_config.Mode = MODE_OUTPUT | OUTPUT_OD | EXTI_IT | TRIGGER_FALLING;
		PIN_DHT22_config.Pull = GPIO_PULLUP;
		PIN_DHT22_config.Speed = GPIO_SPEED_FREQ_HIGH;
		//Cargar configuración PIN
		HAL_GPIO_Init(GPIO_port, &PIN_DHT22_config);	//Modificar por GPIO_port y GPIO_num

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
		HAL_NVIC_SetPriority(IRQn,0,0);//EXTI15_10_IRQn//EXTI2_IRQn
		NVIC_EnableIRQ(IRQn);

		/*Inicializar Timer*/
		Timer_Init();

		_DHT22.Port = GPIO_port;
		_DHT22.Pin = GPIO_num;
	}
}



/*
 * @brief	Poner en bajo o liberar PIN
 * @param	Número de PIN
 * @param	Estado (bajo u open)
 * @return	None
 * @note	GPIO en modo open-drain
 */
void GPIO_write(GPIO_TypeDef * GPIO_port, uint16_t GPIO_num, bool_t GPIO_state){
	if(is_pin(GPIO_num)){
		if(GPIO_state) {
			HAL_GPIO_WritePin(GPIO_port, GPIO_num, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(GPIO_port, GPIO_num, GPIO_PIN_RESET);
		}
	} else {
		/*Error en el número de PIN*/
	}
}


/*
 * @brief	Leer estado del pin
 * @param	Número de PIN
 * @return	1 o 0 dependiendo el estado del PIN
 */
bool_t GPIO_read(GPIO_TypeDef * GPIO_port, uint16_t GPIO_num){
	if(is_pin(GPIO_num)){
		return HAL_GPIO_ReadPin(GPIO_port, GPIO_num);
	} else {
		/*Error en el número de PIN*/
		return 0;
	}
}


/*
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

/*
 * @brief	Recetea la variable T_Array_counter
 * @param	None
 * @return	None
 */
void reset_T_Array_counter(void){
	T_Array_counter=0;
}

/*---------------------------------------------- Interrupciones --------------------------------------------*/

/*
 * @brief	Manejador de interrupción EXIT0
 * @param	None
 * @return	None
 */
void EXTI0_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

/*
 * @brief	Manejador de interrupción EXIT1
 * @param	None
 * @return	None
 */
void EXTI1_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

/*
 * @brief	Manejador de interrupción EXIT2
 * @param	None
 * @return	None
 */
void EXTI2_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}

/*
 * @brief	Manejador de interrupción EXIT3
 * @param	None
 * @return	None
 */
void EXTI3_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}

/*
 * @brief	Manejador de interrupción EXIT4
 * @param	None
 * @return	None
 */
void EXTI4_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}

/*
 * @brief	Manejador de interrupción EXIT9_5
 * @param	None
 * @return	None
 */
void EXTI9_5_IRQHandler(void){
	if(_DHT22.Pin<=GPIO_PIN_9 && _DHT22.Pin>=GPIO_PIN_5){
		HAL_GPIO_EXTI_IRQHandler(_DHT22.Pin);
	} else {
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
	}
}

/*
 * @brief	Manejador de interrupción EXIT15_10
 * @param	None
 * @return	None
 */
void EXTI15_10_IRQHandler (void){
	if(_DHT22.Pin<=GPIO_PIN_15 && _DHT22.Pin>=GPIO_PIN_10){
		HAL_GPIO_EXTI_IRQHandler(_DHT22.Pin);
	} else {
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
	}
}



/*
 * @brief	Control de interrupción por PIN
 * @param	Número de PIN
 * @return	None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

	  /*
	   * Verificar si el pin que actuó el ISR es el del DHT22
	   * Asignar a variable el tiempo del PIN en alto o en bajo
	   * Ver estado del PIN para definir si fue flanco ascendente o descendente
	   */
	  /*if(__HAL_GPIO_EXTI_GET_IT(GPIO_Pin)){
		  //flanco_ascendente();
	  } else{
		  //flanco_descendente();
	  }*/
	_DHT22.T_Array[T_Array_counter] = __HAL_TIM_GET_COUNTER(&hTim2);
	__HAL_TIM_SET_COUNTER(&hTim2,0);
	T_Array_counter++;
	if(T_Array_counter> (sizeof(_DHT22.T_Array)/sizeof(_DHT22.T_Array[0]))-1 ){
		T_Array_counter=0;
	}
	//uartSendString("a");
	BSP_LED_Toggle(LED2);
}


/*----------------------------------------------------- Time ---------------------------------------------*/
/*
 * @brief	Devuelve el tiempo actual desde que está corriendo el programa
 * @param	None
 * @return	Tiempo desde que el uC está encendido en milisegundos
 */
uint32_t tiempo_actual(void){
	return HAL_GetTick();
}

/*
 * @brief	CGenerar delay bloqueante en milisegundos
 * @param	Tiempo en milisegundos a esperar
 * @return	None
 */
void delay_ms(uint32_t delay){
	HAL_Delay(delay);
}


/*------------------------------------------------------------------ TIMER -----------------------------------------------------------------------------*/
/*
 * @brief	Resetea el tiempo del timer 2
 * @param	None
 * @return	None
 */
void reset_timer(void){
	T_Array_counter=0;
	__HAL_TIM_SET_COUNTER(&hTim2,0);
}

/*Funciones copiadas del archivo de "stm32f4xx_hal_tim.c", pero compilador no las reconoce*/

void Timer_Init(void){
	__HAL_RCC_TIM2_CLK_ENABLE();
    hTim2.Instance = TIM2;
    hTim2.Init.Prescaler = 80-1;//Para que cuente en us
    hTim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    hTim2.Init.Period = 0xffff;
    hTim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    hTim2.Init.RepetitionCounter = 0;
    hTim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&hTim2);
    HAL_TIM_Base_Start(&hTim2); // Trying to start the base counter
}

HAL_StatusTypeDef HAL_TIM_Base_Init(TIM_HandleTypeDef *htim)
{
  /* Check the TIM handle allocation */
  if (htim == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_TIM_INSTANCE(htim->Instance));
  assert_param(IS_TIM_COUNTER_MODE(htim->Init.CounterMode));
  assert_param(IS_TIM_CLOCKDIVISION_DIV(htim->Init.ClockDivision));
  assert_param(IS_TIM_AUTORELOAD_PRELOAD(htim->Init.AutoReloadPreload));

  if (htim->State == HAL_TIM_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    htim->Lock = HAL_UNLOCKED;

#if (USE_HAL_TIM_REGISTER_CALLBACKS == 1)
    /* Reset interrupt callbacks to legacy weak callbacks */
    TIM_ResetCallback(htim);

    if (htim->Base_MspInitCallback == NULL)
    {
      htim->Base_MspInitCallback = HAL_TIM_Base_MspInit;
    }
    /* Init the low level hardware : GPIO, CLOCK, NVIC */
    htim->Base_MspInitCallback(htim);
#else
    /* Init the low level hardware : GPIO, CLOCK, NVIC */
    HAL_TIM_Base_MspInit(htim);
#endif /* USE_HAL_TIM_REGISTER_CALLBACKS */
  }

  /* Set the TIM state */
  htim->State = HAL_TIM_STATE_BUSY;

  /* Set the Time Base configuration */
  TIM_Base_SetConfig(htim->Instance, &htim->Init);

  /* Initialize the DMA burst operation state */
  htim->DMABurstState = HAL_DMA_BURST_STATE_READY;

  /* Initialize the TIM channels state */
  TIM_CHANNEL_STATE_SET_ALL(htim, HAL_TIM_CHANNEL_STATE_READY);
  TIM_CHANNEL_N_STATE_SET_ALL(htim, HAL_TIM_CHANNEL_STATE_READY);

  /* Initialize the TIM state*/
  htim->State = HAL_TIM_STATE_READY;

  return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Base_Start(TIM_HandleTypeDef *htim)
{
  uint32_t tmpsmcr;

  /* Check the parameters */
  assert_param(IS_TIM_INSTANCE(htim->Instance));

  /* Check the TIM state */
  if (htim->State != HAL_TIM_STATE_READY)
  {
    return HAL_ERROR;
  }

  /* Set the TIM state */
  htim->State = HAL_TIM_STATE_BUSY;

  /* Enable the Peripheral, except in trigger mode where enable is automatically done with trigger */
  if (IS_TIM_SLAVE_INSTANCE(htim->Instance))
  {
    tmpsmcr = htim->Instance->SMCR & TIM_SMCR_SMS;
    if (!IS_TIM_SLAVEMODE_TRIGGER_ENABLED(tmpsmcr))
    {
      __HAL_TIM_ENABLE(htim);
    }
  }
  else
  {
    __HAL_TIM_ENABLE(htim);
  }

  /* Return function status */
  return HAL_OK;
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_TIM_Base_MspInit could be implemented in the user file
   */
}

void TIM_Base_SetConfig(TIM_TypeDef *TIMx, TIM_Base_InitTypeDef *Structure)
{
  uint32_t tmpcr1;
  tmpcr1 = TIMx->CR1;

  /* Set TIM Time Base Unit parameters ---------------------------------------*/
  if (IS_TIM_COUNTER_MODE_SELECT_INSTANCE(TIMx))
  {
    /* Select the Counter Mode */
    tmpcr1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
    tmpcr1 |= Structure->CounterMode;
  }

  if (IS_TIM_CLOCK_DIVISION_INSTANCE(TIMx))
  {
    /* Set the clock division */
    tmpcr1 &= ~TIM_CR1_CKD;
    tmpcr1 |= (uint32_t)Structure->ClockDivision;
  }

  /* Set the auto-reload preload */
  MODIFY_REG(tmpcr1, TIM_CR1_ARPE, Structure->AutoReloadPreload);

  TIMx->CR1 = tmpcr1;

  /* Set the Autoreload value */
  TIMx->ARR = (uint32_t)Structure->Period ;

  /* Set the Prescaler value */
  TIMx->PSC = Structure->Prescaler;

  if (IS_TIM_REPETITION_COUNTER_INSTANCE(TIMx))
  {
    /* Set the Repetition Counter value */
    TIMx->RCR = Structure->RepetitionCounter;
  }

  /* Generate an update event to reload the Prescaler
     and the repetition counter (only for advanced timer) value immediately */
  TIMx->EGR = TIM_EGR_UG;
}



