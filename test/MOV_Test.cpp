#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"

class MOV_Test : public ::testing::Test {
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

// ==================== Tests MOV con registro A como destino ====================

TEST_F(MOV_Test, MOV_A_B) {
    cpu.registers_m.setRegister(Registers::Register::B, 0x42);
    
    uint8_t cycles = cpu.MOV_R_R<Registers::Register::B, Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x42);
    EXPECT_EQ(cycles, 5);
}

TEST_F(MOV_Test, MOV_A_C) {
    cpu.registers_m.setRegister(Registers::Register::C, 0x55);
    
    uint8_t cycles = cpu.MOV_R_R<Registers::Register::C, Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x55);
    EXPECT_EQ(cycles, 5);
}

TEST_F(MOV_Test, MOV_A_D) {
    cpu.registers_m.setRegister(Registers::Register::D, 0xAA);
    
    uint8_t cycles = cpu.MOV_R_R<Registers::Register::D, Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xAA);
    EXPECT_EQ(cycles, 5);
}

TEST_F(MOV_Test, MOV_A_E) {
    cpu.registers_m.setRegister(Registers::Register::E, 0xFF);
    
    uint8_t cycles = cpu.MOV_R_R<Registers::Register::E, Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(cycles, 5);
}

TEST_F(MOV_Test, MOV_A_H) {
    cpu.registers_m.setRegister(Registers::Register::H, 0x12);
    
    uint8_t cycles = cpu.MOV_R_R<Registers::Register::H, Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x12);
    EXPECT_EQ(cycles, 5);
}

TEST_F(MOV_Test, MOV_A_L) {
    cpu.registers_m.setRegister(Registers::Register::L, 0x34);
    
    uint8_t cycles = cpu.MOV_R_R<Registers::Register::L, Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x34);
    EXPECT_EQ(cycles, 5);
}

TEST_F(MOV_Test, MOV_A_A) {
    // MOV A, A (copia de sí mismo)
    cpu.registers_m.setRegister(Registers::Register::A, 0x88);
    
    uint8_t cycles = cpu.MOV_R_R<Registers::Register::A, Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x88);
    EXPECT_EQ(cycles, 5);
}

// ==================== Tests MOV con registro B como destino ====================

TEST_F(MOV_Test, MOV_B_A) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x11);
    
    uint8_t cycles = cpu.MOV_R_R<Registers::Register::A, Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x11);
    EXPECT_EQ(cycles, 5);
}

TEST_F(MOV_Test, MOV_B_C) {
    cpu.registers_m.setRegister(Registers::Register::C, 0x22);
    
    cpu.MOV_R_R<Registers::Register::C, Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x22);
}

TEST_F(MOV_Test, MOV_B_D) {
    cpu.registers_m.setRegister(Registers::Register::D, 0x33);
    
    cpu.MOV_R_R<Registers::Register::D, Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x33);
}

TEST_F(MOV_Test, MOV_B_E) {
    cpu.registers_m.setRegister(Registers::Register::E, 0x44);
    
    cpu.MOV_R_R<Registers::Register::E, Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x44);
}

TEST_F(MOV_Test, MOV_B_H) {
    cpu.registers_m.setRegister(Registers::Register::H, 0x56);
    
    cpu.MOV_R_R<Registers::Register::H, Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x56);
}

TEST_F(MOV_Test, MOV_B_L) {
    cpu.registers_m.setRegister(Registers::Register::L, 0x67);
    
    cpu.MOV_R_R<Registers::Register::L, Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x67);
}

TEST_F(MOV_Test, MOV_B_B) {
    cpu.registers_m.setRegister(Registers::Register::B, 0x78);
    
    cpu.MOV_R_R<Registers::Register::B, Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x78);
}

// ==================== Tests MOV con registro C como destino ====================

TEST_F(MOV_Test, MOV_C_A) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x89);
    
    cpu.MOV_R_R<Registers::Register::A, Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x89);
}

TEST_F(MOV_Test, MOV_C_B) {
    cpu.registers_m.setRegister(Registers::Register::B, 0x9A);
    
    cpu.MOV_R_R<Registers::Register::B, Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x9A);
}

TEST_F(MOV_Test, MOV_C_D) {
    cpu.registers_m.setRegister(Registers::Register::D, 0xAB);
    
    cpu.MOV_R_R<Registers::Register::D, Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0xAB);
}

TEST_F(MOV_Test, MOV_C_E) {
    cpu.registers_m.setRegister(Registers::Register::E, 0xBC);
    
    cpu.MOV_R_R<Registers::Register::E, Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0xBC);
}

TEST_F(MOV_Test, MOV_C_H) {
    cpu.registers_m.setRegister(Registers::Register::H, 0xCD);
    
    cpu.MOV_R_R<Registers::Register::H, Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0xCD);
}

TEST_F(MOV_Test, MOV_C_L) {
    cpu.registers_m.setRegister(Registers::Register::L, 0xDE);
    
    cpu.MOV_R_R<Registers::Register::L, Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0xDE);
}

TEST_F(MOV_Test, MOV_C_C) {
    cpu.registers_m.setRegister(Registers::Register::C, 0xEF);
    
    cpu.MOV_R_R<Registers::Register::C, Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0xEF);
}

// ==================== Tests MOV con registro D como destino ====================

TEST_F(MOV_Test, MOV_D_A) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x01);
    
    cpu.MOV_R_R<Registers::Register::A, Registers::Register::D>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x01);
}

TEST_F(MOV_Test, MOV_D_B) {
    cpu.registers_m.setRegister(Registers::Register::B, 0x02);
    
    cpu.MOV_R_R<Registers::Register::B, Registers::Register::D>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x02);
}

TEST_F(MOV_Test, MOV_D_C) {
    cpu.registers_m.setRegister(Registers::Register::C, 0x03);
    
    cpu.MOV_R_R<Registers::Register::C, Registers::Register::D>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x03);
}

TEST_F(MOV_Test, MOV_D_E) {
    cpu.registers_m.setRegister(Registers::Register::E, 0x04);
    
    cpu.MOV_R_R<Registers::Register::E, Registers::Register::D>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x04);
}

TEST_F(MOV_Test, MOV_D_H) {
    cpu.registers_m.setRegister(Registers::Register::H, 0x05);
    
    cpu.MOV_R_R<Registers::Register::H, Registers::Register::D>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x05);
}

TEST_F(MOV_Test, MOV_D_L) {
    cpu.registers_m.setRegister(Registers::Register::L, 0x06);
    
    cpu.MOV_R_R<Registers::Register::L, Registers::Register::D>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x06);
}

TEST_F(MOV_Test, MOV_D_D) {
    cpu.registers_m.setRegister(Registers::Register::D, 0x07);
    
    cpu.MOV_R_R<Registers::Register::D, Registers::Register::D>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x07);
}

// ==================== Tests MOV con registro E como destino ====================

TEST_F(MOV_Test, MOV_E_A) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    
    cpu.MOV_R_R<Registers::Register::A, Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x10);
}

TEST_F(MOV_Test, MOV_E_B) {
    cpu.registers_m.setRegister(Registers::Register::B, 0x20);
    
    cpu.MOV_R_R<Registers::Register::B, Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x20);
}

TEST_F(MOV_Test, MOV_E_C) {
    cpu.registers_m.setRegister(Registers::Register::C, 0x30);
    
    cpu.MOV_R_R<Registers::Register::C, Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x30);
}

TEST_F(MOV_Test, MOV_E_D) {
    cpu.registers_m.setRegister(Registers::Register::D, 0x40);
    
    cpu.MOV_R_R<Registers::Register::D, Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x40);
}

TEST_F(MOV_Test, MOV_E_H) {
    cpu.registers_m.setRegister(Registers::Register::H, 0x50);
    
    cpu.MOV_R_R<Registers::Register::H, Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x50);
}

TEST_F(MOV_Test, MOV_E_L) {
    cpu.registers_m.setRegister(Registers::Register::L, 0x60);
    
    cpu.MOV_R_R<Registers::Register::L, Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x60);
}

TEST_F(MOV_Test, MOV_E_E) {
    cpu.registers_m.setRegister(Registers::Register::E, 0x70);
    
    cpu.MOV_R_R<Registers::Register::E, Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x70);
}

// ==================== Tests MOV con registro H como destino ====================

TEST_F(MOV_Test, MOV_H_A) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x15);
    
    cpu.MOV_R_R<Registers::Register::A, Registers::Register::H>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x15);
}

TEST_F(MOV_Test, MOV_H_B) {
    cpu.registers_m.setRegister(Registers::Register::B, 0x25);
    
    cpu.MOV_R_R<Registers::Register::B, Registers::Register::H>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x25);
}

TEST_F(MOV_Test, MOV_H_C) {
    cpu.registers_m.setRegister(Registers::Register::C, 0x35);
    
    cpu.MOV_R_R<Registers::Register::C, Registers::Register::H>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x35);
}

TEST_F(MOV_Test, MOV_H_D) {
    cpu.registers_m.setRegister(Registers::Register::D, 0x45);
    
    cpu.MOV_R_R<Registers::Register::D, Registers::Register::H>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x45);
}

TEST_F(MOV_Test, MOV_H_E) {
    cpu.registers_m.setRegister(Registers::Register::E, 0x55);
    
    cpu.MOV_R_R<Registers::Register::E, Registers::Register::H>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x55);
}

TEST_F(MOV_Test, MOV_H_L) {
    cpu.registers_m.setRegister(Registers::Register::L, 0x65);
    
    cpu.MOV_R_R<Registers::Register::L, Registers::Register::H>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x65);
}

TEST_F(MOV_Test, MOV_H_H) {
    cpu.registers_m.setRegister(Registers::Register::H, 0x75);
    
    cpu.MOV_R_R<Registers::Register::H, Registers::Register::H>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x75);
}

// ==================== Tests MOV con registro L como destino ====================

TEST_F(MOV_Test, MOV_L_A) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x18);
    
    cpu.MOV_R_R<Registers::Register::A, Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x18);
}

TEST_F(MOV_Test, MOV_L_B) {
    cpu.registers_m.setRegister(Registers::Register::B, 0x28);
    
    cpu.MOV_R_R<Registers::Register::B, Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x28);
}

TEST_F(MOV_Test, MOV_L_C) {
    cpu.registers_m.setRegister(Registers::Register::C, 0x38);
    
    cpu.MOV_R_R<Registers::Register::C, Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x38);
}

TEST_F(MOV_Test, MOV_L_D) {
    cpu.registers_m.setRegister(Registers::Register::D, 0x48);
    
    cpu.MOV_R_R<Registers::Register::D, Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x48);
}

TEST_F(MOV_Test, MOV_L_E) {
    cpu.registers_m.setRegister(Registers::Register::E, 0x58);
    
    cpu.MOV_R_R<Registers::Register::E, Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x58);
}

TEST_F(MOV_Test, MOV_L_H) {
    cpu.registers_m.setRegister(Registers::Register::H, 0x68);
    
    cpu.MOV_R_R<Registers::Register::H, Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x68);
}

TEST_F(MOV_Test, MOV_L_L) {
    cpu.registers_m.setRegister(Registers::Register::L, 0x78);
    
    cpu.MOV_R_R<Registers::Register::L, Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x78);
}

// ==================== Tests de casos especiales ====================

TEST_F(MOV_Test, MOV_PreservesAllFlags) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.MOV_R_R<Registers::Register::A, Registers::Register::B>();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(MOV_Test, MOV_DoesNotModifyOtherRegisters) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x11);
    cpu.registers_m.setRegister(Registers::Register::B, 0x22);
    cpu.registers_m.setRegister(Registers::Register::C, 0x33);
    cpu.registers_m.setRegister(Registers::Register::D, 0x44);
    cpu.registers_m.setRegister(Registers::Register::E, 0x55);
    cpu.registers_m.setRegister(Registers::Register::H, 0x66);
    cpu.registers_m.setRegister(Registers::Register::L, 0x77);
    
    cpu.MOV_R_R<Registers::Register::A, Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x11);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x11);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x33);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x44);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x55);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x66);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x77);
}

TEST_F(MOV_Test, MOV_WithZeroValue) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setRegister(Registers::Register::B, 0xFF);
    
    cpu.MOV_R_R<Registers::Register::A, Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x00);
}

TEST_F(MOV_Test, MOV_WithMaxValue) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setRegister(Registers::Register::B, 0x00);
    
    cpu.MOV_R_R<Registers::Register::A, Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0xFF);
}

TEST_F(MOV_Test, MOV_ChainedTransfers) {
    // Transferencia en cadena: A -> B -> C -> D
    cpu.registers_m.setRegister(Registers::Register::A, 0xAB);
    
    cpu.MOV_R_R<Registers::Register::A, Registers::Register::B>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0xAB);
    
    cpu.MOV_R_R<Registers::Register::B, Registers::Register::C>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0xAB);
    
    cpu.MOV_R_R<Registers::Register::C, Registers::Register::D>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0xAB);
}

TEST_F(MOV_Test, MOV_SwapUsingTemp) {
    // Simular swap de A y B usando H como temporal
    cpu.registers_m.setRegister(Registers::Register::A, 0x11);
    cpu.registers_m.setRegister(Registers::Register::B, 0x22);
    
    cpu.MOV_R_R<Registers::Register::A, Registers::Register::H>(); // H = A
    cpu.MOV_R_R<Registers::Register::B, Registers::Register::A>(); // A = B
    cpu.MOV_R_R<Registers::Register::H, Registers::Register::B>(); // B = H
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x22);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x11);
}

TEST_F(MOV_Test, MOV_AllRegisterPairs) {
    // Probar todas las combinaciones de pares de registros comunes
    cpu.registers_m.setRegister(Registers::Register::B, 0x12);
    cpu.registers_m.setRegister(Registers::Register::C, 0x34);
    cpu.registers_m.setRegister(Registers::Register::D, 0x56);
    cpu.registers_m.setRegister(Registers::Register::E, 0x78);
    
    // BC -> DE
    cpu.MOV_R_R<Registers::Register::B, Registers::Register::D>();
    cpu.MOV_R_R<Registers::Register::C, Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x12);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x34);
}

TEST_F(MOV_Test, MOV_LoadAccumulatorFromMultipleSources) {
    // Cargar A desde diferentes registros secuencialmente
    cpu.registers_m.setRegister(Registers::Register::B, 0x01);
    cpu.registers_m.setRegister(Registers::Register::C, 0x02);
    cpu.registers_m.setRegister(Registers::Register::D, 0x03);
    
    cpu.MOV_R_R<Registers::Register::B, Registers::Register::A>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x01);
    
    cpu.MOV_R_R<Registers::Register::C, Registers::Register::A>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x02);
    
    cpu.MOV_R_R<Registers::Register::D, Registers::Register::A>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x03);
}

TEST_F(MOV_Test, MOV_BroadcastValue) {
    // Copiar un valor a todos los registros
    cpu.registers_m.setRegister(Registers::Register::A, 0x99);
    
    cpu.MOV_R_R<Registers::Register::A, Registers::Register::B>();
    cpu.MOV_R_R<Registers::Register::A, Registers::Register::C>();
    cpu.MOV_R_R<Registers::Register::A, Registers::Register::D>();
    cpu.MOV_R_R<Registers::Register::A, Registers::Register::E>();
    cpu.MOV_R_R<Registers::Register::A, Registers::Register::H>();
    cpu.MOV_R_R<Registers::Register::A, Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x99);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x99);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x99);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x99);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x99);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x99);
}
