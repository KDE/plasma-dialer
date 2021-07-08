# SPDX-FileCopyrightText: 2021 Nicolas Fella <nicolas.fella@gmx.de>
# SPDX-License-Identifier: BSD-3-Clause

find_package(Qt5Test REQUIRED)

ecm_add_test(TEST_NAME contactmappertest contactmappertest.cpp fakecontactsource.cpp LINK_LIBRARIES Qt5::Test Qt5::Sql KF5::PeopleBackend dialer)
