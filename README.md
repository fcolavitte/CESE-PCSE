# CESE-PCSE
Trabajo final para materia Protocolos de Comunicación de la Especialización en Sistemas Embebidos - UBA 

Medición de humedad y temperatura con DHT22 y placa núcleo STM32F429ZI

[![datasheet_DHT22](https://img.shields.io/badge/Datasheet-DHT22-gold.svg)](https://files.seeedstudio.com/wiki/Grove-Temperature_and_Humidity_Sensor_Pro/res/AM2302-EN.pdf)

## Resumen
> El Driver funciona completamente por interrupciones. Se requiere inicializar el DHT usando la función DHT_init(DHT22_sensor * DHT22_struct); para definir el número de pin y puerto al que se conectará el cable de datos del DHT22.
> El sensor se debe alimentar con los pines de GND y 3.3v, no se requiere resistencia PULL-UP para distancias cortas.
> El valor de las lescturas se actualizan cada 2,5 segundos de forma automática por interrupción, por lo que para leerlas no se requiere esperar a que se realice la comunicación, sino que se toma el dato de la última lectura. Para leer los valores se debe llamar alguna de las funciones GET mencionadas a continuación.

## Funciones


### void DHT22_init(DHT22_sensor * DHT22_struct);
> @brief	Inicializa el DHT22 con los valores de la estructura cargada<br/>
> @param	Puntero a estructura del DHT22<br/>
> @return	None



### float DHT22_get_temp(DHT22_sensor * DHT22_struct);
> @brief	Solicita el valor de temperatura<br/>
> @param	Puntero a estructura del DHT22<br/>
> @return	Temperatura en grados celcius

### uint8_t * DHT22_get_temp_string(DHT22_sensor * DHT22_struct);
> @brief	Solicita el valor de temperatura<br/>
> @param	Puntero a estructura del DHT22<br/>
> @return	Temperatura en grados celcius en String

### float DHT22_get_hum(DHT22_sensor * DHT22_struct);
> @brief	Solicita el valor de humedad<br/>
> @param	Puntero a estructura del DHT22<br/>
> @return	Humedad en String

### uint8_t * DHT22_get_hum_string(DHT22_sensor * DHT22_struct);
> @brief	Solicita el valor de humedad<br/>
> @param	Puntero a estructura del DHT22<br/>
> @return	Humedad
