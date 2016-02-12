#include "MyBusController.h"

using namespace qcc;
using namespace ajn;
using namespace services;

MyBusController::MyBusController(std::function<void(int)> _digitalWrite) :
DigitalWrite(_digitalWrite)
{
	auto sbj = rxcpp::subjects::subject<bool>();
	auto sub = sbj.get_subscriber();

	auto source = sbj.get_observable();

	rxcpp::observable<>::timer(std::chrono::minutes(3))
		.subscribe([&](long l){
			this->SendNotification(true);
		});

    source
	.subscribe([&](bool b){
		printf("onNext:%d", b);
		this->SendSignal(b);
	});

	AllJoynInit();
	//AllJoynRouterInit();

	printf("AllJoyn Library version: %s.\n", ajn::GetVersion());

	busAtt = new ajn::BusAttachment("SensorAbout", true);

	busListener = std::unique_ptr<MyBusListener>(new MyBusListener(busAtt));
	busAtt->RegisterBusListener(*busListener);

	busAtt->Start();

	const ajn::InterfaceDescription* intf = createInterfaceDescription(busAtt);

	busObj = new MyBusObject(Const::SERVICE_PATH, intf);
	busAtt->RegisterBusObject(*busObj);

	busAtt->Connect();

	ntfServ = ajn::services::NotificationService::getInstance();
	QCC_SetDebugLevel(logModules::NOTIFICATION_MODULE_LOG_NAME, logModules::ALL_LOG_LEVELS);

	ajn::SessionPort sp = Const::SERVICE_PORT;
	ajn::SessionOpts opts(ajn::SessionOpts::TRAFFIC_MESSAGES, false, ajn::SessionOpts::PROXIMITY_ANY, TRANSPORT_ANY);
	busAtt->BindSessionPort(sp, opts, *busListener);

	aboutData = std::unique_ptr<ajn::AboutData>(getAboutData());

	ntfSender = std::unique_ptr<ajn::services::NotificationSender>(ntfServ->initSend(busAtt, aboutData.get()));

	aboutObj = new AboutObj(*busAtt);
	aboutObj->Announce(sp, *aboutData);
};

MyBusController::~MyBusController()
{
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
};

ajn::AboutData* MyBusController::getAboutData()
{
	ajn::AboutData* aboutData = new AboutData("jp");
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
};

const ajn::InterfaceDescription* MyBusController::createInterfaceDescription(ajn::BusAttachment* _busAtt)
{
	qcc::String iface = "<node name='" + qcc::String(Const::SERVICE_PATH) + "'>"
	"<interface name='" + qcc::String(Const::INTERFACE_NAME) + "'>"
	"<signal name='sensord' sessionless='false'>"
		"<arg type='b'/>"
	"</signal>"
	"<property name='sensor' type='b' access='read'/>"
	"</interface>"
	"</node>";

	_busAtt->CreateInterfacesFromXml(iface.c_str());
    const ajn::InterfaceDescription* intf = _busAtt->GetInterface(Const::INTERFACE_NAME);

	sensed = intf->GetSignal("sensed");

	return intf;
};

QStatus MyBusController::SendNotification(bool ret)
{
	NotificationMessageType messageType = INFO;

	std::vector<NotificationText> vecMessages;
	NotificationText textToSend1("ja", "test");
	vecMessages.push_back(textToSend1);


	std::map<qcc::String, qcc::String> customAttributes = {
		{ "sensor",  ret ? "true" : "false" },
	};

	Notification notification(messageType, vecMessages);
	notification.setCustomAttributes(customAttributes);

	return ntfSender->send(notification, 0);
};

void MyBusController::SendSignal(bool ret)
{
	busObj->SetSenseProp(ret);

	ajn::MsgArg* arg = new MsgArg(ALLJOYN_BOOLEAN);
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
};
