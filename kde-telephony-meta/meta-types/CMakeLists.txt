# SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
#
# SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

set(metatypes_SRCS
    include/kTelephonyMetaTypes/dialer-types.h
    )

add_library(ktelephonymetatypes STATIC ${metatypes_SRCS})

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED Core DBus)

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5DBus_INCLUDE_DIRS})

target_include_directories(ktelephonymetatypes PUBLIC include)
target_link_libraries(ktelephonymetatypes PRIVATE
    Qt5::Core
    Qt5::DBus
    )

file(GLOB HEADERS_TO_MOC include/*.h)

qt5_wrap_cpp(PROCESSED_MOCS
    ${HEADERS_TO_MOC}
    TARGET ktelephonymetatypes
    OPTIONS --no-notes)

target_sources(ktelephonymetatypes PRIVATE ${PROCESSED_MOCS})

install(FILES ${metatypes_SRCS} DESTINATION "${KDE_INSTALL_INCLUDEDIR_KF5}/kTelephonyMetaTypes")
install(TARGETS ktelephonymetatypes DESTINATION "${KDE_INSTALL_LIBDIR}")
# TODO: install KF5 cmake files
