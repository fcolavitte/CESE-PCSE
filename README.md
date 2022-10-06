# CESE - PCSE
## Implementación de un driver genérico basado en interrupciones para DHT22
Trabajo final para materia Protocolos de Comunicación de la Especialización en Sistemas Embebidos - UBA 

### Introducción
Driver genérico para el sensor de humedad y temperatura DHT22. Incluye portabilidad para microcontrolador STM32F4xx.

[![datasheet_DHT22](https://img.shields.io/badge/Datasheet-DHT22-gold.svg)](https://files.seeedstudio.com/wiki/Grove-Temperature_and_Humidity_Sensor_Pro/res/AM2302-EN.pdf)

### Resumen
El Driver funciona completamente por interrupciones. Se requiere inicializar el DHT22 usando la función ``` DHT22_init(_pin, _port); ``` para definir el número de pin y puerto al que se conectará el cable de datos del DHT22.
El sensor se debe alimentar con los pines de GND y 3.3v, no se requiere resistencia PULL-UP para distancias cortas.
El valor de la lectura se actualiza cada 2,5 segundos de forma automática por interrupción, por lo que para leerlas no se requiere esperar a que se realice la comunicación, sino que se toma el dato de la última lectura. Para leer los valores se debe llamar alguna de las funciones GET.

### Implementación 
El driver está diseñado para tomar datos de humedad y temperatura por medio de interrupciones para anular los tiempos de lectura en el código principal. Se utiliza el timer 2 y la interrupción de GPIO correspondiente al número de pin con el que se inicialice el sensor.
Se organiza en la siguiente estructura de directorios.

```bash
DHT22/
├── inc
│   └── DHT22.h
└── src
    ├── DHT22.c
    └── DHT22_STM32f4xx_port.c
```

## Funciones


```C
boolt_t DHT22_init(uint16_t _pin, uint8_t _port);
```
> @brief	Inicializa las funciones de lectura del DHT22 en el GPIO asignado<br/>
> @param	Número de GPIO del Port. Revisar HAL o datsheet para obtener el número de cada pin.<br/>
> @param	Valores válidos PORT_A a PORT_G<br/>
> @return	True si se inició correctamente. Sino regresa False<br/>
<br/>

```C
float DHT22_get_temp(void);
```
> @brief	Solicita el valor de temperatura<br/>
> @return	Temperatura en grados celcius<br/>
<br/>

```C
uint8_t * DHT22_get_temp_string(void);
```
> @brief	Solicita el valor de temperatura<br/>
> @return	Temperatura en grados celcius en formato String<br/>
<br/>

```C
float DHT22_get_hum(void);
```
> @brief	Solicita el valor de humedad<br/>
> @return	Humedad<br/>
<br/>

```C
uint8_t * DHT22_get_hum_string(void);
```
> @brief	Solicita el valor de humedad<br/>
> @return	Humedad en formato String<br/>
<br/>

## Ejemplo de aplicación
Aclaración, el ejemplo usa a su vez otro driver para enviar Strings por UART el cual no se encuentra en este repositorio.
```C
int main(void) {
  HAL_Init();
  SystemClock_Config();
  uartInit();
  
  /*Inicializa el sensor DHT22*/
  DHT22_init(GPIO_PIN_4, PORT_E);

  while (1) {

    /*Muestra última lectura de humedad y temperatura por UART*/
    uartSendString((uint8_t *)"\r\nTemp: ");
    uartSendString(DHT22_get_temp_string());
    uartSendString((uint8_t *)"\r\nHum: ");
    uartSendString(DHT22_get_hum_string());
    uartSendString((uint8_t *)"\r\n");
    
    delay_ms(2000);
  }
}
```
