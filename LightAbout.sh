#!/bin/sh

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:~/bin
GECKO_HOME=/usr/local/src/xulrunner-sdk
GECKO_BASE=/usr/local/src/xulrunner-sdk
DUKTAPE_DIST=/usr/local/src/duktape-1.2.1
ALLJOYN_DISTDIR=/root/repos2/alljoyn/core/alljoyn/build/linux/arm/release/dist
AJ_ROOT=/root/repos2/alljoyn
LD_LIBRARY_PATH=/root/repos2/alljoyn/core/ajtcl:${ALLJOYN_DISTDIR}/cpp/lib:${ALLJOYN_DISTDIR}/notification/lib:${ALLJOYN_DISTDIR}/onboarding/lib:${ALLJOYN_DISTDIR}/config/lib:${ALLJOYN_DISTDIR}/controlpanel/lib:${ALLJOYN_DISTDIR}/services_common/lib

cd `dirname $0`
d=`pwd`

./LightAbout 

