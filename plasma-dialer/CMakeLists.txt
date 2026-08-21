# SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
#
# SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

# plasma-dialer

add_subdirectory(shared)
add_subdirectory(call-screen)
add_subdirectory(dialer)

configure_file(org.kde.plasma.dialer.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/org.kde.plasma.dialer.desktop @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.plasma.dialer.desktop DESTINATION ${KDE_INSTALL_APPDIR})

configure_file(org.kde.plasma.callscreen.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/org.kde.plasma.callscreen.desktop @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.plasma.callscreen.desktop DESTINATION ${KDE_INSTALL_APPDIR})

install(FILES dialer.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps/)
install(FILES org.kde.plasma.dialer.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

# for Qt Creator IDE:
set(DEPLOY_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/org.kde.plasma.dialer.desktop.in
    ${CMAKE_CURRENT_SOURCE_DIR}/dialer.svg
    ${CMAKE_CURRENT_SOURCE_DIR}/org.kde.plasma.dialer.appdata.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/org.kde.plasma.callscreen.desktop.in
)
add_custom_target(deploy-files SOURCES ${DEPLOY_FILES})
