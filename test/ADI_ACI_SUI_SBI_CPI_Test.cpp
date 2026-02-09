#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"
#include <array>

class ADI_ACI_SUI_SBI_CPI_Test : public ::testing::Test {
protected:
    CPUTest cpu;
    std::array<uint8_t, 0x10000> rom;  // 64KB ROM

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

// ==================== Tests para ADI (Add Immediate) ====================

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, ADI_BasicAddition) {
    rom[0] = 0x10; // ADI 0x10
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x20);
    
    uint8_t cycles = cpu.ADI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x30);
    EXPECT_EQ(cycles, 7);
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, ADI_ZeroResult) {
    rom[0] = 0x00;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    
    cpu.ADI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, ADI_CarryFlag) {
    rom[0] = 0x01;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    
    cpu.ADI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, ADI_SignFlag) {
    rom[0] = 0x01;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x7F);
    
    cpu.ADI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x80);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, ADI_AuxiliaryCarry) {
    rom[0] = 0x0F;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x0F);
    
    cpu.ADI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x1E);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, ADI_ParityFlag) {
    rom[0] = 0x02;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x01);
    
    cpu.ADI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x03);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P)); // 3 = 0b00000011, parity even
}

// ==================== Tests para ACI (Add with Carry Immediate) ====================

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, ACI_BasicAdditionNoCarry) {
    rom[0] = 0x10;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x20);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    uint8_t cycles = cpu.ACI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x30);
    EXPECT_EQ(cycles, 7);
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, ACI_BasicAdditionWithCarry) {
    rom[0] = 0x10;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x20);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.ACI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x31);
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, ACI_CarryPropagation) {
    rom[0] = 0xFE;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x01);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.ACI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, ACI_CarryFromPrevious) {
    rom[0] = 0xFF;
    rom[1] = 0x00;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x01);
    cpu.ADI_d8(); // A = 0x00, CY = 1
    
    cpu.ACI_d8(); // A = 0x00 + 0x00 + 1 = 0x01
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x01);
}

// ==================== Tests para SUI (Subtract Immediate) ====================

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, SUI_BasicSubtraction) {
    rom[0] = 0x10;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x30);
    
    uint8_t cycles = cpu.SBI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x20);
    EXPECT_EQ(cycles, 7);
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, SUI_ZeroResult) {
    rom[0] = 0x20;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x20);
    
    cpu.SBI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, SUI_BorrowFlag) {
    rom[0] = 0x10;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x05);
    
    cpu.SBI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xF5);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, SUI_SignFlag) {
    rom[0] = 0x7F;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    
    cpu.SBI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x80);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

// ==================== Tests para SBI (Subtract with Borrow Immediate) ====================

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, SBI_BasicSubtractionNoBorrow) {
    rom[0] = 0x10;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x30);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    uint8_t cycles = cpu.SCI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x20);
    EXPECT_EQ(cycles, 7);
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, SBI_BasicSubtractionWithBorrow) {
    rom[0] = 0x10;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x30);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.SCI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x1F);
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, SBI_BorrowPropagation) {
    rom[0] = 0x01;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.SCI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFE);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

// ==================== Tests para CPI (Compare Immediate) ====================

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, CPI_Equal) {
    rom[0] = 0x20;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x20);
    
    uint8_t cycles = cpu.CPI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x20); // A no cambia
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_EQ(cycles, 7);
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, CPI_Greater) {
    rom[0] = 0x10;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x20);
    
    cpu.CPI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x20); // A no cambia
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, CPI_Less) {
    rom[0] = 0x30;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x20);
    
    cpu.CPI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x20); // A no cambia
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, CPI_DoesNotModifyA) {
    rom[0] = 0x50;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    
    cpu.CPI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
}

// ==================== Tests de PC increment ====================

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, Sequential_PCIncrement) {
    rom[0] = 0x10; // ADI 0x10
    rom[1] = 0x05; // ACI 0x05
    rom[2] = 0x03; // SUI 0x03
    rom[3] = 0x02; // SBI 0x02
    rom[4] = 0x10; // CPI 0x10
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    
    cpu.ADI_d8(); // A = 0x10
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x10);
    
    cpu.ACI_d8(); // A = 0x15
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x15);
    
    cpu.SBI_d8(); // A = 0x12
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x12);
    
    cpu.SCI_d8(); // A = 0x10
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x10);
    
    cpu.CPI_d8(); // A = 0x10 (sin cambios)
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x10);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

// ==================== Tests de casos prácticos ====================

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, RealisticUseCase_AddConstant) {
    rom[0] = 0x05;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    
    cpu.ADI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x15);
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, RealisticUseCase_IncrementWithOverflow) {
    rom[0] = 0x01;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    
    cpu.ADI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, RealisticUseCase_CheckRange) {
    rom[0] = 0x0A;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x05);
    
    cpu.CPI_d8();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY)); // A < 0x0A
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, RealisticUseCase_MultiByteAddition) {
    // Simular suma de 16 bits: 0x00FF + 0x0002 = 0x0101
    rom[0] = 0x02; // ADI para byte bajo
    rom[1] = 0x00; // ACI para byte alto (con carry)
    cpu.setROM(rom);
    
    // Byte bajo
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.ADI_d8(); // A = 0x01, CY = 1
    uint8_t lowByte = cpu.registers_m.getRegister(Registers::Register::A);
    
    // Byte alto
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.ACI_d8(); // A = 0x01 (0x00 + 0x00 + carry)
    uint8_t highByte = cpu.registers_m.getRegister(Registers::Register::A);
    
    EXPECT_EQ(lowByte, 0x01);
    EXPECT_EQ(highByte, 0x01);
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, BoundaryCondition_MaxValue) {
    rom[0] = 0xFF;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    
    cpu.ADI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, BoundaryCondition_MinValue) {
    rom[0] = 0x00;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    
    cpu.SBI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, PreservesOtherRegisters) {
    rom[0] = 0x10;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x20);
    cpu.registers_m.setRegister(Registers::Register::B, 0xBB);
    cpu.registers_m.setRegister(Registers::Register::C, 0xCC);
    cpu.registers_m.setRegister(Registers::Register::D, 0xDD);
    cpu.registers_m.setRegister(Registers::Register::E, 0xEE);
    cpu.registers_m.setRegister(Registers::Register::H, 0x11);
    cpu.registers_m.setRegister(Registers::Register::L, 0x22);
    
    cpu.ADI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0xBB);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0xCC);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0xDD);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0xEE);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x11);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x22);
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, ADI_AuxiliaryCarryBoundary) {
    rom[0] = 0x01;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x0F);
    
    cpu.ADI_d8();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, SUI_AuxiliaryCarryBoundary) {
    rom[0] = 0x01;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    
    cpu.SBI_d8();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, CPI_WithZeroImmediate) {
    rom[0] = 0x00;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    
    cpu.CPI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, ACI_ChainedCarry) {
    rom[0] = 0x80;
    rom[1] = 0x80;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x80);
    cpu.ADI_d8(); // A = 0x00, CY = 1
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x80);
    cpu.ACI_d8(); // A = 0x01 (0x80 + 0x80 + 1)
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x01);
}

TEST_F(ADI_ACI_SUI_SBI_CPI_Test, SBI_ChainedBorrow) {
    rom[0] = 0x01;
    rom[1] = 0x01;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.SBI_d8(); // A = 0xFF, CY = 1
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    cpu.SCI_d8(); // A = 0x0E (0x10 - 0x01 - 1)
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x0E);
}
