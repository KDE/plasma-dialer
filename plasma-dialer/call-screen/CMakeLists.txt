# SPDX-FileCopyrightText: 2026 Micah Stanley <stanleymicah@proton.me>
#
# SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

set(call-screen_SRCS
    lockscreenutils.cpp
    main.cpp
)

qt_add_executable(plasma-call-screen ${call-screen_SRCS})

target_link_libraries(plasma-call-screen PRIVATE DialerShared)

ecm_add_qml_module(plasma-call-screen GENERATE_PLUGIN_SOURCE URI org.kde.plasma.callscreen)

qt_target_qml_sources(plasma-call-screen QML_FILES
    qml/Main.qml
    qml/call/ActiveCallView.qml
    qml/call/AnswerButtons.qml
    qml/call/AsymmetricAnswerSwipe.qml
    qml/call/Avatar.qml
    qml/call/CallPage.qml
    qml/call/CallPageButton.qml
    qml/call/SymmetricAnswerSwipe.qml
)

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED Core Quick Qml QuickControls2 Multimedia)
find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS Config Crash CoreAddons I18n DBusAddons)
find_package(KF6KirigamiAddons 1.3 REQUIRED)

if (DIALER_BUILD_SHELL_OVERLAY)
    find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED Gui WaylandClient)

    if (Qt6Gui_VERSION VERSION_GREATER_EQUAL "6.10.0")
        find_package(Qt6 REQUIRED COMPONENTS GuiPrivate WaylandClientPrivate)
    else()
        find_package(Qt6Gui ${QT_MIN_VERSION} CONFIG REQUIRED Private)
        find_package(Qt6WaylandClient ${QT_MIN_VERSION} COMPONENTS CONFIG REQUIRED Private)
    endif()

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

target_link_libraries(plasma-call-screen PRIVATE
    Qt::Core
    Qt::Quick
    Qt::Qml
    Qt::QuickControls2
    Qt::Multimedia
    KF6::ConfigGui
    KF6::CoreAddons
    KF6::Crash
    KF6::I18n
    KF6::I18nQml
    KF6::DBusAddons
)
if (DIALER_BUILD_SHELL_OVERLAY)
    target_link_libraries(plasma-call-screen PRIVATE
        Qt::GuiPrivate
        Qt::WaylandClient
        Qt::WaylandClientPrivate
        Wayland::Client
        KF6::WindowSystem
    )

    if (Qt6_VERSION VERSION_GREATER_EQUAL "6.8.0")
        set(private_code_option  "PRIVATE_CODE")
    endif()
    qt6_generate_wayland_protocol_client_sources(plasma-call-screen FILES
        ${PLASMA_WAYLAND_PROTOCOLS_DIR}/kde-lockscreen-overlay-v1.xml
        ${private_code_option}
    )
endif() # DIALER_BUILD_SHELL_OVERLAY

target_include_directories(plasma-call-screen PRIVATE ${CMAKE_BINARY_DIR})
install(TARGETS plasma-call-screen ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
set(RINGING_SOUND "phone-incoming-call")
set(RINGING_WITHOUT_POPUP_SOUND "phone-incoming-call")

configure_file(plasma-dialer.notifyrc.in ${CMAKE_CURRENT_BINARY_DIR}/plasma-dialer.notifyrc)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/plasma-dialer.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})

