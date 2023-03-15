# SPDX-FileCopyrightText: 2022 Alexey Andreyev <aa13q@ya.ru>
#
# SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

add_executable(plasmaphonedialer
    main.cpp
    resources.qrc   
)

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} CONFIG REQUIRED Core Quick Qml QuickControls2)
find_package(KF${KF_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS Config CoreAddons I18n DBusAddons WindowSystem)
find_package(KF${KF_MAJOR_VERSION}KirigamiAddons 0.11 REQUIRED)

if (DIALER_BUILD_SHELL_OVERLAY)
    find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} CONFIG REQUIRED Gui WaylandClient)
    find_package(Qt${QT_MAJOR_VERSION}Gui ${QT_MIN_VERSION} CONFIG REQUIRED Private)
    find_package(Qt${QT_MAJOR_VERSION}WaylandClient ${QT_MIN_VERSION} COMPONENTS CONFIG REQUIRED Private)
    find_package(Wayland REQUIRED)
    find_package(PlasmaWaylandProtocols 1.8 CONFIG)
    find_package(KWinEffects 5.26.0 CONFIG)
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
    set_package_properties(KWinEffects PROPERTIES DESCRIPTION "KWin (Effects)"
        TYPE RECOMMENDED
        PURPOSE "Appropriate KWin version is expected to support above lockscreen mode (shell overlay)"
        URL "https://invent.kde.org/plasma/kwin/"
    )
endif() # DIALER_BUILD_SHELL_OVERLAY

target_include_directories(plasmaphonedialer PRIVATE ${CMAKE_BINARY_DIR})

target_link_libraries(plasmaphonedialer
    Qt::Core
    Qt::Quick
    Qt::Qml
    Qt::QuickControls2
    KF${QT_MAJOR_VERSION}::ConfigGui
    KF${QT_MAJOR_VERSION}::CoreAddons
    KF${QT_MAJOR_VERSION}::I18n
    KF${QT_MAJOR_VERSION}::DBusAddons
    KF${QT_MAJOR_VERSION}::WindowSystem
)
if (DIALER_BUILD_SHELL_OVERLAY)
    target_link_libraries(plasmaphonedialer
        Qt::GuiPrivate
        Qt::WaylandClient
        Qt::WaylandClientPrivate
        Wayland::Client
    )

    if (QT_MAJOR_VERSION EQUAL "5")
        ecm_add_qtwayland_client_protocol(plasmaphonedialer
            PROTOCOL ${PLASMA_WAYLAND_PROTOCOLS_DIR}/kde-lockscreen-overlay-v1.xml
            BASENAME kde-lockscreen-overlay-v1
        )
    else()
        qt6_generate_wayland_protocol_client_sources(plasmaphonedialer FILES
            ${PLASMA_WAYLAND_PROTOCOLS_DIR}/kde-lockscreen-overlay-v1.xml)
    endif()
endif() # DIALER_BUILD_SHELL_OVERLAY

kconfig_add_kcfg_files(plasmaphonedialer config.kcfgc GENERATE_MOC)
install(TARGETS plasmaphonedialer ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
set(RINGING_SOUND "phone-incoming-call")
set(RINGING_WITHOUT_POPUP_SOUND "phone-incoming-call")
if (QT_MAJOR_VERSION EQUAL "5")
    set(RINGING_SOUND "Oxygen-Im-Phone-Ring.ogg")
    set(RINGING_WITHOUT_POPUP_SOUND "Oxygen-Im-Phone-Ring.ogg")
endif()

configure_file(plasma_dialer.notifyrc.in ${CMAKE_CURRENT_BINARY_DIR}/plasma_dialer.notifyrc)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/plasma_dialer.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})
