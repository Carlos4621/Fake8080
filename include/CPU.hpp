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

    uint8_t internalRegisterW_m{ 0 };
    uint8_t internalRegisterZ_m{ 0 };

    /// @brief Lee el siguiente byte e incrementa el pc
    /// @return Byte leído
    [[nodiscard]]
    uint8_t readNextByte();

    /// @brief Lee los siguientes 2 bytes, los combina en little endian e incrementa el pc
    /// @return Los siguientes 2 bytes en little endian
    [[nodiscard]]
    uint16_t readNextTwoBytes();

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

    /// @brief Superfunción para opcodes ADD, ADC, SUB, SBB y CMP con R
    /// @tparam R Registro a usar
    /// @tparam Op Operación a aplicar, ya sea suma o resta
    /// @tparam useCarry Tener en cuenta la flag CY
    /// @tparam storeResult Indica si se quiere modificar el registro con el resultado
    /// @return Número de ciclos usados
    template<Registers::Register R, AritmeticOperation Op, bool useCarry, bool storeResult>
    uint8_t ADD_ADC_SUB_SBB_CMP_R();

    /// @brief Superfunción para INR y DCR con R
    /// @tparam R Registro a usar
    /// @tparam Op Operación a aplicar, ya sea suma o resta
    /// @return Número de ciclos usados
    template<Registers::Register R, AritmeticOperation Op>
    uint8_t INR_DCR_R();

    /// @brief Superfunción para ANA, ORA y XRA con R
    /// @tparam R Registro a usar
    /// @tparam Op Operación lógica a usar, AND, OR o XOR
    /// @return Número de ciclos usados
    template<Registers::Register R, LogicOperation Op>
    uint8_t ANA_ORA_XRA_R();

    template<Registers::Register R, bool useCY>
    uint8_t RLC_RAL_R();
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
    return RLC_RAL_R<R, true>();
}

template <Registers::Register R>
inline uint8_t CPU::RAL_R() {
    return RLC_RAL_R<R, false>();
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

    return ADD_ADC_SUB_SBB_CMP_Cycles;
}

template <Registers::Register R, CPU::AritmeticOperation Op>
inline uint8_t CPU::INR_DCR_R() {
    const auto result{ aritmeticOperation_8bits(registers_m.getRegister(R), 1, Op, false, false) };

    registers_m.setRegister(R, result);

    return INR_DCR_Cycles;
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

    return ANA_ORA_XRA_Cycles;
}

template <Registers::Register R, bool useCY>
inline uint8_t CPU::RLC_RAL_R() {
    auto registerValue{ registers_m.getRegister(R) };
    const bool dropedBit{ getBit(registerValue, 7) };

    registerValue <<= 1;

    registerValue = setBit(registerValue, 0, useCY ? dropedBit : registers_m.getFlag(Registers::Flags::CY));

    registers_m.setFlag(Registers::Flags::CY, dropedBit);
    registers_m.setRegister(R, registerValue);

    return RLC_RRC_RAL_RAR_Cycles;
}

#endif // !CPU_HEADER