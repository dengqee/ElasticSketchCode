################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/CPU/common/common.cc \
../src/CPU/common/os_galoisField.cc \
../src/CPU/common/os_mangler.cc 

CC_DEPS += \
./src/CPU/common/common.d \
./src/CPU/common/os_galoisField.d \
./src/CPU/common/os_mangler.d 

OBJS += \
./src/CPU/common/common.o \
./src/CPU/common/os_galoisField.o \
./src/CPU/common/os_mangler.o 


# Each subdirectory must supply rules for building sources it contributes
src/CPU/common/%.o: ../src/CPU/common/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


