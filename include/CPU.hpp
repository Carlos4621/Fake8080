#ifndef CPU_HEADER
#define CPU_HEADER

#include <cstdint>
#include <span>
#include <stdexcept>
#include "Registers.hpp"

class CPU {
public:
    void setROM(std::span<uint8_t> rom);

    void cycle();

private:
    using MemberFunction = void(CPU::*)();

    static constexpr uint8_t Byte_Shift{ 8 };
    static constexpr uint16_t Opcodes_Number{ 256 };

    static std::array<MemberFunction, Opcodes_Number> Opcodes;

    std::span<uint8_t> rom_m;

    uint16_t pc_m{ 0 };
    Registers registers_m;

    uint8_t internalRegisterW_m{ 0 };
    uint8_t internalRegisterZ_m{ 0 };

    [[nodiscard]]
    uint8_t readByte();

    [[nodiscard]]
    uint16_t readTwoBytes();

    [[noreturn]]
    void InvalidOpcode();
};

#endif // !CPU_HEADER