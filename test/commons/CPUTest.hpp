#ifndef CPU_TEST_HELPER_HPP
#define CPU_TEST_HELPER_HPP

#include "../../include/CPU.hpp"

// Clase helper para exponer métodos privados de CPU en tests
class CPUTest : public CPU {
public:
    // Exponer métodos de gestión de flags
    using CPU::manageZeroFlag;
    using CPU::manageParityFlag;
    using CPU::manageSignedFlag;
    using CPU::manageCarryFlag;
    using CPU::manageAuxilaryCarryFlag;
    
    // Exponer método de operaciones aritméticas
    using CPU::aritmeticOperation_8bits;
    
    // Exponer método template de operaciones con registros
    using CPU::ADD_ADC_SUB_SBB_CMP_R;
    
    // Exponer el enum Operations
    using CPU::Operations;
    
    // Acceso a registros para testing
    using CPU::registers_m;
};

#endif // CPU_TEST_HELPER_HPP
