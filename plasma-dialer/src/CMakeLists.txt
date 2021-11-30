add_executable(plasmaphonedialer
    main.cpp
    resources.qrc   
)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED Core Quick Qml QuickControls2)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS CoreAddons I18n DBusAddons WindowSystem)

target_include_directories(plasmaphonedialer PRIVATE ${CMAKE_BINARY_DIR})

target_link_libraries(plasmaphonedialer
    Qt5::Core
    Qt5::Quick
    Qt5::Qml
    Qt5::QuickControls2
    KF5::CoreAddons
    KF5::I18n
    KF5::DBusAddons
    KF5::WindowSystem
)

install(TARGETS plasmaphonedialer ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES plasma_dialer.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFY5RCDIR})
