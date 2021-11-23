find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED Core Quick)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Contacts People ModemManagerQt)
find_package(PhoneNumber COMPONENTS PhoneNumber REQUIRED)

add_library(contactphonenumbermapper STATIC phone-number-utils.cpp contact-phone-number-mapper.cpp)
target_compile_definitions(contactphonenumbermapper PRIVATE -DQT_NO_CAST_FROM_ASCII)
target_link_libraries(contactphonenumbermapper PUBLIC
    Qt5::Core
    KF5::Contacts
    KF5::People
    KF5::PeopleBackend
    PhoneNumber::PhoneNumber
)
target_include_directories(contactphonenumbermapper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
