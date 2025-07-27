enable_testing()

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS *.cpp)

foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})

    find_package(spdlog CONFIG REQUIRED)
    target_link_libraries(${test_name} gtest_main spdlog::spdlog)

    include(GoogleTest)
    gtest_discover_tests(${test_name})
endforeach()