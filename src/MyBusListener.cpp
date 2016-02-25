#include "MyBusListener.h"

using namespace qcc;
using namespace ajn;
using namespace services;

MyBusListener::MyBusListener(std::weak_ptr<ajn::BusAttachment> _busAtt) :
busAtt(_busAtt)
{
};

void MyBusListener::NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner)
{
	printf("MyBusListener::NameOwnerChanged: %s\n", busName);
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
	printf("MyBusListener::FoundAdvertisedName\n");
	BusListener::FoundAdvertisedName(name, transport, namePrefix);
};
void MyBusListener::LostAdvertisedName(const char* name, ajn::TransportMask transport, const char* namePrefix)
{
	printf("MyBusListener::LostAdvertisedName\n");
	BusListener::LostAdvertisedName(name, transport, namePrefix);
};
void MyBusListener::ListenerRegistered(ajn::BusAttachment* bus)
{
	printf("MyBusListener::ListenerRegistered\n");
	BusListener::ListenerRegistered(bus);
};
void MyBusListener::ListenerUnregistered()
{
	printf("MyBusListener::ListenerUnregistered\n");
	BusListener::ListenerUnregistered();
};
void MyBusListener::BusStopping()
{
	printf("MyBusListener::BusStopping\n");
	BusListener::BusStopping();
};
void MyBusListener::BusDisconnected()
{
	printf("MyBusListener::BusDisconnected\n");
	BusListener::BusDisconnected();
};
void MyBusListener::SessionJoined(ajn::SessionPort sessionPort, ajn::SessionId id, const char* joiner)
{
	printf("MyBusListener::SessionJoined\n");
	SessionPortListener::SessionJoined(sessionPort, id, joiner);
};

void MyBusListener::SessionLost(ajn::SessionId sessionId, ajn::SessionListener::SessionLostReason reason)
{
	printf("MyBusListener::SessionLost\n");
	SessionListener::SessionLost(sessionId, reason);
};
void MyBusListener::SessionMemberAdded(ajn::SessionId sessionId, const char* uniqueName)
{
	printf("MyBusListener::SessionMemberAdded: %s\n", uniqueName);
	SessionListener::SessionMemberAdded(sessionId, uniqueName);
};
void MyBusListener::SessionMemberRemoved(ajn::SessionId sessionId, const char* uniqueName)
{
	printf("MyBusListener::SessionMemberRemoved: %s\n", uniqueName);
	SessionListener::SessionMemberRemoved(sessionId, uniqueName);
};

