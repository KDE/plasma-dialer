# SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
#
# SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

# kde-telephony-daemon

add_subdirectory(contactphonenumbermapper)
add_subdirectory(src)

include(ECMAddTests)
if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()
