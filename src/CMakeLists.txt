# SPDX-FileCopyrightText: (C) 2015 Marco Martin <notmart@gmail.com>
# SPDX-FileCopyrightText: (C) 2015 Martin Klapetek <mklapetek@kde.org>
# SPDX-FileCopyrightText: (C) 2019 Jonah Brüchert <jbb.prv@gmx.de>
# SPDX-FileCopyrightText: (C) 2020 Nicolas Fella <nicolas.fella@gmx.de>
# SPDX-FileCopyrightText: (C) 2020 Bhushan Shah <bshah@kde.org>
# SPDX-License-Identifier: BSD-3-Clause

add_executable(plasmaphonedialer
    main.cpp
    dialerutils.cpp
    call-handler.cpp
    callhistorymodel.cpp
    call-manager.cpp
    qpulseaudioengine.cpp
    database.cpp
    contactmapper.cpp
    resources.qrc
)

target_include_directories(plasmaphonedialer PRIVATE ${CMAKE_BINARY_DIR})

target_link_libraries(plasmaphonedialer
    Qt5::Quick
    Qt5::Sql
    KF5::CoreAddons
    KF5::I18n
    KF5::DBusAddons
    KF5::Notifications
    KF5::People
    KF5::PeopleBackend
    KF5::Contacts
    TelepathyQt5::Core
    PhoneNumber::PhoneNumber
    PkgConfig::LIBPULSE
)

install(TARGETS plasmaphonedialer ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES plasma_dialer.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFY5RCDIR})

ecm_install_configured_files(INPUT org.freedesktop.Telepathy.Client.Plasma.Dialer.service.in DESTINATION ${DBUS_SERVICES_INSTALL_DIR} @ONLY)

install(FILES Plasma.Dialer.client DESTINATION ${SHARE_INSTALL_PREFIX}/telepathy/clients/)
