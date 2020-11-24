################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UTILS/Utils.c 

OBJS += \
./UTILS/Utils.o 

C_DEPS += \
./UTILS/Utils.d 


# Each subdirectory must supply rules for building sources it contributes
UTILS/%.o: ../UTILS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


