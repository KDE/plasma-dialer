set(plasma-dialer-fakeserver_SRCS
    main.cpp
    server.cpp
    modemmocker.cpp
    rootmocker.cpp
    callmocker.cpp
    objectmanagermocker.cpp
    dbusobject.cpp
    callmocker.cpp
    simmocker.cpp
    types.h
)

set(mm_call_dbusXML dbus/org.freedesktop.ModemManager1.Call.xml)
set(mm_modem_dbusXML dbus/org.freedesktop.ModemManager1.Modem.xml)
set(mm_modem_3gpp_dbusXML dbus/org.freedesktop.ModemManager1.Modem.Modem3gpp.xml)
set(mm_modem_3gpp_ussd_dbusXML dbus/org.freedesktop.ModemManager1.Modem.Modem3gpp.Ussd.xml)
set(mm_modem_voice_dbusXML dbus/org.freedesktop.ModemManager1.Modem.Voice.xml)
set(mm_root_dbusXML dbus/org.freedesktop.ModemManager1.xml)
set(mm_objectmanager_dbusXML dbus/org.freedesktop.DBus.ObjectManager.xml)
set(mm_sim_dbusXML dbus/org.freedesktop.ModemManager1.Sim.xml)

qt_add_dbus_adaptor(
    plasma-dialer-fakeserver_SRCS
    ${mm_call_dbusXML}
    callmocker.h
    CallMocker
    calladaptor)

qt_add_dbus_adaptor(
    plasma-dialer-fakeserver_SRCS
    ${mm_modem_dbusXML}
    modemmocker.h
    ModemMocker
    modemadaptor)

qt_add_dbus_adaptor(
    plasma-dialer-fakeserver_SRCS
    ${mm_modem_3gpp_dbusXML}
    modemmocker.h
    ModemMocker
    modem3gppadaptor)

qt_add_dbus_adaptor(
    plasma-dialer-fakeserver_SRCS
    ${mm_modem_3gpp_ussd_dbusXML}
    modemmocker.h
    ModemMocker
    modem3gppussdadaptor)

qt_add_dbus_adaptor(
    plasma-dialer-fakeserver_SRCS
    ${mm_modem_voice_dbusXML}
    modemmocker.h
    ModemMocker
    modemvoiceadaptor)

qt_add_dbus_adaptor(
    plasma-dialer-fakeserver_SRCS
    ${mm_root_dbusXML}
    rootmocker.h
    RootMocker
    rootadaptor)

qt_add_dbus_adaptor(
    plasma-dialer-fakeserver_SRCS
    ${mm_objectmanager_dbusXML}
    objectmanagermocker.h
    ObjectManagerMocker
    objectmanageradaptor)

qt_add_dbus_adaptor(
    plasma-dialer-fakeserver_SRCS
    ${mm_sim_dbusXML}
    simmocker.h
    SimMocker
    simadaptor)

add_executable(plasma-dialer-fakeserver ${plasma-dialer-fakeserver_SRCS})

qt_add_qml_module(plasma-dialer-fakeserver
    URI org.kde.plasma.dialer.fakeserver
    VERSION 1.0
)

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED Core Quick Qml Gui Widgets DBus QuickControls2)
find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS CoreAddons DBusAddons ModemManagerQt)

target_link_libraries(plasma-dialer-fakeserver PUBLIC
    Qt::Qml
    Qt::Gui
    Qt::Widgets
    Qt::Quick
    Qt::DBus
    Qt::QuickControls2
    KF6::ModemManagerQt
    KF6::CoreAddons
    KF6::DBusAddons
)

qt_target_qml_sources(plasma-dialer-fakeserver
    QML_FILES
        qml/Main.qml
)

target_include_directories(plasma-dialer-fakeserver PRIVATE ${CMAKE_BINARY_DIR})
install(TARGETS plasma-dialer-fakeserver ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
