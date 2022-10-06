################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/DHT22/src/DHT22.c \
../Drivers/DHT22/src/DHT22_STM32f4xx_port.c 

OBJS += \
./Drivers/DHT22/src/DHT22.o \
./Drivers/DHT22/src/DHT22_STM32f4xx_port.o 

C_DEPS += \
./Drivers/DHT22/src/DHT22.d \
./Drivers/DHT22/src/DHT22_STM32f4xx_port.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/DHT22/src/%.o Drivers/DHT22/src/%.su: ../Drivers/DHT22/src/%.c Drivers/DHT22/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F429ZITx -DSTM32F4 -DNUCLEO_F429ZI -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../Inc -I"C:/Users/Facu/Desktop/UBA Esp en Sist Emb/Materias CESE/PCSE/CESE-PCSE/PF_PCSE/Drivers/STM32F4xx_HAL_Driver/Inc" -I"C:/Users/Facu/Desktop/UBA Esp en Sist Emb/Materias CESE/PCSE/CESE-PCSE/PF_PCSE/Drivers/CMSIS/Include" -I"C:/Users/Facu/Desktop/UBA Esp en Sist Emb/Materias CESE/PCSE/CESE-PCSE/PF_PCSE/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/Facu/Desktop/UBA Esp en Sist Emb/Materias CESE/PCSE/CESE-PCSE/PF_PCSE/Drivers/BSP/STM32F4xx_Nucleo_144" -I"C:/Users/Facu/Desktop/UBA Esp en Sist Emb/Materias CESE/PCSE/CESE-PCSE/PF_PCSE/Drivers/Core/Inc" -I"C:/Users/Facu/Desktop/UBA Esp en Sist Emb/Materias CESE/PCSE/CESE-PCSE/PF_PCSE/Drivers/API" -I"C:/Users/Facu/Desktop/UBA Esp en Sist Emb/Materias CESE/PCSE/CESE-PCSE/PF_PCSE/Drivers" -I"C:/Users/Facu/Desktop/UBA Esp en Sist Emb/Materias CESE/PCSE/CESE-PCSE/PF_PCSE/Drivers/API/inc" -I"C:/Users/Facu/Desktop/UBA Esp en Sist Emb/Materias CESE/PCSE/CESE-PCSE/PF_PCSE/Drivers/API/src" -I"C:/Users/Facu/Desktop/UBA Esp en Sist Emb/Materias CESE/PCSE/CESE-PCSE/PF_PCSE/Drivers/DHT22/inc" -I"C:/Users/Facu/Desktop/UBA Esp en Sist Emb/Materias CESE/PCSE/CESE-PCSE/PF_PCSE/Drivers/DHT22/src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-DHT22-2f-src

clean-Drivers-2f-DHT22-2f-src:
	-$(RM) ./Drivers/DHT22/src/DHT22.d ./Drivers/DHT22/src/DHT22.o ./Drivers/DHT22/src/DHT22.su ./Drivers/DHT22/src/DHT22_STM32f4xx_port.d ./Drivers/DHT22/src/DHT22_STM32f4xx_port.o ./Drivers/DHT22/src/DHT22_STM32f4xx_port.su

.PHONY: clean-Drivers-2f-DHT22-2f-src

