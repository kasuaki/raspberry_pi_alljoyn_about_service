#pragma once

#include "Common.h"

class MyNotificationReceiver : public ajn::services::NotificationReceiver
{
private:
	std::function<void(bool)> func;
public:
	MyNotificationReceiver(std::function<void(bool)> _func);
	void Receive(ajn::services::Notification const& notification);
	void Dismiss(const int32_t msgId, const qcc::String appId);
};
