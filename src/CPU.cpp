#include "CPU.hpp"

void CPU::setROM(std::span<uint8_t> rom) {
    rom_m = rom;
}

void CPU::cycle() {
}

uint8_t CPU::readNextByte() {
    const auto byte{ rom_m[pc_m] };
    ++pc_m;
    return byte;
}

uint16_t CPU::readNextTwoBytes() {
    const auto lowByte{ readNextByte() };
    const auto highByte{ readNextByte() };
    return (static_cast<uint16_t>(highByte) << Byte_Shift | static_cast<uint16_t>(lowByte));
}

void CPU::InvalidOpcode() {
    throw std::runtime_error{ "The opcode isn't implemented" };
}

void CPU::manageZeroFlag(uint8_t value) noexcept {
    registers_m.setFlag(Registers::Flags::Z, value == 0);
}

void CPU::manageParityFlag(uint8_t value) noexcept {
    uint8_t accumulator{ 0 };
    for (uint8_t i{ 0 }; i < Byte_Shift; ++i) {
        accumulator += (value >> i) & 1;
    }
    
    registers_m.setFlag(Registers::Flags::P, (accumulator % 2) == 0);
}

void CPU::manageSignedFlag(uint8_t value) noexcept {
    registers_m.setFlag(Registers::Flags::S, (value >> 7) == 1 ? true : false);
}

void CPU::manageCarryFlag(uint8_t first, uint8_t second, Operations op, bool useCarry) {
    bool isCarry{ false };
    const uint8_t carryValue{ useCarry && registers_m.getFlag(Registers::Flags::CY) };
    
    switch (op) {
    case Operations::ADD:
        isCarry = (static_cast<uint16_t>(first) + static_cast<uint16_t>(second) + carryValue) > 0xFF;
        break;
        
    case Operations::SUB:
        isCarry = (static_cast<uint16_t>(first) < (static_cast<uint16_t>(second) + carryValue));
        break;
    }

    registers_m.setFlag(Registers::Flags::CY, isCarry);
}

void CPU::manageAuxilaryCarryFlag(uint8_t first, uint8_t second, Operations op, bool useCarry) {
    bool isHalfCarry{ false };
    const uint8_t carryValue{ useCarry && registers_m.getFlag(Registers::Flags::CY) };
    
    switch (op) {
    case Operations::ADD:
        isHalfCarry = ((first & 0x0F) + (second & 0x0F) + carryValue) > 0x0F;
        break;

    case Operations::SUB:
        isHalfCarry = ((first & 0x0F) < ((second & 0x0F) + carryValue));
        break;
    }

    registers_m.setFlag(Registers::Flags::AC, isHalfCarry);
}

uint8_t CPU::aritmeticOperation_8bits(uint8_t first, uint8_t second, Operations op, bool useCarry) {
    const uint8_t result = first + ((second + (useCarry && registers_m.getFlag(Registers::Flags::CY))) * (op == Operations::ADD ? 1 : -1));

    manageAuxilaryCarryFlag(first, second, op, useCarry);
    manageCarryFlag(first, second, op, useCarry); 
    manageZeroFlag(result);
    manageParityFlag(result);
    manageSignedFlag(result);

    return result;
}
