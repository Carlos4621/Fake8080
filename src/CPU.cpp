#include "CPU.hpp"

void CPU::setROM(std::span<uint8_t> rom) {
    rom_m = rom;
    pc_m = 0;
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

void CPU::decreaseSP() {
    registers_m.setCombinedRegister(Registers::CombinedRegister::SP, registers_m.getCombinedRegister(Registers::CombinedRegister::SP) - 1);
}

void CPU::increaseSP() {
    registers_m.setCombinedRegister(Registers::CombinedRegister::SP, registers_m.getCombinedRegister(Registers::CombinedRegister::SP) + 1);
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

void CPU::writeToM(uint8_t value) {
    rom_m[registers_m.getCombinedRegister(Registers::CombinedRegister::HL)] = value;
}

void CPU::writeTwoBytes(uint16_t address, uint16_t value) {
    rom_m[address] = getLowBytes(value);
    rom_m[address + 1] = getHighByte(value);
}

uint16_t CPU::readTwoBytes(uint16_t address) const {
    return static_cast<uint16_t>(rom_m[address + 1]) << Byte_Shift | rom_m[address];
}

void CPU::invalidOpcode()
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
    registers_m.setFlag(Registers::Flags::S, (value >> 7) != 0);
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

void CPU::callAddress(uint16_t address) {
    const auto addressToCall{ address };

    registers_m.setCombinedRegister(Registers::CombinedRegister::WZ, pc_m);

    PUSH_RR<Registers::CombinedRegister::WZ>();

    pc_m = addressToCall;
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
    uint8_t accumulator{ registers_m.getRegister(Registers::Register::A) };
    bool carry{ registers_m.getFlag(Registers::Flags::CY) };
    bool auxCarry{ registers_m.getFlag(Registers::Flags::AC) };
    
    // BCD correction: Si el nibble bajo (4 bits inferiores) > 9 o hay auxiliary carry,
    // añadir 6 para ajustar al rango BCD válido (0-9)
    if (auxCarry || (accumulator & 0x0F) > 9) {
        accumulator += 0x06;
    }
    
    // BCD correction: Si el nibble alto (4 bits superiores) > 9 o hay carry,
    // añadir 0x60 (96 decimal) para ajustar. Esto establece carry.
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
    writeToM(readNextByte());

    return MVI_M_d8_Cycles;
}

uint8_t CPU::SHLD_a16() {
    const auto address{ readNextTwoBytes() };
    writeTwoBytes(address, registers_m.getCombinedRegister(Registers::CombinedRegister::HL));

    return SHLD_Cycles;
}

uint8_t CPU::LHLD_a16() {
    const auto address{ readNextTwoBytes() };
    registers_m.setCombinedRegister(Registers::CombinedRegister::HL, readTwoBytes(address));

    return LHLD_Cycles;
}

uint8_t CPU::ADI_d8() {
    return ADI_ACI_SUI_SBI_CPI_d8<AritmeticOperation::ADD, false, true>();
}

uint8_t CPU::ACI_d8() {
    return ADI_ACI_SUI_SBI_CPI_d8<AritmeticOperation::ADD, true, true>();
}

uint8_t CPU::SBI_d8() {
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

uint8_t CPU::XTHL() {
    const uint16_t SP_value{ registers_m.getCombinedRegister(Registers::CombinedRegister::SP) };
    const uint16_t HL_value{ registers_m.getCombinedRegister(Registers::CombinedRegister::HL) };
    const uint16_t stack_value{ readTwoBytes(SP_value) };
    
    writeTwoBytes(SP_value, HL_value);
    registers_m.setCombinedRegister(Registers::CombinedRegister::HL, stack_value);
    
    return XTHL_Cycles;
}

uint8_t CPU::XCHG() {
    const auto swapAux{ registers_m.getCombinedRegister(Registers::CombinedRegister::HL) };

    registers_m.setCombinedRegister(Registers::CombinedRegister::HL, registers_m.getCombinedRegister(Registers::CombinedRegister::DE));
    registers_m.setCombinedRegister(Registers::CombinedRegister::DE, swapAux);

    return XCHG_Cycles;
}

uint8_t CPU::JMP_a16() {
    pc_m = readNextTwoBytes();

    return JMP_conditionalJUMP_a16_Cycles;
}

uint8_t CPU::JM_a16() {
    return conditionalJMP_a16<Registers::Flags::S, false>();
}

uint8_t CPU::JZ_a16() {
    return conditionalJMP_a16<Registers::Flags::Z, false>();
}

uint8_t CPU::JC_a16() {
    return conditionalJMP_a16<Registers::Flags::CY, false>();
}

uint8_t CPU::JPE_a16() {
    return conditionalJMP_a16<Registers::Flags::P, false>();
}

uint8_t CPU::JP_a16() {
    return conditionalJMP_a16<Registers::Flags::S, true>();
}

uint8_t CPU::JPO_a16() {
    return conditionalJMP_a16<Registers::Flags::P, true>();
}

uint8_t CPU::JNC_a16() {
    return conditionalJMP_a16<Registers::Flags::CY, true>();
}

uint8_t CPU::JNZ_a16() {
    return conditionalJMP_a16<Registers::Flags::Z, true>();
}

uint8_t CPU::CALL_a16() {
    callAddress(readNextTwoBytes());

    return CALL_a16_Cycles;
}

uint8_t CPU::RET() {
    POP_RR<Registers::CombinedRegister::WZ>();

    pc_m = registers_m.getCombinedRegister(Registers::CombinedRegister::WZ);

    return RET_Cycles;
}

uint8_t CPU::SPHL() {
    registers_m.setCombinedRegister(Registers::CombinedRegister::SP, registers_m.getCombinedRegister(Registers::CombinedRegister::HL));

    return SPHL_Cycles;
}

uint8_t CPU::PCHL() {
    pc_m = registers_m.getCombinedRegister(Registers::CombinedRegister::HL);

    return PCHL_Cycles;
}

uint8_t CPU::CZ_a16() {
    return conditionalCall_a16<Registers::Flags::Z, false>();
}

uint8_t CPU::CC_a16() {
    return conditionalCall_a16<Registers::Flags::CY, false>();
}

uint8_t CPU::CPE_a16() {
    return conditionalCall_a16<Registers::Flags::P, false>();
}

uint8_t CPU::CM_a16() {
    return conditionalCall_a16<Registers::Flags::S, false>();
}

uint8_t CPU::CNZ_a16() {
    return conditionalCall_a16<Registers::Flags::Z, true>();
}

uint8_t CPU::CNC_a16() {
    return conditionalCall_a16<Registers::Flags::CY, true>();
}

uint8_t CPU::CPO_a16() {
    return conditionalCall_a16<Registers::Flags::P, true>();
}

uint8_t CPU::CP_a16() {
    return conditionalCall_a16<Registers::Flags::S, true>();
}

uint8_t CPU::RZ() {
    return conditionalRet<Registers::Flags::Z, false>();
}

uint8_t CPU::RC() {
    return conditionalRet<Registers::Flags::CY, false>();
}

uint8_t CPU::RPE() {
    return conditionalRet<Registers::Flags::P, false>();
}

uint8_t CPU::RM() {
    return conditionalRet<Registers::Flags::S, false>();
}

uint8_t CPU::RNZ() {
    return conditionalRet<Registers::Flags::Z, true>();
}

uint8_t CPU::RNC() {
    return conditionalRet<Registers::Flags::CY, true>();
}

uint8_t CPU::RPO() {
    return conditionalRet<Registers::Flags::P, true>();
}

uint8_t CPU::RP() {
    return conditionalRet<Registers::Flags::S, true>();
}
