/*
 * port.c
 *
 *  Created on: 27 sep. 2022
 *      Author: Facundo Colavitte
 */
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_tim.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>


#define PIN_DHT22		GPIO_PIN_2 	/*Número de Pin del puerto al que se conectó DHT22. De 0 a 15*/
#define PORT_PIN_DHT22	GPIOA		/*Puerto al que pertenece el pin que conecta con el DHT22*/

typedef bool bool_t;





/*Functions timer ------------------------------------------------------------------------*/
void Timer_Init(void);
HAL_StatusTypeDef HAL_TIM_Base_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Start(TIM_HandleTypeDef *htim);
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim);
void TIM_Base_SetConfig(TIM_TypeDef *TIMx, TIM_Base_InitTypeDef *Structure);

/*Variables timer -----------------------------------------------------------------------*/
TIM_HandleTypeDef hTim2;

/*Orden para activar TIMER --------------------------------------------------------------*/
//__TIM2_CLK_ENABLE();
//Timer_Init();
//uint32_t count = __HAL_TIM_GetCounter(&hTim2);






uint32_t T_Low;	/*Tiempo en us del PIN en bajo*/
uint32_t T_High;/*Tiempo en us del PIN en alto*/


uint32_t tiempo_actual(void){
	return HAL_GetTick();
}

void delay_ms(uint32_t delay){
	HAL_Delay(delay);
}


/*
 * @brief	Configurar GPIO para comunicación con DHT22
 * @param	Puntero a estructura delay
 * @return	None
 * @note	GPIO modo salida (permite leer PIN), open-drain con interrupciones por flanco ascendente y descendente
 */
void GPIO_set_config(uint8_t GPIO_port, uint8_t GPIO_num){
	/*
	 * Verificar que el pin solicitado pertenezca al hardware
	 */



	//Preparar configuración PIN
	GPIO_InitTypeDef PIN_DHT22_config={
			.Pin = PIN_DHT22,
			.Mode = MODE_OUTPUT | OUTPUT_OD | EXTI_IT | TRIGGER_RISING | TRIGGER_FALLING,
			/*https://www.st.com/resource/en/reference_manual/dm00093941-stm32f334xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
			 *Página 145. Modo OUTPUT tiene el registro input conectado a I/O PIN
			* OUTPUT_OD: Open-Drain
			* EXIT_IT habilita interrupciones
			* TRIGGER_RISING Interrupción por flanco ascendente
			* TRIGGER_FALLING Interrupción por flanco descendente
			*/
			.Pull = GPIO_PULLUP,      /*Specifies the Pull-up or Pull-Down resistence, GPIO_NOPULL, GPIO_PULLUP, GPIO_PULLDOWN*/
			.Speed = GPIO_SPEED_FREQ_MEDIUM
	};


	HAL_GPIO_Init(PORT_PIN_DHT22, &PIN_DHT22_config);	//Modificar por GPIO_port y GPIO_num
}



/*
 * @brief	Poner en bajo o liberar PIN
 * @param	Número de PIN
 * @param	Estado (bajo u open)
 * @return	None
 * @note	GPIO en modo open-drain
 */
void GPIO_write(uint8_t GPIO_port, uint8_t GPIO_num, bool_t GPIO_state){
	/*Agregar PORT*/
	if(GPIO_num<15){
		uint16_t _GPIO_num = 2^(GPIO_num);
		if(GPIO_state) {
			HAL_GPIO_WritePin(PORT_PIN_DHT22, _GPIO_num, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(PORT_PIN_DHT22, _GPIO_num, GPIO_PIN_RESET);
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
bool_t GPIO_read(uint8_t GPIO_num){
	if(GPIO_num<15){
		return HAL_GPIO_ReadPin(PORT_PIN_DHT22, PIN_DHT22);
	} else {
		/*Error en el número de PIN*/
		return 0;
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
	  if(__HAL_GPIO_EXTI_GET_IT(GPIO_Pin)){
		  //flanco_ascendente();
	  } else{
		  //flanco_descendente();
	  }

}









/*------------------------------------------------------------------ TIMER -----------------------------------------------------------------------------*/
/*Funciones sacadas de "stm32f4xx_hal_tim.c", pero compilador no las reconoce*/

void Timer_Init(void){
    hTim2.Instance = TIM2;
    hTim2.Init.Prescaler = 180-1;//Para que cuente en us
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



