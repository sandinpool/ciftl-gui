project(test_ciftl_core)

find_package(GTest CONFIG REQUIRED)
find_package(fmt REQUIRED)

include_directories(${CIFTL_CORE_INCLUDE_PATH})

add_executable(test_ciftl_core 
    test_core.h 
    test_encoding.cpp 
    test_crypter.cpp 
    test_hash.cpp 
    test_core.cpp
)

target_link_libraries(test_ciftl_core PRIVATE
    GTest::gtest
    GTest::gtest_main
    fmt::fmt
    ciftl_core)

add_test(NAME TestEncoding COMMAND test_ciftl_core)
add_test(NAME TestCrypter COMMAND test_ciftl_core)
add_test(NAME TestHash COMMAND test_ciftl_core)
add_test(NAME TestCore COMMAND test_ciftl_core)
