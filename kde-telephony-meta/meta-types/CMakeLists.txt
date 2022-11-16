# SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
#
# SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

set(metatypes_SRCS
    include/kTelephonyMetaTypes/dialer-types.h
    include/kTelephonyMetaTypes/DialerTypes
    )

set(CMAKE_AUTOMOC ON)

add_library(ktelephonymetatypes SHARED ${metatypes_SRCS})

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} CONFIG REQUIRED Core DBus)

include_directories(${QtCore_INCLUDE_DIRS})
include_directories(${QtDBus_INCLUDE_DIRS})

target_include_directories(ktelephonymetatypes PUBLIC include)
target_link_libraries(ktelephonymetatypes PRIVATE
    Qt::Core
    Qt::DBus
    )

install(FILES ${metatypes_SRCS} DESTINATION "${KDE_INSTALL_INCLUDEDIR_KF}/kTelephonyMetaTypes" COMPONENT Devel)
install(TARGETS ktelephonymetatypes DESTINATION "${KDE_INSTALL_LIBDIR}" COMPONENT Devel)
