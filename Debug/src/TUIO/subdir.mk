################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/TUIO/TuioClient.cpp \
../src/TUIO/TuioServer.cpp \
../src/TUIO/TuioTime.cpp 

OBJS += \
./src/TUIO/TuioClient.o \
./src/TUIO/TuioServer.o \
./src/TUIO/TuioTime.o 

CPP_DEPS += \
./src/TUIO/TuioClient.d \
./src/TUIO/TuioServer.d \
./src/TUIO/TuioTime.d 


# Each subdirectory must supply rules for building sources it contributes
src/TUIO/%.o: ../src/TUIO/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/ni -I"../src/oscpack" -I"../src/TUIO" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


