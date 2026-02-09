#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"
#include <array>

class INR_DCR_M_Test : public ::testing::Test {
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

// ==================== Tests para INR M (Increment Memory) ====================

TEST_F(INR_DCR_M_Test, INR_M_BasicIncrement) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x10;
    
    uint8_t cycles = cpu.INR_M();
    
    EXPECT_EQ(rom[0x2000], 0x11);
    EXPECT_EQ(cycles, 10);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(INR_DCR_M_Test, INR_M_FromZero) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x00;
    
    uint8_t cycles = cpu.INR_M();
    
    EXPECT_EQ(rom[0x2000], 0x01);
    EXPECT_EQ(cycles, 10);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(INR_DCR_M_Test, INR_M_HalfCarry) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x0F;
    
    uint8_t cycles = cpu.INR_M();
    
    EXPECT_EQ(rom[0x2000], 0x10);
    EXPECT_EQ(cycles, 10);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(INR_DCR_M_Test, INR_M_OverflowTo0x80_SignFlag) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x7F;
    
    uint8_t cycles = cpu.INR_M();
    
    EXPECT_EQ(rom[0x2000], 0x80);
    EXPECT_EQ(cycles, 10);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(INR_DCR_M_Test, INR_M_OverflowToZero_ZeroAndParityFlags) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0xFF;
    
    uint8_t cycles = cpu.INR_M();
    
    EXPECT_EQ(rom[0x2000], 0x00);
    EXPECT_EQ(cycles, 10);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(INR_DCR_M_Test, INR_M_ParityFlagEven) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x02; // Resultado: 0x03 - 2 bits en 1 (par)
    
    uint8_t cycles = cpu.INR_M();
    
    EXPECT_EQ(rom[0x2000], 0x03);
    EXPECT_EQ(cycles, 10);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(INR_DCR_M_Test, INR_M_ParityFlagOdd) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x06; // Resultado: 0x07 - 3 bits en 1 (impar)
    
    uint8_t cycles = cpu.INR_M();
    
    EXPECT_EQ(rom[0x2000], 0x07);
    EXPECT_EQ(cycles, 10);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(INR_DCR_M_Test, INR_M_DoesNotAffectCarryFlag_CarryClear) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x10;
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    uint8_t cycles = cpu.INR_M();
    
    EXPECT_EQ(rom[0x2000], 0x11);
    EXPECT_EQ(cycles, 10);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(INR_DCR_M_Test, INR_M_DoesNotAffectCarryFlag_CarrySet) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x10;
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.INR_M();
    
    EXPECT_EQ(rom[0x2000], 0x11);
    EXPECT_EQ(cycles, 10);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(INR_DCR_M_Test, INR_M_MultipleHalfCarries) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x0F;
    
    cpu.INR_M();
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
    
    rom[0x2000] = 0x1F;
    cpu.INR_M();
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(INR_DCR_M_Test, INR_M_PreservesHLRegister) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x10;
    
    cpu.INR_M();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2000);
}

TEST_F(INR_DCR_M_Test, INR_M_AtDifferentAddresses) {
    // Incrementar en 0x1000
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1000);
    rom[0x1000] = 0x10;
    cpu.INR_M();
    EXPECT_EQ(rom[0x1000], 0x11);
    
    // Incrementar en 0x3000
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3000);
    rom[0x3000] = 0x20;
    cpu.INR_M();
    EXPECT_EQ(rom[0x3000], 0x21);
}

// ==================== Tests para DCR M (Decrement Memory) ====================

TEST_F(INR_DCR_M_Test, DCR_M_BasicDecrement) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x10;
    
    uint8_t cycles = cpu.DCR_M();
    
    EXPECT_EQ(rom[0x2000], 0x0F);
    EXPECT_EQ(cycles, 10);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(INR_DCR_M_Test, DCR_M_ToZero) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x01;
    
    uint8_t cycles = cpu.DCR_M();
    
    EXPECT_EQ(rom[0x2000], 0x00);
    EXPECT_EQ(cycles, 10);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(INR_DCR_M_Test, DCR_M_HalfBorrow) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x10;
    
    uint8_t cycles = cpu.DCR_M();
    
    EXPECT_EQ(rom[0x2000], 0x0F);
    EXPECT_EQ(cycles, 10);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(INR_DCR_M_Test, DCR_M_UnderflowFrom0x00) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x00;
    
    uint8_t cycles = cpu.DCR_M();
    
    EXPECT_EQ(rom[0x2000], 0xFF);
    EXPECT_EQ(cycles, 10);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(INR_DCR_M_Test, DCR_M_From0x80_SignFlagClears) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x80;
    
    uint8_t cycles = cpu.DCR_M();
    
    EXPECT_EQ(rom[0x2000], 0x7F);
    EXPECT_EQ(cycles, 10);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(INR_DCR_M_Test, DCR_M_ParityCheck) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x04; // Resultado: 0x03 - 2 bits en 1 (par)
    
    uint8_t cycles = cpu.DCR_M();
    
    EXPECT_EQ(rom[0x2000], 0x03);
    EXPECT_EQ(cycles, 10);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(INR_DCR_M_Test, DCR_M_MultipleDecrements) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x05;
    
    cpu.DCR_M();
    EXPECT_EQ(rom[0x2000], 0x04);
    
    cpu.DCR_M();
    EXPECT_EQ(rom[0x2000], 0x03);
    
    cpu.DCR_M();
    EXPECT_EQ(rom[0x2000], 0x02);
}

TEST_F(INR_DCR_M_Test, DCR_M_DoesNotAffectCarryFlag_CarryClear) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x10;
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    uint8_t cycles = cpu.DCR_M();
    
    EXPECT_EQ(rom[0x2000], 0x0F);
    EXPECT_EQ(cycles, 10);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(INR_DCR_M_Test, DCR_M_DoesNotAffectCarryFlag_CarrySet) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x10;
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.DCR_M();
    
    EXPECT_EQ(rom[0x2000], 0x0F);
    EXPECT_EQ(cycles, 10);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(INR_DCR_M_Test, DCR_M_PreservesHLRegister) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x10;
    
    cpu.DCR_M();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2000);
}

TEST_F(INR_DCR_M_Test, DCR_M_AtDifferentAddresses) {
    // Decrementar en 0x1000
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1000);
    rom[0x1000] = 0x10;
    cpu.DCR_M();
    EXPECT_EQ(rom[0x1000], 0x0F);
    
    // Decrementar en 0x3000
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3000);
    rom[0x3000] = 0x20;
    cpu.DCR_M();
    EXPECT_EQ(rom[0x3000], 0x1F);
}

// ==================== Tests de casos prácticos ====================

TEST_F(INR_DCR_M_Test, LoopCounter_INR_M_Until_Zero) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0xFD; // Cerca del overflow
    
    cpu.INR_M();
    EXPECT_EQ(rom[0x2000], 0xFE);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    
    cpu.INR_M();
    EXPECT_EQ(rom[0x2000], 0xFF);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    
    cpu.INR_M();
    EXPECT_EQ(rom[0x2000], 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(INR_DCR_M_Test, LoopCounter_DCR_M_Until_Zero) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x03;
    
    cpu.DCR_M();
    EXPECT_EQ(rom[0x2000], 0x02);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    
    cpu.DCR_M();
    EXPECT_EQ(rom[0x2000], 0x01);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    
    cpu.DCR_M();
    EXPECT_EQ(rom[0x2000], 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(INR_DCR_M_Test, ArrayIndexing_INR_M) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3000);
    rom[0x3000] = 0x00; // Índice inicial
    
    // Incrementar índice para acceder a diferentes elementos
    for (int i = 0; i < 5; i++) {
        cpu.INR_M();
    }
    
    EXPECT_EQ(rom[0x3000], 0x05);
}

TEST_F(INR_DCR_M_Test, ByteManipulation_INR_DCR_Sequence) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x10;
    
    // Incrementar
    cpu.INR_M();
    EXPECT_EQ(rom[0x2000], 0x11);
    
    cpu.INR_M();
    EXPECT_EQ(rom[0x2000], 0x12);
    
    // Decrementar
    cpu.DCR_M();
    EXPECT_EQ(rom[0x2000], 0x11);
    
    cpu.DCR_M();
    EXPECT_EQ(rom[0x2000], 0x10);
}

TEST_F(INR_DCR_M_Test, BoundaryCondition_0x7F_to_0x80) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x7F;
    
    cpu.INR_M();
    
    EXPECT_EQ(rom[0x2000], 0x80);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(INR_DCR_M_Test, BoundaryCondition_0x80_to_0x7F) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x80;
    
    cpu.DCR_M();
    
    EXPECT_EQ(rom[0x2000], 0x7F);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(INR_DCR_M_Test, CarryFlag_Preservation_Complex) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0xFF;
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.INR_M(); // Overflow a 0x00, pero CY debe mantenerse
    EXPECT_EQ(rom[0x2000], 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    cpu.DCR_M(); // Underflow a 0xFF, CY debe mantenerse
    EXPECT_EQ(rom[0x2000], 0xFF);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(INR_DCR_M_Test, Sequential_Operations_On_Same_Memory) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x50;
    
    // 5 incrementos
    for (int i = 0; i < 5; i++) {
        cpu.INR_M();
    }
    EXPECT_EQ(rom[0x2000], 0x55);
    
    // 3 decrementos
    for (int i = 0; i < 3; i++) {
        cpu.DCR_M();
    }
    EXPECT_EQ(rom[0x2000], 0x52);
}

TEST_F(INR_DCR_M_Test, HighAddress_Operations) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xFFFF);
    rom[0xFFFF] = 0x10;
    
    cpu.INR_M();
    EXPECT_EQ(rom[0xFFFF], 0x11);
    
    cpu.DCR_M();
    EXPECT_EQ(rom[0xFFFF], 0x10);
}
