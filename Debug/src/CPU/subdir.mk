################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CPU/testhash.cpp \
../src/CPU/throughput.cpp 

OBJS += \
./src/CPU/testhash.o \
./src/CPU/throughput.o 

CPP_DEPS += \
./src/CPU/testhash.d \
./src/CPU/throughput.d 


# Each subdirectory must supply rules for building sources it contributes
src/CPU/%.o: ../src/CPU/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


