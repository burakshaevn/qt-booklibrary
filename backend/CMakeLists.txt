cmake_minimum_required(VERSION 3.16)

project(backend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    # src/main.cpp
    src/database_manager.cpp
    src/library.cpp
    include/database_manager.h
    include/library.h
)

add_library(backend ${SOURCES}
    include/domain.h
    include/bookmarks.h src/bookmarks.cpp
    include/cart.h src/cart.cpp
    include/profile.h src/profile.cpp
)

target_include_directories(backend PRIVATE include)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Sql Widgets)

target_link_libraries(backend PRIVATE Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Widgets)
