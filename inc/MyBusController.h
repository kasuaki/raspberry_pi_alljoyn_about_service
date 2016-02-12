#pragma once

#include "Common.h"
#include "MyBusObject.h"
#include "MyBusListener.h"

class MyBusController
{
private:
	ajn::BusAttachment* busAtt = nullptr;
	MyBusObject* busObj = nullptr;
	std::unique_ptr<MyBusListener> busListener;
	ajn::AboutObj* aboutObj = nullptr;
	std::unique_ptr<ajn::AboutData> aboutData;
	ajn::services::NotificationService* ntfServ;
	std::unique_ptr<ajn::services::NotificationSender> ntfSender;
	const ajn::InterfaceDescription::Member* sensed = nullptr;
	std::function<void(int)> DigitalWrite = nullptr;

	ajn::AboutData* getAboutData();
	const ajn::InterfaceDescription* createInterfaceDescription(ajn::BusAttachment* _busAtt);
public:
	MyBusController(std::function<void(int)> _digitalWrite);
	~MyBusController();
	QStatus SendNotification(bool ret);
	void SendSignal(bool ret);
};
