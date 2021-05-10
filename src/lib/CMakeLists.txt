add_library(dialer STATIC contactmapper.cpp)

target_link_libraries(dialer PUBLIC Qt::Core KF5::People PRIVATE PhoneNumber::PhoneNumber KF5::Contacts KF5::PeopleBackend)
