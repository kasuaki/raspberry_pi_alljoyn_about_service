#makefile

CXX:=/usr/bin/arm-linux-gnueabihf-g++

CXXFLAGS:=-std=c++11 -g -march=armv6 -mthumb-interwork -pipe \
          -Wall -Werror \
          -fno-exceptions -fno-strict-aliasing -fno-asynchronous-unwind-tables -fno-unwind-tables -ffunction-sections -fdata-sections \
          -Wno-long-long -Wno-deprecated -Wno-unknown-pragmas -Wno-unused-parameter -Wno-deprecated-declarations \
          -DROUTER -DQCC_OS_GROUP_POSIX -DQCC_OS_LINUX -D_GLIBCXX_USE_C99_FP_MACROS_DYNAMIC -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC 

LIBS:=lib/liballjoyn_notification.a lib/liballjoyn_config.a lib/liballjoyn_controlpanel.a lib/liballjoyn_onboarding.a lib/liballjoyn_services_common.a lib/libajrouter.a lib/liballjoyn.a lib/liballjoyn_about.a \
      -lrt -lstdc++ -lpthread -lm -lcrypto -lssl -lwiringPi

INCLUDES:=-Iinc

.PHONY: default
default: all

.PHONY: all
all: SensorAbout

.PHONY: SensorAbout
SensorAbout: SensorAbout.cpp 
	$(CXX) -o $@ $< $(LIBS) $(CXXFLAGS) $(INCLUDES) 

.PHONY: clean
clean: 
	rm -f SensorAbout
