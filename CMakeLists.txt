cmake_minimum_required(VERSION 3.14)

project(ciftl VERSION 1.0.0.2 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 选择编译器
set(GCC_OR_CLANG ((CMAKE_CXX_COMPILER_ID MATCHES "Clang") OR CMAKE_COMPILER_IS_GNUCXX))

# 设置编译输出目录
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

# 设置构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: Debug Release." FORCE)
endif()

# 根据构建类型设置编译器选项
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Build type: Debug")
    if(MSVC)
        # Visual Studio 编译器的 Debug 选项
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Od /Zi /D_DEBUG")
    else()
        # GCC 编译器的 Debug 选项
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
    endif()
else()
    message(STATUS "Build type: Release")
    if(MSVC)
        # Visual Studio 编译器的 Release 选项
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /Oi /Gy /DNDEBUG")
    else()
        # GCC 编译器的 Release 选项
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
    endif()
endif()

# 对MSVC的特殊支持
if(MSVC)
    # 对于MSVC需要支持utf-8
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /utf-8")
    # 编译选项
    set (CMAKE_CONFIGURATION_TYPES "Debug;Release")
    # Debug生成的后缀
    set (CMAKE_DEBUG_POSTFIX D)
    # windows系统动态库生成lib文件的命令
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(MSVC)

# 对于MSVC而言，不需要指定目录为Debug或是Release
if (NOT MSVC)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(LIBRARY_OUTPUT_PATH "${LIBRARY_OUTPUT_PATH}/Debug")
        set(EXECUTABLE_OUTPUT_PATH "${EXECUTABLE_OUTPUT_PATH}/Debug")
    else ()
        set(LIBRARY_OUTPUT_PATH "${LIBRARY_OUTPUT_PATH}/Release")
        set(EXECUTABLE_OUTPUT_PATH "${EXECUTABLE_OUTPUT_PATH}/Release")
    endif ()
endif ()

include_directories(${CMAKE_SOURCE_DIR}/ciftl_core/include/)
include_directories(${CMAKE_SOURCE_DIR}/ciftl_gui/include/)

add_subdirectory(ciftl_core)

option(COMPILE_CIFTL_GUI "Compile ciftl_gui" ON)
if(COMPILE_CIFTL_GUI)
    add_subdirectory(ciftl_gui)
endif(COMPILE_CIFTL_GUI)
