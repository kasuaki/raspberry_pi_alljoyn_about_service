#makefile

CXX:=/usr/bin/arm-linux-gnueabihf-g++
LD:=/usr/bin/arm-linux-gnueabihf-gcc

OBJS:= SensorAbout.o

CXXFLAGS:=-std=gnu++11 -g -march=armv6 -mthumb-interwork -pipe \
          -Wall -Werror -Wl,--fix-cortex-a8 \
          -fno-exceptions -fno-strict-aliasing -fno-asynchronous-unwind-tables -fno-unwind-tables -ffunction-sections -fdata-sections -fexceptions \
          -Wno-long-long -Wno-deprecated -Wno-unknown-pragmas -Wno-unused-parameter -Wno-deprecated-declarations -Wno-return-type \
          -DROUTER -DQCC_OS_GROUP_POSIX -DQCC_OS_LINUX -D_GLIBCXX_USE_C99_FP_MACROS_DYNAMIC \
          -DQCC_CPU_ARM -DNEED_DATA_STORE -DNEED_DATA_STORE -DQCC_USING_BD 

LIBS:=-L${ALLJOYN_DISTDIR}/cpp/lib -L${ALLJOYN_DISTDIR}/config/lib -L${ALLJOYN_DISTDIR}/onboarding/lib -L${ALLJOYN_DISTDIR}/controlpanel/lib -L${ALLJOYN_DISTDIR}/notification/lib  -L${ALLJOYN_DISTDIR}/services_common/lib \
      -lalljoyn_notification -lalljoyn_config -lalljoyn_controlpanel -lalljoyn_onboarding -lalljoyn_about -lalljoyn_services_common -lalljoyn -lajrouter \
      -lrt -lstdc++ -lpthread -lm -lcrypto -lssl -lwiringPi

INCLUDES:=-Iinc -I${ALLJOYN_DISTDIR}/cpp/inc -I${ALLJOYN_DISTDIR}/config/inc -I${ALLJOYN_DISTDIR}/onboarding/inc -I${ALLJOYN_DISTDIR}/controlpanel/inc -I${ALLJOYN_DISTDIR}/notification/inc  -I${ALLJOYN_DISTDIR}/services_common/inc

.PHONY: default
default: all

.PHONY: all
all: SensorAbout

.PHONY: SensorAbout
SensorAbout: $(OBJS)
	$(LD) -o $@ $^ $(LIBS)

.PHONY: SensorAbout.o
SensorAbout.o: SensorAbout.cpp 
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(INCLUDES) 

.PHONY: clean
clean: 
	rm -f *.o SensorAbout
