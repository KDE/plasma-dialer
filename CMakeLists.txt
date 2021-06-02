cmake_minimum_required (VERSION 3.16)

project(plasma-dialer)
set(PROJECT_VERSION "21.05")

set(QT_MIN_VERSION "5.15.0")
set(KF5_MIN_VERSION "5.73.0")

set(CMAKE_CXX_STANDARD 20)
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
include(KDEClangFormat)
include(KDEGitCommitHooks)
include(ECMCheckOutboundLicense)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX PLASMADIALER
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED Core Qml Quick Sql)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS CoreAddons I18n People DBusAddons Notifications Contacts)

find_package(TelepathyQt5 REQUIRED)

find_package(KF5PulseAudioQt 1.3 REQUIRED)

find_package(PhoneNumber COMPONENTS PhoneNumber REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBQOFONO qofono-qt5 REQUIRED IMPORTED_TARGET)

install(FILES org.kde.phone.dialer.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES telephony-services.desktop DESTINATION ${KDE_INSTALL_AUTOSTARTDIR})
install(FILES dialer-autostart.desktop DESTINATION ${KDE_INSTALL_AUTOSTARTDIR} RENAME org.kde.phone.dialer.desktop)
install(FILES dialer.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps/)
install(FILES org.kde.phone.dialer.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

add_subdirectory(src)
add_subdirectory(approver)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h *.qml)
ecm_check_outbound_license(LICENSES GPL-3.0-only FILES ${ALL_SOURCE_FILES})

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
