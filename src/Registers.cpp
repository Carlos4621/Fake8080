#include "Registers.hpp"

Registers::Registers() {
    setRegister(Register::F, Initial_Flags_Register_Value);
}

uint8_t Registers::getRegister(Register reg) const noexcept {
    return registers_m[std::to_underlying(reg)];
}

uint16_t Registers::getCombinedRegister(CombinedRegister reg) const noexcept {
    const auto [high, low] = RegisterPairs[std::to_underlying(reg)];
    return combineRegisters(high, low);
}

void Registers::setRegister(Register reg, uint8_t value) noexcept {
    registers_m[std::to_underlying(reg)] = value;
}

void Registers::setCombinedRegister(CombinedRegister reg, uint16_t value) noexcept {
    const auto [high, low] = RegisterPairs[std::to_underlying(reg)];
    setRegister(high, getHighByte(value));
    setRegister(low, getLowBytes(value));
}

bool Registers::getFlag(Flags flag) const noexcept {
    return getBit(getRegister(Register::F), FlagsShifts[std::to_underlying(flag)]);
}

void Registers::setFlag(Flags flag, bool value) noexcept {
    const uint8_t shift{ FlagsShifts[std::to_underlying(flag)] };
    setRegister(Register::F, setBit(getRegister(Register::F), shift, value));
}

uint16_t Registers::combineRegisters(Register high, Register low) const noexcept {
    return static_cast<uint16_t>(getRegister(high)) << Byte_Shift | getRegister(low);
}