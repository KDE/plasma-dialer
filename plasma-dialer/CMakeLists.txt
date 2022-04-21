# SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
#
# SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

# plasma-dialer

add_subdirectory(src)

install(FILES org.kde.phone.dialer.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES dialer.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps/)
install(FILES org.kde.phone.dialer.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
