#pragma once

#include "Common.h"
#include "MyBusListener.h"

class MyAboutListener : public ajn::AboutListener
{
private:
	std::shared_ptr<ajn::BusAttachment> busAtt;
	std::vector<std::tuple<const char*, ajn::SessionListener*, ajn::BusObject*>> listenList;
	std::function<void(bool)> func;

public:
	MyAboutListener(std::shared_ptr<ajn::BusAttachment> _busAtt, std::function<void(bool)> _func);
	void Announced(const char* busName, uint16_t version, ajn::SessionPort port, const ajn::MsgArg& objectDescriptionArg, const ajn::MsgArg& aboutDataArg);
};
