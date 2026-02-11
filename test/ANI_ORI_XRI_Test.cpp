#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"
#include <array>

class ANI_ORI_XRI_Test : public ::testing::Test {
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

// ==================== Tests para ANI (AND Immediate) ====================

TEST_F(ANI_ORI_XRI_Test, ANI_BasicAnd) {
    rom[0] = 0x0F; // ANI 0x0F
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    
    uint8_t cycles = cpu.ANI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x0F);
    EXPECT_EQ(cycles, 7);
}

TEST_F(ANI_ORI_XRI_Test, ANI_ZeroResult) {
    rom[0] = 0x00;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    
    cpu.ANI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ANI_ORI_XRI_Test, ANI_AllOnes) {
    rom[0] = 0xFF;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    
    cpu.ANI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ANI_ORI_XRI_Test, ANI_MaskLowNibble) {
    rom[0] = 0x0F;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xAB);
    
    cpu.ANI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x0B);
}

TEST_F(ANI_ORI_XRI_Test, ANI_MaskHighNibble) {
    rom[0] = 0xF0;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xAB);
    
    cpu.ANI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xA0);
}

TEST_F(ANI_ORI_XRI_Test, ANI_ClearCarry) {
    rom[0] = 0xFF;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.ANI_d8();
    
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ANI_ORI_XRI_Test, ANI_ParityEven) {
    rom[0] = 0x03;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x03);
    
    cpu.ANI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x03);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ANI_ORI_XRI_Test, ANI_SignFlag) {
    rom[0] = 0x80;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    
    cpu.ANI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x80);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

// ==================== Tests para ORI (OR Immediate) ====================

TEST_F(ANI_ORI_XRI_Test, ORI_BasicOr) {
    rom[0] = 0x0F;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xF0);
    
    uint8_t cycles = cpu.ORI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(cycles, 7);
}

TEST_F(ANI_ORI_XRI_Test, ORI_ZeroWithZero) {
    rom[0] = 0x00;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    
    cpu.ORI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(ANI_ORI_XRI_Test, ORI_SetBits) {
    rom[0] = 0x0F;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    
    cpu.ORI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x0F);
}

TEST_F(ANI_ORI_XRI_Test, ORI_AllOnes) {
    rom[0] = 0xFF;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    
    cpu.ORI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ANI_ORI_XRI_Test, ORI_ClearCarry) {
    rom[0] = 0x00;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.ORI_d8();
    
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ANI_ORI_XRI_Test, ORI_SetLowNibble) {
    rom[0] = 0x0F;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xA0);
    
    cpu.ORI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xAF);
}

TEST_F(ANI_ORI_XRI_Test, ORI_SetHighNibble) {
    rom[0] = 0xF0;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x0A);
    
    cpu.ORI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFA);
}

TEST_F(ANI_ORI_XRI_Test, ORI_ParityFlag) {
    rom[0] = 0x02;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x01);
    
    cpu.ORI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x03);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

// ==================== Tests para XRI (XOR Immediate) ====================

TEST_F(ANI_ORI_XRI_Test, XRI_BasicXor) {
    rom[0] = 0xFF;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xAA);
    
    uint8_t cycles = cpu.XRI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x55);
    EXPECT_EQ(cycles, 7);
}

TEST_F(ANI_ORI_XRI_Test, XRI_WithZero) {
    rom[0] = 0x00;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xAB);
    
    cpu.XRI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xAB);
}

TEST_F(ANI_ORI_XRI_Test, XRI_WithSelf) {
    rom[0] = 0xAB;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xAB);
    
    cpu.XRI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ANI_ORI_XRI_Test, XRI_ToggleBits) {
    rom[0] = 0x0F;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xF0);
    
    cpu.XRI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
}

TEST_F(ANI_ORI_XRI_Test, XRI_ClearCarry) {
    rom[0] = 0x00;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.XRI_d8();
    
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ANI_ORI_XRI_Test, XRI_InvertAllBits) {
    rom[0] = 0xFF;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    
    cpu.XRI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ANI_ORI_XRI_Test, XRI_SignFlag) {
    rom[0] = 0x7F;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    
    cpu.XRI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x80);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ANI_ORI_XRI_Test, XRI_ParityFlag) {
    rom[0] = 0x01;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x02);
    
    cpu.XRI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x03);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

// ==================== Tests de PC increment ====================

TEST_F(ANI_ORI_XRI_Test, Sequential_PCIncrement) {
    rom[0] = 0x0F; // ANI 0x0F
    rom[1] = 0xF0; // ORI 0xF0
    rom[2] = 0xFF; // XRI 0xFF
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    
    cpu.ANI_d8(); // A = 0x0F
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x0F);
    
    cpu.ORI_d8(); // A = 0xFF
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    
    cpu.XRI_d8(); // A = 0x00
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

// ==================== Tests de casos prácticos ====================

TEST_F(ANI_ORI_XRI_Test, RealisticUseCase_MaskBit) {
    // Enmascarar bit 7 para comprobar signo
    rom[0] = 0x80;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x85);
    
    cpu.ANI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x80);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ANI_ORI_XRI_Test, RealisticUseCase_SetFlag) {
    // Establecer bits de control
    rom[0] = 0x01;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    
    cpu.ORI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x01);
}

TEST_F(ANI_ORI_XRI_Test, RealisticUseCase_ToggleBit) {
    // Invertir bit 0
    rom[0] = 0x01;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.XRI_d8(); // A = 0x01
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x01);
}

TEST_F(ANI_ORI_XRI_Test, RealisticUseCase_ClearLowerBits) {
    // Limpiar los 4 bits inferiores
    rom[0] = 0xF0;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xAB);
    
    cpu.ANI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xA0);
}

TEST_F(ANI_ORI_XRI_Test, RealisticUseCase_CombineNibbles) {
    // Combinar nibbles
    rom[0] = 0x0F; // ANI para mantener nibble bajo
    rom[1] = 0xA0; // ORI para establecer nibble alto
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x5F);
    cpu.ANI_d8(); // A = 0x0F
    cpu.ORI_d8(); // A = 0xAF
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xAF);
}

TEST_F(ANI_ORI_XRI_Test, PreservesOtherRegisters) {
    rom[0] = 0x0F;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setRegister(Registers::Register::B, 0xBB);
    cpu.registers_m.setRegister(Registers::Register::C, 0xCC);
    cpu.registers_m.setRegister(Registers::Register::D, 0xDD);
    cpu.registers_m.setRegister(Registers::Register::E, 0xEE);
    cpu.registers_m.setRegister(Registers::Register::H, 0x11);
    cpu.registers_m.setRegister(Registers::Register::L, 0x22);
    
    cpu.ANI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0xBB);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0xCC);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0xDD);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0xEE);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x11);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x22);
}

TEST_F(ANI_ORI_XRI_Test, BoundaryCondition_AllZeros) {
    rom[0] = 0x00;
    rom[1] = 0x00;
    rom[2] = 0x00;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    
    cpu.ANI_d8();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    
    cpu.ORI_d8();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    
    cpu.XRI_d8();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
}

TEST_F(ANI_ORI_XRI_Test, BoundaryCondition_AllOnes) {
    rom[0] = 0xFF;
    rom[1] = 0xFF;
    rom[2] = 0xFF;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    
    cpu.ANI_d8();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    
    cpu.ORI_d8();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    
    cpu.XRI_d8();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
}

TEST_F(ANI_ORI_XRI_Test, ANI_AlternatingPattern) {
    rom[0] = 0xAA;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x55);
    
    cpu.ANI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(ANI_ORI_XRI_Test, ORI_AlternatingPattern) {
    rom[0] = 0xAA;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x55);
    
    cpu.ORI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ANI_ORI_XRI_Test, XRI_AlternatingPattern) {
    rom[0] = 0xAA;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x55);
    
    cpu.XRI_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ANI_ORI_XRI_Test, Idempotence_ANI) {
    rom[0] = 0x0F;
    rom[1] = 0x0F;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    
    cpu.ANI_d8(); // A = 0x0F
    cpu.ANI_d8(); // A = 0x0F (idempotente)
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x0F);
}

TEST_F(ANI_ORI_XRI_Test, Idempotence_ORI) {
    rom[0] = 0xF0;
    rom[1] = 0xF0;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    
    cpu.ORI_d8(); // A = 0xF0
    cpu.ORI_d8(); // A = 0xF0 (idempotente)
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xF0);
}

TEST_F(ANI_ORI_XRI_Test, Idempotence_XRI_DoubleApplication) {
    rom[0] = 0xFF;
    rom[1] = 0xFF;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xAA);
    
    cpu.XRI_d8(); // A = 0x55
    cpu.XRI_d8(); // A = 0xAA (vuelve al original)
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xAA);
}

TEST_F(ANI_ORI_XRI_Test, CombinedOperations_BuildValue) {
    rom[0] = 0x00; // ANI para limpiar
    rom[1] = 0xA5; // ORI para establecer patrón
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    
    cpu.ANI_d8(); // A = 0x00
    cpu.ORI_d8(); // A = 0xA5
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xA5);
}
