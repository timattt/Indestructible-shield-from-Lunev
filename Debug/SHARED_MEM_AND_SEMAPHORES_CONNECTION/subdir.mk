################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SHARED_MEM_AND_SEMAPHORES_CONNECTION/Shmem.c 

OBJS += \
./SHARED_MEM_AND_SEMAPHORES_CONNECTION/Shmem.o 

C_DEPS += \
./SHARED_MEM_AND_SEMAPHORES_CONNECTION/Shmem.d 


# Each subdirectory must supply rules for building sources it contributes
SHARED_MEM_AND_SEMAPHORES_CONNECTION/%.o: ../SHARED_MEM_AND_SEMAPHORES_CONNECTION/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


