# minimal requirements
cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)
project(plasma-dialer VERSION 0.9)
set(QT_MIN_VERSION "5.14.0")
set(KF5_MIN_VERSION "5.71.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_SOURCE_DIR}/cmake)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(ECMMarkNonGuiExecutable)
include(FeatureSummary)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED Core Qml Quick Sql)

find_package(KF5CoreAddons ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5I18n ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5People ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5DBusAddons ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5Notifications ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5Contacts ${KF5_MIN_VERSION} REQUIRED)

find_package(PulseAudio 5.0.0 REQUIRED)

find_package(TelepathyQt5 REQUIRED)

install(FILES metadata.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} RENAME org.kde.phone.dialer.desktop)
install(FILES telephony-services.desktop DESTINATION ${KDE_INSTALL_AUTOSTARTDIR})
install(FILES dialer.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps/)

add_subdirectory(src)
add_subdirectory(approver)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
