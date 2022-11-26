# SPDX-FileCopyrightText: 2022 Alexey Andreyev <aa13q@ya.ru>
#
# SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

add_executable(plasmaphonedialer
    main.cpp
    resources.qrc   
)

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} CONFIG REQUIRED Core Quick Qml QuickControls2)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Config CoreAddons I18n DBusAddons WindowSystem)

if (DIALER_BUILD_SHELL_OVERLAY)
    find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} CONFIG REQUIRED WaylandClient)
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
    KF5::ConfigGui
    KF5::CoreAddons
    KF5::I18n
    KF5::DBusAddons
    KF5::WindowSystem
)
if (DIALER_BUILD_SHELL_OVERLAY)
    target_link_libraries(plasmaphonedialer
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

ecm_find_qmlmodule(org.kde.kirigamiaddons.labs.mobileform 0.1)

kconfig_add_kcfg_files(plasmaphonedialer config.kcfgc GENERATE_MOC)
install(TARGETS plasmaphonedialer ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES plasma_dialer.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})
