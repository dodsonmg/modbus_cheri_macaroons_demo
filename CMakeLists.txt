cmake_minimum_required(VERSION 3.5)
project(modbus_macaroons_demo)

macro(print_all_variables)
    message(STATUS "print_all_variables------------------------------------------{")
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
    message(STATUS "print_all_variables------------------------------------------}")
endmacro()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(libmodbus REQUIRED)
find_package(libmacaroons REQUIRED)

add_executable(unit_test_client
  src/unit-test-client.cpp)
target_include_directories(unit_test_client PRIVATE
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
$<INSTALL_INTERFACE:include>)
ament_target_dependencies(unit_test_client libmodbus libmacaroons)

install(TARGETS
unit_test_client
DESTINATION lib/${PROJECT_NAME})

add_executable(unit_test_server
  src/unit-test-server.cpp
  src/cheri_modbus_shim.c)
target_include_directories(unit_test_server PRIVATE
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
$<INSTALL_INTERFACE:include>)
ament_target_dependencies(unit_test_server libmodbus libmacaroons)

install(TARGETS
unit_test_server
DESTINATION lib/${PROJECT_NAME})

print_all_variables()

ament_package()