# SPDX-FileCopyrightText: 2026 Micah Stanley <stanleymicah@proton.me>
#
# SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED Core Quick Qml QuickControls2 Multimedia)
find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS Config Crash CoreAddons I18n)
find_package(KF6KirigamiAddons 1.3 REQUIRED)

set(shared_SRCS
    ringtoneutils.cpp
    sharedutils.cpp
)

kconfig_add_kcfg_files(shared_SRCS GENERATE_MOC config.kcfgc)

add_library(DialerShared STATIC)

ecm_add_qml_module(DialerShared
    URI org.kde.plasma.dialer.shared
    GENERATE_PLUGIN_SOURCE
    SOURCES
        ${shared_SRCS}
    QML_FILES
        qml/dialpad/DialerButton.qml
        qml/dialpad/DialerIconButton.qml
        qml/dialpad/Dialpad.qml
        qml/call/InCallInlineMessage.qml
)

target_link_libraries(DialerShared PUBLIC
    Qt::Core
    Qt::Qml
    KF6::ConfigCore
    KF6::ConfigGui
)

target_include_directories(DialerShared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
