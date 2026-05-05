#ifndef FAKE8080_TEST_HELPER_HPP
#define FAKE8080_TEST_HELPER_HPP

#include "../../include/Fake8080.hpp"
#include "../../include/MemoryBus.hpp"
#include "../../include/IOBus.hpp"
#include <memory>
#include <span>
#include <vector>

// Clase helper para exponer métodos privados de Fake8080 en tests
class Fake8080Test : public Fake8080 {
public:
    MemoryBus memoryBus_test;
    IOBus ioBus_test;
    
    // Almacenar las regiones de memoria para tests (mantenerlas vivas)
    std::vector<std::unique_ptr<RamRegion>> ramRegions_test;
    
    // Constructor que inicializa el Fake8080 con buses de test
    Fake8080Test() : Fake8080(memoryBus_test, ioBus_test) {}
    
    // Helper para establecer memoria RAM en el bus (reemplazo de setROM)
    template<size_t N>
    void mapMemory(std::array<uint8_t, N>& memory, uint16_t startAddr = 0) {
        auto ramRegion = std::make_unique<RamRegion>(std::span<uint8_t>(memory.data(), memory.size()));
        memoryBus_test.map(startAddr, startAddr + N - 1, *ramRegion);
        ramRegions_test.push_back(std::move(ramRegion));
    }
    
    // Sobrecarga para std::vector
    void mapMemory(std::vector<uint8_t>& memory, uint16_t startAddr = 0) {
        auto ramRegion = std::make_unique<RamRegion>(std::span<uint8_t>(memory.data(), memory.size()));
        memoryBus_test.map(startAddr, startAddr + memory.size() - 1, *ramRegion);
        ramRegions_test.push_back(std::move(ramRegion));
    }
    
    // Exponer métodos de gestión de flags
    using Fake8080::manageZeroFlag;
    using Fake8080::manageParityFlag;
    using Fake8080::manageSignedFlag;
    using Fake8080::manageCarryFlag;
    using Fake8080::manageAuxilaryCarryFlag;
    
    // Exponer método de operaciones aritméticas
    using Fake8080::aritmeticOperation_8bits;
    
    // Exponer método template de operaciones con registros
    using Fake8080::ADD_ADC_SUB_SBB_CMP_R;
    
    // Exponer funciones de operaciones aritméticas
    using Fake8080::ADD_R;
    using Fake8080::ADC_R;
    using Fake8080::SUB_R;
    using Fake8080::SBB_R;
    using Fake8080::CMP_R;
    
    // Exponer funciones de operaciones aritméticas con memoria
    using Fake8080::ADD_M;
    using Fake8080::ADC_M;
    using Fake8080::SUB_M;
    using Fake8080::SBB_M;
    using Fake8080::CMP_M;
    
    // Exponer funciones de incremento/decremento
    using Fake8080::INR_R;
    using Fake8080::DCR_R;
    using Fake8080::INR_DCR_R;
    using Fake8080::INR_M;
    using Fake8080::DCR_M;
    using Fake8080::INX_RR;
    using Fake8080::DCX_RR;
    using Fake8080::DAD_RR;
    using Fake8080::LXI_RR_d16;
    using Fake8080::SHLD_a16;
    using Fake8080::LHLD_a16;
    
    // Exponer funciones de operaciones inmediatas
    using Fake8080::ADI_d8;
    using Fake8080::ACI_d8;
    using Fake8080::SBI_d8;
    using Fake8080::CPI_d8;
    
    // Exponer funciones de operaciones lógicas inmediatas
    using Fake8080::ANI_d8;
    using Fake8080::ORI_d8;
    using Fake8080::XRI_d8;
    
    // Exponer funciones de almacenamiento y carga
    using Fake8080::STAX_RR;
    using Fake8080::STA_a16;
    using Fake8080::LDAX_RR;
    using Fake8080::LDA_a16;
    
    // Exponer funciones de stack
    using Fake8080::PUSH_RR;
    using Fake8080::POP_RR;
    using Fake8080::XTHL;
    using Fake8080::XCHG;
    using Fake8080::SPHL;
    using Fake8080::PCHL;
    
    // Exponer funciones de salto
    using Fake8080::JMP_a16;
    using Fake8080::JZ_a16;
    using Fake8080::JNZ_a16;
    using Fake8080::JC_a16;
    using Fake8080::JNC_a16;
    using Fake8080::JP_a16;
    using Fake8080::JM_a16;
    using Fake8080::JPE_a16;
    using Fake8080::JPO_a16;
    
    // Exponer funciones de llamada y retorno
    using Fake8080::CALL_a16;
    using Fake8080::RET;
    using Fake8080::CZ_a16;
    using Fake8080::CNZ_a16;
    using Fake8080::CC_a16;
    using Fake8080::CNC_a16;
    using Fake8080::CM_a16;
    using Fake8080::CP_a16;
    using Fake8080::CPE_a16;
    using Fake8080::CPO_a16;
    using Fake8080::RZ;
    using Fake8080::RNZ;
    using Fake8080::RC;
    using Fake8080::RNC;
    using Fake8080::RM;
    using Fake8080::RP;
    using Fake8080::RPE;
    using Fake8080::RPO;

    // Exponer funciones RST
    using Fake8080::RST_N;

    // Exponer funciones de interrupciones
    using Fake8080::EI;
    
    // Exponer funciones de operaciones lógicas
    using Fake8080::ANA_R;
    using Fake8080::ORA_R;
    using Fake8080::XRA_R;
    using Fake8080::ANA_ORA_XRA_R;
    
    // Exponer funciones de operaciones lógicas con memoria
    using Fake8080::ANA_M;
    using Fake8080::ORA_M;
    using Fake8080::XRA_M;
    
    // Exponer funciones de rotación
    using Fake8080::RLC_R;
    using Fake8080::RAL_R;
    using Fake8080::RRC_R;
    using Fake8080::RAR_R;
    
    // Exponer funciones de control de flags y operaciones especiales
    using Fake8080::STC;
    using Fake8080::CMA;
    using Fake8080::CMC;
    using Fake8080::DAA;

    // Exponer funciones de I/O
    using Fake8080::IN_d8;
    using Fake8080::OUT_d8;
    
    // Exponer función MOV
    using Fake8080::MOV_R_R;
    
    // Exponer función MVI
    using Fake8080::MVI_R_d8;
    using Fake8080::MVI_M_d8;
    
    // Exponer función MOV M,R
    using Fake8080::MOV_M_R;
    
    // Exponer función MOV R,M
    using Fake8080::MOV_R_M;
    
    // Exponer el enum AritmeticOperation
    using Fake8080::AritmeticOperation;
    
    // Exponer la tabla de opcodes
    using Fake8080::opcodes_m;
    using Fake8080::Opcodes_Number;
    
    // Acceso a registros para testing
    using Fake8080::registers_m;
    
    // Acceso a PC para testing
    using Fake8080::pc_m;
    
    // Acceso a los buses para testing
    using Fake8080::memoryBus_m;
    using Fake8080::IOBus_m;
};

#endif // FAKE8080_TEST_HELPER_HPP
