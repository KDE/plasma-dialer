# SPDX-FileCopyrightText: 2021 Nicolas Fella <nicolas.fella@gmx.de>
# SPDX-License-Identifier: BSD-3-Clause

find_package(Qt6Test REQUIRED)

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED Test Sql)
find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS People)

ecm_add_test(TEST_NAME contactmappertest contact-mapper-test.cpp fake-contact-source.cpp LINK_LIBRARIES Qt::Test Qt::Sql KF6::PeopleBackend contactphonenumbermapper)
