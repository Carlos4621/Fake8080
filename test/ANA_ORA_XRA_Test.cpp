#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"

class ANA_ORA_XRA_Test : public ::testing::Test {
protected:
    CPUTest cpu;

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
    }
};

// ==================== Tests para ANA (AND lógico) ====================

TEST_F(ANA_ORA_XRA_Test, ANA_B_BasicAnd) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setRegister(Registers::Register::B, 0x0F);
    
    uint8_t cycles = cpu.ANA_R<Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x0F);
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC)); // ANA siempre pone AC=1
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY)); // ANA siempre pone CY=0
}

TEST_F(ANA_ORA_XRA_Test, ANA_C_ResultZero) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xF0);
    cpu.registers_m.setRegister(Registers::Register::C, 0x0F);
    
    uint8_t cycles = cpu.ANA_R<Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ANA_ORA_XRA_Test, ANA_D_SignBit) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setRegister(Registers::Register::D, 0x80);
    
    uint8_t cycles = cpu.ANA_R<Registers::Register::D>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x80);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ANA_ORA_XRA_Test, ANA_E_ParityEven) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setRegister(Registers::Register::E, 0x03);
    
    uint8_t cycles = cpu.ANA_R<Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x03);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ANA_ORA_XRA_Test, ANA_H_ParityOdd) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setRegister(Registers::Register::H, 0x01);
    
    uint8_t cycles = cpu.ANA_R<Registers::Register::H>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x01);
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ANA_ORA_XRA_Test, ANA_L_AllBitsSet) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setRegister(Registers::Register::L, 0xFF);
    
    uint8_t cycles = cpu.ANA_R<Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(ANA_ORA_XRA_Test, ANA_A_WithItself) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x55);
    
    uint8_t cycles = cpu.ANA_R<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x55);
    EXPECT_EQ(cycles, 4);
}

TEST_F(ANA_ORA_XRA_Test, ANA_ClearsPreviousCarry) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setRegister(Registers::Register::B, 0xFF);
    cpu.registers_m.setFlag(Registers::Flags::CY, true); // CY previamente establecido
    
    cpu.ANA_R<Registers::Register::B>();
    
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY)); // ANA siempre limpia CY
}

TEST_F(ANA_ORA_XRA_Test, ANA_SetsAuxiliaryCarry) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x0F);
    cpu.registers_m.setRegister(Registers::Register::C, 0xF0);
    cpu.registers_m.setFlag(Registers::Flags::AC, false);
    
    cpu.ANA_R<Registers::Register::C>();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC)); // ANA siempre establece AC
}

// ==================== Tests para ORA (OR lógico) ====================

TEST_F(ANA_ORA_XRA_Test, ORA_B_BasicOr) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xF0);
    cpu.registers_m.setRegister(Registers::Register::B, 0x0F);
    
    uint8_t cycles = cpu.ORA_R<Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC)); // ORA siempre pone AC=0
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY)); // ORA siempre pone CY=0
}

TEST_F(ANA_ORA_XRA_Test, ORA_C_BothZero) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setRegister(Registers::Register::C, 0x00);
    
    uint8_t cycles = cpu.ORA_R<Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ANA_ORA_XRA_Test, ORA_D_SignBit) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setRegister(Registers::Register::D, 0x80);
    
    uint8_t cycles = cpu.ORA_R<Registers::Register::D>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x80);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ANA_ORA_XRA_Test, ORA_E_ParityEven) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x01);
    cpu.registers_m.setRegister(Registers::Register::E, 0x02);
    
    uint8_t cycles = cpu.ORA_R<Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x03);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ANA_ORA_XRA_Test, ORA_H_ParityOdd) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setRegister(Registers::Register::H, 0x01);
    
    uint8_t cycles = cpu.ORA_R<Registers::Register::H>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x01);
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ANA_ORA_XRA_Test, ORA_L_AllBitsSet) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xAA);
    cpu.registers_m.setRegister(Registers::Register::L, 0x55);
    
    uint8_t cycles = cpu.ORA_R<Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(cycles, 4);
}

TEST_F(ANA_ORA_XRA_Test, ORA_A_WithItself) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x42);
    
    uint8_t cycles = cpu.ORA_R<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x42);
    EXPECT_EQ(cycles, 4);
}

TEST_F(ANA_ORA_XRA_Test, ORA_ClearsPreviousCarry) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x01);
    cpu.registers_m.setRegister(Registers::Register::B, 0x02);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.ORA_R<Registers::Register::B>();
    
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY)); // ORA siempre limpia CY
}

TEST_F(ANA_ORA_XRA_Test, ORA_ClearsAuxiliaryCarry) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x01);
    cpu.registers_m.setRegister(Registers::Register::C, 0x02);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    
    cpu.ORA_R<Registers::Register::C>();
    
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC)); // ORA siempre limpia AC
}

// ==================== Tests para XRA (XOR lógico) ====================

TEST_F(ANA_ORA_XRA_Test, XRA_B_BasicXor) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setRegister(Registers::Register::B, 0x0F);
    
    uint8_t cycles = cpu.XRA_R<Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xF0);
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC)); // XRA siempre pone AC=0
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY)); // XRA siempre pone CY=0
}

TEST_F(ANA_ORA_XRA_Test, XRA_C_IdenticalValues) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x55);
    cpu.registers_m.setRegister(Registers::Register::C, 0x55);
    
    uint8_t cycles = cpu.XRA_R<Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ANA_ORA_XRA_Test, XRA_A_ClearRegister) {
    // Patrón típico en 8080: XRA A para limpiar A
    cpu.registers_m.setRegister(Registers::Register::A, 0x99);
    
    uint8_t cycles = cpu.XRA_R<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ANA_ORA_XRA_Test, XRA_D_SignBit) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setRegister(Registers::Register::D, 0x7F);
    
    uint8_t cycles = cpu.XRA_R<Registers::Register::D>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x80);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ANA_ORA_XRA_Test, XRA_E_ParityEven) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x01);
    cpu.registers_m.setRegister(Registers::Register::E, 0x02);
    
    uint8_t cycles = cpu.XRA_R<Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x03);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ANA_ORA_XRA_Test, XRA_H_ParityOdd) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setRegister(Registers::Register::H, 0x01);
    
    uint8_t cycles = cpu.XRA_R<Registers::Register::H>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x01);
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ANA_ORA_XRA_Test, XRA_L_InvertBits) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xAA);
    cpu.registers_m.setRegister(Registers::Register::L, 0xFF);
    
    uint8_t cycles = cpu.XRA_R<Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x55);
    EXPECT_EQ(cycles, 4);
}

TEST_F(ANA_ORA_XRA_Test, XRA_ClearsPreviousCarry) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x01);
    cpu.registers_m.setRegister(Registers::Register::B, 0x02);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.XRA_R<Registers::Register::B>();
    
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY)); // XRA siempre limpia CY
}

TEST_F(ANA_ORA_XRA_Test, XRA_ClearsAuxiliaryCarry) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x01);
    cpu.registers_m.setRegister(Registers::Register::C, 0x02);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    
    cpu.XRA_R<Registers::Register::C>();
    
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC)); // XRA siempre limpia AC
}

// ==================== Tests de casos típicos en programas 8080 ====================

TEST_F(ANA_ORA_XRA_Test, BitMasking_ANA) {
    // Patrón típico: máscara de bits para aislar nibble bajo
    cpu.registers_m.setRegister(Registers::Register::A, 0xAB);
    cpu.registers_m.setRegister(Registers::Register::B, 0x0F);
    
    cpu.ANA_R<Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x0B);
}

TEST_F(ANA_ORA_XRA_Test, BitSetting_ORA) {
    // Patrón típico: establecer bits específicos
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setRegister(Registers::Register::C, 0x80);
    
    cpu.ORA_R<Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x80);
}

TEST_F(ANA_ORA_XRA_Test, BitToggling_XRA) {
    // Patrón típico: invertir bits específicos
    cpu.registers_m.setRegister(Registers::Register::A, 0xAA);
    cpu.registers_m.setRegister(Registers::Register::D, 0xFF);
    
    cpu.XRA_R<Registers::Register::D>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x55);
}

TEST_F(ANA_ORA_XRA_Test, ZeroTest_ANA) {
    // Patrón típico: probar si hay bits comunes
    cpu.registers_m.setRegister(Registers::Register::A, 0x0F);
    cpu.registers_m.setRegister(Registers::Register::E, 0xF0);
    
    cpu.ANA_R<Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(ANA_ORA_XRA_Test, CombiningFlags_ORA) {
    // Patrón típico: combinar flags de estado
    cpu.registers_m.setRegister(Registers::Register::A, 0x01);
    cpu.registers_m.setRegister(Registers::Register::H, 0x04);
    
    cpu.ORA_R<Registers::Register::H>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x05);
}

TEST_F(ANA_ORA_XRA_Test, CompareRegisters_XRA) {
    // Patrón típico: comparar si dos registros son iguales
    cpu.registers_m.setRegister(Registers::Register::A, 0x42);
    cpu.registers_m.setRegister(Registers::Register::L, 0x42);
    
    cpu.XRA_R<Registers::Register::L>();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z)); // Z=1 significa iguales
}

TEST_F(ANA_ORA_XRA_Test, SequentialLogicOperations) {
    // Operaciones lógicas secuenciales
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setRegister(Registers::Register::B, 0xF0);
    cpu.registers_m.setRegister(Registers::Register::C, 0x0F);
    
    cpu.ANA_R<Registers::Register::B>(); // A = 0xF0
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xF0);
    
    cpu.ORA_R<Registers::Register::C>(); // A = 0xFF
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
}

TEST_F(ANA_ORA_XRA_Test, AllRegisters_ANA) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setRegister(Registers::Register::B, 0x01);
    cpu.registers_m.setRegister(Registers::Register::C, 0x02);
    cpu.registers_m.setRegister(Registers::Register::D, 0x04);
    cpu.registers_m.setRegister(Registers::Register::E, 0x08);
    cpu.registers_m.setRegister(Registers::Register::H, 0x10);
    cpu.registers_m.setRegister(Registers::Register::L, 0x20);
    
    cpu.ANA_R<Registers::Register::B>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x01);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.ANA_R<Registers::Register::C>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x02);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.ANA_R<Registers::Register::D>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x04);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.ANA_R<Registers::Register::E>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x08);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.ANA_R<Registers::Register::H>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x10);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.ANA_R<Registers::Register::L>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x20);
}

TEST_F(ANA_ORA_XRA_Test, AllRegisters_ORA) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setRegister(Registers::Register::B, 0x01);
    
    cpu.ORA_R<Registers::Register::B>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x01);
    
    cpu.registers_m.setRegister(Registers::Register::C, 0x02);
    cpu.ORA_R<Registers::Register::C>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x03);
}

TEST_F(ANA_ORA_XRA_Test, AllRegisters_XRA) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setRegister(Registers::Register::B, 0x01);
    
    cpu.XRA_R<Registers::Register::B>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFE);
    
    cpu.registers_m.setRegister(Registers::Register::C, 0x02);
    cpu.XRA_R<Registers::Register::C>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFC);
}
