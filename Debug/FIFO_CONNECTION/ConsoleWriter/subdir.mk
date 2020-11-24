################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FIFO_CONNECTION/ConsoleWriter/ConsoleWriter.c 

OBJS += \
./FIFO_CONNECTION/ConsoleWriter/ConsoleWriter.o 

C_DEPS += \
./FIFO_CONNECTION/ConsoleWriter/ConsoleWriter.d 


# Each subdirectory must supply rules for building sources it contributes
FIFO_CONNECTION/ConsoleWriter/%.o: ../FIFO_CONNECTION/ConsoleWriter/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


