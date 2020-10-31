add_executable(plasma-telepathy-approver
    main.cpp
    phoneapprover.cpp
    dispatchoperation.cpp
    channelapprover.cpp
    callchannelapprover.cpp
)

target_include_directories(plasma-telepathy-approver PRIVATE ${CMAKE_BINARY_DIR})

target_link_libraries(plasma-telepathy-approver
    Qt5::Core
    Qt5::Quick
    KF5::CoreAddons
    KF5::DBusAddons
    KF5::I18n
    KF5::Notifications
    TelepathyQt5::Core
)

install(TARGETS plasma-telepathy-approver ${INSTALL_TARGETS_DEFAULT_ARGS})

ecm_install_configured_files(INPUT org.freedesktop.Telepathy.Client.Plasma.Approver.service.in DESTINATION ${DBUS_SERVICES_INSTALL_DIR} @ONLY)
