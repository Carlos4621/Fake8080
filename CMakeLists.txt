cmake_minimum_required(VERSION 3.10)

project(fake8080 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

# Executable principal
add_executable(fake8080 main.cpp src/CPU.cpp src/Fake8080.cpp src/Registers.cpp)

# Configuración de Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
# Para Windows: Prevenir la sobrescritura de las flags de compilación del proyecto padre
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Test ejecutable para Registers
add_executable(
  registers_test
  test/RegistersTest.cpp
  src/Registers.cpp
)

target_link_libraries(
  registers_test
  GTest::gtest_main
)

# Test ejecutable para CPU Flags
add_executable(
  cpu_flags_test
  test/CPUFlagsTest.cpp
  src/CPU.cpp
  src/Registers.cpp
)

target_link_libraries(
  cpu_flags_test
  GTest::gtest_main
)

# Test ejecutable para Arithmetic Operation
add_executable(
  arithmetic_operation_test
  test/ArithmeticOperationTest.cpp
  src/CPU.cpp
  src/Registers.cpp
)

target_link_libraries(
  arithmetic_operation_test
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(registers_test)
gtest_discover_tests(cpu_flags_test)
gtest_discover_tests(arithmetic_operation_test)