add_executable(test_navigation_security test_navigation_security.cpp ../../src/navigation/navigation_handler.cpp)
add_test(NAME SecurityTests COMMAND test_navigation_security)
