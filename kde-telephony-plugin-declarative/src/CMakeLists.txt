# SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
#
# SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

set(PLUGIN_SRCS
    ktelephonyplugindeclarative.cpp
    call-model.cpp
    active-call-model.cpp
    call-history-model.cpp
    declarative-device-utils.cpp
    declarative-ussd-utils.cpp
    declarative-call-utils.cpp
    declarative-dialer-utils.cpp
    declarative-contact-utils.cpp
    )

set_source_files_properties(
    # calls-daemon
    "${DBUS_INTERFACES_PATH}/org.kde.telephony.DeviceUtils.xml"
    "${DBUS_INTERFACES_PATH}/org.kde.telephony.UssdUtils.xml"
    "${DBUS_INTERFACES_PATH}/org.kde.telephony.CallHistoryDatabase.xml"
    "${DBUS_INTERFACES_PATH}/org.kde.telephony.CallUtils.xml"
    # kde-telephony-daemon
    "${DBUS_INTERFACES_PATH}/org.kde.telephony.DialerUtils.xml"
    "${DBUS_INTERFACES_PATH}/org.kde.telephony.ContactUtils.xml"
    PROPERTIES
    INCLUDE "kTelephonyMetaTypes/dialer-types.h"
)

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} CONFIG REQUIRED Core Quick Qml Sql DBus)
find_package(KF${KF_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS CoreAddons I18n DBusAddons Contacts People)
find_package(PhoneNumber COMPONENTS PhoneNumber REQUIRED)

# calls-daemon

qt_add_dbus_interface(PLUGIN_SRCS "${DBUS_INTERFACES_PATH}/org.kde.telephony.DeviceUtils.xml"
    deviceutilsinterface)

qt_add_dbus_interface(PLUGIN_SRCS "${DBUS_INTERFACES_PATH}/org.kde.telephony.VoiceMailUtils.xml"
    voicemailutilsinterface)

qt_add_dbus_interface(PLUGIN_SRCS "${DBUS_INTERFACES_PATH}/org.kde.telephony.UssdUtils.xml"
    ussdutilsinterface)

qt_add_dbus_interface(PLUGIN_SRCS "${DBUS_INTERFACES_PATH}/org.kde.telephony.CallHistoryDatabase.xml"
    callhistorydatabaseinterface)

qt_add_dbus_interface(PLUGIN_SRCS "${DBUS_INTERFACES_PATH}/org.kde.telephony.CallUtils.xml"
    callutilsinterface)

# kde-telephony-daemon

qt_add_dbus_interface(PLUGIN_SRCS "${DBUS_INTERFACES_PATH}/org.kde.telephony.DialerUtils.xml"
    dialerutilsinterface)

qt_add_dbus_interface(PLUGIN_SRCS "${DBUS_INTERFACES_PATH}/org.kde.telephony.ContactUtils.xml"
    contactutilsinterface)

add_library(KTelephonyPluginDeclarative SHARED
    # For IDE's project tree
    qmldir
    ${PLUGIN_SRCS}
    )

target_link_libraries(KTelephonyPluginDeclarative
    Qt::Core
    Qt::Quick
    Qt::Qml
    Qt::Sql
    Qt::DBus
    KF${QT_MAJOR_VERSION}::CoreAddons
    KF${QT_MAJOR_VERSION}::I18n
    KF${QT_MAJOR_VERSION}::DBusAddons
    KF${QT_MAJOR_VERSION}::Contacts
    KF${QT_MAJOR_VERSION}::People
    KF${QT_MAJOR_VERSION}::PeopleBackend
    ktelephonymetatypes # TODO: KF${KF_MAJOR_VERSION} cmake files
    )

install(TARGETS KTelephonyPluginDeclarative DESTINATION "${KDE_INSTALL_QMLDIR}/org/kde/telephony")
install(FILES qmldir DESTINATION "${KDE_INSTALL_QMLDIR}/org/kde/telephony")
