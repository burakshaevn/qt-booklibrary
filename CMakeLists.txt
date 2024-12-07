cmake_minimum_required(VERSION 3.16)

project(qt-booklibrary VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Подключаем необходимые модули Qt
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

# Источники главного проекта
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

# Подключаем подпроект backend
add_subdirectory(backend)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(qt-booklibrary
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}

    )
else()
    if(ANDROID)
        add_library(qt-booklibrary SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(qt-booklibrary
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Добавляем зависимости для qt-booklibrary
target_link_libraries(qt-booklibrary PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql backend)
target_include_directories(qt-booklibrary PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/backend/include)

# Настройки для macOS и Windows
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.qt-booklibrary)
endif()
set_target_properties(qt-booklibrary PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Установка
include(GNUInstallDirs)
install(TARGETS qt-booklibrary
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(qt-booklibrary)
endif()
