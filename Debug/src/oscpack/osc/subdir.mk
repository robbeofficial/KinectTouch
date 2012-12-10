################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/oscpack/osc/OscOutboundPacketStream.cpp \
../src/oscpack/osc/OscPrintReceivedElements.cpp \
../src/oscpack/osc/OscReceivedElements.cpp \
../src/oscpack/osc/OscTypes.cpp 

OBJS += \
./src/oscpack/osc/OscOutboundPacketStream.o \
./src/oscpack/osc/OscPrintReceivedElements.o \
./src/oscpack/osc/OscReceivedElements.o \
./src/oscpack/osc/OscTypes.o 

CPP_DEPS += \
./src/oscpack/osc/OscOutboundPacketStream.d \
./src/oscpack/osc/OscPrintReceivedElements.d \
./src/oscpack/osc/OscReceivedElements.d \
./src/oscpack/osc/OscTypes.d 


# Each subdirectory must supply rules for building sources it contributes
src/oscpack/osc/%.o: ../src/oscpack/osc/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/ni -I"../src/oscpack" -I"../src/TUIO" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


