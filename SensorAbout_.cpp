#include <wiringPi.h>

#include <qcc/platform.h>

#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <memory>

#include <cpplinq/linq.hpp>
#include <rxcpp/rx.hpp>

#include <qcc/Log.h>
#include <qcc/String.h>

#include <alljoyn/AllJoynStd.h>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/AboutObj.h>
#include <alljoyn/Init.h>
#include <alljoyn/Status.h>
#include <alljoyn/version.h>

#include <alljoyn/services_common/AsyncTaskQueue.h>
#include <alljoyn/services_common/GuidUtil.h>
#include <alljoyn/services_common/LogModule.h>
#include <alljoyn/services_common/LogModulesNames.h>
#include <alljoyn/services_common/ServicesCommonConstants.h>

#include <alljoyn/controlpanel/Action.h>
#include <alljoyn/controlpanel/ActionWithDialog.h>
#include <alljoyn/controlpanel/ConstraintList.h>
#include <alljoyn/controlpanel/ConstraintRange.h>
#include <alljoyn/controlpanel/Container.h>
#include <alljoyn/controlpanel/ControlPanel.h>
#include <alljoyn/controlpanel/ControlPanelBusListener.h>
#include <alljoyn/controlpanel/ControlPanelControllee.h>
#include <alljoyn/controlpanel/ControlPanelControlleeUnit.h>
#include <alljoyn/controlpanel/ControlPanelController.h>
#include <alljoyn/controlpanel/ControlPanelControllerUnit.h>
#include <alljoyn/controlpanel/ControlPanelDevice.h>
#include <alljoyn/controlpanel/ControlPanelEnums.h>
#include <alljoyn/controlpanel/ControlPanelListener.h>
#include <alljoyn/controlpanel/ControlPanelService.h>
#include <alljoyn/controlpanel/ControlPanelSessionHandler.h>
#include <alljoyn/controlpanel/CPSDate.h>
#include <alljoyn/controlpanel/CPSTime.h>
#include <alljoyn/controlpanel/Dialog.h>
#include <alljoyn/controlpanel/ErrorWidget.h>
#include <alljoyn/controlpanel/HttpControl.h>
#include <alljoyn/controlpanel/Label.h>
#include <alljoyn/controlpanel/LanguageSet.h>
#include <alljoyn/controlpanel/LanguageSets.h>
#include <alljoyn/controlpanel/LogModule.h>
#include <alljoyn/controlpanel/NotificationAction.h>
#include <alljoyn/controlpanel/Property.h>
#include <alljoyn/controlpanel/RootWidget.h>
#include <alljoyn/controlpanel/Widget.h>

#include <alljoyn/notification/LogModule.h>
#include <alljoyn/notification/Notification.h>
#include <alljoyn/notification/NotificationAsyncTaskEvents.h>
#include <alljoyn/notification/NotificationEnums.h>
#include <alljoyn/notification/NotificationReceiver.h>
#include <alljoyn/notification/NotificationSender.h>
#include <alljoyn/notification/NotificationService.h>
#include <alljoyn/notification/NotificationText.h>
#include <alljoyn/notification/RichAudioUrl.h>

#include <alljoyn/config/AboutDataStoreInterface.h>
#include <alljoyn/config/ConfigClient.h>
#include <alljoyn/config/ConfigService.h>
#include <alljoyn/config/LogModule.h>

#include <alljoyn/onboarding/LogModule.h>
#include <alljoyn/onboarding/Onboarding.h>
#include <alljoyn/onboarding/OnboardingClient.h>
#include <alljoyn/onboarding/OnboardingClientListener.h>
#include <alljoyn/onboarding/OnboardingControllerAPI.h>
#include <alljoyn/onboarding/OnboardingService.h>

using namespace std;
using namespace qcc;
using namespace ajn;
using namespace services;

static const char* INTERFACE_NAME = "org.alljoyn.SensorLightCamera.Sensor";
static const char* SERVICE_NAME = "org.alljoyn.SensorLightCamera";
static const char* SERVICE_PATH = "/org/alljoyn/SensorLightCamera/Sensor";
static const SessionPort SERVICE_PORT = 25;

class HumanSensor
{
private:
	const int pin_22 = 6;	   // rsp board pin:22

	rxcpp::subscriber<bool, rxcpp::observer<bool, rxcpp::subjects::detail::multicast_observer<bool>, void, void, void>>* sub;

	// 静的からローカルに変更.
	static HumanSensor* thisPtr;
	static void staticSig()
	{
		thisPtr->sig();
	}
	void sig()
	{
		printf("sig: %d \n", digitalRead(pin_22));
		sub->on_next(digitalRead(pin_22));
	}
public:
	HumanSensor(rxcpp::subscriber<bool, rxcpp::observer<bool, rxcpp::subjects::detail::multicast_observer<bool>, void, void, void>>* _sub) :
	sub(_sub)
	{
		thisPtr = this;

		wiringPiSetup();

		pinMode(pin_22, INPUT);
		pullUpDnControl(pin_22, PUD_DOWN);
		wiringPiISR(pin_22, INT_EDGE_BOTH, &HumanSensor::staticSig);
	}
	~HumanSensor()
	{
	}
};
HumanSensor* HumanSensor::thisPtr;

class MyBusObject : public BusObject {
  private:
	bool senseProp;
  public:
	MyBusObject(const char* path, const InterfaceDescription* _intf) :
		BusObject(path),
		senseProp(false)
	{
//		AddInterface(*_intf);
		AddInterface(*_intf, ANNOUNCED);
	}

	void SetSenseProp(bool _senseProp)
	{
		senseProp = _senseProp;
	}

	QStatus Get(const char* ifcName, const char* propName, MsgArg& val)
	{
		QCC_UNUSED(ifcName);

		QStatus status = ER_OK;

		if (0 == strcmp("sense", propName)) {

			val.typeId = ALLJOYN_BOOLEAN;
			val.v_bool = senseProp;

		} else {

			status = ER_BUS_NO_SUCH_PROPERTY;
		}

		return status;
	}
};

class MyBusListener : public BusListener, public SessionPortListener {
private:
	BusAttachment* busAtt;
	String sessionHost;
public:
	MyBusListener(BusAttachment* _busAtt)
	{
		busAtt = _busAtt;
	}

	void NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner)
	{
		if (newOwner && (0 == strcmp(busName, SERVICE_NAME))) {
			printf("NameOwnerChanged: name=%s, oldOwner=%s, newOwner=%s.\n",
				   busName,
				   previousOwner ? previousOwner : "<none>",
				   newOwner ? newOwner : "<none>");
		}
	}
	bool AcceptSessionJoiner(SessionPort sessionPort, const char* joiner, const SessionOpts& opts)
	{
		if (sessionPort != SERVICE_PORT) {
			printf("Rejecting join attempt on unexpected session port %d.\n", sessionPort);
			return false;
		}
		printf("Accepting join session request from %s (opts.proximity=%x, opts.traffic=%x, opts.transports=%x).\n",
			   joiner, opts.proximity, opts.traffic, opts.transports);
		return true;
	}
	void FoundAdvertisedName(const char* name, TransportMask transport, const char* namePrefix)
	{
		BusListener::FoundAdvertisedName(name, transport, namePrefix);
	}
	void LostAdvertisedName(const char* name, TransportMask transport, const char* namePrefix)
	{
		BusListener::LostAdvertisedName(name, transport, namePrefix);
	}
	void ListenerRegistered(BusAttachment* bus)
	{
		BusListener::ListenerRegistered(bus);
	}
	void ListenerUnregistered()
	{
		BusListener::ListenerUnregistered();
	}
	void BusStopping()
	{
		BusListener::BusStopping();
	}
	void BusDisconnected()
	{
		BusListener::BusDisconnected();
	}
	void SessionJoined(SessionPort sessionPort, SessionId id, const char* joiner)
	{
		SessionPortListener::SessionJoined(sessionPort, id, joiner);
	}
};

class SensorBus
{
private:
	BusAttachment* busAtt = nullptr;
	MyBusObject* busObj = nullptr;
	unique_ptr<MyBusListener> busListener;
	AboutObj* aboutObj = nullptr;
	unique_ptr<AboutData> aboutData;
	NotificationService* ntfServ;
	unique_ptr<NotificationSender> ntfSender;
	const InterfaceDescription::Member* sensed = nullptr;
	unique_ptr<HumanSensor> sensor;

	AboutData* getAboutData()
	{
		AboutData* aboutData = new AboutData("jp");
		uint8_t appId[] = { 0xDB, 0xC5, 0xB5, 0xA2,
		                    0x5D, 0x9D, 0x4C, 0x9E,
		                    0x86, 0xD6, 0x94, 0x95,
		                    0x85, 0x8D, 0x85, 0x3B };
		aboutData->SetAppId(appId, 16);
		aboutData->SetDeviceName("Sensorデバイス");
		aboutData->SetDeviceId("930b90a1-3d46-4342-ba3a-5ec8c5619571");
		aboutData->SetAppName("Sensorアプリ");
		aboutData->SetManufacturer("自作");
		aboutData->SetModelNumber("0");
		aboutData->SetDescription("人感センサーです。");
		aboutData->SetDateOfManufacture("2016-01-15");
		aboutData->SetSoftwareVersion("0.0.1");
		aboutData->SetHardwareVersion("0.0.1");
		aboutData->SetSupportUrl("");

		return aboutData;
	}

	const InterfaceDescription* createInterfaceDescription(BusAttachment* _busAtt)
	{
		qcc::String iface = "<node name='" + qcc::String(SERVICE_PATH) + "'>"
		"<interface name='" + qcc::String(INTERFACE_NAME) + "'>"
		"<signal name='sensord' sessionless='false'>"
			"<arg type='b'/>"
		"</signal>"
		"<property name='sensor' type='b' access='read'/>"
		"</interface>"
		"</node>";

		_busAtt->CreateInterfacesFromXml(iface.c_str());
        const InterfaceDescription* intf = _busAtt->GetInterface(INTERFACE_NAME);

		sensed = intf->GetSignal("sensed");

		return intf;
	}

	public:
	SensorBus()
	{
	}

	void ThreadStart()
	{
		auto sbj = rxcpp::subjects::subject<bool>();
		auto sub = sbj.get_subscriber();
		sensor = unique_ptr<HumanSensor>(new HumanSensor(&sub));

		auto source = sbj.get_observable();
		auto trigger = rxcpp::observable<>::timer(std::chrono::minutes(30));

		time_t currentTime;
		time(&currentTime);
		auto diffPublishTime = source.filter([&](bool b){
			time_t tmpTime;
			time(&tmpTime);
			auto diffSecond = difftime(tmpTime, currentTime);
			printf("diffTime:%f, value:%d", diffSecond, b);
			currentTime = tmpTime;
			return diffSecond > 60;
		}).map([](bool b){ return (long)1; });

		// 30分後か、イベントの発行間隔が1分以上開いたらスキップ解除.
	    source.skip_until(trigger.merge(diffPublishTime))
		.subscribe([&](bool b){
			printf("onNext:%d", b);
			this->SendSignal(b);
		});

		AllJoynInit();
		//AllJoynRouterInit();

		printf("AllJoyn Library version: %s.\n", ajn::GetVersion());

		busAtt = new BusAttachment("SensorAbout", true);

		busListener = unique_ptr<MyBusListener>(new MyBusListener(busAtt));
		busAtt->RegisterBusListener(*busListener);

		busAtt->Start();

		const InterfaceDescription* intf = createInterfaceDescription(busAtt);

		busObj = new MyBusObject(SERVICE_PATH, intf);
		busAtt->RegisterBusObject(*busObj);

		busAtt->Connect();

		ntfServ = NotificationService::getInstance();
		QCC_SetDebugLevel(logModules::NOTIFICATION_MODULE_LOG_NAME, logModules::ALL_LOG_LEVELS);

		SessionPort sp = SERVICE_PORT;
		SessionOpts opts(SessionOpts::TRAFFIC_MESSAGES, false, SessionOpts::PROXIMITY_ANY, TRANSPORT_ANY);
		busAtt->BindSessionPort(sp, opts, *busListener);

		aboutData = unique_ptr<AboutData>(getAboutData());

		ntfSender = unique_ptr<NotificationSender>(ntfServ->initSend(busAtt, aboutData.get()));

		aboutObj = new AboutObj(*busAtt);
		aboutObj->Announce(sp, *aboutData);

		while(1)
	        this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	~SensorBus()
	{
		sensor.reset();
		ntfServ->shutdown();
		ntfServ = nullptr;
		aboutObj->Unannounce();
		delete aboutObj;
		aboutObj = nullptr;
		delete busAtt;
		busAtt = nullptr;
		delete busObj;
		busObj = nullptr;

		//AllJoynRouterShutdown();
		AllJoynShutdown();
	}

	void SendSignal(bool ret)
	{
		busObj->SetSenseProp(ret);

		MsgArg* arg = new MsgArg(ALLJOYN_BOOLEAN);
		arg->v_bool = ret;

		time_t timer;

		/* 現在時刻の取得 */
		time(&timer);

		struct tm* localTime = localtime(&timer);

		printf("Send Signal(%d): %02d:%02d:%02d\n", ret, localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

		busObj->Signal(NULL,							// NULL for broadcast signals.
						0,								// For broadcast or sessionless signals, the sessionId must be 0.
						*sensed,						// Interface member of signal being emitted.
						arg, 							// The arguments for the signal (can be NULL).
						1, 								// The number of arguments.
						0, 								// timeToLive.
						ALLJOYN_FLAG_GLOBAL_BROADCAST	// broadcast signal (null destination) will be forwarded to all Routing Nodes in the system.
		);
	}
};

thread* t;
// メイン処理.
int CDECL_CALL main(int argc, char** argv, char** envArg)
{
	QCC_UNUSED(argc);
	QCC_UNUSED(argv);
	QCC_UNUSED(envArg);

	printf("start¥n");
	SensorBus* bus = new SensorBus();

	t = new thread([bus](){
		printf("thred srsrt¥n");
		bus->ThreadStart();
	});

	// ctrl-c受付.
	signal(SIGINT, [](int sig) {
		printf("detatch");
		t->detach();
	});

	printf("join¥n");
	t->join();

	delete t;
	delete bus;
	printf("end¥n");
	return 0;
}
