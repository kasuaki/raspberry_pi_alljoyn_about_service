#pragma once

#include "Common.h"

class MyBusListener : public ajn::BusListener, public ajn::SessionPortListener, public ajn::SessionListener
{
private:
	std::weak_ptr<ajn::BusAttachment> busAtt;
public:
	MyBusListener(std::weak_ptr<ajn::BusAttachment> _busAtt);
	void NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner);
	bool AcceptSessionJoiner(ajn::SessionPort sessionPort, const char* joiner, const ajn::SessionOpts& opts);
	void FoundAdvertisedName(const char* name, ajn::TransportMask transport, const char* namePrefix);
	void LostAdvertisedName(const char* name, ajn::TransportMask transport, const char* namePrefix);
	void ListenerRegistered(ajn::BusAttachment* bus);
	void ListenerUnregistered();
	void BusStopping();
	void BusDisconnected();
	void SessionJoined(ajn::SessionPort sessionPort, ajn::SessionId id, const char* joiner);
	void SessionLost(ajn::SessionId sessionId, ajn::SessionListener::SessionLostReason reason);
	void SessionMemberAdded(ajn::SessionId sessionId, const char* uniqueName);
	void SessionMemberRemoved(ajn::SessionId sessionId, const char* uniqueName);
};
