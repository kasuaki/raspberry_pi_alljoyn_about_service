#pragma once

#include "Common.h"

class SensorRecvObject : public ajn::BusObject {
protected:
	std::shared_ptr<ajn::BusAttachment> busAtt;
	std::function<void(bool)> func;
public:
	SensorRecvObject(std::shared_ptr<ajn::BusAttachment> _busAtt, const char* path, const ajn::InterfaceDescription& _intf, std::function<void(bool)> _func);
	void signalHandler(const ajn::InterfaceDescription::Member* member, const char* sourcePath, ajn::Message& msg);
};
