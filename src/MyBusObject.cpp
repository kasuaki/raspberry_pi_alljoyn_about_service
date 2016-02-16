#include "MyBusObject.h"

using namespace qcc;
using namespace ajn;
using namespace services;

MyBusObject::MyBusObject(const char* path, const ajn::InterfaceDescription* _intf) :
	BusObject(path),
	senseProp(false)
{
//	AddInterface(*_intf);
	AddInterface(*_intf, ANNOUNCED);
};

void MyBusObject::SetSenseProp(bool _senseProp)
{
	senseProp = _senseProp;
};

QStatus MyBusObject::Get(const char* ifcName, const char* propName, ajn::MsgArg& val)
{
	QCC_UNUSED(ifcName);

	QStatus status = ER_OK;

	if (0 == strcmp("light", propName)) {

		val.typeId = ALLJOYN_BOOLEAN;
		val.v_bool = senseProp;

	} else {

		status = ER_BUS_NO_SUCH_PROPERTY;
	}

	return status;
};
