# SPDX-FileCopyrightText: 2022 Alexey Andreyev <aa13q@ya.ru>
#
# SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

set(dialer_SRCS
    lockscreenutils.cpp
)

kconfig_add_kcfg_files(dialer_SRCS GENERATE_MOC config.kcfgc)

add_executable(plasma-dialer main.cpp ${dialer_SRCS})

ecm_add_qml_module(plasma-dialer GENERATE_PLUGIN_SOURCE URI org.kde.plasma.dialer)

qt_target_qml_sources(plasma-dialer QML_FILES
    qml/Main.qml
    qml/components/Sidebar.qml
    qml/components/BottomToolbar.qml
    qml/DialerPage.qml
    qml/ContactsPage.qml
    qml/HistoryPage.qml
    qml/AboutPage.qml
    qml/settings/SettingsPage.qml
    qml/settings/CallBlockSettingsPage.qml
    qml/call/InCallInlineMessage.qml
    qml/call/AsymmetricAnswerSwipe.qml
    qml/call/SymmetricAnswerSwipe.qml
    qml/call/AnswerButtons.qml
    qml/call/CallPage.qml
    qml/call/Avatar.qml
    qml/call/ActiveCallView.qml
    qml/call/CallPageButton.qml
    qml/history/HistoryDelegate.qml
    qml/dialpad/Dialpad.qml
    qml/dialpad/DialerButton.qml
    qml/dialpad/DialerIconButton.qml
    qml/USSDSheet.qml
    qml/ImeiSheet.qml
    qml/PhoneNumberDialog.qml
)


find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED Core Quick Qml QuickControls2)
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

target_link_libraries(plasma-dialer PRIVATE
    Qt::Core
    Qt::Quick
    Qt::Qml
    Qt::QuickControls2
    KF6::ConfigGui
    KF6::CoreAddons
    KF6::Crash
    KF6::I18n
    KF6::I18nQml
    KF6::DBusAddons
)
if (DIALER_BUILD_SHELL_OVERLAY)
    target_link_libraries(plasma-dialer PRIVATE
        Qt::GuiPrivate
        Qt::WaylandClient
        Qt::WaylandClientPrivate
        Wayland::Client
        KF6::WindowSystem
    )

    if (Qt6_VERSION VERSION_GREATER_EQUAL "6.8.0")
        set(private_code_option  "PRIVATE_CODE")
    endif()
    qt6_generate_wayland_protocol_client_sources(plasma-dialer FILES
        ${PLASMA_WAYLAND_PROTOCOLS_DIR}/kde-lockscreen-overlay-v1.xml
        ${private_code_option}
    )
endif() # DIALER_BUILD_SHELL_OVERLAY

target_include_directories(plasma-dialer PRIVATE ${CMAKE_BINARY_DIR})
install(TARGETS plasma-dialer ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
set(RINGING_SOUND "phone-incoming-call")
set(RINGING_WITHOUT_POPUP_SOUND "phone-incoming-call")

configure_file(plasma-dialer.notifyrc.in ${CMAKE_CURRENT_BINARY_DIR}/plasma-dialer.notifyrc)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/plasma-dialer.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})
