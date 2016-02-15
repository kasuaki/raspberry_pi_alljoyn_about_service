#pragma once

#include "Common.h"
#include "MyBusListener.h"

class MyAboutListener : public ajn::AboutListener
{
private:
	std::weak_ptr<ajn::BusAttachment> busAtt;
	std::vector<std::tuple<const char*, MyBusListener*>> listenList;

public:
	MyAboutListener(std::weak_ptr<ajn::BusAttachment> _busAtt);
	void Announced(const char* busName, uint16_t version, ajn::SessionPort port, const ajn::MsgArg& objectDescriptionArg, const ajn::MsgArg& aboutDataArg);
};
