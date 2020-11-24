################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FIFO_CONNECTION/FileReader/FileReader.c 

OBJS += \
./FIFO_CONNECTION/FileReader/FileReader.o 

C_DEPS += \
./FIFO_CONNECTION/FileReader/FileReader.d 


# Each subdirectory must supply rules for building sources it contributes
FIFO_CONNECTION/FileReader/%.o: ../FIFO_CONNECTION/FileReader/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


