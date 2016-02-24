#makefile

CXX:=/usr/bin/arm-linux-gnueabihf-g++
LD:=/usr/bin/arm-linux-gnueabihf-gcc

OBJS:= LightAbout.o MyBusObject.o MyBusListener.o MyBusController.o MyAboutListener.o MyNotificationReceiver.o SensorRecvObject.o

CXXFLAGS:=-std=gnu++11  -march=armv6 -mthumb-interwork -pipe \
          -Wall -Werror -Wl,--fix-cortex-a8 \
          -fno-exceptions -fno-strict-aliasing -fno-asynchronous-unwind-tables -fno-unwind-tables -ffunction-sections -fdata-sections -fexceptions \
          -Wno-long-long -Wno-deprecated -Wno-unknown-pragmas -Wno-unused-parameter -Wno-deprecated-declarations -Wno-return-type \
          -DROUTER -DQCC_OS_GROUP_POSIX -DQCC_OS_LINUX -D_GLIBCXX_USE_C99_FP_MACROS_DYNAMIC \
          -DQCC_CPU_ARM -DNEED_DATA_STORE -DQCC_USING_BD 

LIBS:=-L${ALLJOYN_DISTDIR}/cpp/lib -L${ALLJOYN_DISTDIR}/config/lib -L${ALLJOYN_DISTDIR}/onboarding/lib -L${ALLJOYN_DISTDIR}/controlpanel/lib -L${ALLJOYN_DISTDIR}/notification/lib  -L${ALLJOYN_DISTDIR}/services_common/lib \
      -lalljoyn_notification -lalljoyn_config -lalljoyn_controlpanel -lalljoyn_onboarding -lalljoyn_about -lalljoyn_services_common -lalljoyn -lajrouter \
      -lrt -lstdc++ -lpthread -lm -lcrypto -lssl -lwiringPi

INCLUDES:=-Iinc -I${ALLJOYN_DISTDIR}/cpp/inc -I${ALLJOYN_DISTDIR}/config/inc -I${ALLJOYN_DISTDIR}/onboarding/inc -I${ALLJOYN_DISTDIR}/controlpanel/inc -I${ALLJOYN_DISTDIR}/notification/inc  -I${ALLJOYN_DISTDIR}/services_common/inc

.PHONY: default
default: all

.PHONY: all
all: LightAbout

#.PHONY: LightAbout
LightAbout: $(OBJS)
	$(LD) -o $@ $^ $(LIBS)

#.PHONY: LightAbout.o
LightAbout.o: LightAbout.cpp 
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(INCLUDES) 

#.PHONY: MyBusObject.o
MyBusObject.o: src/MyBusObject.cpp 
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(INCLUDES) 

#.PHONY: MyBusListener.o
MyBusListener.o: src/MyBusListener.cpp 
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(INCLUDES) 

#.PHONY: MyBusController.o
MyBusController.o: src/MyBusController.cpp 
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(INCLUDES) 

#.PHONY: MyAboutListener.o
MyAboutListener.o: src/MyAboutListener.cpp 
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(INCLUDES) 

#.PHONY: MyNotificationReceiver.o
MyNotificationReceiver.o: src/MyNotificationReceiver.cpp 
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(INCLUDES) 

#.PHONY: SensorRecvObject.o
SensorRecvObject.o: src/SensorRecvObject.cpp 
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(INCLUDES) 

.PHONY: clean
clean: 
	rm -f *.o LightAbout
