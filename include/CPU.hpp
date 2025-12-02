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
    enum class Operations : uint8_t { ADD = 0, SUB };

    using MemberFunction = uint8_t(CPU::*)();

    static constexpr uint8_t Byte_Shift{ 8 };
    static constexpr uint16_t Opcodes_Number{ 256 };

    static std::array<MemberFunction, Opcodes_Number> Opcodes;

    std::span<uint8_t> rom_m;

    uint16_t pc_m{ 0 };
    Registers registers_m;

    uint8_t internalRegisterW_m{ 0 };
    uint8_t internalRegisterZ_m{ 0 };

    [[nodiscard]]
    uint8_t readNextByte();

    [[nodiscard]]
    uint16_t readNextTwoBytes();

    [[noreturn]]
    void InvalidOpcode();

    void manageZeroFlag(uint8_t value) noexcept;
    void manageParityFlag(uint8_t value) noexcept;
    void manageSignedFlag(uint8_t value) noexcept;
    void manageCarryFlag(uint8_t first, uint8_t second, Operations op, bool useCarry = false);
    void manageAuxilaryCarryFlag(uint8_t first, uint8_t second, Operations op, bool useCarry = false);

    uint8_t aritmeticOperation_8bits(uint8_t first, uint8_t second, Operations op, bool useCarry);
};

#endif // !CPU_HEADER