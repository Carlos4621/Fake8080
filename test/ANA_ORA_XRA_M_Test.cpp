#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"
#include <array>

class ANA_ORA_XRA_M_Test : public ::testing::Test {
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

// ==================== Tests para ANA M (AND lógico con memoria) ====================

TEST_F(ANA_ORA_XRA_M_Test, ANA_M_BasicAnd) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x0F;
    
    uint8_t cycles = cpu.ANA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x0F);
    EXPECT_EQ(cycles, 7);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC)); // ANA siempre pone AC=1
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY)); // ANA siempre pone CY=0
}

TEST_F(ANA_ORA_XRA_M_Test, ANA_M_ResultZero) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xF0);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x0F;
    
    uint8_t cycles = cpu.ANA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ANA_ORA_XRA_M_Test, ANA_M_SignBit) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x80;
    
    uint8_t cycles = cpu.ANA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x80);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ANA_ORA_XRA_M_Test, ANA_M_ParityEven) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x03; // 00000011 - 2 bits en 1 (par)
    
    uint8_t cycles = cpu.ANA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x03);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ANA_ORA_XRA_M_Test, ANA_M_ParityOdd) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x07; // 00000111 - 3 bits en 1 (impar)
    
    uint8_t cycles = cpu.ANA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x07);
    EXPECT_EQ(cycles, 7);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ANA_ORA_XRA_M_Test, ANA_M_AllBitsSet) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0xFF;
    
    uint8_t cycles = cpu.ANA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P)); // 8 bits en 1 (par)
}

TEST_F(ANA_ORA_XRA_M_Test, ANA_M_ClearsPreviousCarry) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x0F;
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.ANA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x0F);
    EXPECT_EQ(cycles, 7);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ANA_ORA_XRA_M_Test, ANA_M_SetsAuxiliaryCarry) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x0F;
    cpu.registers_m.setFlag(Registers::Flags::AC, false);
    
    uint8_t cycles = cpu.ANA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x0F);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(ANA_ORA_XRA_M_Test, ANA_M_BitMasking) {
    cpu.registers_m.setRegister(Registers::Register::A, 0b10110101);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0b00001111; // Máscara para los 4 bits inferiores
    
    cpu.ANA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b00000101);
}

TEST_F(ANA_ORA_XRA_M_Test, ANA_M_PreservesHLRegister) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x0F;
    
    cpu.ANA_M();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2000);
}

TEST_F(ANA_ORA_XRA_M_Test, ANA_M_DoesNotModifyMemory) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x0F;
    
    cpu.ANA_M();
    
    EXPECT_EQ(rom[0x2000], 0x0F);
}

// ==================== Tests para ORA M (OR lógico con memoria) ====================

TEST_F(ANA_ORA_XRA_M_Test, ORA_M_BasicOr) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xF0);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x0F;
    
    uint8_t cycles = cpu.ORA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(cycles, 7);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC)); // ORA siempre pone AC=0
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY)); // ORA siempre pone CY=0
}

TEST_F(ANA_ORA_XRA_M_Test, ORA_M_BothZero) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x00;
    
    uint8_t cycles = cpu.ORA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ANA_ORA_XRA_M_Test, ORA_M_SignBit) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x80;
    
    uint8_t cycles = cpu.ORA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x80);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ANA_ORA_XRA_M_Test, ORA_M_ParityEven) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x01);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x02; // Resultado: 0x03 - 2 bits en 1 (par)
    
    uint8_t cycles = cpu.ORA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x03);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ANA_ORA_XRA_M_Test, ORA_M_ParityOdd) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x03);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x04; // Resultado: 0x07 - 3 bits en 1 (impar)
    
    uint8_t cycles = cpu.ORA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x07);
    EXPECT_EQ(cycles, 7);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ANA_ORA_XRA_M_Test, ORA_M_AllBitsSet) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0xFF;
    
    uint8_t cycles = cpu.ORA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P)); // 8 bits en 1 (par)
}

TEST_F(ANA_ORA_XRA_M_Test, ORA_M_ClearsPreviousCarry) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xF0);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x0F;
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.ORA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(cycles, 7);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ANA_ORA_XRA_M_Test, ORA_M_ClearsAuxiliaryCarry) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xF0);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x0F;
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    
    uint8_t cycles = cpu.ORA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(cycles, 7);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(ANA_ORA_XRA_M_Test, ORA_M_BitSetting) {
    cpu.registers_m.setRegister(Registers::Register::A, 0b10100000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0b00000101; // Establecer bits 0 y 2
    
    cpu.ORA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b10100101);
}

TEST_F(ANA_ORA_XRA_M_Test, ORA_M_PreservesHLRegister) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xF0);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x0F;
    
    cpu.ORA_M();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2000);
}

TEST_F(ANA_ORA_XRA_M_Test, ORA_M_DoesNotModifyMemory) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xF0);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x0F;
    
    cpu.ORA_M();
    
    EXPECT_EQ(rom[0x2000], 0x0F);
}

// ==================== Tests para XRA M (XOR lógico con memoria) ====================

TEST_F(ANA_ORA_XRA_M_Test, XRA_M_BasicXor) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0xAA;
    
    uint8_t cycles = cpu.XRA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x55);
    EXPECT_EQ(cycles, 7);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC)); // XRA siempre pone AC=0
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY)); // XRA siempre pone CY=0
}

TEST_F(ANA_ORA_XRA_M_Test, XRA_M_IdenticalValues) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x55);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x55;
    
    uint8_t cycles = cpu.XRA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ANA_ORA_XRA_M_Test, XRA_M_SignBit) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x7F);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0xFF;
    
    uint8_t cycles = cpu.XRA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x80);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ANA_ORA_XRA_M_Test, XRA_M_ParityEven) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x01);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x02; // Resultado: 0x03 - 2 bits en 1 (par)
    
    uint8_t cycles = cpu.XRA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x03);
    EXPECT_EQ(cycles, 7);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ANA_ORA_XRA_M_Test, XRA_M_ParityOdd) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x03);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x04; // Resultado: 0x07 - 3 bits en 1 (impar)
    
    uint8_t cycles = cpu.XRA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x07);
    EXPECT_EQ(cycles, 7);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ANA_ORA_XRA_M_Test, XRA_M_InvertBits) {
    cpu.registers_m.setRegister(Registers::Register::A, 0b10101010);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0xFF;
    
    cpu.XRA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b01010101);
}

TEST_F(ANA_ORA_XRA_M_Test, XRA_M_ClearsPreviousCarry) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0xAA;
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.XRA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x55);
    EXPECT_EQ(cycles, 7);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ANA_ORA_XRA_M_Test, XRA_M_ClearsAuxiliaryCarry) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0xAA;
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    
    uint8_t cycles = cpu.XRA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x55);
    EXPECT_EQ(cycles, 7);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(ANA_ORA_XRA_M_Test, XRA_M_BitToggling) {
    cpu.registers_m.setRegister(Registers::Register::A, 0b10101010);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0b00001111; // Toggle los 4 bits inferiores
    
    cpu.XRA_M();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b10100101);
}

TEST_F(ANA_ORA_XRA_M_Test, XRA_M_PreservesHLRegister) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0xAA;
    
    cpu.XRA_M();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2000);
}

TEST_F(ANA_ORA_XRA_M_Test, XRA_M_DoesNotModifyMemory) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0xAA;
    
    cpu.XRA_M();
    
    EXPECT_EQ(rom[0x2000], 0xAA);
}

// ==================== Tests de casos prácticos ====================

TEST_F(ANA_ORA_XRA_M_Test, SequentialLogicOperations) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    
    // ANA M - Aplicar máscara
    rom[0x2000] = 0x0F;
    cpu.ANA_M();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x0F);
    
    // ORA M - Establecer bits adicionales
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2001);
    rom[0x2001] = 0xF0;
    cpu.ORA_M();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    
    // XRA M - Toggle bits
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2002);
    rom[0x2002] = 0xAA;
    cpu.XRA_M();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x55);
}

TEST_F(ANA_ORA_XRA_M_Test, AllOperations_DoNotModifyMemory) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x55;
    
    cpu.ANA_M();
    EXPECT_EQ(rom[0x2000], 0x55);
    
    cpu.ORA_M();
    EXPECT_EQ(rom[0x2000], 0x55);
    
    cpu.XRA_M();
    EXPECT_EQ(rom[0x2000], 0x55);
}

TEST_F(ANA_ORA_XRA_M_Test, AllOperations_WithHighAddresses) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xFFFF);
    rom[0xFFFF] = 0x55;
    
    cpu.ANA_M();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x55);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.ORA_M();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x55);
    
    cpu.XRA_M();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
}

TEST_F(ANA_ORA_XRA_M_Test, Realistic_BitManipulation_Sequence) {
    // Simula una secuencia realista de manipulación de bits
    cpu.registers_m.setRegister(Registers::Register::A, 0b11110000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3000);
    
    // Paso 1: Enmascarar para obtener solo los bits superiores
    rom[0x3000] = 0b11110000;
    cpu.ANA_M();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b11110000);
    
    // Paso 2: Establecer bits adicionales en los bits inferiores
    rom[0x3001] = 0b00001010;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3001);
    cpu.ORA_M();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b11111010);
    
    // Paso 3: Toggle bits específicos
    rom[0x3002] = 0b00000101;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3002);
    cpu.XRA_M();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b11111111);
}
