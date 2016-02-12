#include "MyNotificationReceiver.h"

using namespace qcc;
using namespace ajn;
using namespace services;

MyNotificationReceiver::MyNotificationReceiver(std::function<void(bool)> _func) :
func(_func) {
}

void MyNotificationReceiver::Receive(Notification const& notification)
{
	auto appName = notification.getAppName();

	if (strcmp(appName, "SensorApp") == 0) {
		try {

			std::map<qcc::String, qcc::String> customAttributes = notification.getCustomAttributes();
			bool val = cpplinq::from(customAttributes)
				.where([](std::pair<qcc::String, qcc::String> p) { return strcmp(p.first.c_str(), "sensor") == 0; })
				.select([](std::pair<qcc::String, qcc::String> p) { return (strcmp(p.second.c_str(), "true") == 0) ? true : false; })
				.first();

			func(val);
		} catch (std::logic_error) {
		}
	}
}

void MyNotificationReceiver::Dismiss(const int32_t msgId, const qcc::String appId)
{
	QCC_UNUSED(msgId);
	QCC_UNUSED(appId);
}
