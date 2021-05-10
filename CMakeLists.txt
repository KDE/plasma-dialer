cmake_minimum_required (VERSION 3.14 FATAL_ERROR)

project(plasma-dialer)
set(PROJECT_VERSION "21.05")

set(QT_MIN_VERSION "5.14.0")
set(KF5_MIN_VERSION "5.73.0")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(ECMMarkNonGuiExecutable)
include(FeatureSummary)
include(ECMConfiguredInstall)
include(ECMSetupVersion)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX PLASMADIALER
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED Core Qml Quick Sql)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS CoreAddons I18n People DBusAddons Notifications Contacts)

find_package(TelepathyQt5 REQUIRED)

find_package(PhoneNumber COMPONENTS PhoneNumber REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBPULSE libpulse REQUIRED IMPORTED_TARGET)
pkg_check_modules(LIBQOFONO qofono-qt5 REQUIRED IMPORTED_TARGET)

install(FILES metadata.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} RENAME org.kde.phone.dialer.desktop)
install(FILES telephony-services.desktop DESTINATION ${KDE_INSTALL_AUTOSTARTDIR})
install(FILES dialer-autostart.desktop DESTINATION ${KDE_INSTALL_AUTOSTARTDIR} RENAME org.kde.phone.dialer.desktop)
install(FILES dialer.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps/)

add_subdirectory(src)
add_subdirectory(approver)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
