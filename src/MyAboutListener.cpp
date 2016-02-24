#include "MyAboutListener.h"
#include "SensorRecvObject.h"

using namespace qcc;
using namespace ajn;

MyAboutListener::MyAboutListener(std::shared_ptr<ajn::BusAttachment> _busAtt, std::function<void(bool)> _func) :
busAtt(_busAtt), func(_func)
{
}

void MyAboutListener::Announced(const char* busName, uint16_t version, ajn::SessionPort port, const ajn::MsgArg& objectDescriptionArg, const ajn::MsgArg& aboutDataArg)
{
	AboutObjectDescription objectDescription;
	objectDescription.CreateFromMsgArg(objectDescriptionArg);

	AboutObjectDescription aod(objectDescriptionArg);

	// インターフェースを持っているパスを取得.
	size_t pathCount = aod.GetInterfacePaths("SensorLightCamera.DSB.Sensor.interface_1", NULL, 0);
	std::vector<const char*> paths(pathCount);
	aod.GetInterfacePaths("SensorLightCamera.DSB.Sensor.interface_1", paths.data(), paths.size());

	// ないならjoinしない.
	if (paths.size() == 0)
		return;

	SessionId sessionId;
	SessionOpts opts(SessionOpts::TRAFFIC_MESSAGES, false, SessionOpts::PROXIMITY_ANY, TRANSPORT_ANY);
	busAtt->EnableConcurrentCallbacks();
	MyBusListener* listener = new MyBusListener(busAtt);
	busAtt->JoinSession(busName, port, listener, sessionId, opts);

	// プロキシオブジェクト取得.
	for (auto path : paths) {

		printf("%s\n", path);
		ProxyBusObject proxyObject(*busAtt, busName, path, sessionId);
		proxyObject.IntrospectRemoteObject();

		const InterfaceDescription* intf = proxyObject.GetInterface("org.freedesktop.DBus.Properties");
		if (intf != nullptr) {

			SensorRecvObject* sensorRecvObject = new SensorRecvObject(busAtt, path, *intf, func);

			busAtt->RegisterBusObject(*sensorRecvObject);
			busAtt->AddMatch("type='signal',interface='org.freedesktop.DBus.Properties");
			listenList.push_back(std::make_tuple(path, listener, sensorRecvObject));
		}
	}
}
