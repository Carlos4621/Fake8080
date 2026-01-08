#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"

class RLC_RAL_Test : public ::testing::Test {
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

// ==================== Tests para RLC (Rotate Left Circular) ====================

TEST_F(RLC_RAL_Test, RLC_A_BasicRotate) {
    cpu.registers_m.setRegister(Registers::Register::A, 0b10110010);
    
    uint8_t cycles = cpu.RLC_R<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b01100101);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY)); // Bit 7 era 1
}

TEST_F(RLC_RAL_Test, RLC_B_WithBit7Clear) {
    cpu.registers_m.setRegister(Registers::Register::B, 0b01010101);
    
    uint8_t cycles = cpu.RLC_R<Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0b10101010);
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY)); // Bit 7 era 0
}

TEST_F(RLC_RAL_Test, RLC_C_WithBit7Set) {
    cpu.registers_m.setRegister(Registers::Register::C, 0b10000000);
    
    uint8_t cycles = cpu.RLC_R<Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0b00000001);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(RLC_RAL_Test, RLC_D_AllBitsSet) {
    cpu.registers_m.setRegister(Registers::Register::D, 0xFF);
    
    uint8_t cycles = cpu.RLC_R<Registers::Register::D>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0xFF);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(RLC_RAL_Test, RLC_E_AllBitsClear) {
    cpu.registers_m.setRegister(Registers::Register::E, 0x00);
    
    uint8_t cycles = cpu.RLC_R<Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x00);
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(RLC_RAL_Test, RLC_H_SingleBitRotation) {
    cpu.registers_m.setRegister(Registers::Register::H, 0b00000001);
    
    cpu.RLC_R<Registers::Register::H>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0b00000010);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    cpu.RLC_R<Registers::Register::H>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0b00000100);
    
    cpu.RLC_R<Registers::Register::H>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0b00001000);
}

TEST_F(RLC_RAL_Test, RLC_L_EightRotations) {
    // Después de 8 rotaciones debe volver al valor original
    cpu.registers_m.setRegister(Registers::Register::L, 0b10110010);
    const uint8_t original = 0b10110010;
    
    for (int i = 0; i < 8; ++i) {
        cpu.RLC_R<Registers::Register::L>();
    }
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), original);
}

TEST_F(RLC_RAL_Test, RLC_IgnoresPreviousCarry) {
    // RLC no usa el valor previo de CY, solo lo actualiza
    cpu.registers_m.setRegister(Registers::Register::A, 0b01000000);
    cpu.registers_m.setFlag(Registers::Flags::CY, true); // CY previamente establecido
    
    cpu.RLC_R<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b10000000);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY)); // CY se actualiza según bit 7
}

TEST_F(RLC_RAL_Test, RLC_PreservesOtherFlags) {
    // RLC solo afecta CY, no otros flags
    cpu.registers_m.setRegister(Registers::Register::B, 0b10000000);
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    
    cpu.RLC_R<Registers::Register::B>();
    
    // Otros flags deben permanecer sin cambios
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

// ==================== Tests para RAL (Rotate Left through Carry) ====================

TEST_F(RLC_RAL_Test, RAL_A_BasicRotate_CarryClear) {
    cpu.registers_m.setRegister(Registers::Register::A, 0b10110010);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    uint8_t cycles = cpu.RAL_R<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b01100100);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY)); // Bit 7 era 1
}

TEST_F(RLC_RAL_Test, RAL_B_BasicRotate_CarrySet) {
    cpu.registers_m.setRegister(Registers::Register::B, 0b01010101);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.RAL_R<Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0b10101011);
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY)); // Bit 7 era 0
}

TEST_F(RLC_RAL_Test, RAL_C_WithBit7Set_CarryClear) {
    cpu.registers_m.setRegister(Registers::Register::C, 0b10000000);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    uint8_t cycles = cpu.RAL_R<Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0b00000000);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(RLC_RAL_Test, RAL_D_WithBit7Set_CarrySet) {
    cpu.registers_m.setRegister(Registers::Register::D, 0b10000000);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.RAL_R<Registers::Register::D>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0b00000001);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(RLC_RAL_Test, RAL_E_AllBitsSet_CarryClear) {
    cpu.registers_m.setRegister(Registers::Register::E, 0xFF);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    uint8_t cycles = cpu.RAL_R<Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0xFE);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(RLC_RAL_Test, RAL_H_AllBitsClear_CarrySet) {
    cpu.registers_m.setRegister(Registers::Register::H, 0x00);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.RAL_R<Registers::Register::H>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x01);
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(RLC_RAL_Test, RAL_L_ShiftCarryThrough) {
    // Demostrar que el carry se mueve a través del registro
    cpu.registers_m.setRegister(Registers::Register::L, 0b00000001);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.RAL_R<Registers::Register::L>(); // L = 0b00000010, CY = 0
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0b00000010);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    cpu.RAL_R<Registers::Register::L>(); // L = 0b00000100, CY = 0
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0b00000100);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    cpu.RAL_R<Registers::Register::L>(); // L = 0b00001000, CY = 0
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0b00001000);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(RLC_RAL_Test, RAL_NineRotationsReturnOriginal) {
    // Después de 9 rotaciones (8 bits + 1 carry) debe volver al valor original
    cpu.registers_m.setRegister(Registers::Register::A, 0b10110010);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    const uint8_t original = 0b10110010;
    const bool originalCarry = true;
    
    for (int i = 0; i < 9; ++i) {
        cpu.RAL_R<Registers::Register::A>();
    }
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), original);
    EXPECT_EQ(cpu.registers_m.getFlag(Registers::Flags::CY), originalCarry);
}

TEST_F(RLC_RAL_Test, RAL_UsesPreviousCarry) {
    // RAL usa el valor previo de CY
    cpu.registers_m.setRegister(Registers::Register::A, 0b00000000);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.RAL_R<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b00000001);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(RLC_RAL_Test, RAL_PreservesOtherFlags) {
    // RAL solo afecta CY, no otros flags
    cpu.registers_m.setRegister(Registers::Register::B, 0b10000000);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    
    cpu.RAL_R<Registers::Register::B>();
    
    // Otros flags deben permanecer sin cambios
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

// ==================== Tests comparativos RLC vs RAL ====================

TEST_F(RLC_RAL_Test, Comparison_RLC_vs_RAL_WithCarryClear) {
    cpu.registers_m.setRegister(Registers::Register::A, 0b10110010);
    cpu.registers_m.setRegister(Registers::Register::B, 0b10110010);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.RLC_R<Registers::Register::A>();
    // Después de RLC: A = 0b01100101, CY = 1 (bit 7 original era 1)
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b01100101);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    // Ahora RAL con el CY actualizado por RLC
    cpu.RAL_R<Registers::Register::B>();
    // RAL: B se desplaza y el CY (ahora 1) entra en bit 0
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0b01100101);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY)); // bit 7 de B era 1
}

TEST_F(RLC_RAL_Test, Comparison_RLC_vs_RAL_WithCarrySet) {
    cpu.registers_m.setRegister(Registers::Register::A, 0b01010101);
    cpu.registers_m.setRegister(Registers::Register::B, 0b01010101);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.RLC_R<Registers::Register::A>();
    // Después de RLC: A = 0b10101010, CY = 0 (bit 7 original era 0)
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b10101010);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    // Ahora RAL con el CY actualizado por RLC
    cpu.RAL_R<Registers::Register::B>();
    // RAL: B se desplaza y el CY (ahora 0) entra en bit 0
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0b10101010);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY)); // bit 7 de B era 0
}

// ==================== Tests de casos típicos en programas 8080 ====================

TEST_F(RLC_RAL_Test, MultiplyByTwo_RLC) {
    // RLC puede usarse para multiplicar por 2 (desplazamiento lógico izquierdo)
    cpu.registers_m.setRegister(Registers::Register::A, 0b00000101); // 5
    
    cpu.RLC_R<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b00001010); // 10
}

TEST_F(RLC_RAL_Test, TestBit7_RLC) {
    // Patrón típico: probar el bit 7 sin modificar el registro
    cpu.registers_m.setRegister(Registers::Register::A, 0b10000000);
    
    cpu.RLC_R<Registers::Register::A>();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY)); // Bit 7 estaba establecido
}

TEST_F(RLC_RAL_Test, BitShifting_RAL) {
    // Desplazamiento de bits a través de múltiples bytes usando carry
    cpu.registers_m.setRegister(Registers::Register::A, 0b10000000);
    cpu.registers_m.setRegister(Registers::Register::B, 0b00000000);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.RAL_R<Registers::Register::B>(); // B = 0, CY = 0
    cpu.RAL_R<Registers::Register::A>(); // A desplaza, bit 7 va a CY
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b00000000);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    cpu.RAL_R<Registers::Register::B>(); // B recibe el bit de A
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0b00000001);
}

TEST_F(RLC_RAL_Test, AllRegisters_RLC) {
    // Verificar que RLC funciona en todos los registros
    cpu.registers_m.setRegister(Registers::Register::A, 0b10000000);
    cpu.registers_m.setRegister(Registers::Register::B, 0b10000001);
    cpu.registers_m.setRegister(Registers::Register::C, 0b10000010);
    cpu.registers_m.setRegister(Registers::Register::D, 0b10000011);
    cpu.registers_m.setRegister(Registers::Register::E, 0b10000100);
    cpu.registers_m.setRegister(Registers::Register::H, 0b10000101);
    cpu.registers_m.setRegister(Registers::Register::L, 0b10000110);
    
    cpu.RLC_R<Registers::Register::A>();
    cpu.RLC_R<Registers::Register::B>();
    cpu.RLC_R<Registers::Register::C>();
    cpu.RLC_R<Registers::Register::D>();
    cpu.RLC_R<Registers::Register::E>();
    cpu.RLC_R<Registers::Register::H>();
    cpu.RLC_R<Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b00000001);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0b00000011);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0b00000101);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0b00000111);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0b00001001);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0b00001011);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0b00001101);
}

TEST_F(RLC_RAL_Test, AllRegisters_RAL) {
    // Verificar que RAL funciona en todos los registros
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.registers_m.setRegister(Registers::Register::A, 0b01000000);
    cpu.registers_m.setRegister(Registers::Register::B, 0b01000001);
    cpu.registers_m.setRegister(Registers::Register::C, 0b01000010);
    cpu.registers_m.setRegister(Registers::Register::D, 0b01000011);
    cpu.registers_m.setRegister(Registers::Register::E, 0b01000100);
    cpu.registers_m.setRegister(Registers::Register::H, 0b01000101);
    cpu.registers_m.setRegister(Registers::Register::L, 0b01000110);
    
    cpu.RAL_R<Registers::Register::A>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b10000001);
    
    cpu.RAL_R<Registers::Register::B>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0b10000010);
    
    cpu.RAL_R<Registers::Register::C>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0b10000100);
    
    cpu.RAL_R<Registers::Register::D>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0b10000110);
    
    cpu.RAL_R<Registers::Register::E>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0b10001000);
    
    cpu.RAL_R<Registers::Register::H>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0b10001010);
    
    cpu.RAL_R<Registers::Register::L>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0b10001100);
}
