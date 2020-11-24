################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MESSAGES/Main.c \
../MESSAGES/ProcessManager.c 

OBJS += \
./MESSAGES/Main.o \
./MESSAGES/ProcessManager.o 

C_DEPS += \
./MESSAGES/Main.d \
./MESSAGES/ProcessManager.d 


# Each subdirectory must supply rules for building sources it contributes
MESSAGES/%.o: ../MESSAGES/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


