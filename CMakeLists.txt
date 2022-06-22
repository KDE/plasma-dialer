# SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
#
# SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

cmake_minimum_required (VERSION 3.16)

project(plasma-dialer-with-subprojects)
set(PROJECT_VERSION "22.06")

option(DIALER_BUILD_SHELL_OVERLAY "Enable building of Dialer lockscreen functionality" ON)
if (DIALER_BUILD_SHELL_OVERLAY)
    add_definitions(-DDIALER_BUILD_SHELL_OVERLAY)
endif() # DIALER_BUILD_SHELL_OVERLAY

set(QT_MIN_VERSION "5.15.0")
set(KF5_MIN_VERSION "5.89.0")
set(KDE_COMPILERSETTINGS_LEVEL "5.82.0")

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

find_package(KF5I18n NO_MODULE)
ki18n_install(po)

find_package(PkgConfig QUIET)
pkg_check_modules(LIBCALLAUDIO libcallaudio-0.1 REQUIRED IMPORTED_TARGET)
#set(DBUS_INTERFACES_PATH "${CMAKE_INSTALL_PREFIX}/${KDE_INSTALL_DBUSINTERFACEDIR}/")

add_subdirectory(kde-telephony-meta)

set(DBUS_INTERFACES_PATH "${CMAKE_INSTALL_PREFIX}/${KDE_INSTALL_DBUSINTERFACEDIR}/")
set(DBUS_INTERFACES_PATH "${SHARED_DBUS_XML_DIR}")
message("Using D-Bus interface descriptions from: ${DBUS_INTERFACES_PATH}")

set(MODEM_SUBSYSTEM "ModemManager" CACHE INTERNAL "")

add_subdirectory(modem-daemon)
add_subdirectory(kde-telephony-daemon)
add_subdirectory(kde-telephony-plugin-declarative)
add_subdirectory(plasma-dialer)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h *.qml)
ecm_check_outbound_license(LICENSES GPL-3.0-only FILES ${ALL_SOURCE_FILES})

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
