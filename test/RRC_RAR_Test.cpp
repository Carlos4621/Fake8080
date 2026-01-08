#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"

class RRC_RAR_Test : public ::testing::Test {
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

// ==================== Tests para RRC (Rotate Right Circular) ====================

TEST_F(RRC_RAR_Test, RRC_A_BasicRotate) {
    cpu.registers_m.setRegister(Registers::Register::A, 0b10110010);
    
    uint8_t cycles = cpu.RRC_R<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b01011001);
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY)); // Bit 0 era 0
}

TEST_F(RRC_RAR_Test, RRC_B_WithBit0Set) {
    cpu.registers_m.setRegister(Registers::Register::B, 0b01010101);
    
    uint8_t cycles = cpu.RRC_R<Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0b10101010);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY)); // Bit 0 era 1
}

TEST_F(RRC_RAR_Test, RRC_C_WithBit0Clear) {
    cpu.registers_m.setRegister(Registers::Register::C, 0b00000010);
    
    uint8_t cycles = cpu.RRC_R<Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0b00000001);
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(RRC_RAR_Test, RRC_D_AllBitsSet) {
    cpu.registers_m.setRegister(Registers::Register::D, 0xFF);
    
    uint8_t cycles = cpu.RRC_R<Registers::Register::D>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0xFF);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(RRC_RAR_Test, RRC_E_AllBitsClear) {
    cpu.registers_m.setRegister(Registers::Register::E, 0x00);
    
    uint8_t cycles = cpu.RRC_R<Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x00);
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(RRC_RAR_Test, RRC_H_SingleBitRotation) {
    cpu.registers_m.setRegister(Registers::Register::H, 0b10000000);
    
    cpu.RRC_R<Registers::Register::H>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0b01000000);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    cpu.RRC_R<Registers::Register::H>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0b00100000);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    cpu.RRC_R<Registers::Register::H>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0b00010000);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(RRC_RAR_Test, RRC_L_EightRotations) {
    // Después de 8 rotaciones debe volver al valor original
    cpu.registers_m.setRegister(Registers::Register::L, 0b10110010);
    const uint8_t original = 0b10110010;
    
    for (int i = 0; i < 8; ++i) {
        cpu.RRC_R<Registers::Register::L>();
    }
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), original);
}

TEST_F(RRC_RAR_Test, RRC_IgnoresPreviousCarry) {
    // RRC no usa el valor previo de CY, solo lo actualiza
    cpu.registers_m.setRegister(Registers::Register::A, 0b00000010);
    cpu.registers_m.setFlag(Registers::Flags::CY, true); // CY previamente establecido
    
    cpu.RRC_R<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b00000001);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY)); // CY se actualiza según bit 0
}

TEST_F(RRC_RAR_Test, RRC_PreservesOtherFlags) {
    // RRC solo afecta CY, no otros flags
    cpu.registers_m.setRegister(Registers::Register::B, 0b00000001);
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    
    cpu.RRC_R<Registers::Register::B>();
    
    // Otros flags deben permanecer sin cambios
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

// ==================== Tests para RAR (Rotate Right through Carry) ====================

TEST_F(RRC_RAR_Test, RAR_A_BasicRotate_CarryClear) {
    cpu.registers_m.setRegister(Registers::Register::A, 0b10110010);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    uint8_t cycles = cpu.RAR_R<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b01011001);
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY)); // Bit 0 era 0
}

TEST_F(RRC_RAR_Test, RAR_B_BasicRotate_CarrySet) {
    cpu.registers_m.setRegister(Registers::Register::B, 0b01010101);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.RAR_R<Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0b10101010);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY)); // Bit 0 era 1
}

TEST_F(RRC_RAR_Test, RAR_C_WithBit0Clear_CarryClear) {
    cpu.registers_m.setRegister(Registers::Register::C, 0b00000010);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    uint8_t cycles = cpu.RAR_R<Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0b00000001);
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(RRC_RAR_Test, RAR_D_WithBit0Clear_CarrySet) {
    cpu.registers_m.setRegister(Registers::Register::D, 0b00000010);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.RAR_R<Registers::Register::D>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0b10000001);
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(RRC_RAR_Test, RAR_E_AllBitsSet_CarryClear) {
    cpu.registers_m.setRegister(Registers::Register::E, 0xFF);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    uint8_t cycles = cpu.RAR_R<Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0b01111111);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(RRC_RAR_Test, RAR_H_AllBitsClear_CarrySet) {
    cpu.registers_m.setRegister(Registers::Register::H, 0x00);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.RAR_R<Registers::Register::H>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0b10000000);
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(RRC_RAR_Test, RAR_L_ShiftCarryThrough) {
    // Demostrar que el carry se mueve a través del registro
    cpu.registers_m.setRegister(Registers::Register::L, 0b10000000);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.RAR_R<Registers::Register::L>(); // L = 0b01000000, CY = 0
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0b01000000);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    cpu.RAR_R<Registers::Register::L>(); // L = 0b00100000, CY = 0
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0b00100000);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    cpu.RAR_R<Registers::Register::L>(); // L = 0b00010000, CY = 0
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0b00010000);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(RRC_RAR_Test, RAR_NineRotationsReturnOriginal) {
    // Después de 9 rotaciones (8 bits + 1 carry) debe volver al valor original
    cpu.registers_m.setRegister(Registers::Register::A, 0b10110010);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    const uint8_t original = 0b10110010;
    const bool originalCarry = true;
    
    for (int i = 0; i < 9; ++i) {
        cpu.RAR_R<Registers::Register::A>();
    }
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), original);
    EXPECT_EQ(cpu.registers_m.getFlag(Registers::Flags::CY), originalCarry);
}

TEST_F(RRC_RAR_Test, RAR_UsesPreviousCarry) {
    // RAR usa el valor previo de CY
    cpu.registers_m.setRegister(Registers::Register::A, 0b00000000);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.RAR_R<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b10000000);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(RRC_RAR_Test, RAR_PreservesOtherFlags) {
    // RAR solo afecta CY, no otros flags
    cpu.registers_m.setRegister(Registers::Register::B, 0b00000001);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    
    cpu.RAR_R<Registers::Register::B>();
    
    // Otros flags deben permanecer sin cambios
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

// ==================== Tests comparativos RRC vs RAR ====================

TEST_F(RRC_RAR_Test, Comparison_RRC_vs_RAR_WithCarryClear) {
    cpu.registers_m.setRegister(Registers::Register::A, 0b10110010);
    cpu.registers_m.setRegister(Registers::Register::B, 0b10110010);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.RRC_R<Registers::Register::A>();
    // Después de RRC: A = 0b01011001, CY = 0 (bit 0 original era 0)
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b01011001);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    // Ahora RAR con el CY actualizado por RRC
    cpu.RAR_R<Registers::Register::B>();
    // RAR: B se desplaza y el CY (ahora 0) entra en bit 7
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0b01011001);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY)); // bit 0 de B era 0
}

TEST_F(RRC_RAR_Test, Comparison_RRC_vs_RAR_WithCarrySet) {
    cpu.registers_m.setRegister(Registers::Register::A, 0b01010101);
    cpu.registers_m.setRegister(Registers::Register::B, 0b01010101);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.RRC_R<Registers::Register::A>();
    // Después de RRC: A = 0b10101010, CY = 1 (bit 0 original era 1)
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b10101010);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    // Ahora RAR con el CY actualizado por RRC
    cpu.RAR_R<Registers::Register::B>();
    // RAR: B se desplaza y el CY (ahora 1) entra en bit 7
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0b10101010);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY)); // bit 0 de B era 1
}

// ==================== Tests de casos típicos en programas 8080 ====================

TEST_F(RRC_RAR_Test, DivideByTwo_RRC) {
    // RRC puede usarse para dividir por 2 (desplazamiento lógico derecho)
    cpu.registers_m.setRegister(Registers::Register::A, 0b00001010); // 10
    
    cpu.RRC_R<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b00000101); // 5
}

TEST_F(RRC_RAR_Test, TestBit0_RRC) {
    // Patrón típico: probar el bit 0 sin modificar el registro
    cpu.registers_m.setRegister(Registers::Register::A, 0b00000001);
    
    cpu.RRC_R<Registers::Register::A>();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY)); // Bit 0 estaba establecido
}

TEST_F(RRC_RAR_Test, BitShifting_RAR) {
    // Desplazamiento de bits a través de múltiples bytes usando carry
    cpu.registers_m.setRegister(Registers::Register::A, 0b00000001);
    cpu.registers_m.setRegister(Registers::Register::B, 0b00000000);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.RAR_R<Registers::Register::A>(); // A desplaza, bit 0 va a CY
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b00000000);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    cpu.RAR_R<Registers::Register::B>(); // B recibe el bit de A
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0b10000000);
}

TEST_F(RRC_RAR_Test, AllRegisters_RRC) {
    // Verificar que RRC funciona en todos los registros
    cpu.registers_m.setRegister(Registers::Register::A, 0b00000001);
    cpu.registers_m.setRegister(Registers::Register::B, 0b00000011);
    cpu.registers_m.setRegister(Registers::Register::C, 0b00000101);
    cpu.registers_m.setRegister(Registers::Register::D, 0b00000111);
    cpu.registers_m.setRegister(Registers::Register::E, 0b00001001);
    cpu.registers_m.setRegister(Registers::Register::H, 0b00001011);
    cpu.registers_m.setRegister(Registers::Register::L, 0b00001101);
    
    cpu.RRC_R<Registers::Register::A>();
    cpu.RRC_R<Registers::Register::B>();
    cpu.RRC_R<Registers::Register::C>();
    cpu.RRC_R<Registers::Register::D>();
    cpu.RRC_R<Registers::Register::E>();
    cpu.RRC_R<Registers::Register::H>();
    cpu.RRC_R<Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b10000000);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0b10000001);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0b10000010);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0b10000011);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0b10000100);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0b10000101);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0b10000110);
}

TEST_F(RRC_RAR_Test, AllRegisters_RAR) {
    // Verificar que RAR funciona en todos los registros
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.registers_m.setRegister(Registers::Register::A, 0b00000010);
    cpu.registers_m.setRegister(Registers::Register::B, 0b00000100);
    cpu.registers_m.setRegister(Registers::Register::C, 0b00000110);
    cpu.registers_m.setRegister(Registers::Register::D, 0b00001000);
    cpu.registers_m.setRegister(Registers::Register::E, 0b00001010);
    cpu.registers_m.setRegister(Registers::Register::H, 0b00001100);
    cpu.registers_m.setRegister(Registers::Register::L, 0b00001110);
    
    cpu.RAR_R<Registers::Register::A>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b10000001);
    
    cpu.RAR_R<Registers::Register::B>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0b00000010);
    
    cpu.RAR_R<Registers::Register::C>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0b00000011);
    
    cpu.RAR_R<Registers::Register::D>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0b00000100);
    
    cpu.RAR_R<Registers::Register::E>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0b00000101);
    
    cpu.RAR_R<Registers::Register::H>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0b00000110);
    
    cpu.RAR_R<Registers::Register::L>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0b00000111);
}
