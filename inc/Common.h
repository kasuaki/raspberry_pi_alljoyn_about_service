#pragma once

#include <wiringPi.h>

#include <qcc/platform.h>

#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <memory>

#include <cpplinq/linq.hpp>
#include <rxcpp/rx.hpp>

#include <qcc/Log.h>
#include <qcc/String.h>

#include <alljoyn/AllJoynStd.h>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/AboutObj.h>
#include <alljoyn/Init.h>
#include <alljoyn/Status.h>
#include <alljoyn/version.h>

#include <alljoyn/services_common/AsyncTaskQueue.h>
#include <alljoyn/services_common/GuidUtil.h>
#include <alljoyn/services_common/LogModule.h>
#include <alljoyn/services_common/LogModulesNames.h>
#include <alljoyn/services_common/ServicesCommonConstants.h>

//#include <alljoyn/controlpanel/Action.h>
//#include <alljoyn/controlpanel/ActionWithDialog.h>
//#include <alljoyn/controlpanel/ConstraintList.h>
//#include <alljoyn/controlpanel/ConstraintRange.h>
//#include <alljoyn/controlpanel/Container.h>
//#include <alljoyn/controlpanel/ControlPanel.h>
//#include <alljoyn/controlpanel/ControlPanelBusListener.h>
//#include <alljoyn/controlpanel/ControlPanelControllee.h>
//#include <alljoyn/controlpanel/ControlPanelControlleeUnit.h>
//#include <alljoyn/controlpanel/ControlPanelController.h>
//#include <alljoyn/controlpanel/ControlPanelControllerUnit.h>
//#include <alljoyn/controlpanel/ControlPanelDevice.h>
//#include <alljoyn/controlpanel/ControlPanelEnums.h>
//#include <alljoyn/controlpanel/ControlPanelListener.h>
//#include <alljoyn/controlpanel/ControlPanelService.h>
//#include <alljoyn/controlpanel/ControlPanelSessionHandler.h>
//#include <alljoyn/controlpanel/CPSDate.h>
//#include <alljoyn/controlpanel/CPSTime.h>
//#include <alljoyn/controlpanel/Dialog.h>
//#include <alljoyn/controlpanel/ErrorWidget.h>
//#include <alljoyn/controlpanel/HttpControl.h>
//#include <alljoyn/controlpanel/Label.h>
//#include <alljoyn/controlpanel/LanguageSet.h>
//#include <alljoyn/controlpanel/LanguageSets.h>
//#include <alljoyn/controlpanel/LogModule.h>
//#include <alljoyn/controlpanel/NotificationAction.h>
//#include <alljoyn/controlpanel/Property.h>
//#include <alljoyn/controlpanel/RootWidget.h>
//#include <alljoyn/controlpanel/Widget.h>

#include <alljoyn/notification/LogModule.h>
#include <alljoyn/notification/Notification.h>
//#include <alljoyn/notification/NotificationAsyncTaskEvents.h>
//#include <alljoyn/notification/NotificationEnums.h>
#include <alljoyn/notification/NotificationReceiver.h>
#include <alljoyn/notification/NotificationSender.h>
#include <alljoyn/notification/NotificationService.h>
#include <alljoyn/notification/NotificationText.h>
//#include <alljoyn/notification/RichAudioUrl.h>

//#include <alljoyn/config/AboutDataStoreInterface.h>
//#include <alljoyn/config/ConfigClient.h>
//#include <alljoyn/config/ConfigService.h>
//#include <alljoyn/config/LogModule.h>

//#include <alljoyn/onboarding/LogModule.h>
//#include <alljoyn/onboarding/Onboarding.h>
//#include <alljoyn/onboarding/OnboardingClient.h>
//#include <alljoyn/onboarding/OnboardingClientListener.h>
//#include <alljoyn/onboarding/OnboardingControllerAPI.h>
//#include <alljoyn/onboarding/OnboardingService.h>

class Const {
public:
	static const char* INTERFACE_NAME;
	static const char* SERVICE_NAME;
	static const char* SERVICE_PATH;
	static const ajn::SessionPort SERVICE_PORT;
};
