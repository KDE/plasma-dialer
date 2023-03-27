# SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
#
# SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

cmake_minimum_required (VERSION 3.16)

set(PROJECT_VERSION "23.01")
project(plasma-dialer-with-subprojects VERSION ${PROJECT_VERSION})

include(FeatureSummary)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ModemManager, oFono
set(MODEM_SUBSYSTEM "ModemManager" CACHE STRING "Modem subsystem (ModemManager or oFono)")
set_property(CACHE MODEM_SUBSYSTEM PROPERTY STRINGS ModemManager oFono)
if (MODEM_SUBSYSTEM STREQUAL "ModemManager")
    add_definitions(-DMODEM_SUBSYSTEM_MM)
    message(STATUS "Using ModemManager")
elseif(MODEM_SUBSYSTEM STREQUAL "oFono")
    add_definitions(-DMODEM_SUBSYSTEM_OFONO)
    message(STATUS "Using oFono")
endif()

option(DIALER_BUILD_SHELL_OVERLAY "Enable building of Dialer lockscreen functionality" ON)
if (DIALER_BUILD_SHELL_OVERLAY)
    add_definitions(-DDIALER_BUILD_SHELL_OVERLAY)
endif() # DIALER_BUILD_SHELL_OVERLAY

# QT_MAJOR_VERSION is defined automatically by QtVersionOption (since ECM 5.82)
# which is included by KDEInstallDirs;
# This variables are updated below if Qt6 is found by ECM
set(QT_REQUIRED_VERSION "5.15.2")
set(KF_MIN_VERSION "5.101.0")
set(KF_MAJOR_VERSION "5")

find_package(ECM ${KF_MIN_VERSION} NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://projects.kde.org/projects/kdesupport/extra-cmake-modules")

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(ECMMarkNonGuiExecutable)
include(ECMConfiguredInstall)
include(ECMFindQmlModule)
include(ECMSetupVersion)
include(KDEClangFormat)
include(KDEGitCommitHooks)
include(ECMCheckOutboundLicense)

if (QT_MAJOR_VERSION STREQUAL "6")
    set(REQUIRED_QT_VERSION "6.4")
    set(KF_MIN_VERSION "5.240.0")
    set(KF_MAJOR_VERSION "6")
endif()

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX PLASMADIALER
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

find_package(KF${KF_MAJOR_VERSION}I18n NO_MODULE)
ki18n_install(po)

#set(DBUS_INTERFACES_PATH "${CMAKE_INSTALL_PREFIX}/${KDE_INSTALL_DBUSINTERFACEDIR}/")

add_subdirectory(kde-telephony-meta)

set(DBUS_INTERFACES_PATH "${CMAKE_INSTALL_PREFIX}/${KDE_INSTALL_DBUSINTERFACEDIR}/")
set(DBUS_INTERFACES_PATH "${SHARED_DBUS_XML_DIR}")
message("Using D-Bus interface descriptions from: ${DBUS_INTERFACES_PATH}")

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
