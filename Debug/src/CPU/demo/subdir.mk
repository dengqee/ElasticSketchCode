################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CPU/demo/cmheap.cpp \
../src/CPU/demo/cmsketch.cpp \
../src/CPU/demo/countheap.cpp \
../src/CPU/demo/countsketch.cpp \
../src/CPU/demo/css.cpp \
../src/CPU/demo/cusketch.cpp \
../src/CPU/demo/elastic.cpp \
../src/CPU/demo/flowradar.cpp \
../src/CPU/demo/hashpipe.cpp \
../src/CPU/demo/linearcounting.cpp \
../src/CPU/demo/mrac.cpp \
../src/CPU/demo/reversiblesketch.cpp \
../src/CPU/demo/sieving.cpp \
../src/CPU/demo/spacesaving.cpp \
../src/CPU/demo/univmon.cpp 

O_SRCS += \
../src/CPU/demo/common.o \
../src/CPU/demo/os_galoisField.o \
../src/CPU/demo/os_mangler.o 

OBJS += \
./src/CPU/demo/cmheap.o \
./src/CPU/demo/cmsketch.o \
./src/CPU/demo/countheap.o \
./src/CPU/demo/countsketch.o \
./src/CPU/demo/css.o \
./src/CPU/demo/cusketch.o \
./src/CPU/demo/elastic.o \
./src/CPU/demo/flowradar.o \
./src/CPU/demo/hashpipe.o \
./src/CPU/demo/linearcounting.o \
./src/CPU/demo/mrac.o \
./src/CPU/demo/reversiblesketch.o \
./src/CPU/demo/sieving.o \
./src/CPU/demo/spacesaving.o \
./src/CPU/demo/univmon.o 

CPP_DEPS += \
./src/CPU/demo/cmheap.d \
./src/CPU/demo/cmsketch.d \
./src/CPU/demo/countheap.d \
./src/CPU/demo/countsketch.d \
./src/CPU/demo/css.d \
./src/CPU/demo/cusketch.d \
./src/CPU/demo/elastic.d \
./src/CPU/demo/flowradar.d \
./src/CPU/demo/hashpipe.d \
./src/CPU/demo/linearcounting.d \
./src/CPU/demo/mrac.d \
./src/CPU/demo/reversiblesketch.d \
./src/CPU/demo/sieving.d \
./src/CPU/demo/spacesaving.d \
./src/CPU/demo/univmon.d 


# Each subdirectory must supply rules for building sources it contributes
src/CPU/demo/%.o: ../src/CPU/demo/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


