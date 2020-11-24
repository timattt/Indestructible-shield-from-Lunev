################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FIFO_CONNECTION/Base/Base.c 

OBJS += \
./FIFO_CONNECTION/Base/Base.o 

C_DEPS += \
./FIFO_CONNECTION/Base/Base.d 


# Each subdirectory must supply rules for building sources it contributes
FIFO_CONNECTION/Base/%.o: ../FIFO_CONNECTION/Base/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


