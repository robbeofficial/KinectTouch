################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/oscpack/ip/IpEndpointName.cpp 

OBJS += \
./src/oscpack/ip/IpEndpointName.o 

CPP_DEPS += \
./src/oscpack/ip/IpEndpointName.d 


# Each subdirectory must supply rules for building sources it contributes
src/oscpack/ip/%.o: ../src/oscpack/ip/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/ni -I"../src/oscpack" -I"../src/TUIO" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


