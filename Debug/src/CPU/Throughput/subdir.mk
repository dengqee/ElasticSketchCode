################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CPU/Throughput/cmsketch-throughput.cpp \
../src/CPU/Throughput/countsketch-throughput.cpp \
../src/CPU/Throughput/cusketch-throughput.cpp \
../src/CPU/Throughput/elastic-throughput.cpp \
../src/CPU/Throughput/spacesaving-throughput.cpp \
../src/CPU/Throughput/tcamsketch-throughput.cpp 

OBJS += \
./src/CPU/Throughput/cmsketch-throughput.o \
./src/CPU/Throughput/countsketch-throughput.o \
./src/CPU/Throughput/cusketch-throughput.o \
./src/CPU/Throughput/elastic-throughput.o \
./src/CPU/Throughput/spacesaving-throughput.o \
./src/CPU/Throughput/tcamsketch-throughput.o 

CPP_DEPS += \
./src/CPU/Throughput/cmsketch-throughput.d \
./src/CPU/Throughput/countsketch-throughput.d \
./src/CPU/Throughput/cusketch-throughput.d \
./src/CPU/Throughput/elastic-throughput.d \
./src/CPU/Throughput/spacesaving-throughput.d \
./src/CPU/Throughput/tcamsketch-throughput.d 


# Each subdirectory must supply rules for building sources it contributes
src/CPU/Throughput/%.o: ../src/CPU/Throughput/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


