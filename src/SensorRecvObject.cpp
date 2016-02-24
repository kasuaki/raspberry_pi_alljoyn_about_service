#include "SensorRecvObject.h"

using namespace qcc;
using namespace ajn;
using namespace services;

SensorRecvObject::SensorRecvObject(std::shared_ptr<BusAttachment> _busAtt, const char* path, const ajn::InterfaceDescription& _intf, std::function<void(bool)> _func) :
BusObject(path),
busAtt(_busAtt), func(_func)
{
	AddInterface(_intf);

	size_t val = _intf.GetMembers(NULL);					// ÉÅÉìÉoêîéÊìæ.
	std::vector<const ajn::InterfaceDescription::Member*> vec(val);	// óÃàÊämï€.
	_intf.GetMembers(vec.data(), vec.size());				// éÊìæ.

	auto signal = cpplinq::from(vec).where([](const ajn::InterfaceDescription::Member* member) {
		return member->memberType == MESSAGE_SIGNAL;
	}).first();

	busAtt->RegisterSignalHandler(this,
		static_cast<MessageReceiver::SignalHandler>(&SensorRecvObject::signalHandler),
		signal,
		NULL);
};

void SensorRecvObject::signalHandler(const ajn::InterfaceDescription::Member* member, const char* sourcePath, ajn::Message& msg)
{
	printf("signalHandler\n");
	rxcpp::observable<>::create<const MsgArg*>([&msg](rxcpp::subscriber<const MsgArg*> dest) {

		printf("first create\n");
		size_t numargs = 0;
		const MsgArg* args = nullptr;
		msg->GetArgs(numargs, args);
		for (size_t i = 0; i < numargs; i++) {
			printf("on_next: %d\n", i);
			dest.on_next(&args[i]);
		}
		dest.on_completed();
	})
	.filter([](const MsgArg* arg) {
		printf("filter 1\n");
		return arg->Signature() == "a{sv}";
	}).filter([](const MsgArg* arg) {

		size_t nelem = 0;
		MsgArg* elems = nullptr;
		auto status = arg->Get("a{sv}", &nelem, &elems);
		printf("filter 2: %d\n", status);
		return (status == ER_OK);
	}).concat_map([](const MsgArg* arg) {

		printf("concat_map\n");
		return rxcpp::observable<>::create<const MsgArg*>([&arg](rxcpp::subscriber<const MsgArg*> dest) {
			printf("create 2\n");
			size_t nelem = 0;
			MsgArg* elems = nullptr;
			arg->Get("a{sv}", &nelem, &elems);
			printf("%d\n", nelem);
			for (size_t j = 0; j < nelem; j++) {
				printf("on_next 2: %d\n", j);
				dest.on_next(&elems[j]);
			}
			dest.on_completed();
		});
	}, [](const MsgArg* i, const MsgArg* j){
		return j;
	}).filter([](const MsgArg* elem) {
		const char* prop;
		MsgArg* val;
		auto status = elem->Get("{sv}", &prop, &val);
		printf("filter 3: %s\n", prop);
		return ((status == ER_OK) && (strcmp(prop, "SensorAttr") == 0));
	}).map([](const MsgArg* elem) {
		const char* prop;
		MsgArg* val;
		elem->Get("{sv}", &prop, &val);
		printf("map: %d\n", val->v_bool);
		return val->v_bool;
	}).subscribe([this](bool b) {
		printf("subscribe\n");
		func(b);
		printf("Receive Signal(%d)\n", b);
	});

	QCC_UNUSED(member);
	QCC_UNUSED(sourcePath);
	QCC_UNUSED(msg);
};
