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

uint8_t CPU::getM() {
    return rom_m[registers_m.getCombinedRegister(Registers::CombinedRegister::HL)];
}

void CPU::loadMtoW() {
    registers_m.setRegister(Registers::Register::W, getM());
}

void CPU::writeWtoM() {
    rom_m[registers_m.getCombinedRegister(Registers::CombinedRegister::HL)] = registers_m.getRegister(Registers::Register::W);
}

void CPU::InvalidOpcode()
{
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

void CPU::manageCarryFlag(uint8_t first, uint8_t second, AritmeticOperation op, bool useCarry) noexcept {
    bool isCarry{ false };
    const uint8_t carryValue{ useCarry && registers_m.getFlag(Registers::Flags::CY) };
    
    switch (op) {
    case AritmeticOperation::ADD:
        isCarry = (static_cast<uint16_t>(first) + static_cast<uint16_t>(second) + carryValue) > 0xFF;
        break;
        
    case AritmeticOperation::SUB:
        isCarry = (static_cast<uint16_t>(first) < (static_cast<uint16_t>(second) + carryValue));
        break;
    }

    registers_m.setFlag(Registers::Flags::CY, isCarry);
}

void CPU::manageAuxilaryCarryFlag(uint8_t first, uint8_t second, AritmeticOperation op, bool useCarry) noexcept {
    bool isHalfCarry{ false };
    const uint8_t carryValue{ useCarry && registers_m.getFlag(Registers::Flags::CY) };
    
    switch (op) {
    case AritmeticOperation::ADD:
        isHalfCarry = ((first & 0x0F) + (second & 0x0F) + carryValue) > 0x0F;
        break;

    case AritmeticOperation::SUB:
        isHalfCarry = ((first & 0x0F) < ((second & 0x0F) + carryValue));
        break;
    }

    registers_m.setFlag(Registers::Flags::AC, isHalfCarry);
}

uint8_t CPU::aritmeticOperation_8bits(uint8_t first, uint8_t second, AritmeticOperation op, bool useCarry, bool modifyCarry) noexcept {
    const uint8_t result = first + ((second + (useCarry && registers_m.getFlag(Registers::Flags::CY))) * (op == AritmeticOperation::ADD ? 1 : -1));

    manageAuxilaryCarryFlag(first, second, op, useCarry);

    if (modifyCarry) {
        manageCarryFlag(first, second, op, useCarry);
    }

    manageZeroFlag(result);
    manageParityFlag(result);
    manageSignedFlag(result);

    return result;
}

uint8_t CPU::STC() {
    registers_m.setFlag(Registers::Flags::CY, 1);

    return STC_DAA_CMA_CMC_Cycles;
}

uint8_t CPU::CMA() {
    registers_m.setRegister(Registers::Register::A, ~registers_m.getRegister(Registers::Register::A));

    return STC_DAA_CMA_CMC_Cycles;
}

uint8_t CPU::CMC() {
    registers_m.setFlag(Registers::Flags::CY, !registers_m.getFlag(Registers::Flags::CY));

    return STC_DAA_CMA_CMC_Cycles;
}

uint8_t CPU::DAA() {
    uint8_t accumulator = registers_m.getRegister(Registers::Register::A);
    uint8_t lowerNibble = accumulator & 0x0F;
    uint8_t upperNibble = accumulator >> 4;
    bool carry = registers_m.getFlag(Registers::Flags::CY);
    bool auxCarry = registers_m.getFlag(Registers::Flags::AC);
    
    // Ajustar el nibble inferior
    if (auxCarry || lowerNibble > 9) {
        accumulator += 0x06;
    }
    
    // Ajustar el nibble superior (usar el nibble superior actualizado)
    if (carry || (accumulator >> 4) > 9) {
        accumulator += 0x60;
        carry = true;
    }
    
    registers_m.setRegister(Registers::Register::A, accumulator);
    registers_m.setFlag(Registers::Flags::CY, carry);
    
    // Actualizar flags de condición
    manageZeroFlag(accumulator);
    manageParityFlag(accumulator);
    manageSignedFlag(accumulator);
    
    return STC_DAA_CMA_CMC_Cycles;
}

uint8_t CPU::ADD_M() {
    return ADD_ADC_SUB_SBB_CMP_M<CPU::AritmeticOperation::ADD, false, true>();
}

uint8_t CPU::SUB_M() {
    return ADD_ADC_SUB_SBB_CMP_M<CPU::AritmeticOperation::SUB, false, true>();
}

uint8_t CPU::ADC_M() {
    return ADD_ADC_SUB_SBB_CMP_M<CPU::AritmeticOperation::ADD, true, true>();
}

uint8_t CPU::SBB_M() {
    return ADD_ADC_SUB_SBB_CMP_M<CPU::AritmeticOperation::SUB, true, true>();
}

uint8_t CPU::CMP_M() {
    return ADD_ADC_SUB_SBB_CMP_M<CPU::AritmeticOperation::SUB, false, false>();
}

uint8_t CPU::ANA_M() {
    return ANA_ORA_XRA_M<LogicOperation::AND>();
}

uint8_t CPU::ORA_M() {
    return ANA_ORA_XRA_M<LogicOperation::OR>();
}

uint8_t CPU::XRA_M() {
    return ANA_ORA_XRA_M<LogicOperation::XOR>();
}

uint8_t CPU::INR_M() {
    return INR_DCR_M<AritmeticOperation::ADD>();
}

uint8_t CPU::DCR_M() {
    return INR_DCR_M<AritmeticOperation::SUB>();
}

uint8_t CPU::MVI_M_d8() {
    rom_m[registers_m.getCombinedRegister(Registers::CombinedRegister::HL)] = readNextByte();

    return MVI_M_d8_Cycles;
}

uint8_t CPU::SHLD_a16() {
    const auto address{ readNextTwoBytes() };
    rom_m[address] = registers_m.getRegister(Registers::Register::L);
    rom_m[address + 1] = registers_m.getRegister(Registers::Register::H);

    return SHLD_Cycles;
}

uint8_t CPU::LHLD_a16() {
    const auto address{ readNextTwoBytes() };
    registers_m.setRegister(Registers::Register::L, rom_m[address]);
    registers_m.setRegister(Registers::Register::H, rom_m[address + 1]);

    return LHLD_Cycles;
}

uint8_t CPU::ADI_d8() {
    return ADI_ACI_SUI_SBI_CPI_d8<AritmeticOperation::ADD, false, true>();
}

uint8_t CPU::ACI_d8() {
    return ADI_ACI_SUI_SBI_CPI_d8<AritmeticOperation::ADD, true, true>();
}

uint8_t CPU::SBI_d8() {
    return ADI_ACI_SUI_SBI_CPI_d8<AritmeticOperation::SUB, false, true>();
}

uint8_t CPU::SCI_d8() {
    return ADI_ACI_SUI_SBI_CPI_d8<AritmeticOperation::SUB, true, true>();
}

uint8_t CPU::CPI_d8() {
    return ADI_ACI_SUI_SBI_CPI_d8<AritmeticOperation::SUB, false, false>();
}

uint8_t CPU::ANI_d8() {
    return ANI_ORI_XRI_d8<LogicOperation::AND>();
}

uint8_t CPU::ORI_d8() {
    return ANI_ORI_XRI_d8<LogicOperation::OR>();
}

uint8_t CPU::XRI_d8() {
    return ANI_ORI_XRI_d8<LogicOperation::XOR>();
}

uint8_t CPU::STA_a16() {
    registers_m.setCombinedRegister(Registers::CombinedRegister::WZ, readNextTwoBytes());
    STAX_RR<Registers::CombinedRegister::WZ>();

    return STA_a16_Cycles;
}

uint8_t CPU::LDA_a16() {
    registers_m.setCombinedRegister(Registers::CombinedRegister::WZ, readNextTwoBytes());
    LDAX_RR<Registers::CombinedRegister::WZ>();

    return LDA_a16_Cycles;
}
