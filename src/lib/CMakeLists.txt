add_library(dialer STATIC contactmapper.cpp)

target_link_libraries(dialer PUBLIC Qt::Core Qt::DBus KF5::People PkgConfig::LIBQOFONO PRIVATE PhoneNumber::PhoneNumber KF5::Contacts KF5::PeopleBackend)
