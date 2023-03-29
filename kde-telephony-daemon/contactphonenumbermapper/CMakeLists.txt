# SPDX-FileCopyrightText: 2018 Volker Krause <vkrause@kde.org>
# SPDX-FileCopyrightText: 2022 Alexey Andreyev <aa13q@ya.ru>
#
# SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} CONFIG REQUIRED Core Quick)
find_package(KF${KF_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS Contacts People)
find_package(PhoneNumber COMPONENTS PhoneNumber REQUIRED)

add_library(contactphonenumbermapper STATIC phone-number-utils.cpp contact-phone-number-mapper.cpp)
target_compile_definitions(contactphonenumbermapper PRIVATE -DQT_NO_CAST_FROM_ASCII)
target_link_libraries(contactphonenumbermapper PUBLIC
    Qt::Core
    KF${QT_MAJOR_VERSION}::Contacts
    KF${QT_MAJOR_VERSION}::People
    KF${QT_MAJOR_VERSION}::PeopleBackend
    PhoneNumber::PhoneNumber
)
target_include_directories(contactphonenumbermapper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
