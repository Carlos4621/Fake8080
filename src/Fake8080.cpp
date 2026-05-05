#include "Fake8080.hpp"

Fake8080::Fake8080(MemoryBus &memoryBus, IOBus &ioBus) noexcept
: memoryBus_m{ memoryBus }
, IOBus_m{ ioBus }
{
}

void Fake8080::cycle() {
    if (interruptsEnableRequested_m) {
        interruptsEnabled_m = true;
        interruptsEnableRequested_m = false;
    }

    if (interruptRequested_m && interruptsEnabled_m) {
        callAddress(interruptVectorRequested_m * 8);
        interruptRequested_m = false;
        interruptsEnabled_m = false;
        halted_m = false;
        return;
    }

    if (halted_m) {
        return;
    }
    
    const auto opcode{ memoryBus_m.read(pc_m) };
    ++pc_m;

    (this->*opcodes_m[opcode])();
}

void Fake8080::requestInterrupt(uint8_t interruptNum) {
    interruptRequested_m = true;
    interruptVectorRequested_m = interruptNum;
}

void Fake8080::reset() {
    interruptRequested_m = false;
    interruptsEnabled_m = false;
    pc_m = 0;
    halted_m = false;
}

uint8_t Fake8080::readNextByte() {
    const auto byte{ memoryBus_m.read(pc_m) };
    ++pc_m;
    return byte;
}

uint16_t Fake8080::readNextTwoBytes() {
    const auto lowByte{ readNextByte() };
    const auto highByte{ readNextByte() };
    return (static_cast<uint16_t>(highByte) << Byte_Shift | static_cast<uint16_t>(lowByte));
}

void Fake8080::decreaseSP() {
    registers_m.setCombinedRegister(Registers::CombinedRegister::SP, registers_m.getCombinedRegister(Registers::CombinedRegister::SP) - 1);
}

void Fake8080::increaseSP() {
    registers_m.setCombinedRegister(Registers::CombinedRegister::SP, registers_m.getCombinedRegister(Registers::CombinedRegister::SP) + 1);
}

uint8_t Fake8080::getM() {
    return memoryBus_m.read(registers_m.getCombinedRegister(Registers::CombinedRegister::HL));
}

void Fake8080::loadMtoW() {
    registers_m.setRegister(Registers::Register::W, getM());
}

void Fake8080::writeWtoM() {
    memoryBus_m.write(registers_m.getCombinedRegister(Registers::CombinedRegister::HL), registers_m.getRegister(Registers::Register::W));
}

void Fake8080::writeToM(uint8_t value) {
    memoryBus_m.write(registers_m.getCombinedRegister(Registers::CombinedRegister::HL), value);
}

void Fake8080::writeTwoBytes(uint16_t address, uint16_t value) {
    memoryBus_m.write(address, getLowByte(value));
    memoryBus_m.write(address + 1, getHighByte(value));
}

uint16_t Fake8080::readTwoBytes(uint16_t address) const {
    return static_cast<uint16_t>(memoryBus_m.read(address + 1) << Byte_Shift | memoryBus_m.read(address));
}

void Fake8080::manageZeroFlag(uint8_t value) noexcept {
    registers_m.setFlag(Registers::Flags::Z, value == 0);
}

void Fake8080::manageParityFlag(uint8_t value) noexcept {
    uint8_t accumulator{ 0 };
    for (uint8_t i{ 0 }; i < Byte_Shift; ++i) {
        accumulator += (value >> i) & 1;
    }
    
    registers_m.setFlag(Registers::Flags::P, (accumulator % 2) == 0);
}

void Fake8080::manageSignedFlag(uint8_t value) noexcept {
    registers_m.setFlag(Registers::Flags::S, (value >> 7) != 0);
}

void Fake8080::manageCarryFlag(uint8_t first, uint8_t second, AritmeticOperation op, bool useCarry) noexcept {
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

void Fake8080::manageAuxilaryCarryFlag(uint8_t first, uint8_t second, AritmeticOperation op, bool useCarry) noexcept {
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

uint8_t Fake8080::aritmeticOperation_8bits(uint8_t first, uint8_t second, AritmeticOperation op, bool useCarry, bool modifyCarry) noexcept {
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

void Fake8080::callAddress(uint16_t address) {
    const auto addressToCall{ address };

    registers_m.setCombinedRegister(Registers::CombinedRegister::WZ, pc_m);

    PUSH_RR<Registers::CombinedRegister::WZ>();

    pc_m = addressToCall;
}

uint8_t Fake8080::NOP() {
    return NOP_Cycles;
}

uint8_t Fake8080::STC() {
    registers_m.setFlag(Registers::Flags::CY, 1);

    return STC_DAA_CMA_CMC_Cycles;
}

uint8_t Fake8080::CMA() {
    registers_m.setRegister(Registers::Register::A, ~registers_m.getRegister(Registers::Register::A));

    return STC_DAA_CMA_CMC_Cycles;
}

uint8_t Fake8080::CMC() {
    registers_m.setFlag(Registers::Flags::CY, !registers_m.getFlag(Registers::Flags::CY));

    return STC_DAA_CMA_CMC_Cycles;
}

uint8_t Fake8080::DAA() {
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

uint8_t Fake8080::ADD_M() {
    return ADD_ADC_SUB_SBB_CMP_M<Fake8080::AritmeticOperation::ADD, false, true>();
}

uint8_t Fake8080::SUB_M() {
    return ADD_ADC_SUB_SBB_CMP_M<Fake8080::AritmeticOperation::SUB, false, true>();
}

uint8_t Fake8080::ADC_M() {
    return ADD_ADC_SUB_SBB_CMP_M<Fake8080::AritmeticOperation::ADD, true, true>();
}

uint8_t Fake8080::SBB_M() {
    return ADD_ADC_SUB_SBB_CMP_M<Fake8080::AritmeticOperation::SUB, true, true>();
}

uint8_t Fake8080::CMP_M() {
    return ADD_ADC_SUB_SBB_CMP_M<Fake8080::AritmeticOperation::SUB, false, false>();
}

uint8_t Fake8080::ANA_M() {
    return ANA_ORA_XRA_M<LogicOperation::AND>();
}

uint8_t Fake8080::ORA_M() {
    return ANA_ORA_XRA_M<LogicOperation::OR>();
}

uint8_t Fake8080::XRA_M() {
    return ANA_ORA_XRA_M<LogicOperation::XOR>();
}

uint8_t Fake8080::INR_M() {
    return INR_DCR_M<AritmeticOperation::ADD>();
}

uint8_t Fake8080::DCR_M() {
    return INR_DCR_M<AritmeticOperation::SUB>();
}

uint8_t Fake8080::MVI_M_d8() {
    writeToM(readNextByte());

    return MVI_M_d8_Cycles;
}

uint8_t Fake8080::SHLD_a16() {
    const auto address{ readNextTwoBytes() };
    writeTwoBytes(address, registers_m.getCombinedRegister(Registers::CombinedRegister::HL));

    return SHLD_Cycles;
}

uint8_t Fake8080::LHLD_a16() {
    const auto address{ readNextTwoBytes() };
    registers_m.setCombinedRegister(Registers::CombinedRegister::HL, readTwoBytes(address));

    return LHLD_Cycles;
}

uint8_t Fake8080::ADI_d8() {
    return ADI_ACI_SUI_SBI_CPI_d8<AritmeticOperation::ADD, false, true>();
}

uint8_t Fake8080::ACI_d8() {
    return ADI_ACI_SUI_SBI_CPI_d8<AritmeticOperation::ADD, true, true>();
}

uint8_t Fake8080::SUI_d8() {
    return ADI_ACI_SUI_SBI_CPI_d8<AritmeticOperation::SUB, false, true>();
}

uint8_t Fake8080::SBI_d8() {
    return ADI_ACI_SUI_SBI_CPI_d8<AritmeticOperation::SUB, true, true>();
}

uint8_t Fake8080::CPI_d8() {
    return ADI_ACI_SUI_SBI_CPI_d8<AritmeticOperation::SUB, false, false>();
}

uint8_t Fake8080::ANI_d8() {
    return ANI_ORI_XRI_d8<LogicOperation::AND>();
}

uint8_t Fake8080::ORI_d8() {
    return ANI_ORI_XRI_d8<LogicOperation::OR>();
}

uint8_t Fake8080::XRI_d8() {
    return ANI_ORI_XRI_d8<LogicOperation::XOR>();
}

uint8_t Fake8080::STA_a16() {
    registers_m.setCombinedRegister(Registers::CombinedRegister::WZ, readNextTwoBytes());
    STAX_RR<Registers::CombinedRegister::WZ>();

    return STA_a16_Cycles;
}

uint8_t Fake8080::LDA_a16() {
    registers_m.setCombinedRegister(Registers::CombinedRegister::WZ, readNextTwoBytes());
    LDAX_RR<Registers::CombinedRegister::WZ>();

    return LDA_a16_Cycles;
}

uint8_t Fake8080::XTHL() {
    const uint16_t SP_value{ registers_m.getCombinedRegister(Registers::CombinedRegister::SP) };
    const uint16_t HL_value{ registers_m.getCombinedRegister(Registers::CombinedRegister::HL) };
    const uint16_t stack_value{ readTwoBytes(SP_value) };
    
    writeTwoBytes(SP_value, HL_value);
    registers_m.setCombinedRegister(Registers::CombinedRegister::HL, stack_value);
    
    return XTHL_Cycles;
}

uint8_t Fake8080::XCHG() {
    const auto swapAux{ registers_m.getCombinedRegister(Registers::CombinedRegister::HL) };

    registers_m.setCombinedRegister(Registers::CombinedRegister::HL, registers_m.getCombinedRegister(Registers::CombinedRegister::DE));
    registers_m.setCombinedRegister(Registers::CombinedRegister::DE, swapAux);

    return XCHG_Cycles;
}

uint8_t Fake8080::JMP_a16() {
    pc_m = readNextTwoBytes();

    return JMP_conditionalJUMP_a16_Cycles;
}

uint8_t Fake8080::JM_a16() {
    return conditionalJMP_a16<Registers::Flags::S, false>();
}

uint8_t Fake8080::JZ_a16() {
    return conditionalJMP_a16<Registers::Flags::Z, false>();
}

uint8_t Fake8080::JC_a16() {
    return conditionalJMP_a16<Registers::Flags::CY, false>();
}

uint8_t Fake8080::JPE_a16() {
    return conditionalJMP_a16<Registers::Flags::P, false>();
}

uint8_t Fake8080::JP_a16() {
    return conditionalJMP_a16<Registers::Flags::S, true>();
}

uint8_t Fake8080::JPO_a16() {
    return conditionalJMP_a16<Registers::Flags::P, true>();
}

uint8_t Fake8080::JNC_a16() {
    return conditionalJMP_a16<Registers::Flags::CY, true>();
}

uint8_t Fake8080::JNZ_a16() {
    return conditionalJMP_a16<Registers::Flags::Z, true>();
}

uint8_t Fake8080::CALL_a16() {
    callAddress(readNextTwoBytes());

    return CALL_a16_Cycles;
}

uint8_t Fake8080::RET() {
    POP_RR<Registers::CombinedRegister::WZ>();

    pc_m = registers_m.getCombinedRegister(Registers::CombinedRegister::WZ);

    return RET_Cycles;
}

uint8_t Fake8080::SPHL() {
    registers_m.setCombinedRegister(Registers::CombinedRegister::SP, registers_m.getCombinedRegister(Registers::CombinedRegister::HL));

    return SPHL_Cycles;
}

uint8_t Fake8080::PCHL() {
    pc_m = registers_m.getCombinedRegister(Registers::CombinedRegister::HL);

    return PCHL_Cycles;
}

uint8_t Fake8080::CZ_a16() {
    return conditionalCall_a16<Registers::Flags::Z, false>();
}

uint8_t Fake8080::CC_a16() {
    return conditionalCall_a16<Registers::Flags::CY, false>();
}

uint8_t Fake8080::CPE_a16() {
    return conditionalCall_a16<Registers::Flags::P, false>();
}

uint8_t Fake8080::CM_a16() {
    return conditionalCall_a16<Registers::Flags::S, false>();
}

uint8_t Fake8080::CNZ_a16() {
    return conditionalCall_a16<Registers::Flags::Z, true>();
}

uint8_t Fake8080::CNC_a16() {
    return conditionalCall_a16<Registers::Flags::CY, true>();
}

uint8_t Fake8080::CPO_a16() {
    return conditionalCall_a16<Registers::Flags::P, true>();
}

uint8_t Fake8080::CP_a16() {
    return conditionalCall_a16<Registers::Flags::S, true>();
}

uint8_t Fake8080::RZ() {
    return conditionalRet<Registers::Flags::Z, false>();
}

uint8_t Fake8080::RC() {
    return conditionalRet<Registers::Flags::CY, false>();
}

uint8_t Fake8080::RPE() {
    return conditionalRet<Registers::Flags::P, false>();
}

uint8_t Fake8080::RM() {
    return conditionalRet<Registers::Flags::S, false>();
}

uint8_t Fake8080::RNZ() {
    return conditionalRet<Registers::Flags::Z, true>();
}

uint8_t Fake8080::RNC() {
    return conditionalRet<Registers::Flags::CY, true>();
}

uint8_t Fake8080::RPO() {
    return conditionalRet<Registers::Flags::P, true>();
}

uint8_t Fake8080::RP() {
    return conditionalRet<Registers::Flags::S, true>();
}

uint8_t Fake8080::EI() {
    interruptsEnableRequested_m = true;

    return EI_DI_Cycles;
}

uint8_t Fake8080::DI() {
    interruptsEnabled_m = false;

    return EI_DI_Cycles;
}

uint8_t Fake8080::IN_d8() {
    const auto port{ readNextByte() };
    
    registers_m.setRegister(Registers::Register::A, IOBus_m.in(port));

    return IN_d8_Cycles;
}

uint8_t Fake8080::OUT_d8() {
    const auto port{ readNextByte() };

    IOBus_m.out(port, registers_m.getRegister(Registers::Register::A));

    return OUT_d8_Cycles;
}

uint8_t Fake8080::HLT() {
    halted_m = true;

    return HLT_Cycles;
}
