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
    
    // Exponer funciones de operaciones aritméticas con memoria
    using CPU::ADD_M;
    using CPU::ADC_M;
    using CPU::SUB_M;
    using CPU::SBB_M;
    using CPU::CMP_M;
    
    // Exponer funciones de incremento/decremento
    using CPU::INR_R;
    using CPU::DCR_R;
    using CPU::INR_DCR_R;
    using CPU::INR_M;
    using CPU::DCR_M;
    using CPU::INX_RR;
    using CPU::DCX_RR;
    using CPU::DAD_RR;
    
    // Exponer funciones de operaciones lógicas
    using CPU::ANA_R;
    using CPU::ORA_R;
    using CPU::XRA_R;
    using CPU::ANA_ORA_XRA_R;
    
    // Exponer funciones de operaciones lógicas con memoria
    using CPU::ANA_M;
    using CPU::ORA_M;
    using CPU::XRA_M;
    
    // Exponer funciones de rotación
    using CPU::RLC_R;
    using CPU::RAL_R;
    using CPU::RRC_R;
    using CPU::RAR_R;
    
    // Exponer funciones de control de flags y operaciones especiales
    using CPU::STC;
    using CPU::CMA;
    using CPU::CMC;
    using CPU::DAA;
    
    // Exponer función MOV
    using CPU::MOV_R_R;
    
    // Exponer función MVI
    using CPU::MVI_R_d8;
    using CPU::MVI_M_d8;
    
    // Exponer función MOV M,R
    using CPU::MOV_M_R;
    
    // Exponer función MOV R,M
    using CPU::MOV_R_M;
    
    // Exponer el enum AritmeticOperation
    using CPU::AritmeticOperation;
    
    // Acceso a registros para testing
    using CPU::registers_m;
    
    // Acceso a ROM para testing
    using CPU::rom_m;
};

#endif // CPU_TEST_HELPER_HPP
