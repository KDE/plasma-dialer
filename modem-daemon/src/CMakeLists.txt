# SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
#
# SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

include(ECMPoQmTools)
ecm_install_po_files_as_qm(po)

ecm_create_qm_loader(modem-daemon_QM_LOADER modem-daemon_qt)

set(MODEM_SUBSYSTEM_SRCS "")
if (${MODEM_SUBSYSTEM} STREQUAL "ModemManager")
    set(MODEM_SUBSYSTEM_SRCS mm-modem-controller.cpp)
elseif(${MODEM_SUBSYSTEM} STREQUAL "oFono")
    message(STATUS "oFono support is not implemented yet")
endif()

set(modemd_SRCS
    main.cpp
    call-utils.cpp
    call-object.cpp
    call-manager.cpp
    ussd-utils.cpp
    ussd-manager.cpp
    device-utils.cpp
    device-manager.cpp
    modem-controller.cpp
    ${MODEM_SUBSYSTEM_SRCS}
    ${modem-daemon_QM_LOADER}
    )

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} CONFIG REQUIRED Core Gui DBus)
if (${MODEM_SUBSYSTEM} STREQUAL "ModemManager")
    find_package(KF${KF_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS ModemManagerQt)
elseif(${MODEM_SUBSYSTEM} STREQUAL "oFono")
    message(STATUS "oFono support is not implemented yet")
endif()
find_package(PhoneNumber COMPONENTS PhoneNumber REQUIRED)

qt_add_dbus_adaptor(modemd_SRCS "${DBUS_INTERFACES_PATH}/org.kde.telephony.DeviceUtils.xml"
    device-utils.h DeviceUtils)

qt_add_dbus_adaptor(modemd_SRCS "${DBUS_INTERFACES_PATH}/org.kde.telephony.UssdUtils.xml"
    ussd-utils.h UssdUtils)

qt_add_dbus_adaptor(modemd_SRCS "${DBUS_INTERFACES_PATH}/org.kde.telephony.CallUtils.xml"
    call-utils.h CallUtils)

add_executable(modem-daemon
    ${modemd_SRCS}
    )

target_include_directories(modem-daemon PRIVATE ${CMAKE_BINARY_DIR})

target_link_libraries(modem-daemon
    Qt::Core
    Qt::DBus
    PhoneNumber::PhoneNumber
    ktelephonymetatypes # FIXME: KF${KF_MAJOR_VERSION} cmake files
    )

if (${MODEM_SUBSYSTEM} STREQUAL "ModemManager")
    target_link_libraries(modem-daemon KF${QT_MAJOR_VERSION}::ModemManagerQt)
elseif(${MODEM_SUBSYSTEM} STREQUAL "oFono")
    message(STATUS "oFono support is not implemented yet")
endif()

configure_file(daemon-autostart.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/daemon-autostart.desktop)

set(SERV_EXEC ${KDE_INSTALL_FULL_LIBEXECDIR}/modem-daemon)
configure_file(org.kde.modemdaemon.service.in ${CMAKE_CURRENT_BINARY_DIR}/org.kde.modemdaemon.service)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.modemdaemon.service DESTINATION ${KDE_INSTALL_DBUSSERVICEDIR})

install(TARGETS modem-daemon DESTINATION ${KDE_INSTALL_FULL_LIBEXECDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/daemon-autostart.desktop DESTINATION ${KDE_INSTALL_AUTOSTARTDIR} RENAME org.kde.modem.daemon.desktop)
