#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"

class INR_DCR_Test : public ::testing::Test {
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

// ==================== Tests para INR (Increment) ====================

TEST_F(INR_DCR_Test, INR_B_BasicIncrement) {
    cpu.registers_m.setRegister(Registers::Register::B, 0x10);
    
    uint8_t cycles = cpu.INR_R<Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x11);
    EXPECT_EQ(cycles, 5);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(INR_DCR_Test, INR_C_FromZero) {
    cpu.registers_m.setRegister(Registers::Register::C, 0x00);
    
    uint8_t cycles = cpu.INR_R<Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x01);
    EXPECT_EQ(cycles, 5);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(INR_DCR_Test, INR_D_HalfCarry) {
    cpu.registers_m.setRegister(Registers::Register::D, 0x0F);
    
    uint8_t cycles = cpu.INR_R<Registers::Register::D>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x10);
    EXPECT_EQ(cycles, 5);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(INR_DCR_Test, INR_E_OverflowTo0x80_SignFlag) {
    cpu.registers_m.setRegister(Registers::Register::E, 0x7F);
    
    uint8_t cycles = cpu.INR_R<Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x80);
    EXPECT_EQ(cycles, 5);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(INR_DCR_Test, INR_H_OverflowToZero_ZeroAndParityFlags) {
    cpu.registers_m.setRegister(Registers::Register::H, 0xFF);
    
    uint8_t cycles = cpu.INR_R<Registers::Register::H>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x00);
    EXPECT_EQ(cycles, 5);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(INR_DCR_Test, INR_L_ParityFlagEven) {
    cpu.registers_m.setRegister(Registers::Register::L, 0x02);
    
    uint8_t cycles = cpu.INR_R<Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x03);
    EXPECT_EQ(cycles, 5);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(INR_DCR_Test, INR_A_ParityFlagOdd) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    
    uint8_t cycles = cpu.INR_R<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x01);
    EXPECT_EQ(cycles, 5);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(INR_DCR_Test, INR_DoesNotAffectCarryFlag_CarryClear) {
    cpu.registers_m.setRegister(Registers::Register::B, 0xFF);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.INR_R<Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x00);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY)); // Carry NO debe cambiar
}

TEST_F(INR_DCR_Test, INR_DoesNotAffectCarryFlag_CarrySet) {
    cpu.registers_m.setRegister(Registers::Register::C, 0xFF);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.INR_R<Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY)); // Carry NO debe cambiar
}

TEST_F(INR_DCR_Test, INR_MultipleHalfCarries) {
    cpu.registers_m.setRegister(Registers::Register::D, 0x1F);
    
    cpu.INR_R<Registers::Register::D>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x20);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
    
    cpu.registers_m.setRegister(Registers::Register::D, 0x2F);
    cpu.INR_R<Registers::Register::D>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x30);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

// ==================== Tests para DCR (Decrement) ====================

TEST_F(INR_DCR_Test, DCR_B_BasicDecrement) {
    cpu.registers_m.setRegister(Registers::Register::B, 0x10);
    
    uint8_t cycles = cpu.DCR_R<Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x0F);
    EXPECT_EQ(cycles, 5);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(INR_DCR_Test, DCR_C_ToZero) {
    cpu.registers_m.setRegister(Registers::Register::C, 0x01);
    
    uint8_t cycles = cpu.DCR_R<Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x00);
    EXPECT_EQ(cycles, 5);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(INR_DCR_Test, DCR_D_HalfBorrow) {
    cpu.registers_m.setRegister(Registers::Register::D, 0x10);
    
    uint8_t cycles = cpu.DCR_R<Registers::Register::D>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x0F);
    EXPECT_EQ(cycles, 5);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(INR_DCR_Test, DCR_E_UnderflowFrom0x00) {
    cpu.registers_m.setRegister(Registers::Register::E, 0x00);
    
    uint8_t cycles = cpu.DCR_R<Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0xFF);
    EXPECT_EQ(cycles, 5);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(INR_DCR_Test, DCR_H_From0x80_SignFlagClears) {
    cpu.registers_m.setRegister(Registers::Register::H, 0x80);
    
    uint8_t cycles = cpu.DCR_R<Registers::Register::H>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x7F);
    EXPECT_EQ(cycles, 5);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(INR_DCR_Test, DCR_L_ParityCheck) {
    cpu.registers_m.setRegister(Registers::Register::L, 0x04);
    
    uint8_t cycles = cpu.DCR_R<Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x03);
    EXPECT_EQ(cycles, 5);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(INR_DCR_Test, DCR_A_MultipleDecrements) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x05);
    
    cpu.DCR_R<Registers::Register::A>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x04);
    
    cpu.DCR_R<Registers::Register::A>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x03);
    
    cpu.DCR_R<Registers::Register::A>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x02);
}

TEST_F(INR_DCR_Test, DCR_DoesNotAffectCarryFlag_CarryClear) {
    cpu.registers_m.setRegister(Registers::Register::B, 0x00);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.DCR_R<Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0xFF);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY)); // Carry NO debe cambiar
}

TEST_F(INR_DCR_Test, DCR_DoesNotAffectCarryFlag_CarrySet) {
    cpu.registers_m.setRegister(Registers::Register::C, 0x00);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.DCR_R<Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0xFF);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY)); // Carry NO debe cambiar
}

// ==================== Tests de casos de uso típicos en programas 8080 ====================

TEST_F(INR_DCR_Test, LoopCounter_INR_Until_Zero) {
    // Patrón típico: contador de bucle con INR hasta overflow
    cpu.registers_m.setRegister(Registers::Register::B, 0xFD);
    
    cpu.INR_R<Registers::Register::B>(); // 0xFE
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    
    cpu.INR_R<Registers::Register::B>(); // 0xFF
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    
    cpu.INR_R<Registers::Register::B>(); // 0x00
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(INR_DCR_Test, LoopCounter_DCR_Until_Zero) {
    // Patrón típico: contador descendente de bucle
    cpu.registers_m.setRegister(Registers::Register::C, 0x03);
    
    cpu.DCR_R<Registers::Register::C>(); // 0x02
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    
    cpu.DCR_R<Registers::Register::C>(); // 0x01
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    
    cpu.DCR_R<Registers::Register::C>(); // 0x00
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(INR_DCR_Test, ArrayIndexing_INR) {
    // Patrón típico: indexación de array
    cpu.registers_m.setRegister(Registers::Register::L, 0x00);
    
    for (int i = 0; i < 10; ++i) {
        cpu.INR_R<Registers::Register::L>();
    }
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x0A);
}

TEST_F(INR_DCR_Test, ByteManipulation_INR_DCR_Sequence) {
    // Patrón típico: manipulación de bytes en operaciones de I/O
    cpu.registers_m.setRegister(Registers::Register::A, 0x50);
    
    cpu.INR_R<Registers::Register::A>();
    cpu.INR_R<Registers::Register::A>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x52);
    
    cpu.DCR_R<Registers::Register::A>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x51);
}

TEST_F(INR_DCR_Test, BoundaryCondition_0x7F_to_0x80) {
    // Importante para detección de overflow con signo
    cpu.registers_m.setRegister(Registers::Register::D, 0x7F);
    
    cpu.INR_R<Registers::Register::D>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x80);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(INR_DCR_Test, BoundaryCondition_0x80_to_0x7F) {
    // Importante para detección de underflow con signo
    cpu.registers_m.setRegister(Registers::Register::E, 0x80);
    
    cpu.DCR_R<Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x7F);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(INR_DCR_Test, CarryFlag_Preservation_Complex) {
    // Verificar que Carry se preserva en operaciones complejas
    cpu.registers_m.setRegister(Registers::Register::A, 0x80);
    cpu.registers_m.setRegister(Registers::Register::B, 0x80);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    // Simular: ADD A, B (pondría CY=1), luego INR C
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.registers_m.setRegister(Registers::Register::C, 0xFF);
    
    cpu.INR_R<Registers::Register::C>();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY)); // Debe preservarse
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x00);
}

TEST_F(INR_DCR_Test, AllRegisters_INR) {
    // Verificar que INR funciona en todos los registros
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    cpu.registers_m.setRegister(Registers::Register::B, 0x20);
    cpu.registers_m.setRegister(Registers::Register::C, 0x30);
    cpu.registers_m.setRegister(Registers::Register::D, 0x40);
    cpu.registers_m.setRegister(Registers::Register::E, 0x50);
    cpu.registers_m.setRegister(Registers::Register::H, 0x60);
    cpu.registers_m.setRegister(Registers::Register::L, 0x70);
    
    cpu.INR_R<Registers::Register::A>();
    cpu.INR_R<Registers::Register::B>();
    cpu.INR_R<Registers::Register::C>();
    cpu.INR_R<Registers::Register::D>();
    cpu.INR_R<Registers::Register::E>();
    cpu.INR_R<Registers::Register::H>();
    cpu.INR_R<Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x11);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x21);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x31);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x41);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x51);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x61);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x71);
}

TEST_F(INR_DCR_Test, AllRegisters_DCR) {
    // Verificar que DCR funciona en todos los registros
    cpu.registers_m.setRegister(Registers::Register::A, 0x11);
    cpu.registers_m.setRegister(Registers::Register::B, 0x22);
    cpu.registers_m.setRegister(Registers::Register::C, 0x33);
    cpu.registers_m.setRegister(Registers::Register::D, 0x44);
    cpu.registers_m.setRegister(Registers::Register::E, 0x55);
    cpu.registers_m.setRegister(Registers::Register::H, 0x66);
    cpu.registers_m.setRegister(Registers::Register::L, 0x77);
    
    cpu.DCR_R<Registers::Register::A>();
    cpu.DCR_R<Registers::Register::B>();
    cpu.DCR_R<Registers::Register::C>();
    cpu.DCR_R<Registers::Register::D>();
    cpu.DCR_R<Registers::Register::E>();
    cpu.DCR_R<Registers::Register::H>();
    cpu.DCR_R<Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x10);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x21);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x32);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x43);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x54);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x65);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x76);
}
