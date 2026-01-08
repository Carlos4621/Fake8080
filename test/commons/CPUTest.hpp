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
    
    // Exponer funciones de operaciones aritméticas
    using CPU::ADD_R;
    using CPU::ADC_R;
    using CPU::SUB_R;
    using CPU::SBB_R;
    using CPU::CMP_R;
    
    // Exponer funciones de incremento/decremento
    using CPU::INR_R;
    using CPU::DCR_R;
    using CPU::INR_DCR_R;
    
    // Exponer funciones de operaciones lógicas
    using CPU::ANA_R;
    using CPU::ORA_R;
    using CPU::XRA_R;
    using CPU::ANA_ORA_XRA_R;
    
    // Exponer funciones de rotación
    using CPU::RLC_R;
    using CPU::RAL_R;
    using CPU::RLC_RAL_R;
    using CPU::RRC_R;
    using CPU::RAR_R;
    using CPU::RRC_RAR_R;
    
    // Exponer el enum AritmeticOperation
    using CPU::AritmeticOperation;
    
    // Acceso a registros para testing
    using CPU::registers_m;
};

#endif // CPU_TEST_HELPER_HPP
