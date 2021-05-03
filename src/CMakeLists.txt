add_executable(plasmaphonedialer
    main.cpp
    dialerutils.cpp
    call-handler.cpp
    callhistorymodel.cpp
    call-manager.cpp
    ussd-manager.cpp
    qpulseaudioengine.cpp
    database.cpp
    contactmapper.cpp
    resources.qrc
)

target_include_directories(plasmaphonedialer PRIVATE ${CMAKE_BINARY_DIR})

target_link_libraries(plasmaphonedialer
    Qt5::Quick
    Qt5::Sql
    KF5::CoreAddons
    KF5::I18n
    KF5::DBusAddons
    KF5::Notifications
    KF5::People
    KF5::PeopleBackend
    KF5::Contacts
    TelepathyQt5::Core
    PhoneNumber::PhoneNumber
    PkgConfig::LIBPULSE
    PkgConfig::LIBQOFONO
)

install(TARGETS plasmaphonedialer ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES plasma_dialer.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFY5RCDIR})

ecm_install_configured_files(INPUT org.freedesktop.Telepathy.Client.Plasma.Dialer.service.in DESTINATION ${DBUS_SERVICES_INSTALL_DIR} @ONLY)

install(FILES Plasma.Dialer.client DESTINATION ${SHARE_INSTALL_PREFIX}/telepathy/clients/)
