# SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
#
# SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

set(metatypes_SRCS
    include/kTelephonyMetaTypes/dialer-types.h
    )

add_library(ktelephonymetatypes STATIC ${metatypes_SRCS})

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} CONFIG REQUIRED Core DBus)

include_directories(${QtCore_INCLUDE_DIRS})
include_directories(${QtDBus_INCLUDE_DIRS})

target_include_directories(ktelephonymetatypes PUBLIC include)
target_link_libraries(ktelephonymetatypes PRIVATE
    Qt::Core
    Qt::DBus
    )

file(GLOB HEADERS_TO_MOC include/*.h)

qt_wrap_cpp(PROCESSED_MOCS
    ${HEADERS_TO_MOC}
    TARGET ktelephonymetatypes
    OPTIONS --no-notes)

target_sources(ktelephonymetatypes PRIVATE ${PROCESSED_MOCS})

install(FILES ${metatypes_SRCS} DESTINATION "${KDE_INSTALL_INCLUDEDIR_KF}/kTelephonyMetaTypes")
install(TARGETS ktelephonymetatypes DESTINATION "${KDE_INSTALL_LIBDIR}")
# TODO: install KF${KF_MAJOR_VERSION} cmake files
