#include "CPU.hpp"

void CPU::setROM(std::span<uint8_t> rom) {
    rom_m = rom;
}

void CPU::cycle() {
}

uint8_t CPU::readByte() {
    const auto byte{ rom_m[pc_m] };
    ++pc_m;
    return byte;
}

uint16_t CPU::readTwoBytes() {
    const auto lowByte{ readByte() };
    const auto highByte{ readByte() };
    return (static_cast<uint16_t>(highByte) << Byte_Shift | static_cast<uint16_t>(lowByte));
}

void CPU::InvalidOpcode() {
    throw std::runtime_error{ "The opcode isn't implemented" };
}
