set(approver_SRCS
    main.cpp
    phoneapprover.cpp
    dispatchoperation.cpp
    channelapprover.cpp
    callchannelapprover.cpp
)

add_executable(plasma-telepathy-approver ${approver_SRCS})
target_compile_definitions(plasma-telepathy-approver PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")

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


configure_file(org.freedesktop.Telepathy.Client.Plasma.Approver.service.in
                org.freedesktop.Telepathy.Client.Plasma.Approver.service)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.freedesktop.Telepathy.Client.Plasma.Approver.service
        DESTINATION ${DBUS_SERVICES_INSTALL_DIR})
