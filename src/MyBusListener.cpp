#include "MyBusListener.h"

using namespace qcc;
using namespace ajn;
using namespace services;

MyBusListener::MyBusListener(ajn::BusAttachment* _busAtt) :
busAtt(_busAtt)
{
};

void MyBusListener::NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner)
{
	if (newOwner && (0 == strcmp(busName, Const::SERVICE_NAME))) {
		printf("NameOwnerChanged: name=%s, oldOwner=%s, newOwner=%s.\n",
			   busName,
			   previousOwner ? previousOwner : "<none>",
			   newOwner ? newOwner : "<none>");
	}
};
bool MyBusListener::AcceptSessionJoiner(ajn::SessionPort sessionPort, const char* joiner, const ajn::SessionOpts& opts)
{
	if (sessionPort != Const::SERVICE_PORT) {
		printf("Rejecting join attempt on unexpected session port %d.\n", sessionPort);
		return false;
	}
	printf("Accepting join session request from %s (opts.proximity=%x, opts.traffic=%x, opts.transports=%x).\n",
		   joiner, opts.proximity, opts.traffic, opts.transports);
	return true;
};
void MyBusListener::FoundAdvertisedName(const char* name, ajn::TransportMask transport, const char* namePrefix)
{
	BusListener::FoundAdvertisedName(name, transport, namePrefix);
};
void MyBusListener::LostAdvertisedName(const char* name, ajn::TransportMask transport, const char* namePrefix)
{
	BusListener::LostAdvertisedName(name, transport, namePrefix);
};
void MyBusListener::ListenerRegistered(ajn::BusAttachment* bus)
{
	BusListener::ListenerRegistered(bus);
};
void MyBusListener::ListenerUnregistered()
{
	BusListener::ListenerUnregistered();
};
void MyBusListener::BusStopping()
{
	BusListener::BusStopping();
};
void MyBusListener::BusDisconnected()
{
	BusListener::BusDisconnected();
};
void MyBusListener::SessionJoined(ajn::SessionPort sessionPort, ajn::SessionId id, const char* joiner)
{
	SessionPortListener::SessionJoined(sessionPort, id, joiner);
};
