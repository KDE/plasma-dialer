# SPDX-FileCopyrightText: 2022 Alexey Andreyev <aa13q@ya.ru>
#
# SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

qt_add_executable(plasma-dialer main.cpp)

target_link_libraries(plasma-dialer PRIVATE DialerShared)

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
    qml/settings/RingtoneSettingsPage.qml
    qml/history/HistoryDelegate.qml
    qml/USSDSheet.qml
    qml/ImeiSheet.qml
    qml/PhoneNumberDialog.qml
)

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED Core Quick Qml QuickControls2 Multimedia)
find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS Config Crash CoreAddons I18n DBusAddons)
find_package(KF6KirigamiAddons 1.3 REQUIRED)

target_link_libraries(plasma-dialer PRIVATE
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

target_include_directories(plasma-dialer PRIVATE ${CMAKE_BINARY_DIR})
install(TARGETS plasma-dialer ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

