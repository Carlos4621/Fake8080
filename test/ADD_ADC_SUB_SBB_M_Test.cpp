#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"
#include <array>

class ADD_ADC_SUB_SBB_M_Test : public ::testing::Test {
protected:
    CPUTest cpu;
    std::array<uint8_t, 0x10000> rom;  // 64KB ROM (tamaño real del 8080)

    void SetUp() override {
        // Inicializar todos los registros a 0
        cpu.registers_m.setRegister(Registers::Register::A, 0x00);
        cpu.registers_m.setRegister(Registers::Register::B, 0x00);
        cpu.registers_m.setRegister(Registers::Register::C, 0x00);
        cpu.registers_m.setRegister(Registers::Register::D, 0x00);
        cpu.registers_m.setRegister(Registers::Register::E, 0x00);
        cpu.registers_m.setRegister(Registers::Register::H, 0x00);
        cpu.registers_m.setRegister(Registers::Register::L, 0x00);
        
        // Limpiar todos los flags
        cpu.registers_m.setFlag(Registers::Flags::S, false);
        cpu.registers_m.setFlag(Registers::Flags::Z, false);
        cpu.registers_m.setFlag(Registers::Flags::AC, false);
        cpu.registers_m.setFlag(Registers::Flags::P, false);
        cpu.registers_m.setFlag(Registers::Flags::CY, false);
        
        // Inicializar ROM con ceros
        rom.fill(0x00);
        cpu.setROM(rom);
    }
};

// ==================== Tests para ADD M ====================

TEST_F(ADD_ADC_SUB_SBB_M_Test, ADD_M_BasicAddition) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x20;
    
    uint8_t cycles = cpu.ADD_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x30);
    EXPECT_EQ(cycles, 7);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, ADD_M_WithOverflow) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x01;
    
    uint8_t cycles = cpu.ADD_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, ADD_M_WithHalfCarry) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x0F);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x01;
    
    uint8_t cycles = cpu.ADD_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x10);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, ADD_M_ResultNegative) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x7F);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x01;
    
    uint8_t cycles = cpu.ADD_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x80);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, ADD_M_ZeroResult) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x00;
    
    uint8_t cycles = cpu.ADD_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, ADD_M_ParityFlag) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x0E);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x01;
    
    uint8_t cycles = cpu.ADD_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x0F);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P)); // 0x0F tiene 4 bits en 1 (par)
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, ADD_M_MaxValue) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0xFF;
    
    uint8_t cycles = cpu.ADD_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFE);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, ADD_M_AtDifferentAddresses) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    
    // Primer ADD M en dirección 0x1000
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1000);
    rom[0x1000] = 0x05;
    cpu.ADD_M();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x15);
    
    // Segundo ADD M en dirección 0x3000
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3000);
    rom[0x3000] = 0x0A;
    cpu.ADD_M();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x1F);
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, ADD_M_PreservesHLRegister) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x20;
    
    cpu.ADD_M();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2000);
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, ADD_M_DoesNotModifyMemory) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x20;
    
    cpu.ADD_M();
    
    EXPECT_EQ(rom[0x2000], 0x20);
}

// ==================== Tests para ADC M ====================

TEST_F(ADD_ADC_SUB_SBB_M_Test, ADC_M_WithCarryClear) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x20;
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    uint8_t cycles = cpu.ADC_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x30);
    EXPECT_EQ(cycles, 7);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, ADC_M_WithCarrySet) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x20;
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.ADC_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x31);
    EXPECT_EQ(cycles, 7);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, ADC_M_CarryCausesOverflow) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x00;
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.ADC_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, ADC_M_CarryCausesHalfCarry) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x0E);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x00;
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.ADC_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x0F);
    EXPECT_EQ(cycles, 7);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, ADC_M_ComplexCase) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x88);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x77;
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.ADC_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, ADC_M_MaxValues) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0xFF;
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.ADC_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, ADC_M_WithCarry_AtAddressZero) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0000);
    rom[0x0000] = 0x05;
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.ADC_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x16);
    EXPECT_EQ(cycles, 7);
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, ADC_M_PreservesHLRegister) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x20;
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.ADC_M();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2000);
}

// ==================== Tests para SUB M ====================

TEST_F(ADD_ADC_SUB_SBB_M_Test, SUB_M_BasicSubtraction) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x30);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x10;
    
    uint8_t cycles = cpu.SUB_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x20);
    EXPECT_EQ(cycles, 7);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, SUB_M_ResultZero) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x50);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x50;
    
    uint8_t cycles = cpu.SUB_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, SUB_M_WithBorrow) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x20;
    
    uint8_t cycles = cpu.SUB_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xF0);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, SUB_M_WithHalfBorrow) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x20);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x01;
    
    uint8_t cycles = cpu.SUB_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x1F);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, SUB_M_LargerFromSmaller) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x05);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x0A;
    
    uint8_t cycles = cpu.SUB_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFB);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, SUB_M_ParityCheck) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x08);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x01;
    
    uint8_t cycles = cpu.SUB_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x07);
    EXPECT_EQ(cycles, 7);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P)); // 0x07 tiene 3 bits en 1 (impar)
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, SUB_M_FromZero) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x01;
    
    uint8_t cycles = cpu.SUB_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, SUB_M_MaxValue) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0xFF;
    
    uint8_t cycles = cpu.SUB_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, SUB_M_PreservesHLRegister) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x30);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x10;
    
    cpu.SUB_M();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2000);
}

// ==================== Tests para SBB M ====================

TEST_F(ADD_ADC_SUB_SBB_M_Test, SBB_M_WithCarryClear) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x30);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x10;
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    uint8_t cycles = cpu.SBB_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x20);
    EXPECT_EQ(cycles, 7);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, SBB_M_WithCarrySet) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x30);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x10;
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.SBB_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x1F);
    EXPECT_EQ(cycles, 7);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, SBB_M_CarryCausesBorrow) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x10;
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.SBB_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, SBB_M_CarryCausesHalfBorrow) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x20);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x00;
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.SBB_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x1F);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, SBB_M_ComplexCase) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x88);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x44;
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.SBB_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x43);
    EXPECT_EQ(cycles, 7);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, SBB_M_EdgeCase) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x00;
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.SBB_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, SBB_M_WithCarry_ResultZero) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x51);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x50;
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.SBB_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, SBB_M_PreservesHLRegister) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x30);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x10;
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.SBB_M();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2000);
}

// ==================== Tests de casos prácticos ====================

TEST_F(ADD_ADC_SUB_SBB_M_Test, Sequential_ADD_M_Operations) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    
    // Primera suma
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x05;
    cpu.ADD_M();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x15);
    
    // Segunda suma
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2001);
    rom[0x2001] = 0x0A;
    cpu.ADD_M();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x1F);
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, Mixed_Operations_ADD_SUB) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x20);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    
    // ADD M
    rom[0x2000] = 0x10;
    cpu.ADD_M();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x30);
    
    // SUB M
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2001);
    rom[0x2001] = 0x05;
    cpu.SUB_M();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x2B);
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, ADC_M_Chain_WithCarryPropagation) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    
    // Primera ADC M (genera carry)
    rom[0x2000] = 0x01;
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    cpu.ADC_M();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    // Segunda ADC M (usa el carry)
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2001);
    rom[0x2001] = 0x00;
    cpu.ADC_M();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x01);
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, AllOperations_DoNotModifyMemory) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x50);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x20;
    
    cpu.ADD_M();
    EXPECT_EQ(rom[0x2000], 0x20);
    
    cpu.ADC_M();
    EXPECT_EQ(rom[0x2000], 0x20);
    
    cpu.SUB_M();
    EXPECT_EQ(rom[0x2000], 0x20);
    
    cpu.SBB_M();
    EXPECT_EQ(rom[0x2000], 0x20);
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, AllOperations_WithHighAddresses) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x50);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xFFFF);
    rom[0xFFFF] = 0x10;
    
    cpu.ADD_M();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x60);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x50);
    cpu.SUB_M();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x40);
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, Realistic_Calculator_Sequence) {
    // Simula una secuencia de operaciones realista
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3000);
    
    // 0 + 25
    rom[0x3000] = 0x19; // 25 en decimal
    cpu.ADD_M();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x19);
    
    // 25 + 30
    rom[0x3001] = 0x1E; // 30 en decimal
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3001);
    cpu.ADD_M();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x37); // 55 en decimal
    
    // 55 - 15
    rom[0x3002] = 0x0F; // 15 en decimal
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3002);
    cpu.SUB_M();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x28); // 40 en decimal
}

// ==================== Tests para CMP M ====================

TEST_F(ADD_ADC_SUB_SBB_M_Test, CMP_M_Equal) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x50);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x50;
    
    uint8_t cycles = cpu.CMP_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x50); // CMP no modifica A
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, CMP_M_AGreaterThanM) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x60);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x30;
    
    uint8_t cycles = cpu.CMP_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x60); // CMP no modifica A
    EXPECT_EQ(cycles, 7);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, CMP_M_ALessThanM) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x30);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x60;
    
    uint8_t cycles = cpu.CMP_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x30); // CMP no modifica A
    EXPECT_EQ(cycles, 7);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, CMP_M_WithZero) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x00;
    
    uint8_t cycles = cpu.CMP_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, CMP_M_NegativeResult) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x05);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x0A;
    
    uint8_t cycles = cpu.CMP_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x05); // No cambia
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, CMP_M_ParityCheck) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x08);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x01;
    
    uint8_t cycles = cpu.CMP_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x08);
    EXPECT_EQ(cycles, 7);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P)); // 0x07 tiene 3 bits en 1 (impar)
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, CMP_M_MaxValue) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0xFF;
    
    uint8_t cycles = cpu.CMP_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, CMP_M_DoesNotModifyOtherRegisters) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x50);
    cpu.registers_m.setRegister(Registers::Register::B, 0xAA);
    cpu.registers_m.setRegister(Registers::Register::C, 0xBB);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x30;
    
    cpu.CMP_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x50);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0xAA);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0xBB);
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, CMP_M_PreservesHLRegister) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x50);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x30;
    
    cpu.CMP_M();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2000);
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, CMP_M_DoesNotModifyMemory) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x50);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x30;
    
    cpu.CMP_M();
    
    EXPECT_EQ(rom[0x2000], 0x30);
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, CMP_M_MultipleTimes) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x50);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x30;
    
    // Primera comparación
    cpu.CMP_M();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x50);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    
    // Segunda comparación - A no cambia
    cpu.CMP_M();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x50);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, CMP_M_AtDifferentAddresses) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x50);
    
    // Primera comparación en 0x1000
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1000);
    rom[0x1000] = 0x50;
    cpu.CMP_M();
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    
    // Segunda comparación en 0x2000
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x30;
    cpu.CMP_M();
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(ADD_ADC_SUB_SBB_M_Test, CMP_M_WithHalfBorrow) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x20);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x01;
    
    uint8_t cycles = cpu.CMP_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x20);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

