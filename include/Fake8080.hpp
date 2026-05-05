#ifndef FAKE8080_HEADER
#define FAKE8080_HEADER

#include <cstdint>
#include <stdexcept>
#include "Registers.hpp"
#include "OpcodesCycles.hpp"
#include "MemoryBus.hpp"
#include "IOBus.hpp"
#include "BitsUtilities.hpp"

class Fake8080Test;

class Fake8080 {
    friend class Fake8080Test;
    
public:
    Fake8080(MemoryBus& memoryBus, IOBus& ioBus) noexcept;

    void cycle();

    void requestInterrupt(uint8_t interruptNum);

    void reset();

private:
    enum class AritmeticOperation : uint8_t { ADD = 0, SUB };
    enum class LogicOperation : uint8_t { AND = 0, OR, XOR };
    enum class ShiftDirection : uint8_t { RIGHT = 0, LEFT };

    using MemberFunction = uint8_t(Fake8080::*)();

    static constexpr uint16_t Opcodes_Number{ 256 };

    static const std::array<MemberFunction, Opcodes_Number> opcodes_m;

    uint16_t pc_m{ 0 };
    Registers registers_m;

    MemoryBus& memoryBus_m;
    IOBus& IOBus_m;

    bool interruptsEnableRequested_m{ false };
    bool interruptsEnabled_m{ false };

    bool interruptRequested_m{ false };
    uint8_t interruptVectorRequested_m{ 0 };

    bool halted_m{ false };

    /// @brief Lee el siguiente byte e incrementa el pc
    /// @return Byte leído
    [[nodiscard]]
    uint8_t readNextByte();

    /// @brief Lee los siguientes 2 bytes, los combina en little endian e incrementa el pc
    /// @return Los siguientes 2 bytes en little endian
    [[nodiscard]]
    uint16_t readNextTwoBytes();

    /// @brief Función de convenienciá para decrementar SP
    void decreaseSP();

    /// @brief Función de conveniencia para incrementar SP
    void increaseSP();

    /// @brief Devuelve el valor al que HL apunta, es decir [HL]
    /// @return [HL]
    [[nodiscard]]
    uint8_t getM();

    /// @brief Carga [HL] al registro W
    void loadMtoW();

    /// @brief Escribe W en [HL]
    void writeWtoM();

    /// @brief Escribe un byte directamente en [HL]
    /// @param value Byte a escribir
    void writeToM(uint8_t value);

    /// @brief Escribe un valor de 16 bits en memoria (formato little-endian)
    /// @param address Dirección base
    /// @param value Valor a escribir
    void writeTwoBytes(uint16_t address, uint16_t value);

    /// @brief Lee un valor de 16 bits desde memoria (formato little-endian)
    /// @param address Dirección base
    /// @return Valor leído
    [[nodiscard]]
    uint16_t readTwoBytes(uint16_t address) const;
    
    /// @brief Establece el zero flag si el valor es cero
    /// @param value Valor a evaluar
    void manageZeroFlag(uint8_t value) noexcept;

    /// @brief Establece el parity flag si el valor cumple con los requisitos
    /// @param value Valor a evaluar
    void manageParityFlag(uint8_t value) noexcept;

    /// @brief Establece el signed flag si el valor cumple con los requisitos
    /// @param value Valor a evaluar
    void manageSignedFlag(uint8_t value) noexcept;

    /// @brief Establece el carry flag si la suma desencadena un overflow
    /// @param first Primer operando
    /// @param second Segundo operando
    /// @param op Operación a aplicar
    /// @param useCarry Tener en cuenta la flag CY, útil para ADC, SBB, etc.
    void manageCarryFlag(uint8_t first, uint8_t second, AritmeticOperation op, bool useCarry = false) noexcept;

    /// @brief Establece el Auxilary Carry Flag si la suma deseancadena un "overflow" en el primer byte
    /// @param first Primer operando
    /// @param second Segundo operando
    /// @param op Operación a aplicar
    /// @param useCarry Tener en cuenta la flag CY
    void manageAuxilaryCarryFlag(uint8_t first, uint8_t second, AritmeticOperation op, bool useCarry = false) noexcept;

    /// @brief Superfunción para operaciones aritméticas de 8 bits, devuelve el resultado y aplica las flags necesarias
    /// @param first Primer operando
    /// @param second Segundo operando
    /// @param op Operación a aplicar
    /// @param useCarry Tener en cuenta CY
    /// @param modifyCarry indica si CY puede ser modificado
    /// @return Resultado de la operación
    uint8_t aritmeticOperation_8bits(uint8_t first, uint8_t second, AritmeticOperation op, bool useCarry, bool modifyCarry = true) noexcept;

    /// @brief Superfunción para opcodes ADD, ADC, SUB, SBB y CMP con R
    /// @tparam R Registro a usar
    /// @tparam Op Operación a aplicar, ya sea suma o resta
    /// @tparam useCarry Tener en cuenta la flag CY
    /// @tparam storeResult Indica si se quiere modificar el registro con el resultado
    /// @return Número de ciclos usados
    template<Registers::Register R, AritmeticOperation Op, bool useCarry, bool storeResult>
    uint8_t ADD_ADC_SUB_SBB_CMP_R();

    /// @brief Superfunción para opcodes ADD, ADC, SUB, SBB y CMP con M
    /// @tparam Op Operación a aplicar
    /// @tparam useCarry Tener en cuenta la flag CY
    /// @tparam storeResult Indica si se quiere modificar el registro con el resultado
    /// @return Número de ciclos usados
    template<AritmeticOperation Op, bool useCarry, bool storeResult>
    uint8_t ADD_ADC_SUB_SBB_CMP_M();

    /// @brief Superfunción para INR y DCR con R
    /// @tparam R Registro a usar
    /// @tparam Op Operación a aplicar, ya sea suma o resta
    /// @return Número de ciclos usados
    template<Registers::Register R, AritmeticOperation Op>
    uint8_t INR_DCR_R();

    /// @brief Superfunción para INR y DCR con M
    /// @tparam Op Operación a aplicar
    /// @return Número de ciclos usados
    template<AritmeticOperation Op>
    uint8_t INR_DCR_M();

    /// @brief Superfunción para ANA, ORA y XRA con R
    /// @tparam R Registro a usar
    /// @tparam Op Operación lógica a usar, AND, OR o XOR
    /// @return Número de ciclos usados
    template<Registers::Register R, LogicOperation Op>
    uint8_t ANA_ORA_XRA_R();

    /// @brief Superfunción para ANA, ORA y XRA con M
    /// @tparam Op Operación a usar
    /// @return Número de ciclos usados
    template<LogicOperation Op>
    uint8_t ANA_ORA_XRA_M();

    /// @brief Superfunction for RLC, RRC, RAL and RAR with R
    /// @tparam R Register to use
    /// @tparam direction Direction of bit shift
    /// @tparam rotateThrough If true, rotates through CY flag (RAL/RAR); if false, circular rotation (RLC/RRC)
    /// @return Number of cycles used
    template<Registers::Register R, ShiftDirection direction, bool rotateThrough>
    uint8_t RLC_RRC_RAL_RAR_R();

    /// @brief Superfunción para INX y DCX con RR
    /// @tparam RR Conjunto de registros a usar
    /// @tparam Op Operación a realizar
    /// @return Número de ciclos usados
    template<Registers::CombinedRegister RR, AritmeticOperation Op>
    uint8_t INX_DCX_RR();

    /// @brief Superfunción para ADI, ACI, SUI, SBI y CPI con el byte inmediato
    /// @tparam Op Operación a realizar
    /// @tparam useCarry Considerar la flag CY para la operación
    /// @tparam storeResult Decide si guardar el resultado en el registro A
    /// @return Número de ciclos usados
    template<AritmeticOperation Op, bool useCarry, bool storeResult>
    uint8_t ADI_ACI_SUI_SBI_CPI_d8();

    /// @brief Superfunción para ANI, ORI y XRI con el byte inmediato
    /// @tparam Op Operación a realizar
    /// @return Número de ciclos usaos
    template<LogicOperation Op>
    uint8_t ANI_ORI_XRI_d8();

    /// @brief Superfunción para todos los saltos condicionares
    /// @tparam FlagToVerify Flag con la que se conddiciona el salto
    /// @tparam Negate Indica si se quiere negar la bandera (flag xor Negate)
    /// @return Número de ciclos usados
    template<Registers::Flags FlagToVerify, bool Negate>
    uint8_t conditionalJMP_a16();

    /// @brief Superfunción para todas las calls condicionales
    /// @tparam FlagToVerify Flag con la que se condiciona la call
    /// @tparam Negate Indica si se quiere negar la bandera (flag xor Negate)
    /// @return Número de ciclos usados
    template<Registers::Flags FlagToVerify, bool Negate>
    uint8_t conditionalCall_a16();

    /// @brief Superunción para todos los retornos condicionados
    /// @tparam FlagToVerify Flag con la que se condiciona el retorno
    /// @tparam Negate Indica si se quiere negar la bandera (flag xor Negate)
    /// @return Número de ciclos usados
    template<Registers::Flags FlagToVerify, bool Negate>
    uint8_t conditionalRet();

    /// @brief Hace CALL a la dirección especificada
    /// @param address Dirección a hacer call
    void callAddress(uint16_t address);

    uint8_t NOP();

    template<Registers::Register R>
    uint8_t ADD_R();

    template<Registers::Register R>
    uint8_t ADC_R();

    template<Registers::Register R>
    uint8_t SUB_R();

    template<Registers::Register R>
    uint8_t SBB_R();

    template<Registers::Register R>
    uint8_t CMP_R();

    template<Registers::Register R>
    uint8_t INR_R();

    template<Registers::Register R>
    uint8_t DCR_R();

    template<Registers::Register R>
    uint8_t ANA_R();

    template<Registers::Register R>
    uint8_t ORA_R();
    
    template<Registers::Register R>
    uint8_t XRA_R();

    template<Registers::Register R>
    uint8_t RLC_R();

    template<Registers::Register R>
    uint8_t RAL_R();

    template<Registers::Register R>
    uint8_t RRC_R();

    template<Registers::Register R>
    uint8_t RAR_R();

    uint8_t STC();

    uint8_t CMA();

    uint8_t CMC();

    uint8_t DAA();

    template<Registers::Register Source, Registers::Register Destination>
    uint8_t MOV_R_R();

    template<Registers::Register R>
    uint8_t MVI_R_d8();

    template<Registers::Register R>
    uint8_t MOV_M_R();

    template<Registers::Register R>
    uint8_t MOV_R_M();

    uint8_t ADD_M();

    uint8_t SUB_M();

    uint8_t ADC_M();

    uint8_t SBB_M();

    uint8_t CMP_M();

    uint8_t ANA_M();

    uint8_t ORA_M();

    uint8_t XRA_M();

    uint8_t INR_M();

    uint8_t DCR_M();

    uint8_t MVI_M_d8();

    template<Registers::CombinedRegister RR>
    uint8_t INX_RR();

    template<Registers::CombinedRegister RR>
    uint8_t DCX_RR();

    template<Registers::CombinedRegister RR>
    uint8_t DAD_RR();

    template<Registers::CombinedRegister RR>
    uint8_t LXI_RR_d16();

    uint8_t SHLD_a16();

    uint8_t LHLD_a16();

    uint8_t ADI_d8();

    uint8_t ACI_d8();

    uint8_t SUI_d8();

    uint8_t SBI_d8();

    uint8_t CPI_d8();

    uint8_t ANI_d8();

    uint8_t ORI_d8();

    uint8_t XRI_d8();

    template<Registers::CombinedRegister RR>
    uint8_t STAX_RR();

    uint8_t STA_a16();

    template<Registers::CombinedRegister RR>
    uint8_t LDAX_RR();

    uint8_t LDA_a16();

    template<Registers::CombinedRegister RR>
    uint8_t PUSH_RR();

    template<Registers::CombinedRegister RR>
    uint8_t POP_RR();

    uint8_t XTHL();

    uint8_t XCHG();

    uint8_t JMP_a16();

    uint8_t JM_a16();

    uint8_t JZ_a16();

    uint8_t JC_a16();

    uint8_t JPE_a16();

    uint8_t JP_a16();

    uint8_t JPO_a16();

    uint8_t JNC_a16();

    uint8_t JNZ_a16();

    uint8_t CALL_a16();

    uint8_t RET();

    uint8_t SPHL();

    uint8_t PCHL();

    uint8_t CZ_a16();

    uint8_t CC_a16();

    uint8_t CPE_a16();

    uint8_t CM_a16();

    uint8_t CNZ_a16();

    uint8_t CNC_a16();

    uint8_t CPO_a16();

    uint8_t CP_a16();

    uint8_t RZ();

    uint8_t RC();
    
    uint8_t RPE();

    uint8_t RM();

    uint8_t RNZ();

    uint8_t RNC();

    uint8_t RPO();

    uint8_t RP();

    template<uint8_t RstVector>
        requires (RstVector <= 7)
    uint8_t RST_N();

    uint8_t EI();

    uint8_t DI();

    uint8_t IN_d8();
    
    uint8_t OUT_d8();

    uint8_t HLT();
};

template <Registers::Register R>
inline uint8_t Fake8080::ADD_R() {
    return ADD_ADC_SUB_SBB_CMP_R<R, AritmeticOperation::ADD, false, true>();
}

template <Registers::Register R>
inline uint8_t Fake8080::ADC_R() {
    return ADD_ADC_SUB_SBB_CMP_R<R, AritmeticOperation::ADD, true, true>();
}

template <Registers::Register R>
inline uint8_t Fake8080::SUB_R() {
    return ADD_ADC_SUB_SBB_CMP_R<R, AritmeticOperation::SUB, false, true>();
}

template <Registers::Register R>
inline uint8_t Fake8080::SBB_R() {
    return ADD_ADC_SUB_SBB_CMP_R<R, AritmeticOperation::SUB, true, true>();
}

template <Registers::Register R>
inline uint8_t Fake8080::CMP_R() {
    return ADD_ADC_SUB_SBB_CMP_R<R, AritmeticOperation::SUB, false, false>();
}

template <Registers::Register R>
inline uint8_t Fake8080::INR_R() {
    return INR_DCR_R<R, AritmeticOperation::ADD>();
}

template<Registers::Register R>
inline uint8_t Fake8080::DCR_R() {
    return INR_DCR_R<R, AritmeticOperation::SUB>();
}

template <Registers::Register R>
inline uint8_t Fake8080::ANA_R() {
    return ANA_ORA_XRA_R<R, LogicOperation::AND>();
}

template <Registers::Register R>
inline uint8_t Fake8080::ORA_R() {
    return ANA_ORA_XRA_R<R, LogicOperation::OR>();
}

template <Registers::Register R>
inline uint8_t Fake8080::XRA_R() {
    return ANA_ORA_XRA_R<R, LogicOperation::XOR>();
}

template <Registers::Register R>
inline uint8_t Fake8080::RLC_R() {
    return RLC_RRC_RAL_RAR_R<R, ShiftDirection::LEFT, false>();
}

template <Registers::Register R>
inline uint8_t Fake8080::RAL_R() {
    return RLC_RRC_RAL_RAR_R<R, ShiftDirection::LEFT, true>();
}

template <Registers::Register R>
inline uint8_t Fake8080::RRC_R() {
    return RLC_RRC_RAL_RAR_R<R, ShiftDirection::RIGHT, false>();
}

template <Registers::Register R>
inline uint8_t Fake8080::RAR_R() {
    return RLC_RRC_RAL_RAR_R<R, ShiftDirection::RIGHT, true>();
}

template <Registers::Register Source, Registers::Register Destination>
inline uint8_t Fake8080::MOV_R_R() {
    registers_m.setRegister(Destination, registers_m.getRegister(Source));

    return MOV_R_R_Cycles;
}

template <Registers::Register R>
inline uint8_t Fake8080::MVI_R_d8() {
    registers_m.setRegister(R, readNextByte());

    return MVI_R_d8_Cycles;
}

template <Registers::Register R>
inline uint8_t Fake8080::MOV_M_R() {
    writeToM(registers_m.getRegister(R));

    return MOV_M_R_Cycles;
}

template <Registers::Register R>
inline uint8_t Fake8080::MOV_R_M() {
    registers_m.setRegister(R, memoryBus_m.read(registers_m.getCombinedRegister(Registers::CombinedRegister::HL)));

    return MOV_R_M_Cycles;
}

template <Registers::CombinedRegister RR>
inline uint8_t Fake8080::INX_RR() {
    return INX_DCX_RR<RR, AritmeticOperation::ADD>();
}

template <Registers::CombinedRegister RR>
inline uint8_t Fake8080::DCX_RR() {
    return INX_DCX_RR<RR, AritmeticOperation::SUB>();
}

template <Registers::CombinedRegister RR>
inline uint8_t Fake8080::DAD_RR() {
    const auto HL_value{ registers_m.getCombinedRegister(Registers::CombinedRegister::HL) };
    const auto RR_value{ registers_m.getCombinedRegister(RR) };

    const uint16_t result = HL_value + RR_value;

    if (result < HL_value) {
        registers_m.setFlag(Registers::Flags::CY, true);
    } 
    else {
        registers_m.setFlag(Registers::Flags::CY, false);
    }

    registers_m.setCombinedRegister(Registers::CombinedRegister::HL, result);
    
    return DAD_RR_Cycles;
}

template <Registers::CombinedRegister RR>
inline uint8_t Fake8080::LXI_RR_d16() {
    registers_m.setCombinedRegister(RR, readNextTwoBytes());

    return LXI_Cycles;
}

template <Registers::Register R, Fake8080::AritmeticOperation Op, bool useCarry, bool storeResult>
inline uint8_t Fake8080::ADD_ADC_SUB_SBB_CMP_R() {
    const auto register_A{ registers_m.getRegister(Registers::Register::A) };
    const auto register_R{ registers_m.getRegister(R) };

    [[maybe_unused]]
    const auto result{ aritmeticOperation_8bits(register_A, register_R, Op, useCarry, true) };

    if constexpr (storeResult) {
        registers_m.setRegister(Registers::Register::A, result);
    }

    return ADD_ADC_SUB_SBB_CMP_R_Cycles;
}

template <Fake8080::AritmeticOperation Op, bool useCarry, bool storeResult>
inline uint8_t Fake8080::ADD_ADC_SUB_SBB_CMP_M() {
    loadMtoW();
    ADD_ADC_SUB_SBB_CMP_R<Registers::Register::W, Op, useCarry, storeResult>();

    return ADD_ADC_SUB_SBB_CMP_M_Cycles;
}

template <Registers::Register R, Fake8080::AritmeticOperation Op>
inline uint8_t Fake8080::INR_DCR_R() {
    const auto result{ aritmeticOperation_8bits(registers_m.getRegister(R), 1, Op, false, false) };

    registers_m.setRegister(R, result);

    return INR_DCR_R_Cycles;
}

template <Fake8080::AritmeticOperation Op>
inline uint8_t Fake8080::INR_DCR_M() {
    loadMtoW();
    INR_DCR_R<Registers::Register::W, Op>();
    writeWtoM();

    return INR_DCR_M_Cycles;
}

template <Registers::Register R, Fake8080::LogicOperation Op>
inline uint8_t Fake8080::ANA_ORA_XRA_R() {
    const auto register_A{ registers_m.getRegister(Registers::Register::A) };
    const auto register_R( registers_m.getRegister(R) );
    uint8_t result;

    switch (Op) {
    case LogicOperation::AND:
        result = register_A & register_R;
        break;
    
    case LogicOperation::OR:
        result = register_A | register_R;
        break;

    case LogicOperation::XOR:
        result = register_A ^ register_R;
        break;
    }

    manageZeroFlag(result);
    manageSignedFlag(result);
    manageParityFlag(result);
    registers_m.setFlag(Registers::Flags::CY, 0);
    registers_m.setFlag(Registers::Flags::AC, (Op == LogicOperation::AND ? 1 : 0));

    registers_m.setRegister(Registers::Register::A, result);

    return ANA_ORA_XRA_R_Cycles;
}

template <Fake8080::LogicOperation Op>
inline uint8_t Fake8080::ANA_ORA_XRA_M() {
    loadMtoW();
    ANA_ORA_XRA_R<Registers::Register::W, Op>();

    return ANA_ORA_XRA_M_Cycles;
}

template <Registers::Register R, Fake8080::ShiftDirection direction, bool rotateThrough>
inline uint8_t Fake8080::RLC_RRC_RAL_RAR_R() {
    auto registerValue{ registers_m.getRegister(R) };
    
    // Identificar qué bit se "expulsa" del registro
    constexpr uint8_t exitBitPosition = (direction == ShiftDirection::RIGHT) ? 0 : 7;
    constexpr uint8_t entryBitPosition = (direction == ShiftDirection::RIGHT) ? 7 : 0;
    
    const bool exitBit{ getBit(registerValue, exitBitPosition) };
    
    // Realizar el shift
    if constexpr (direction == ShiftDirection::RIGHT) {
        registerValue >>= 1;
    } else {
        registerValue <<= 1;
    }
    
    // Determinar qué bit entra:
    // - Si rotateThrough=false (RLC/RRC): usa el bit expulsado
    // - Si rotateThrough=true (RAL/RAR): usa la flag CY
    const bool entryBit{ rotateThrough ? registers_m.getFlag(Registers::Flags::CY) : exitBit };
    registerValue = setBit(registerValue, entryBitPosition, entryBit);

    registers_m.setFlag(Registers::Flags::CY, exitBit);
    registers_m.setRegister(R, registerValue);

    return RLC_RRC_RAL_RAR_Cycles;
}

template <Registers::CombinedRegister RR, Fake8080::AritmeticOperation Op>
inline uint8_t Fake8080::INX_DCX_RR() {
    const uint16_t result = registers_m.getCombinedRegister(RR) + (Op == AritmeticOperation::ADD ? 1 : -1);
    registers_m.setCombinedRegister(RR, result);

    return INX_DCX_RR_Cycles;
}

template <Fake8080::AritmeticOperation Op, bool useCarry, bool storeResult>
inline uint8_t Fake8080::ADI_ACI_SUI_SBI_CPI_d8() {
    registers_m.setRegister(Registers::Register::W, readNextByte());
    ADD_ADC_SUB_SBB_CMP_R<Registers::Register::W, Op, useCarry, storeResult>();
    
    return ADI_ACI_SUI_SBI_CPI_d8_Cycles;
}

template <Fake8080::LogicOperation Op>
inline uint8_t Fake8080::ANI_ORI_XRI_d8() {
    registers_m.setRegister(Registers::Register::W, readNextByte());
    ANA_ORA_XRA_R<Registers::Register::W, Op>();

    return ANI_ORI_XRI_d8_Cycles;
}

template <Registers::Flags FlagToVerify, bool Negate>
inline uint8_t Fake8080::conditionalJMP_a16() {
    [[maybe_unused]]
    const auto addressToJump{ readNextTwoBytes() };

    if (registers_m.getFlag(FlagToVerify) != Negate) {
        pc_m = addressToJump;
    }

    return JMP_conditionalJUMP_a16_Cycles;
}

template <Registers::Flags FlagToVerify, bool Negate>
inline uint8_t Fake8080::conditionalCall_a16() {
    if (registers_m.getFlag(FlagToVerify) != Negate) {
        return CALL_a16();
    }

    (void)readNextTwoBytes(); // Salto ignorado, consumiendo siguientes dos bytes
    
    return Ignored_CALL_Cycles;
}

template <Registers::Flags FlagToVerify, bool Negate>
inline uint8_t Fake8080::conditionalRet() {
    if (registers_m.getFlag(FlagToVerify) != Negate) {
        RET();

        return Taken_Conditional_RET_Cycles;
    }

    return Ignored_RET_Cycles;
}

template <Registers::CombinedRegister RR>
inline uint8_t Fake8080::STAX_RR() {
    memoryBus_m.write(registers_m.getCombinedRegister(RR), registers_m.getRegister(Registers::Register::A));

    return STAX_RR_Cycles;
}

template <Registers::CombinedRegister RR>
inline uint8_t Fake8080::LDAX_RR() {
    registers_m.setRegister(Registers::Register::A, memoryBus_m.read(registers_m.getCombinedRegister(RR)));

    return LDAX_RR_Cycles;
}

template <Registers::CombinedRegister RR>
inline uint8_t Fake8080::PUSH_RR() {
    const auto RR_value{ registers_m.getCombinedRegister(RR) };

    decreaseSP();

    memoryBus_m.write(registers_m.getCombinedRegister(Registers::CombinedRegister::SP), getHighByte(RR_value));

    decreaseSP();

    memoryBus_m.write(registers_m.getCombinedRegister(Registers::CombinedRegister::SP), getLowByte(RR_value));

    return PUSH_RR_Cycles;
}

template <Registers::CombinedRegister RR>
inline uint8_t Fake8080::POP_RR() {
    const auto lowByte{ memoryBus_m.read(registers_m.getCombinedRegister(Registers::CombinedRegister::SP)) };

    increaseSP();

    const auto highByte{ memoryBus_m.read(registers_m.getCombinedRegister(Registers::CombinedRegister::SP)) };

    increaseSP();

    registers_m.setCombinedRegister(RR, static_cast<uint16_t>(highByte << Byte_Shift) | lowByte);

    return POP_RR_Cycles;
}

template <uint8_t RstVector>
    requires (RstVector <= 7)
inline uint8_t Fake8080::RST_N() {
    callAddress(Byte_Shift * RstVector);

    return RST_Cycles;
}

// Definition of opcodes array
inline constexpr std::array<Fake8080::MemberFunction, Fake8080::Opcodes_Number> Fake8080::opcodes_m{
    // 0x00 - 0x0F
    &Fake8080::NOP,                                              // 0x00: NOP
    &Fake8080::LXI_RR_d16<Registers::CombinedRegister::BC>,      // 0x01: LXI B,d16
    &Fake8080::STAX_RR<Registers::CombinedRegister::BC>,         // 0x02: STAX B
    &Fake8080::INX_RR<Registers::CombinedRegister::BC>,          // 0x03: INX B
    &Fake8080::INR_R<Registers::Register::B>,                    // 0x04: INR B
    &Fake8080::DCR_R<Registers::Register::B>,                    // 0x05: DCR B
    &Fake8080::MVI_R_d8<Registers::Register::B>,                 // 0x06: MVI B,d8
    &Fake8080::RLC_R<Registers::Register::A>,                    // 0x07: RLC
    &Fake8080::NOP,                                              // 0x08: *NOP (alternativo)
    &Fake8080::DAD_RR<Registers::CombinedRegister::BC>,          // 0x09: DAD B
    &Fake8080::LDAX_RR<Registers::CombinedRegister::BC>,         // 0x0A: LDAX B
    &Fake8080::DCX_RR<Registers::CombinedRegister::BC>,          // 0x0B: DCX B
    &Fake8080::INR_R<Registers::Register::C>,                    // 0x0C: INR C
    &Fake8080::DCR_R<Registers::Register::C>,                    // 0x0D: DCR C
    &Fake8080::MVI_R_d8<Registers::Register::C>,                 // 0x0E: MVI C,d8
    &Fake8080::RRC_R<Registers::Register::A>,                    // 0x0F: RRC

    // 0x10 - 0x1F
    &Fake8080::NOP,                                              // 0x10: *NOP (alternativo)
    &Fake8080::LXI_RR_d16<Registers::CombinedRegister::DE>,      // 0x11: LXI D,d16
    &Fake8080::STAX_RR<Registers::CombinedRegister::DE>,         // 0x12: STAX D
    &Fake8080::INX_RR<Registers::CombinedRegister::DE>,          // 0x13: INX D
    &Fake8080::INR_R<Registers::Register::D>,                    // 0x14: INR D
    &Fake8080::DCR_R<Registers::Register::D>,                    // 0x15: DCR D
    &Fake8080::MVI_R_d8<Registers::Register::D>,                 // 0x16: MVI D,d8
    &Fake8080::RAL_R<Registers::Register::A>,                    // 0x17: RAL
    &Fake8080::NOP,                                              // 0x18: *NOP (alternativo)
    &Fake8080::DAD_RR<Registers::CombinedRegister::DE>,          // 0x19: DAD D
    &Fake8080::LDAX_RR<Registers::CombinedRegister::DE>,         // 0x1A: LDAX D
    &Fake8080::DCX_RR<Registers::CombinedRegister::DE>,          // 0x1B: DCX D
    &Fake8080::INR_R<Registers::Register::E>,                    // 0x1C: INR E
    &Fake8080::DCR_R<Registers::Register::E>,                    // 0x1D: DCR E
    &Fake8080::MVI_R_d8<Registers::Register::E>,                 // 0x1E: MVI E,d8
    &Fake8080::RAR_R<Registers::Register::A>,                    // 0x1F: RAR

    // 0x20 - 0x2F
    &Fake8080::NOP,                                              // 0x20: *NOP (alternativo)
    &Fake8080::LXI_RR_d16<Registers::CombinedRegister::HL>,      // 0x21: LXI H,d16
    &Fake8080::SHLD_a16,                                         // 0x22: SHLD a16
    &Fake8080::INX_RR<Registers::CombinedRegister::HL>,          // 0x23: INX H
    &Fake8080::INR_R<Registers::Register::H>,                    // 0x24: INR H
    &Fake8080::DCR_R<Registers::Register::H>,                    // 0x25: DCR H
    &Fake8080::MVI_R_d8<Registers::Register::H>,                 // 0x26: MVI H,d8
    &Fake8080::DAA,                                              // 0x27: DAA
    &Fake8080::NOP,                                              // 0x28: *NOP (alternativo)
    &Fake8080::DAD_RR<Registers::CombinedRegister::HL>,          // 0x29: DAD H
    &Fake8080::LHLD_a16,                                         // 0x2A: LHLD a16
    &Fake8080::DCX_RR<Registers::CombinedRegister::HL>,          // 0x2B: DCX H
    &Fake8080::INR_R<Registers::Register::L>,                    // 0x2C: INR L
    &Fake8080::DCR_R<Registers::Register::L>,                    // 0x2D: DCR L
    &Fake8080::MVI_R_d8<Registers::Register::L>,                 // 0x2E: MVI L,d8
    &Fake8080::CMA,                                              // 0x2F: CMA

    // 0x30 - 0x3F
    &Fake8080::NOP,                                              // 0x30: *NOP (alternativo)
    &Fake8080::LXI_RR_d16<Registers::CombinedRegister::SP>,      // 0x31: LXI SP,d16
    &Fake8080::STA_a16,                                          // 0x32: STA a16
    &Fake8080::INX_RR<Registers::CombinedRegister::SP>,          // 0x33: INX SP
    &Fake8080::INR_M,                                            // 0x34: INR M
    &Fake8080::DCR_M,                                            // 0x35: DCR M
    &Fake8080::MVI_M_d8,                                         // 0x36: MVI M,d8
    &Fake8080::STC,                                              // 0x37: STC
    &Fake8080::NOP,                                              // 0x38: *NOP (alternativo)
    &Fake8080::DAD_RR<Registers::CombinedRegister::SP>,          // 0x39: DAD SP
    &Fake8080::LDA_a16,                                          // 0x3A: LDA a16
    &Fake8080::DCX_RR<Registers::CombinedRegister::SP>,          // 0x3B: DCX SP
    &Fake8080::INR_R<Registers::Register::A>,                    // 0x3C: INR A
    &Fake8080::DCR_R<Registers::Register::A>,                    // 0x3D: DCR A
    &Fake8080::MVI_R_d8<Registers::Register::A>,                 // 0x3E: MVI A,d8
    &Fake8080::CMC,                                              // 0x3F: CMC

    // 0x40 - 0x4F: MOV B,r & MOV C,r
    &Fake8080::MOV_R_R<Registers::Register::B, Registers::Register::B>,  // 0x40: MOV B,B
    &Fake8080::MOV_R_R<Registers::Register::C, Registers::Register::B>,  // 0x41: MOV B,C
    &Fake8080::MOV_R_R<Registers::Register::D, Registers::Register::B>,  // 0x42: MOV B,D
    &Fake8080::MOV_R_R<Registers::Register::E, Registers::Register::B>,  // 0x43: MOV B,E
    &Fake8080::MOV_R_R<Registers::Register::H, Registers::Register::B>,  // 0x44: MOV B,H
    &Fake8080::MOV_R_R<Registers::Register::L, Registers::Register::B>,  // 0x45: MOV B,L
    &Fake8080::MOV_R_M<Registers::Register::B>,                          // 0x46: MOV B,M
    &Fake8080::MOV_R_R<Registers::Register::A, Registers::Register::B>,  // 0x47: MOV B,A
    &Fake8080::MOV_R_R<Registers::Register::B, Registers::Register::C>,  // 0x48: MOV C,B
    &Fake8080::MOV_R_R<Registers::Register::C, Registers::Register::C>,  // 0x49: MOV C,C
    &Fake8080::MOV_R_R<Registers::Register::D, Registers::Register::C>,  // 0x4A: MOV C,D
    &Fake8080::MOV_R_R<Registers::Register::E, Registers::Register::C>,  // 0x4B: MOV C,E
    &Fake8080::MOV_R_R<Registers::Register::H, Registers::Register::C>,  // 0x4C: MOV C,H
    &Fake8080::MOV_R_R<Registers::Register::L, Registers::Register::C>,  // 0x4D: MOV C,L
    &Fake8080::MOV_R_M<Registers::Register::C>,                          // 0x4E: MOV C,M
    &Fake8080::MOV_R_R<Registers::Register::A, Registers::Register::C>,  // 0x4F: MOV C,A

    // 0x50 - 0x5F: MOV D,r & MOV E,r
    &Fake8080::MOV_R_R<Registers::Register::B, Registers::Register::D>,  // 0x50: MOV D,B
    &Fake8080::MOV_R_R<Registers::Register::C, Registers::Register::D>,  // 0x51: MOV D,C
    &Fake8080::MOV_R_R<Registers::Register::D, Registers::Register::D>,  // 0x52: MOV D,D
    &Fake8080::MOV_R_R<Registers::Register::E, Registers::Register::D>,  // 0x53: MOV D,E
    &Fake8080::MOV_R_R<Registers::Register::H, Registers::Register::D>,  // 0x54: MOV D,H
    &Fake8080::MOV_R_R<Registers::Register::L, Registers::Register::D>,  // 0x55: MOV D,L
    &Fake8080::MOV_R_M<Registers::Register::D>,                          // 0x56: MOV D,M
    &Fake8080::MOV_R_R<Registers::Register::A, Registers::Register::D>,  // 0x57: MOV D,A
    &Fake8080::MOV_R_R<Registers::Register::B, Registers::Register::E>,  // 0x58: MOV E,B
    &Fake8080::MOV_R_R<Registers::Register::C, Registers::Register::E>,  // 0x59: MOV E,C
    &Fake8080::MOV_R_R<Registers::Register::D, Registers::Register::E>,  // 0x5A: MOV E,D
    &Fake8080::MOV_R_R<Registers::Register::E, Registers::Register::E>,  // 0x5B: MOV E,E
    &Fake8080::MOV_R_R<Registers::Register::H, Registers::Register::E>,  // 0x5C: MOV E,H
    &Fake8080::MOV_R_R<Registers::Register::L, Registers::Register::E>,  // 0x5D: MOV E,L
    &Fake8080::MOV_R_M<Registers::Register::E>,                          // 0x5E: MOV E,M
    &Fake8080::MOV_R_R<Registers::Register::A, Registers::Register::E>,  // 0x5F: MOV E,A

    // 0x60 - 0x6F: MOV H,r & MOV L,r
    &Fake8080::MOV_R_R<Registers::Register::B, Registers::Register::H>,  // 0x60: MOV H,B
    &Fake8080::MOV_R_R<Registers::Register::C, Registers::Register::H>,  // 0x61: MOV H,C
    &Fake8080::MOV_R_R<Registers::Register::D, Registers::Register::H>,  // 0x62: MOV H,D
    &Fake8080::MOV_R_R<Registers::Register::E, Registers::Register::H>,  // 0x63: MOV H,E
    &Fake8080::MOV_R_R<Registers::Register::H, Registers::Register::H>,  // 0x64: MOV H,H
    &Fake8080::MOV_R_R<Registers::Register::L, Registers::Register::H>,  // 0x65: MOV H,L
    &Fake8080::MOV_R_M<Registers::Register::H>,                          // 0x66: MOV H,M
    &Fake8080::MOV_R_R<Registers::Register::A, Registers::Register::H>,  // 0x67: MOV H,A
    &Fake8080::MOV_R_R<Registers::Register::B, Registers::Register::L>,  // 0x68: MOV L,B
    &Fake8080::MOV_R_R<Registers::Register::C, Registers::Register::L>,  // 0x69: MOV L,C
    &Fake8080::MOV_R_R<Registers::Register::D, Registers::Register::L>,  // 0x6A: MOV L,D
    &Fake8080::MOV_R_R<Registers::Register::E, Registers::Register::L>,  // 0x6B: MOV L,E
    &Fake8080::MOV_R_R<Registers::Register::H, Registers::Register::L>,  // 0x6C: MOV L,H
    &Fake8080::MOV_R_R<Registers::Register::L, Registers::Register::L>,  // 0x6D: MOV L,L
    &Fake8080::MOV_R_M<Registers::Register::L>,                          // 0x6E: MOV L,M
    &Fake8080::MOV_R_R<Registers::Register::A, Registers::Register::L>,  // 0x6F: MOV L,A

    // 0x70 - 0x7F: MOV M,r & MOV A,r
    &Fake8080::MOV_M_R<Registers::Register::B>,                          // 0x70: MOV M,B
    &Fake8080::MOV_M_R<Registers::Register::C>,                          // 0x71: MOV M,C
    &Fake8080::MOV_M_R<Registers::Register::D>,                          // 0x72: MOV M,D
    &Fake8080::MOV_M_R<Registers::Register::E>,                          // 0x73: MOV M,E
    &Fake8080::MOV_M_R<Registers::Register::H>,                          // 0x74: MOV M,H
    &Fake8080::MOV_M_R<Registers::Register::L>,                          // 0x75: MOV M,L
    &Fake8080::HLT,                                                      // 0x76: HLT
    &Fake8080::MOV_M_R<Registers::Register::A>,                          // 0x77: MOV M,A
    &Fake8080::MOV_R_R<Registers::Register::B, Registers::Register::A>,  // 0x78: MOV A,B
    &Fake8080::MOV_R_R<Registers::Register::C, Registers::Register::A>,  // 0x79: MOV A,C
    &Fake8080::MOV_R_R<Registers::Register::D, Registers::Register::A>,  // 0x7A: MOV A,D
    &Fake8080::MOV_R_R<Registers::Register::E, Registers::Register::A>,  // 0x7B: MOV A,E
    &Fake8080::MOV_R_R<Registers::Register::H, Registers::Register::A>,  // 0x7C: MOV A,H
    &Fake8080::MOV_R_R<Registers::Register::L, Registers::Register::A>,  // 0x7D: MOV A,L
    &Fake8080::MOV_R_M<Registers::Register::A>,                          // 0x7E: MOV A,M
    &Fake8080::MOV_R_R<Registers::Register::A, Registers::Register::A>,  // 0x7F: MOV A,A

    // 0x80 - 0x8F: ADD, ADC
    &Fake8080::ADD_R<Registers::Register::B>,                    // 0x80: ADD B
    &Fake8080::ADD_R<Registers::Register::C>,                    // 0x81: ADD C
    &Fake8080::ADD_R<Registers::Register::D>,                    // 0x82: ADD D
    &Fake8080::ADD_R<Registers::Register::E>,                    // 0x83: ADD E
    &Fake8080::ADD_R<Registers::Register::H>,                    // 0x84: ADD H
    &Fake8080::ADD_R<Registers::Register::L>,                    // 0x85: ADD L
    &Fake8080::ADD_M,                                            // 0x86: ADD M
    &Fake8080::ADD_R<Registers::Register::A>,                    // 0x87: ADD A
    &Fake8080::ADC_R<Registers::Register::B>,                    // 0x88: ADC B
    &Fake8080::ADC_R<Registers::Register::C>,                    // 0x89: ADC C
    &Fake8080::ADC_R<Registers::Register::D>,                    // 0x8A: ADC D
    &Fake8080::ADC_R<Registers::Register::E>,                    // 0x8B: ADC E
    &Fake8080::ADC_R<Registers::Register::H>,                    // 0x8C: ADC H
    &Fake8080::ADC_R<Registers::Register::L>,                    // 0x8D: ADC L
    &Fake8080::ADC_M,                                            // 0x8E: ADC M
    &Fake8080::ADC_R<Registers::Register::A>,                    // 0x8F: ADC A

    // 0x90 - 0x9F: SUB, SBB
    &Fake8080::SUB_R<Registers::Register::B>,                    // 0x90: SUB B
    &Fake8080::SUB_R<Registers::Register::C>,                    // 0x91: SUB C
    &Fake8080::SUB_R<Registers::Register::D>,                    // 0x92: SUB D
    &Fake8080::SUB_R<Registers::Register::E>,                    // 0x93: SUB E
    &Fake8080::SUB_R<Registers::Register::H>,                    // 0x94: SUB H
    &Fake8080::SUB_R<Registers::Register::L>,                    // 0x95: SUB L
    &Fake8080::SUB_M,                                            // 0x96: SUB M
    &Fake8080::SUB_R<Registers::Register::A>,                    // 0x97: SUB A
    &Fake8080::SBB_R<Registers::Register::B>,                    // 0x98: SBB B
    &Fake8080::SBB_R<Registers::Register::C>,                    // 0x99: SBB C
    &Fake8080::SBB_R<Registers::Register::D>,                    // 0x9A: SBB D
    &Fake8080::SBB_R<Registers::Register::E>,                    // 0x9B: SBB E
    &Fake8080::SBB_R<Registers::Register::H>,                    // 0x9C: SBB H
    &Fake8080::SBB_R<Registers::Register::L>,                    // 0x9D: SBB L
    &Fake8080::SBB_M,                                            // 0x9E: SBB M
    &Fake8080::SBB_R<Registers::Register::A>,                    // 0x9F: SBB A

    // 0xA0 - 0xAF: ANA, XRA
    &Fake8080::ANA_R<Registers::Register::B>,                    // 0xA0: ANA B
    &Fake8080::ANA_R<Registers::Register::C>,                    // 0xA1: ANA C
    &Fake8080::ANA_R<Registers::Register::D>,                    // 0xA2: ANA D
    &Fake8080::ANA_R<Registers::Register::E>,                    // 0xA3: ANA E
    &Fake8080::ANA_R<Registers::Register::H>,                    // 0xA4: ANA H
    &Fake8080::ANA_R<Registers::Register::L>,                    // 0xA5: ANA L
    &Fake8080::ANA_M,                                            // 0xA6: ANA M
    &Fake8080::ANA_R<Registers::Register::A>,                    // 0xA7: ANA A
    &Fake8080::XRA_R<Registers::Register::B>,                    // 0xA8: XRA B
    &Fake8080::XRA_R<Registers::Register::C>,                    // 0xA9: XRA C
    &Fake8080::XRA_R<Registers::Register::D>,                    // 0xAA: XRA D
    &Fake8080::XRA_R<Registers::Register::E>,                    // 0xAB: XRA E
    &Fake8080::XRA_R<Registers::Register::H>,                    // 0xAC: XRA H
    &Fake8080::XRA_R<Registers::Register::L>,                    // 0xAD: XRA L
    &Fake8080::XRA_M,                                            // 0xAE: XRA M
    &Fake8080::XRA_R<Registers::Register::A>,                    // 0xAF: XRA A

    // 0xB0 - 0xBF: ORA, CMP
    &Fake8080::ORA_R<Registers::Register::B>,                    // 0xB0: ORA B
    &Fake8080::ORA_R<Registers::Register::C>,                    // 0xB1: ORA C
    &Fake8080::ORA_R<Registers::Register::D>,                    // 0xB2: ORA D
    &Fake8080::ORA_R<Registers::Register::E>,                    // 0xB3: ORA E
    &Fake8080::ORA_R<Registers::Register::H>,                    // 0xB4: ORA H
    &Fake8080::ORA_R<Registers::Register::L>,                    // 0xB5: ORA L
    &Fake8080::ORA_M,                                            // 0xB6: ORA M
    &Fake8080::ORA_R<Registers::Register::A>,                    // 0xB7: ORA A
    &Fake8080::CMP_R<Registers::Register::B>,                    // 0xB8: CMP B
    &Fake8080::CMP_R<Registers::Register::C>,                    // 0xB9: CMP C
    &Fake8080::CMP_R<Registers::Register::D>,                    // 0xBA: CMP D
    &Fake8080::CMP_R<Registers::Register::E>,                    // 0xBB: CMP E
    &Fake8080::CMP_R<Registers::Register::H>,                    // 0xBC: CMP H
    &Fake8080::CMP_R<Registers::Register::L>,                    // 0xBD: CMP L
    &Fake8080::CMP_M,                                            // 0xBE: CMP M
    &Fake8080::CMP_R<Registers::Register::A>,                    // 0xBF: CMP A

    // 0xC0 - 0xCF: Conditional RET, POP, JMP, CALL, PUSH, Operations
    &Fake8080::RNZ,                                              // 0xC0: RNZ
    &Fake8080::POP_RR<Registers::CombinedRegister::BC>,          // 0xC1: POP B
    &Fake8080::JNZ_a16,                                          // 0xC2: JNZ a16
    &Fake8080::JMP_a16,                                          // 0xC3: JMP a16
    &Fake8080::CNZ_a16,                                          // 0xC4: CNZ a16
    &Fake8080::PUSH_RR<Registers::CombinedRegister::BC>,         // 0xC5: PUSH B
    &Fake8080::ADI_d8,                                           // 0xC6: ADI d8
    &Fake8080::RST_N<0>,                                         // 0xC7: RST 0
    &Fake8080::RZ,                                               // 0xC8: RZ
    &Fake8080::RET,                                              // 0xC9: RET
    &Fake8080::JZ_a16,                                           // 0xCA: JZ a16
    &Fake8080::JMP_a16,                                          // 0xCB: *JMP a16 (alternativo)
    &Fake8080::CZ_a16,                                           // 0xCC: CZ a16
    &Fake8080::CALL_a16,                                         // 0xCD: CALL a16
    &Fake8080::ACI_d8,                                           // 0xCE: ACI d8
    &Fake8080::RST_N<1>,                                         // 0xCF: RST 1

    // 0xD0 - 0xDF
    &Fake8080::RNC,                                              // 0xD0: RNC
    &Fake8080::POP_RR<Registers::CombinedRegister::DE>,          // 0xD1: POP D
    &Fake8080::JNC_a16,                                          // 0xD2: JNC a16
    &Fake8080::OUT_d8,                                           // 0xD3: OUT d8
    &Fake8080::CNC_a16,                                          // 0xD4: CNC a16
    &Fake8080::PUSH_RR<Registers::CombinedRegister::DE>,         // 0xD5: PUSH D
    &Fake8080::SUI_d8,                                           // 0xD6: SUI d8
    &Fake8080::RST_N<2>,                                         // 0xD7: RST 2
    &Fake8080::RC,                                               // 0xD8: RC
    &Fake8080::RET,                                              // 0xD9: *RET (alternativo)
    &Fake8080::JC_a16,                                           // 0xDA: JC a16
    &Fake8080::IN_d8,                                            // 0xDB: IN d8
    &Fake8080::CC_a16,                                           // 0xDC: CC a16
    &Fake8080::CALL_a16,                                         // 0xDD: *CALL a16 (alternativo)
    &Fake8080::SBI_d8,                                           // 0xDE: SBI d8
    &Fake8080::RST_N<3>,                                         // 0xDF: RST 3

    // 0xE0 - 0xEF
    &Fake8080::RPO,                                              // 0xE0: RPO
    &Fake8080::POP_RR<Registers::CombinedRegister::HL>,          // 0xE1: POP H
    &Fake8080::JPO_a16,                                          // 0xE2: JPO a16
    &Fake8080::XTHL,                                             // 0xE3: XTHL
    &Fake8080::CPO_a16,                                          // 0xE4: CPO a16
    &Fake8080::PUSH_RR<Registers::CombinedRegister::HL>,         // 0xE5: PUSH H
    &Fake8080::ANI_d8,                                           // 0xE6: ANI d8
    &Fake8080::RST_N<4>,                                         // 0xE7: RST 4
    &Fake8080::RPE,                                              // 0xE8: RPE
    &Fake8080::PCHL,                                             // 0xE9: PCHL
    &Fake8080::JPE_a16,                                          // 0xEA: JPE a16
    &Fake8080::XCHG,                                             // 0xEB: XCHG
    &Fake8080::CPE_a16,                                          // 0xEC: CPE a16
    &Fake8080::CALL_a16,                                         // 0xED: *CALL a16 (alternativo)
    &Fake8080::XRI_d8,                                           // 0xEE: XRI d8
    &Fake8080::RST_N<5>,                                         // 0xEF: RST 5

    // 0xF0 - 0xFF
    &Fake8080::RP,                                               // 0xF0: RP
    &Fake8080::POP_RR<Registers::CombinedRegister::PSW>,         // 0xF1: POP PSW
    &Fake8080::JP_a16,                                           // 0xF2: JP a16
    &Fake8080::DI,                                               // 0xF3: DI
    &Fake8080::CP_a16,                                           // 0xF4: CP a16
    &Fake8080::PUSH_RR<Registers::CombinedRegister::PSW>,        // 0xF5: PUSH PSW
    &Fake8080::ORI_d8,                                           // 0xF6: ORI d8
    &Fake8080::RST_N<6>,                                         // 0xF7: RST 6
    &Fake8080::RM,                                               // 0xF8: RM
    &Fake8080::SPHL,                                             // 0xF9: SPHL
    &Fake8080::JM_a16,                                           // 0xFA: JM a16
    &Fake8080::EI,                                               // 0xFB: EI
    &Fake8080::CM_a16,                                           // 0xFC: CM a16
    &Fake8080::CALL_a16,                                         // 0xFD: *CALL a16 (alterntivo)
    &Fake8080::CPI_d8,                                           // 0xFE: CPI d8
    &Fake8080::RST_N<7>                                          // 0xFF: RST 7
};

#endif // !FAKE8080_HEADER