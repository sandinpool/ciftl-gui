cmake_minimum_required(VERSION 3.14)

project(ciftl-gui VERSION 1.0.0.2 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CIFTL_GUI_INCLUDE_PATH "${PROJECT_SOURCE_DIR}/include")
set(CIFTL_GUI_SOURCE_PATH "${PROJECT_SOURCE_DIR}/src")

file(GLOB CIFTL_GUI_MAIN_HEADER "${CIFTL_GUI_INCLUDE_PATH}/*.h")
file(GLOB CIFTL_GUI_MAIN_SOURCE "${CIFTL_GUI_SOURCE_PATH}/*.cpp")
file(GLOB CIFTL_GUI_MAIN_UI "${CIFTL_GUI_SOURCE_PATH}/*.ui")
file(GLOB CIFTL_GUI_CRYPTION_HEADER "${CIFTL_GUI_INCLUDE_PATH}/cryption/*.h")
file(GLOB CIFTL_GUI_CRYPTION_SOURCE "${CIFTL_GUI_SOURCE_PATH}/cryption/*.cpp")
file(GLOB CIFTL_GUI_CRYPTION_UI "${CIFTL_GUI_SOURCE_PATH}/cryption/*.ui")
file(GLOB CIFTL_GUI_ETC_HEADER "${CIFTL_GUI_INCLUDE_PATH}/etc/*.h")
file(GLOB CIFTL_GUI_ETC_SOURCE "${CIFTL_GUI_SOURCE_PATH}/etc/*.cpp")
file(GLOB CIFTL_GUI_ETC_UI "${CIFTL_GUI_SOURCE_PATH}/etc/*.ui")

set(TS_FILES ciftl_gui_zh_CN.ts)

set(PROJECT_RESOURCES
    ${PROJECT_SOURCE_DIR}/res/main.qrc
    ${PROJECT_SOURCE_DIR}/res/qss.qrc
)

set(RC_FILE
    ${PROJECT_SOURCE_DIR}/res/ciftl.rc
)

set(PROJECT_SOURCES
    ${CIFTL_GUI_MAIN_HEADER}
    ${CIFTL_GUI_MAIN_SOURCE}
    ${CIFTL_GUI_MAIN_UI}
    ${CIFTL_GUI_CRYPTION_HEADER}
    ${CIFTL_GUI_CRYPTION_SOURCE}
    ${CIFTL_GUI_CRYPTION_UI}
    ${CIFTL_GUI_ETC_HEADER}
    ${CIFTL_GUI_ETC_SOURCE}
    ${CIFTL_GUI_ETC_UI}
    ${TS_FILES}
)

include_directories(${CIFTL_GUI_INCLUDE_PATH})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)
find_package(OpenSSL REQUIRED)
find_package(fmt REQUIRED)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${PROJECT_RESOURCES}
        ${RC_FILE}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(${PROJECT_NAME}
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
    OpenSSL::SSL OpenSSL::Crypto
    fmt::fmt
    ciftl_core)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.sdyz.ciftl_gui)
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
