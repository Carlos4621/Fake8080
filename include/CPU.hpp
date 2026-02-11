#ifndef CPU_HEADER
#define CPU_HEADER

#include <cstdint>
#include <span>
#include <stdexcept>
#include "Registers.hpp"
#include <limits>
#include "OpcodesCycles.hpp"

class CPUTest;

class CPU {
    friend class CPUTest;
    
public:
    void setROM(std::span<uint8_t> rom);

    void cycle();

private:
    enum class AritmeticOperation : uint8_t { ADD = 0, SUB };
    enum class LogicOperation : uint8_t { AND = 0, OR, XOR };
    enum class ShiftDirection : uint8_t { RIGHT = 0, LEFT };

    using MemberFunction = uint8_t(CPU::*)();

    static constexpr uint8_t Byte_Shift{ 8 };
    static constexpr uint16_t Opcodes_Number{ 256 };

    static std::array<MemberFunction, Opcodes_Number> Opcodes;

    std::span<uint8_t> rom_m;

    uint16_t pc_m{ 0 };
    Registers registers_m;

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

    [[noreturn]]
    void InvalidOpcode();

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

    /// @brief Superfunción para RLC, RRC, RAL y RAR con R
    /// @tparam R Registro a usar
    /// @tparam direction Dirección del desplazamiento de bits
    /// @tparam useCY Indica si se quiere colocar CY en la dirección opuesta del desplazamiento
    /// @return Número de ciclos usados
    template<Registers::Register R, ShiftDirection direction, bool useCY>
    uint8_t RLC_RRC_RAL_RAR_R();

    /// @brief Superfunción para INX y DCX con RR
    /// @tparam RR Conjunto de registros a usar
    /// @tparam Op Operación a realizar
    /// @return Número de ciclos usados
    template<Registers::CombinedRegister RR, AritmeticOperation Op>
    uint8_t INX_DCX_RR();

    template<AritmeticOperation Op, bool useCarry, bool storeResult>
    uint8_t ADI_ACI_SUI_SBI_CPI_d8();

    template<LogicOperation Op>
    uint8_t ANI_ORI_XRI_d8();

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

    uint8_t SBI_d8();

    uint8_t SCI_d8();

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
};

template <Registers::Register R>
inline uint8_t CPU::ADD_R() {
    return ADD_ADC_SUB_SBB_CMP_R<R, AritmeticOperation::ADD, false, true>();
}

template <Registers::Register R>
inline uint8_t CPU::ADC_R() {
    return ADD_ADC_SUB_SBB_CMP_R<R, AritmeticOperation::ADD, true, true>();
}

template <Registers::Register R>
inline uint8_t CPU::SUB_R() {
    return ADD_ADC_SUB_SBB_CMP_R<R, AritmeticOperation::SUB, false, true>();
}

template <Registers::Register R>
inline uint8_t CPU::SBB_R() {
    return ADD_ADC_SUB_SBB_CMP_R<R, AritmeticOperation::SUB, true, true>();
}

template <Registers::Register R>
inline uint8_t CPU::CMP_R() {
    return ADD_ADC_SUB_SBB_CMP_R<R, AritmeticOperation::SUB, false, false>();
}

template <Registers::Register R>
inline uint8_t CPU::INR_R() {
    return INR_DCR_R<R, AritmeticOperation::ADD>();
}

template<Registers::Register R>
inline uint8_t CPU::DCR_R() {
    return INR_DCR_R<R, AritmeticOperation::SUB>();
}

template <Registers::Register R>
inline uint8_t CPU::ANA_R() {
    return ANA_ORA_XRA_R<R, LogicOperation::AND>();
}

template <Registers::Register R>
inline uint8_t CPU::ORA_R() {
    return ANA_ORA_XRA_R<R, LogicOperation::OR>();
}

template <Registers::Register R>
inline uint8_t CPU::XRA_R() {
    return ANA_ORA_XRA_R<R, LogicOperation::XOR>();
}

template <Registers::Register R>
inline uint8_t CPU::RLC_R() {
    return RLC_RRC_RAL_RAR_R<R, ShiftDirection::LEFT, true>();
}

template <Registers::Register R>
inline uint8_t CPU::RAL_R() {
    return RLC_RRC_RAL_RAR_R<R, ShiftDirection::LEFT, false>();
}

template <Registers::Register R>
inline uint8_t CPU::RRC_R() {
    return RLC_RRC_RAL_RAR_R<R, ShiftDirection::RIGHT, true>();
}

template <Registers::Register R>
inline uint8_t CPU::RAR_R() {
    return RLC_RRC_RAL_RAR_R<R, ShiftDirection::RIGHT, false>();
}

template <Registers::Register Source, Registers::Register Destination>
inline uint8_t CPU::MOV_R_R() {
    registers_m.setRegister(Destination, registers_m.getRegister(Source));

    return MOV_R_R_Cycles;
}

template <Registers::Register R>
inline uint8_t CPU::MVI_R_d8() {
    registers_m.setRegister(R, readNextByte());

    return MVI_R_d8_Cycles;
}

template <Registers::Register R>
inline uint8_t CPU::MOV_M_R() {
    rom_m[registers_m.getCombinedRegister(Registers::CombinedRegister::HL)] = registers_m.getRegister(R);

    return MOV_M_R_Cycles;
}

template <Registers::Register R>
inline uint8_t CPU::MOV_R_M() {
    registers_m.setRegister(R, rom_m[registers_m.getCombinedRegister(Registers::CombinedRegister::HL)]);

    return MOV_R_M_Cycles;
}

template <Registers::CombinedRegister RR>
inline uint8_t CPU::INX_RR() {
    return INX_DCX_RR<RR, AritmeticOperation::ADD>();
}

template <Registers::CombinedRegister RR>
inline uint8_t CPU::DCX_RR() {
    return INX_DCX_RR<RR, AritmeticOperation::SUB>();
}

template <Registers::CombinedRegister RR>
inline uint8_t CPU::DAD_RR() {
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
inline uint8_t CPU::LXI_RR_d16() {
    registers_m.setCombinedRegister(RR, readNextTwoBytes());

    return LXI_Cycles;
}

template <Registers::Register R, CPU::AritmeticOperation Op, bool useCarry, bool storeResult>
inline uint8_t CPU::ADD_ADC_SUB_SBB_CMP_R() {
    const auto register_A{ registers_m.getRegister(Registers::Register::A) };
    const auto register_R{ registers_m.getRegister(R) };

    [[maybe_unused]]
    const auto result{ aritmeticOperation_8bits(register_A, register_R, Op, useCarry, true) };

    if constexpr (storeResult) {
        registers_m.setRegister(Registers::Register::A, result);
    }

    return ADD_ADC_SUB_SBB_CMP_R_Cycles;
}

template <CPU::AritmeticOperation Op, bool useCarry, bool storeResult>
inline uint8_t CPU::ADD_ADC_SUB_SBB_CMP_M() {
    loadMtoW();
    ADD_ADC_SUB_SBB_CMP_R<Registers::Register::W, Op, useCarry, storeResult>();

    return ADD_ADC_SUB_SBB_CMP_M_Cycles;
}

template <Registers::Register R, CPU::AritmeticOperation Op>
inline uint8_t CPU::INR_DCR_R() {
    const auto result{ aritmeticOperation_8bits(registers_m.getRegister(R), 1, Op, false, false) };

    registers_m.setRegister(R, result);

    return INR_DCR_R_Cycles;
}

template <CPU::AritmeticOperation Op>
inline uint8_t CPU::INR_DCR_M() {
    loadMtoW();
    INR_DCR_R<Registers::Register::W, Op>();
    writeWtoM();

    return INR_DCR_M_Cycles;
}

template <Registers::Register R, CPU::LogicOperation Op>
inline uint8_t CPU::ANA_ORA_XRA_R() {
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

template <CPU::LogicOperation Op>
inline uint8_t CPU::ANA_ORA_XRA_M() {
    loadMtoW();
    ANA_ORA_XRA_R<Registers::Register::W, Op>();

    return ANA_ORA_XRA_M_Cycles;
}

template <Registers::Register R, CPU::ShiftDirection direction, bool useCY>
inline uint8_t CPU::RLC_RRC_RAL_RAR_R() {
    auto registerValue{ registers_m.getRegister(R) };
    const bool dropedBit{ getBit(registerValue, direction == ShiftDirection::RIGHT ? 0 : 7) };

    if constexpr (direction == ShiftDirection::RIGHT) {
        registerValue >>= 1;
    } else {
        registerValue <<= 1;
    }
    
    registerValue = setBit(registerValue, direction == ShiftDirection::RIGHT ? 7 : 0, useCY ? dropedBit : registers_m.getFlag(Registers::Flags::CY));

    registers_m.setFlag(Registers::Flags::CY, dropedBit);
    registers_m.setRegister(R, registerValue);

    return RLC_RRC_RAL_RAR_Cycles;
}

template <Registers::CombinedRegister RR, CPU::AritmeticOperation Op>
inline uint8_t CPU::INX_DCX_RR() {
    const uint16_t result = registers_m.getCombinedRegister(RR) + (Op == AritmeticOperation::ADD ? 1 : -1);
    registers_m.setCombinedRegister(RR, result);

    return INX_DCX_RR_Cycles;
}

template <CPU::AritmeticOperation Op, bool useCarry, bool storeResult>
inline uint8_t CPU::ADI_ACI_SUI_SBI_CPI_d8() {
    registers_m.setRegister(Registers::Register::W, readNextByte());
    ADD_ADC_SUB_SBB_CMP_R<Registers::Register::W, Op, useCarry, storeResult>();
    
    return ADI_ACI_SUI_SBI_CPI_d8_Cycles;
}

template <CPU::LogicOperation Op>
inline uint8_t CPU::ANI_ORI_XRI_d8() {
    registers_m.setRegister(Registers::Register::W, readNextByte());
    ANA_ORA_XRA_R<Registers::Register::W, Op>();

    return ANI_ORI_XRI_d8_Cycles;
}

template <Registers::CombinedRegister RR>
inline uint8_t CPU::STAX_RR() {
    rom_m[registers_m.getCombinedRegister(RR)] = registers_m.getRegister(Registers::Register::A);

    return STAX_RR_Cycles;
}

template <Registers::CombinedRegister RR>
inline uint8_t CPU::LDAX_RR() {
    registers_m.setRegister(Registers::Register::A, rom_m[registers_m.getCombinedRegister(RR)]);

    return LDAX_RR_Cycles;
}

template <Registers::CombinedRegister RR>
inline uint8_t CPU::PUSH_RR() {
    const auto RR_value{ registers_m.getCombinedRegister(RR) };

    decreaseSP();

    rom_m[registers_m.getCombinedRegister(Registers::CombinedRegister::SP)] = getHighByte(RR_value);

    decreaseSP();

    rom_m[registers_m.getCombinedRegister(Registers::CombinedRegister::SP)] = getLowBytes(RR_value);

    return PUSH_RR_Cycles;
}

template <Registers::CombinedRegister RR>
inline uint8_t CPU::POP_RR() {
    uint8_t lowByte{ rom_m[registers_m.getCombinedRegister(Registers::CombinedRegister::SP)] };

    increaseSP();

    uint8_t highByte{ rom_m[registers_m.getCombinedRegister(Registers::CombinedRegister::SP)] };

    increaseSP();

    registers_m.setCombinedRegister(RR, static_cast<uint16_t>(highByte << Byte_Shift) | lowByte);

    return POP_RR_Cycles;
}

#endif // !CPU_HEADER