# cmake_minimum_required(VERSION 3.16)

# project(backend LANGUAGES CXX)

# set(CMAKE_AUTOUIC ON)
# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTORCC ON)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core LinguistTools Widgets Sql)
# find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core LinguistTools Widgets Sql)

# set(PostgreSQL_LIBRARY "C:/dev/PostgreSQL/17/lib/libpq.lib")
# set(PostgreSQL_INCLUDE_DIR "C:/dev/PostgreSQL/17/include")

# find_package(PostgreSQL REQUIRED)

# set(TS_FILES backend_ru_RU.ts)

# add_library(backend
#     src/main.cpp
#     ${TS_FILES}
#     include/database_manager.h src/database_manager.cpp
#     include/library.h src/library.cpp
# )

# target_link_libraries(backend PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Sql PostgreSQL::PostgreSQL)
# target_include_directories(backend PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# target_include_directories(backend PRIVATE ${Qt${QT_VERSION_MAJOR}Core_INCLUDE_DIRS})

# message(STATUS "Qt version: ${Qt${QT_VERSION_MAJOR}_VERSION_STRING}")
# message(STATUS "Qt Core include dir: ${Qt${QT_VERSION_MAJOR}Core_INCLUDE_DIRS}")
# message(STATUS "Qt Sql include dir: ${Qt${QT_VERSION_MAJOR}Sql_INCLUDE_DIRS}")

# if(COMMAND qt_create_translation)
#     qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
# else()
#     qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
# endif()

# include(GNUInstallDirs)
# install(TARGETS backend
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#     INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
# )
cmake_minimum_required(VERSION 3.16)

project(backend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/main.cpp
    src/database_manager.cpp
    src/library.cpp
    include/database_manager.h
    include/library.h
)

add_library(backend ${SOURCES})

target_include_directories(backend PRIVATE include)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Sql Widgets)

target_link_libraries(backend PRIVATE Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Widgets)
