# SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
#
# SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

set(ktelephonyd_SRCS
    main.cpp
    dialer-utils.cpp
    contact-utils.cpp
    dialer-manager.cpp
    notification-manager.cpp
    call-history-database.cpp
    call-history-manager.cpp
    )

set_source_files_properties(
    "${DBUS_INTERFACES_PATH}/org.kde.telephony.DeviceUtils.xml"
    "${DBUS_INTERFACES_PATH}/org.kde.telephony.CallUtils.xml"
    "${DBUS_INTERFACES_PATH}/org.kde.telephony.CallHistoryDatabase.xml"
    PROPERTIES
    INCLUDE "kTelephonyMetaTypes/dialer-types.h"
)

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED Core DBus Sql)
find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS Config I18n Notifications KIO)
find_package(KTactileFeedback OPTIONAL_COMPONENTS KTactileFeedback)

qt_add_dbus_interface(ktelephonyd_SRCS "${DBUS_INTERFACES_PATH}/org.kde.telephony.DeviceUtils.xml"
    deviceutilsinterface)

qt_add_dbus_interface(ktelephonyd_SRCS "${DBUS_INTERFACES_PATH}/org.kde.telephony.CallUtils.xml"
    callutilsinterface)

qt_add_dbus_interface(ktelephonyd_SRCS "${DBUS_INTERFACES_PATH}/org.kde.telephony.CallHistoryDatabase.xml"
    callhistorydatabaseinterface)

qt_add_dbus_adaptor(ktelephonyd_SRCS "${DBUS_INTERFACES_PATH}/org.kde.telephony.ContactUtils.xml"
    contact-utils.h ContactUtils)

qt_add_dbus_adaptor(ktelephonyd_SRCS "${DBUS_INTERFACES_PATH}/org.kde.telephony.DialerUtils.xml"
    dialer-utils.h DialerUtils)

qt_add_dbus_adaptor(ktelephonyd_SRCS "${DBUS_INTERFACES_PATH}/org.kde.telephony.CallHistoryDatabase.xml"
    call-history-database.h CallHistoryDatabase)

add_executable(kde-telephony-daemon
    ${ktelephonyd_SRCS}
    )

target_include_directories(kde-telephony-daemon PRIVATE ${CMAKE_BINARY_DIR})

qt_add_dbus_interface(mpris_srcs "${DBUS_INTERFACES_PATH}/org.mpris.MediaPlayer2.Player.xml" mprisplayerinterface)
target_sources(kde-telephony-daemon PRIVATE ${mpris_srcs})

find_package(PkgConfig QUIET)
pkg_check_modules(LIBCALLAUDIO libcallaudio-0.1 REQUIRED IMPORTED_TARGET)

# https://build.opensuse.org/package/show/network:telephony/callaudiod#comment-1675259
if("${LIBCALLAUDIO_VERSION}" STREQUAL "0.1.1")
    message(ERROR "libcallaudiod version 0.1.1 is not supported")
endif()
if("${LIBCALLAUDIO_VERSION}" STREQUAL "0.1.2")
    message(ERROR "libcallaudiod version 0.1.2 is not supported")
endif()
if("${LIBCALLAUDIO_VERSION}" STREQUAL "0.1.3")
    message(WARNING "libcallaudiod version 0.1.3 is found, while 0.1.4 is expected." "\n"
        "This is a known issue in case of CI build, the workaround is skipping callaudiod logic for the build:" "\n"
        "https://build.opensuse.org/package/show/network:telephony/callaudiod#comment-1675259")
    target_compile_definitions(kde-telephony-daemon PUBLIC LOWER_LIBCALLAUDIO_VERSION)
endif()


target_link_libraries(kde-telephony-daemon
    KF6::ConfigGui
    Qt::Core
    Qt::DBus
    Qt::Sql
    KF6::I18n
    KF6::KIOGui
    KF6::Notifications
    PkgConfig::LIBCALLAUDIO
    ktelephonymetatypes # FIXME: KF6 cmake files
    contactphonenumbermapper
    )

if (KTactileFeedback_FOUND)
    target_link_libraries(kde-telephony-daemon KTactileFeedback::KTactileFeedback)
    target_compile_definitions(kde-telephony-daemon PUBLIC HAVE_K_TACTILE_FEEDBACK)
else()
    message(WARNING "KTactileFeedback dependency is not found. It's expected since project's status is work in progress.")
endif()

configure_file(daemon-autostart.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/daemon-autostart.desktop)

set(SERV_EXEC ${KDE_INSTALL_FULL_LIBEXECDIR}/kde-telephony-daemon)
configure_file(org.kde.telephony.service.in ${CMAKE_CURRENT_BINARY_DIR}/org.kde.telephony.service)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.telephony.service DESTINATION ${KDE_INSTALL_DBUSSERVICEDIR})

kconfig_add_kcfg_files(kde-telephony-daemon ../../plasma-dialer/src/config.kcfgc GENERATE_MOC)
install(TARGETS kde-telephony-daemon DESTINATION ${KDE_INSTALL_FULL_LIBEXECDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/daemon-autostart.desktop DESTINATION ${KDE_INSTALL_AUTOSTARTDIR} RENAME org.kde.telephony.daemon.desktop)
