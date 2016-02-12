#pragma once

#include "Common.h"

class MyBusObject : public ajn::BusObject
{
  private:
	bool senseProp;
  public:
	MyBusObject(const char* path, const ajn::InterfaceDescription* _intf);
	void SetSenseProp(bool _senseProp);
	QStatus Get(const char* ifcName, const char* propName, ajn::MsgArg& val);
};
