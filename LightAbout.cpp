#include "Common.h"
#include "MyBusController.h"

using namespace qcc;
using namespace ajn;
using namespace services;

const char* Const::INTERFACE_NAME = "SensorLightCamera.Light";
const char* Const::SERVICE_NAME = "SensorLightCamera";
const char* Const::SERVICE_PATH = "Light";
const ajn::SessionPort Const::SERVICE_PORT = 26;

static const int pin_out = 18;

static void DigitalWrite(int value)
{
	digitalWrite(pin_out, value);
}

// ループ用.
static volatile sig_atomic_t sigFlag = false;

static void CDECL_CALL SigIntHandler(int sig)
{
	QCC_UNUSED(sig);
	sigFlag = true;
}

// メイン処理.
int CDECL_CALL main(int argc, char** argv, char** envArg)
{
	QCC_UNUSED(argc);
	QCC_UNUSED(argv);
	QCC_UNUSED(envArg);

	wiringPiSetup();
	pinMode(pin_out, OUTPUT);
	DigitalWrite(0);

	MyBusController* bus = new MyBusController(DigitalWrite);

	// ctrl-c受付.
	signal(SIGINT, SigIntHandler);

	while (sigFlag == false) {
		usleep(100 * 1000);
	}

	delete bus;
	return 0;
}
