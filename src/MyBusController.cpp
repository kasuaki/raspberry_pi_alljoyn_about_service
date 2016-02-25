#include "MyBusController.h"

using namespace qcc;
using namespace ajn;
using namespace services;

MyBusController::MyBusController(std::function<void(int)> _digitalWrite) :
DigitalWrite(_digitalWrite)
{
	AllJoynInit();
	AllJoynRouterInit();

	printf("AllJoyn Library version: %s.\n", ajn::GetVersion());

	log = fopen("log.log", "w+");
	QCC_RegisterOutputFile(log);
	// change loglevel to debug:
	QCC_SetLogLevels("ALL=15");
	QCC_SetLogLevels("ALARM=15");
	QCC_SetLogLevels("ALLJOYN=15");
	QCC_SetLogLevels("ALLJOYN_ABOUT=15");
	QCC_SetLogLevels("ALLJOYN_ABOUT_ANNOUNCE_HANDLER=15");
	QCC_SetLogLevels("ALLJOYN_ABOUT_ANNOUNCEMENT_REGISTRAR=15");
	QCC_SetLogLevels("ALLJOYN_ABOUT_CLIENT=15");
	QCC_SetLogLevels("ALLJOYN_ABOUT_ICON_CLIENT=15");
	QCC_SetLogLevels("ALLJOYN_ABOUT_ICON_SERVICE=15");
	QCC_SetLogLevels("ALLJOYN_ABOUT_PROPERTYSTORE=15");
	QCC_SetLogLevels("ALLJOYN_ABOUT_SERVICE=15");
	QCC_SetLogLevels("ALLJOYN_AUTH=15");
	QCC_SetLogLevels("ALLJOYN_C=15");
	QCC_SetLogLevels("ALLJOYN_DAEMON=15");
	QCC_SetLogLevels("ALLJOYN_JAVA=15");
	QCC_SetLogLevels("ALLJOYN_JS=15");
	QCC_SetLogLevels("ALLJOYN_OBJ=15");
	QCC_SetLogLevels("ALLJOYN_OBSERVER_TEST=15");
	QCC_SetLogLevels("ALLJOYN_PBO=15");
	QCC_SetLogLevels("ALLJOYN_ROUTER=15");
	QCC_SetLogLevels("ALLJOYN_SECURITY=15");
	QCC_SetLogLevels("ARDP=15");
	QCC_SetLogLevels("ARDP_PROTOCOL=15");
	QCC_SetLogLevels("AUTH_KEY_EXCHANGER=15");
	QCC_SetLogLevels("AUTOPINGER=15");
	QCC_SetLogLevels("BASTRESS2=15");
	QCC_SetLogLevels("CONDITION=15");
	QCC_SetLogLevels("CONFIG=15");
	QCC_SetLogLevels("CONVERSATION_HASH=15");
	QCC_SetLogLevels("CONVERT_UTF=15");
	QCC_SetLogLevels("CRYPTO=15");
	QCC_SetLogLevels("DAEMON_SLAP=15");
	QCC_SetLogLevels("DAEMON_TRANSPORT=15");
	QCC_SetLogLevels("DEBUG=15");
	QCC_SetLogLevels("ENDPOINT_AUTH=15");
	QCC_SetLogLevels("ENVIRON=15");
	QCC_SetLogLevels("EVENT=15");
	QCC_SetLogLevels("GUID=15");
	QCC_SetLogLevels("IFCONFIG=15");
	QCC_SetLogLevels("IODISPATCH=15");
	QCC_SetLogLevels("IPNS=15");
	QCC_SetLogLevels("LOCAL_TRANSPORT=15");
	QCC_SetLogLevels("MUTEX=15");
	QCC_SetLogLevels("NETWORK=15");
	QCC_SetLogLevels("NS=15");
	QCC_SetLogLevels("NULL_TRANSPORT=15");
	QCC_SetLogLevels("OBSERVER=15");
	QCC_SetLogLevels("PBO_TEST=15");
	QCC_SetLogLevels("PERMISSION_MGMT=15");
	QCC_SetLogLevels("PERMISSION_MGR=15");
	QCC_SetLogLevels("POLICYDB=15");
	QCC_SetLogLevels("ROUTER=15");
	QCC_SetLogLevels("RWLOCK=15");
	QCC_SetLogLevels("SESSIONLESS=15");
	QCC_SetLogLevels("SLAP=15");
	QCC_SetLogLevels("STATICGLOBALS=15");
	QCC_SetLogLevels("STREAM=15");
	QCC_SetLogLevels("TCP=15");
	QCC_SetLogLevels("TEST=15");
	QCC_SetLogLevels("THREAD=15");
	QCC_SetLogLevels("THREADPOOL=15");
	QCC_SetLogLevels("TIMER=15");
	QCC_SetLogLevels("TRUSTED_TL_SAMPLE_RN=15");
	QCC_SetLogLevels("TXSTATS=15");
	QCC_SetLogLevels("UART=15");
	QCC_SetLogLevels("UDP=15");
	QCC_SetLogLevels("UTIL=15");
	QCC_SetLogLevels("UTILITY=15");
	QCC_SetLogLevels("XML=15");

	QCC_SetDebugLevel(logModules::NOTIFICATION_MODULE_LOG_NAME, logModules::ALL_LOG_LEVELS);
	QCC_SetDebugLevel(logModules::CONTROLPANEL_MODULE_LOG_NAME, logModules::ALL_LOG_LEVELS);
	QCC_SetDebugLevel(logModules::ONBOARDING_MODULE_LOG_NAME, logModules::ALL_LOG_LEVELS);
	QCC_SetDebugLevel(logModules::CONFIG_MODULE_LOG_NAME, logModules::ALL_LOG_LEVELS);
	QCC_SetDebugLevel(logModules::SERVICES_COMMON_MODULE_LOG_NAME, logModules::ALL_LOG_LEVELS);
	QCC_SetDebugLevel(logModules::TIME_MODULE_LOG_NAME, logModules::ALL_LOG_LEVELS);


	busAtt = std::shared_ptr<ajn::BusAttachment>(new ajn::BusAttachment("LightAbout", true));
	busAtt->Start();
	busAtt->Connect();

	busObj = std::unique_ptr<MyBusObject>(new MyBusObject(Const::SERVICE_PATH, createInterfaceDescription(busAtt)));
	busAtt->RegisterBusObject(*busObj);

	busListener = std::unique_ptr<MyBusListener>(new MyBusListener(busAtt));
	busAtt->RegisterBusListener(*busListener);

	aboutData = std::unique_ptr<ajn::AboutData>(getAboutData());

	auto ntfServ = ajn::services::NotificationService::getInstance();

	bindSendNotification = std::bind(&MyBusController::SendNotification, this, std::placeholders::_1);

	aboutListener = std::unique_ptr<MyAboutListener>(new MyAboutListener(busAtt, bindSendNotification));
	busAtt->RegisterAboutListener(*aboutListener);

//	ntfRecv = std::unique_ptr<MyNotificationReceiver>(new MyNotificationReceiver(bindSendNotification));
//	ntfServ->initReceive(busAtt.get(), ntfRecv.get());

	ajn::SessionPort sp = Const::SERVICE_PORT;
	ajn::SessionOpts opts(ajn::SessionOpts::TRAFFIC_MESSAGES, false, ajn::SessionOpts::PROXIMITY_ANY, TRANSPORT_ANY);
	busAtt->BindSessionPort(sp, opts, *busListener);

	ntfSender = std::unique_ptr<ajn::services::NotificationSender>(ntfServ->initSend(busAtt.get(), aboutData.get()));

	aboutObj = std::unique_ptr<AboutObj>(new AboutObj(*busAtt, BusObject::ANNOUNCED));
	aboutObj->Announce(sp, *aboutData);

	busAtt->WhoImplements(NULL);

	// デバッグ用.定期で発振.
//    rxcpp::observable<>::interval(std::chrono::steady_clock::now() + std::chrono::seconds(5), std::chrono::seconds(5), rxcpp::observe_on_new_thread())
//	.subscribe([this](long l){
//		debugB = !debugB;
//		bindSendNotification(debugB);
//	});
};

MyBusController::~MyBusController()
{
	printf("~MyBusController\n");
	for (auto tpl : aboutListener->listenList) {
		printf("tpl: %d\n", std::get<0>(tpl));
		busAtt->LeaveSession(std::get<0>(tpl));
		delete std::get<2>(tpl);

		busAtt->UnregisterBusObject(*(std::get<3>(tpl)));
		delete std::get<3>(tpl);
	}

	ntfSender->deleteLastMsg(NotificationMessageType(INFO));
	ntfSender.reset(nullptr);

	printf("ntfServ->shutdown\n");
	auto ntfServ = ajn::services::NotificationService::getInstance();
	ntfServ->shutdown();

	printf("Unannounce\n");
	aboutObj->Unannounce();
	aboutObj.reset(nullptr);

	printf("UnbindSessionPort\n");
	busAtt->UnbindSessionPort(Const::SERVICE_PORT);

	printf("CancelWhoImplements\n");
	busAtt->CancelWhoImplements(NULL);

	busAtt->CancelFindAdvertisedName(NULL);

	busAtt->UnregisterAboutListener(*aboutListener);
	busAtt->UnregisterBusListener(*busListener);
	busAtt->UnregisterBusObject(*busObj);

	printf("Disconnect\n");
	busAtt->Disconnect();
	printf("Stop\n");
	busAtt->Stop();

	busAtt.reset();
	busListener.reset(nullptr);
	aboutListener.reset(nullptr);
	busObj.reset(nullptr);
	aboutData.reset(nullptr);
//	ntfRecv.reset(nullptr);

	AllJoynRouterShutdown();
	AllJoynShutdown();

	fclose(log);
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
	aboutData->SetAppName("LightApp");
	aboutData->SetManufacturer("自作");
	aboutData->SetModelNumber("0");
	aboutData->SetDescription("ライトON/OFF");
	aboutData->SetDateOfManufacture("2016-01-15");
	aboutData->SetSoftwareVersion("0.0.1");
	aboutData->SetHardwareVersion("0.0.1");
	aboutData->SetSupportUrl("");

	return aboutData;
};

const ajn::InterfaceDescription* MyBusController::createInterfaceDescription(std::shared_ptr<ajn::BusAttachment> _busAtt)
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
	int val = ret ? 1 : 0;
	DigitalWrite(val);
	std::string str(ret ? "true" : "false");
	std::string text("light: " + str);

	/* 現在時刻の取得 */
	time_t timer;
	time(&timer);

	struct tm* localTime = localtime(&timer);
	printf("%d Send Notification(%s): %02d:%02d:%02d\n", ret, str.c_str(), localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

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
