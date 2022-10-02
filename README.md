# CESE-PCSE
Trabajo final para materia Protocolos de Comunicación de la Especialización en Sistemas Embebidos - UBA 

Medición de humedad y temperatura con DHT22 y placa núcleo STM32F429ZI

[![datasheet_DHT22](https://img.shields.io/badge/Datasheet-DHT22-gold.svg)](https://files.seeedstudio.com/wiki/Grove-Temperature_and_Humidity_Sensor_Pro/res/AM2302-EN.pdf)

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
