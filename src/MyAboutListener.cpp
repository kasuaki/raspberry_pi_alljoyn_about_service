#include "MyAboutListener.h"

using namespace qcc;
using namespace ajn;

MyAboutListener::MyAboutListener(std::shared_ptr<ajn::BusAttachment> _busAtt) :
busAtt(_busAtt)
{
}

void MyAboutListener::Announced(const char* busName, uint16_t version, ajn::SessionPort port, const ajn::MsgArg& objectDescriptionArg, const ajn::MsgArg& aboutDataArg)
{
	QCC_UNUSED(busName);
	QCC_UNUSED(version);
	QCC_UNUSED(port);
	QCC_UNUSED(objectDescriptionArg);
	QCC_UNUSED(aboutDataArg);
}
