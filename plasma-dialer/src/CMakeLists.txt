# SPDX-FileCopyrightText: 2022 Alexey Andreyev <aa13q@ya.ru>
#
# SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

add_executable(plasma-dialer
    main.cpp
    resources.qrc   
)

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED Core Quick Qml QuickControls2)
find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS Config CoreAddons I18n DBusAddons)
find_package(KF6KirigamiAddons 1.3 REQUIRED)

if (DIALER_BUILD_SHELL_OVERLAY)
    find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED Gui WaylandClient)
    find_package(Qt6Gui ${QT_MIN_VERSION} CONFIG REQUIRED Private)
    find_package(Qt6WaylandClient ${QT_MIN_VERSION} COMPONENTS CONFIG REQUIRED Private)
    find_package(Wayland REQUIRED)
    find_package(PlasmaWaylandProtocols 1.8 CONFIG)
    find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS WindowSystem)
    set_package_properties(PlasmaWaylandProtocols PROPERTIES
        TYPE REQUIRED
        PURPOSE "Collection of Plasma-specific Wayland protocols"
        URL "https://invent.kde.org/libraries/plasma-wayland-protocols/"
    )
    find_package(QtWaylandScanner)
    set_package_properties(QtWaylandScanner PROPERTIES
        TYPE REQUIRED
        PURPOSE "Required for building with Wayland above-lock-screen support"
    )
endif() # DIALER_BUILD_SHELL_OVERLAY

target_include_directories(plasma-dialer PRIVATE ${CMAKE_BINARY_DIR})

target_link_libraries(plasma-dialer
    Qt::Core
    Qt::Quick
    Qt::Qml
    Qt::QuickControls2
    KF6::ConfigGui
    KF6::CoreAddons
    KF6::I18n
    KF6::DBusAddons
)
if (DIALER_BUILD_SHELL_OVERLAY)
    target_link_libraries(plasma-dialer
        Qt::GuiPrivate
        Qt::WaylandClient
        Qt::WaylandClientPrivate
        Wayland::Client
        KF6::WindowSystem
    )

    qt6_generate_wayland_protocol_client_sources(plasma-dialer FILES
        ${PLASMA_WAYLAND_PROTOCOLS_DIR}/kde-lockscreen-overlay-v1.xml)
endif() # DIALER_BUILD_SHELL_OVERLAY

kconfig_add_kcfg_files(plasma-dialer config.kcfgc GENERATE_MOC)
install(TARGETS plasma-dialer ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
set(RINGING_SOUND "phone-incoming-call")
set(RINGING_WITHOUT_POPUP_SOUND "phone-incoming-call")

configure_file(plasma-dialer.notifyrc.in ${CMAKE_CURRENT_BINARY_DIR}/plasma-dialer.notifyrc)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/plasma-dialer.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})
