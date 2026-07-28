cmake_minimum_required(VERSION 3.16)

project(QTDS VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(myQTApp
    myQTApp.cpp
)

qt_add_qml_module(myQTApp
    URI QTDS
    VERSION 1.0
    QML_FILES
        Main.qml
)

# Yocto/Embedded Linux specific settings
# Remove desktop-specific bundle properties for embedded builds

target_link_libraries(myQTApp
    PRIVATE Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS myQTApp
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
