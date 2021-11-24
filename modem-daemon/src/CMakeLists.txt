# SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
#
# SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

set(modemd_SRCS
    main.cpp
    call-history-database.cpp
    call-history-manager.cpp
    call-utils.cpp
    call-object.cpp
    call-manager.cpp
    ussd-utils.cpp
    ussd-manager.cpp
    device-utils.cpp
    device-manager.cpp
    modem-controller.cpp
    mm-modem-controller.cpp
    )

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED Core Gui DBus Sql)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS ModemManagerQt)
find_package(PhoneNumber COMPONENTS PhoneNumber REQUIRED)

qt5_add_dbus_adaptor(modemd_SRCS "${DBUS_INTERFACES_PATH}/org.kde.telephony.DeviceUtils.xml"
    device-utils.h DeviceUtils)

qt5_add_dbus_adaptor(modemd_SRCS "${DBUS_INTERFACES_PATH}/org.kde.telephony.UssdUtils.xml"
    ussd-utils.h UssdUtils)

qt5_add_dbus_adaptor(modemd_SRCS "${DBUS_INTERFACES_PATH}/org.kde.telephony.CallUtils.xml"
    call-utils.h CallUtils)

qt5_add_dbus_adaptor(modemd_SRCS "${DBUS_INTERFACES_PATH}/org.kde.telephony.CallHistoryDatabase.xml"
    call-history-database.h CallHistoryDatabase)

add_executable(modem-daemon
    ${modemd_SRCS}
    )

target_include_directories(modem-daemon PRIVATE ${CMAKE_BINARY_DIR})

target_link_libraries(modem-daemon
    Qt5::Core
    Qt5::DBus
    Qt5::Sql
    KF5::ModemManagerQt
    PhoneNumber::PhoneNumber
    ktelephonymetatypes # FIXME: KF5 cmake files
    )

configure_file(daemon-autostart.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/daemon-autostart.desktop)

set(SERV_EXEC ${KDE_INSTALL_FULL_LIBEXECDIR}/modem-daemon)
configure_file(org.kde.modemdaemon.service.in ${CMAKE_CURRENT_BINARY_DIR}/org.kde.modemdaemon.service)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.modemdaemon.service DESTINATION ${DBUS_SERVICES_INSTALL_DIR})

install(TARGETS modem-daemon DESTINATION ${KDE_INSTALL_FULL_LIBEXECDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/daemon-autostart.desktop DESTINATION ${KDE_INSTALL_AUTOSTARTDIR} RENAME org.kde.modem.daemon.desktop)
