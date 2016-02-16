#pragma once

#include "Common.h"
#include "MyBusObject.h"
#include "MyBusListener.h"
#include "MyAboutListener.h"
#include "MyNotificationReceiver.h"

class MyBusController
{
private:
	std::shared_ptr<ajn::BusAttachment> busAtt;
	std::unique_ptr<MyBusObject> busObj;
	std::unique_ptr<MyBusListener> busListener;
	std::unique_ptr<ajn::AboutObj> aboutObj;
	std::unique_ptr<ajn::AboutData> aboutData;
	std::unique_ptr<MyAboutListener> aboutListener;
	std::unique_ptr<MyNotificationReceiver> ntfRecv;
	ajn::services::NotificationSender* ntfSender;
	const ajn::InterfaceDescription::Member* lit = nullptr;

	std::function<void(int)> DigitalWrite;
	std::function<void(bool)> bindSendNotification;

	ajn::AboutData* getAboutData();
	const ajn::InterfaceDescription* createInterfaceDescription(std::shared_ptr<ajn::BusAttachment> _busAtt);

	bool debugB = false;
public:
	MyBusController(std::function<void(int)> _digitalWrite);
	~MyBusController();
	QStatus SendNotification(bool ret);
	void SendSignal(bool ret);
};
