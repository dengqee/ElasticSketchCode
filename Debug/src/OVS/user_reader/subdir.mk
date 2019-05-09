################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/OVS/user_reader/ringbuffer_user.cpp \
../src/OVS/user_reader/user_reader.cpp 

OBJS += \
./src/OVS/user_reader/ringbuffer_user.o \
./src/OVS/user_reader/user_reader.o 

CPP_DEPS += \
./src/OVS/user_reader/ringbuffer_user.d \
./src/OVS/user_reader/user_reader.d 


# Each subdirectory must supply rules for building sources it contributes
src/OVS/user_reader/%.o: ../src/OVS/user_reader/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


