add_executable(plasmaphonedialer
    main.cpp
    resources.qrc   
)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED Core Quick Qml QuickControls2)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS CoreAddons I18n DBusAddons WindowSystem)

option(DIALER_BUILD_SHELL_OVERLAY "Enable building of Dialer lockscreen functionality" ON)
if (DIALER_BUILD_SHELL_OVERLAY)
    add_definitions(-DDIALER_BUILD_SHELL_OVERLAY)
    find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED WaylandClient)
    find_package(Wayland REQUIRED)
    find_package(PlasmaWaylandProtocols 1.8 CONFIG)
    set_package_properties(PlasmaWaylandProtocols PROPERTIES
        TYPE REQUIRED
        PURPOSE "Collection of Plasma-specific Wayland protocols"
        URL "https://invent.kde.org/libraries/plasma-wayland-protocols/"
    )
    if (QT_MAJOR_VERSION EQUAL "5")
        find_package(QtWaylandScanner)
        set_package_properties(QtWaylandScanner PROPERTIES
            TYPE REQUIRED
            PURPOSE "Required for building with Wayland above-lock-screen support"
        )
    endif()
endif() # DIALER_BUILD_SHELL_OVERLAY

target_include_directories(plasmaphonedialer PRIVATE ${CMAKE_BINARY_DIR})

target_link_libraries(plasmaphonedialer
    Qt5::Core
    Qt5::Quick
    Qt5::Qml
    Qt5::QuickControls2
    KF5::CoreAddons
    KF5::I18n
    KF5::DBusAddons
    KF5::WindowSystem
)
if (DIALER_BUILD_SHELL_OVERLAY)
    target_link_libraries(plasmaphonedialer
        Qt5::WaylandClient
        Qt5::WaylandClientPrivate
        Wayland::Client
    )
endif() # DIALER_BUILD_SHELL_OVERLAY

ecm_add_qtwayland_client_protocol(plasmaphonedialer
    PROTOCOL ${PLASMA_WAYLAND_PROTOCOLS_DIR}/kde-lockscreenallowed-v1.xml
    BASENAME kde-lockscreenallowed-v1
)

install(TARGETS plasmaphonedialer ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES plasma_dialer.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFY5RCDIR})
