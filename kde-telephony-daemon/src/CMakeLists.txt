# SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
#
# SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

set(ktelephonyd_SRCS
    main.cpp
    dialer-utils.cpp
    contact-utils.cpp
    dialer-manager.cpp
    notification-manager.cpp
    )

set_source_files_properties(
    "${DBUS_INTERFACES_PATH}/org.kde.telephony.DeviceUtils.xml"
    "${DBUS_INTERFACES_PATH}/org.kde.telephony.CallUtils.xml"
    "${DBUS_INTERFACES_PATH}/org.kde.telephony.CallHistoryDatabase.xml"
    PROPERTIES
    INCLUDE "kTelephonyMetaTypes/dialer-types.h"
)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED Core DBus OPTIONAL_COMPONENTS Feedback)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Config I18n ModemManagerQt Notifications KIO)

qt5_add_dbus_interface(ktelephonyd_SRCS "${DBUS_INTERFACES_PATH}/org.kde.telephony.DeviceUtils.xml"
    deviceutilsinterface)

qt5_add_dbus_interface(ktelephonyd_SRCS "${DBUS_INTERFACES_PATH}/org.kde.telephony.CallUtils.xml"
    callutilsinterface)

qt5_add_dbus_interface(ktelephonyd_SRCS "${DBUS_INTERFACES_PATH}/org.kde.telephony.CallHistoryDatabase.xml"
    callhistorydatabaseinterface)

qt5_add_dbus_adaptor(ktelephonyd_SRCS "${DBUS_INTERFACES_PATH}/org.kde.telephony.ContactUtils.xml"
    contact-utils.h ContactUtils)

qt5_add_dbus_adaptor(ktelephonyd_SRCS "${DBUS_INTERFACES_PATH}/org.kde.telephony.DialerUtils.xml"
    dialer-utils.h DialerUtils)

add_executable(kde-telephony-daemon
    ${ktelephonyd_SRCS}
    )

target_include_directories(kde-telephony-daemon PRIVATE ${CMAKE_BINARY_DIR})

qt5_add_dbus_interface(mpris_srcs "${DBUS_INTERFACES_PATH}/org.mpris.MediaPlayer2.Player.xml" mprisplayerinterface)
target_sources(kde-telephony-daemon PRIVATE ${mpris_srcs})

target_link_libraries(kde-telephony-daemon
    KF5::ConfigGui
    Qt5::Core
    Qt5::DBus
    KF5::I18n
    KF5::KIOGui
    KF5::Notifications
    KF5::ModemManagerQt
    PkgConfig::LIBCALLAUDIO
    ktelephonymetatypes # FIXME: KF5 cmake files
    contactphonenumbermapper
    )

if (Qt5Feedback_FOUND)
    target_link_libraries(kde-telephony-daemon Qt5::Feedback)
    target_compile_definitions(kde-telephony-daemon PUBLIC HAVE_QT5_FEEDBACK)
else()
    message(WARNING "Qt Feedback dependency is not found. While it is optional since unmaintained, haptics feedback functionality is not supported without it.")
endif()

configure_file(daemon-autostart.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/daemon-autostart.desktop)

set(SERV_EXEC ${KDE_INSTALL_FULL_LIBEXECDIR}/kde-telephony-daemon)
configure_file(org.kde.telephony.service.in ${CMAKE_CURRENT_BINARY_DIR}/org.kde.telephony.service)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.telephony.service DESTINATION ${KDE_INSTALL_DBUSSERVICEDIR})

kconfig_add_kcfg_files(kde-telephony-daemon ../../plasma-dialer/src/config.kcfgc GENERATE_MOC)
install(TARGETS kde-telephony-daemon DESTINATION ${KDE_INSTALL_FULL_LIBEXECDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/daemon-autostart.desktop DESTINATION ${KDE_INSTALL_AUTOSTARTDIR} RENAME org.kde.telephony.daemon.desktop)
