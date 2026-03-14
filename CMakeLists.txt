cmake_minimum_required(VERSION 3.16)
project(flight-ticket-system LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 自动处理 moc/uic/rcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

add_executable(${PROJECT_NAME}
    # entry
    src/main.cpp

    # database
    src/database/dbmanager.cpp
    src/database/dbmanager.h

    # models
    src/models/usersession.cpp
    src/models/usersession.h
    src/models/user.h
    src/models/flight.h
    src/models/order.h

    # ui (注意你的文件名是大写 A 的 AdminWindow.cpp/ui)
    src/ui/loginwindow.cpp
    src/ui/loginwindow.h
    src/ui/loginwindow.ui

    src/ui/registerwindow.cpp
    src/ui/registerwindow.h
    src/ui/registerwindow.ui

    src/ui/mainwindow.cpp
    src/ui/mainwindow.h
    src/ui/mainwindow.ui

    src/ui/AdminWindow.cpp
    src/ui/adminwindow.h
    src/ui/AdminWindow.ui

    src/ui/flightsearchwindow.cpp
    src/ui/flightsearchwindow.h
    src/ui/flightsearchwindow.ui

    src/ui/ordermanagerwindow.cpp
    src/ui/ordermanagerwindow.h
    src/ui/ordermanagerwindow.ui

    src/ui/bookTicketConfirm.cpp
    src/ui/bookTicketConfirm.h
    src/ui/bookTicketConfirm.ui
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Sql
)

# MinGW 下可选：让编译信息更友好
if (MINGW)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()