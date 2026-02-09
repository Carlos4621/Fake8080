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

# Test ejecutable para ADD/ADC/SUB/SBB/CMP con registros
add_executable(
  add_adc_sub_sbb_cmp_test
  test/ADD_ADC_SUB_SBB_CMP_Test.cpp
  src/CPU.cpp
  src/Registers.cpp
)

target_link_libraries(
  add_adc_sub_sbb_cmp_test
  GTest::gtest_main
)

# Test ejecutable para INR/DCR (Increment/Decrement)
add_executable(
  inr_dcr_test
  test/INR_DCR_Test.cpp
  src/CPU.cpp
  src/Registers.cpp
)

target_link_libraries(
  inr_dcr_test
  GTest::gtest_main
)

# Test ejecutable para ANA/ORA/XRA (operaciones lógicas)
add_executable(
  ana_ora_xra_test
  test/ANA_ORA_XRA_Test.cpp
  src/CPU.cpp
  src/Registers.cpp
)

target_link_libraries(
  ana_ora_xra_test
  GTest::gtest_main
)

# Test ejecutable para RLC/RAL (rotaciones a la izquierda)
add_executable(
  rlc_ral_test
  test/RLC_RAL_Test.cpp
  src/CPU.cpp
  src/Registers.cpp
)

target_link_libraries(
  rlc_ral_test
  GTest::gtest_main
)

# Test ejecutable para RRC/RAR (rotaciones a la derecha)
add_executable(
  rrc_rar_test
  test/RRC_RAR_Test.cpp
  src/CPU.cpp
  src/Registers.cpp
)

target_link_libraries(
  rrc_rar_test
  GTest::gtest_main
)

# Test ejecutable para STC/CMA/CMC/DAA (operaciones especiales)
add_executable(
  stc_cma_cmc_daa_test
  test/STC_CMA_CMC_DAA_Test.cpp
  src/CPU.cpp
  src/Registers.cpp
)

target_link_libraries(
  stc_cma_cmc_daa_test
  GTest::gtest_main
)

# Test ejecutable para MOV R1, R2
add_executable(
  mov_test
  test/MOV_Test.cpp
  src/CPU.cpp
  src/Registers.cpp
)

target_link_libraries(
  mov_test
  GTest::gtest_main
)

# Test ejecutable para MVI R, d8
add_executable(
  mvi_test
  test/MVI_Test.cpp
  src/CPU.cpp
  src/Registers.cpp
)

target_link_libraries(
  mvi_test
  GTest::gtest_main
)

# Test ejecutable para MOV M, R
add_executable(
  mov_m_r_test
  test/MOV_M_R_Test.cpp
  src/CPU.cpp
  src/Registers.cpp
)

target_link_libraries(
  mov_m_r_test
  GTest::gtest_main
)

# Test ejecutable para MOV R, M
add_executable(
  mov_r_m_test
  test/MOV_R_M_Test.cpp
  src/CPU.cpp
  src/Registers.cpp
)

target_link_libraries(
  mov_r_m_test
  GTest::gtest_main
)

# Test ejecutable para ADD/ADC/SUB/SBB con memoria
add_executable(
  add_adc_sub_sbb_m_test
  test/ADD_ADC_SUB_SBB_M_Test.cpp
  src/CPU.cpp
  src/Registers.cpp
)

target_link_libraries(
  add_adc_sub_sbb_m_test
  GTest::gtest_main
)

# Test ejecutable para ANA/ORA/XRA con memoria
add_executable(
  ana_ora_xra_m_test
  test/ANA_ORA_XRA_M_Test.cpp
  src/CPU.cpp
  src/Registers.cpp
)

target_link_libraries(
  ana_ora_xra_m_test
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(registers_test)
gtest_discover_tests(cpu_flags_test)
gtest_discover_tests(arithmetic_operation_test)
gtest_discover_tests(add_adc_sub_sbb_cmp_test)
gtest_discover_tests(inr_dcr_test)
gtest_discover_tests(ana_ora_xra_test)
gtest_discover_tests(rlc_ral_test)
gtest_discover_tests(rrc_rar_test)
gtest_discover_tests(stc_cma_cmc_daa_test)
gtest_discover_tests(mov_test)
gtest_discover_tests(mvi_test)
gtest_discover_tests(mov_m_r_test)
gtest_discover_tests(mov_r_m_test)
gtest_discover_tests(add_adc_sub_sbb_m_test)
gtest_discover_tests(ana_ora_xra_m_test)