# SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
#
# SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

set(SHARED_DBUS_XML_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/manual
    CACHE INTERNAL ""
    )

set(DBUS_XML_FILES ${CMAKE_CURRENT_SOURCE_DIR}/manual/org.kde.telephony.CallHistoryDatabase.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/manual/org.kde.telephony.ContactUtils.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/manual/org.kde.telephony.DeviceUtils.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/manual/org.kde.telephony.DialerUtils.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/manual/org.kde.telephony.CallUtils.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/manual/org.kde.telephony.UssdUtils.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/manual/org.kde.telephony.VoiceMailUtils.xml
)

# install DBus interface xml to dbus interface directory
install(FILES ${DBUS_XML_FILES} DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})

# for Qt Creator IDE:
add_custom_target(dbus-xml-files SOURCES ${DBUS_XML_FILES})
