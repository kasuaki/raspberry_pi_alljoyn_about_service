#include "MyBusController.h"

using namespace qcc;
using namespace ajn;
using namespace services;

MyBusController::MyBusController(std::function<void(int)> _digitalWrite) :
DigitalWrite(_digitalWrite)
{
	AllJoynInit();
	//AllJoynRouterInit();

	printf("AllJoyn Library version: %s.\n", ajn::GetVersion());

	busAtt = std::shared_ptr<ajn::BusAttachment>(new ajn::BusAttachment("LightAbout", true));
	busAtt->Start();
	busAtt->Connect();

	aboutListener = std::shared_ptr<MyAboutListener>(new MyAboutListener(busAtt));
	busAtt->RegisterAboutListener(*aboutListener);

	auto nsrv = ajn::services::NotificationService::getInstance();
	notificationReceiver = std::unique_ptr<MyNotificationReceiver>(new MyNotificationReceiver([](int b) {}));
	nsrv->initReceive(busAtt.get(), notificationReceiver.get());

	const ajn::InterfaceDescription* intf = createInterfaceDescription(busAtt.get());

	busListener = std::unique_ptr<MyBusListener>(new MyBusListener(busAtt.get()));
	busAtt->RegisterBusListener(*busListener);

	busObj = new MyBusObject(Const::SERVICE_PATH, intf);
	busAtt->RegisterBusObject(*busObj);

	ntfServ = ajn::services::NotificationService::getInstance();
	QCC_SetDebugLevel(logModules::NOTIFICATION_MODULE_LOG_NAME, logModules::ALL_LOG_LEVELS);

	ajn::SessionPort sp = Const::SERVICE_PORT;
	ajn::SessionOpts opts(ajn::SessionOpts::TRAFFIC_MESSAGES, false, ajn::SessionOpts::PROXIMITY_ANY, TRANSPORT_ANY);
	busAtt->BindSessionPort(sp, opts, *busListener);

	aboutData = std::unique_ptr<ajn::AboutData>(getAboutData());

	ntfSender = std::unique_ptr<ajn::services::NotificationSender>(ntfServ->initSend(busAtt.get(), aboutData.get()));

	aboutObj = new AboutObj(*busAtt, BusObject::ANNOUNCED);
	aboutObj->Announce(sp, *aboutData);
	busAtt->WhoImplements(NULL);
};

MyBusController::~MyBusController()
{
	ntfServ->shutdown();
	ntfServ = nullptr;
	aboutObj->Unannounce();
	delete aboutObj;
	aboutObj = nullptr;
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
	                    0x85, 0x8D, 0x85, 0x3C };
	aboutData->SetAppId(appId, 16);
	aboutData->SetDeviceName("Lightデバイス");
	aboutData->SetDeviceId("930b90a1-3d46-4342-ba3a-5ec8c5619572");
	aboutData->SetAppName("Lightアプリ");
	aboutData->SetManufacturer("自作");
	aboutData->SetModelNumber("0");
	aboutData->SetDescription("ライトON/OFF");
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
	"<signal name='lit' sessionless='false'>"
		"<arg type='b'/>"
	"</signal>"
	"<property name='light' type='b' access='read'/>"
	"</interface>"
	"</node>";

	_busAtt->CreateInterfacesFromXml(iface.c_str());
    const ajn::InterfaceDescription* intf = _busAtt->GetInterface(Const::INTERFACE_NAME);

	lit = intf->GetSignal("lit");

	return intf;
};

QStatus MyBusController::SendNotification(bool ret)
{
	std::string str(ret ? "true" : "false");
	std::string text("light: " + str);

	/* 現在時刻の取得 */
	time_t timer;
	time(&timer);

	struct tm* localTime = localtime(&timer);
	printf("Send Notification(%s): %02d:%02d:%02d\n", str.c_str(), localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

	std::vector<NotificationText> vecMessages = {
		NotificationText("ja", qcc::String(text.c_str(), text.size()))
	};

	std::map<qcc::String, qcc::String> customAttributes = {
		{ "light",  qcc::String(str.c_str(), str.size()) },
	};

	Notification notification(NotificationMessageType::INFO, vecMessages);
	notification.setCustomAttributes(customAttributes);

	return ntfSender->send(notification, 7200);
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
					*lit,						// Interface member of signal being emitted.
					arg, 							// The arguments for the signal (can be NULL).
					1, 								// The number of arguments.
					0, 								// timeToLive.
					ALLJOYN_FLAG_GLOBAL_BROADCAST	// broadcast signal (null destination) will be forwarded to all Routing Nodes in the system.
	);
};
