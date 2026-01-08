#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"

class STC_CMA_CMC_DAA_Test : public ::testing::Test {
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

// ==================== Tests para STC (Set Carry) ====================

TEST_F(STC_CMA_CMC_DAA_Test, STC_SetCarryFromClear) {
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    uint8_t cycles = cpu.STC();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_EQ(cycles, 4);
}

TEST_F(STC_CMA_CMC_DAA_Test, STC_SetCarryWhenAlreadySet) {
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.STC();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_EQ(cycles, 4);
}

TEST_F(STC_CMA_CMC_DAA_Test, STC_PreservesOtherFlags) {
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.STC();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(STC_CMA_CMC_DAA_Test, STC_DoesNotModifyRegisters) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setRegister(Registers::Register::B, 0xAA);
    
    cpu.STC();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0xAA);
}

// ==================== Tests para CMA (Complement Accumulator) ====================

TEST_F(STC_CMA_CMC_DAA_Test, CMA_ComplementAllZeros) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    
    uint8_t cycles = cpu.CMA();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(cycles, 4);
}

TEST_F(STC_CMA_CMC_DAA_Test, CMA_ComplementAllOnes) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    
    uint8_t cycles = cpu.CMA();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_EQ(cycles, 4);
}

TEST_F(STC_CMA_CMC_DAA_Test, CMA_ComplementPattern) {
    cpu.registers_m.setRegister(Registers::Register::A, 0b10101010);
    
    cpu.CMA();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0b01010101);
}

TEST_F(STC_CMA_CMC_DAA_Test, CMA_ComplementSpecificValue) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x3C); // 0011 1100
    
    cpu.CMA();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xC3); // 1100 0011
}

TEST_F(STC_CMA_CMC_DAA_Test, CMA_DoubleComplementReturnsOriginal) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x42);
    
    cpu.CMA();
    cpu.CMA();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x42);
}

TEST_F(STC_CMA_CMC_DAA_Test, CMA_PreservesAllFlags) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x55);
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.CMA();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(STC_CMA_CMC_DAA_Test, CMA_DoesNotModifyOtherRegisters) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x12);
    cpu.registers_m.setRegister(Registers::Register::B, 0x34);
    cpu.registers_m.setRegister(Registers::Register::C, 0x56);
    
    cpu.CMA();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x34);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x56);
}

// ==================== Tests para CMC (Complement Carry) ====================

TEST_F(STC_CMA_CMC_DAA_Test, CMC_ComplementCarryFromClear) {
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    uint8_t cycles = cpu.CMC();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_EQ(cycles, 4);
}

TEST_F(STC_CMA_CMC_DAA_Test, CMC_ComplementCarryFromSet) {
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.CMC();
    
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_EQ(cycles, 4);
}

TEST_F(STC_CMA_CMC_DAA_Test, CMC_DoubleComplementReturnsOriginal) {
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.CMC();
    cpu.CMC();
    
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(STC_CMA_CMC_DAA_Test, CMC_PreservesOtherFlags) {
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.CMC();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(STC_CMA_CMC_DAA_Test, CMC_DoesNotModifyRegisters) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x88);
    cpu.registers_m.setRegister(Registers::Register::B, 0x99);
    
    cpu.CMC();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x88);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x99);
}

// ==================== Tests para DAA (Decimal Adjust Accumulator) ====================

TEST_F(STC_CMA_CMC_DAA_Test, DAA_NoAdjustmentNeeded) {
    // Valor BCD válido: 25 (0010 0101)
    cpu.registers_m.setRegister(Registers::Register::A, 0x25);
    cpu.registers_m.setFlag(Registers::Flags::AC, false);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    uint8_t cycles = cpu.DAA();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x25);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_EQ(cycles, 4);
}

TEST_F(STC_CMA_CMC_DAA_Test, DAA_AdjustLowerNibble_ACSet) {
    // Después de ADD con AC=1 (por ejemplo, 9+8=17, 0x11 en binario)
    cpu.registers_m.setRegister(Registers::Register::A, 0x1A); // nibble inferior > 9
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.DAA();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x20);
}

TEST_F(STC_CMA_CMC_DAA_Test, DAA_AdjustLowerNibble_LowerGreaterThan9) {
    // Nibble inferior = 0x0C (12 > 9)
    cpu.registers_m.setRegister(Registers::Register::A, 0x1C);
    cpu.registers_m.setFlag(Registers::Flags::AC, false);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.DAA();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x22);
}

TEST_F(STC_CMA_CMC_DAA_Test, DAA_AdjustUpperNibble_CYSet) {
    // CY establecido indica overflow en el nibble superior
    cpu.registers_m.setRegister(Registers::Register::A, 0x23);
    cpu.registers_m.setFlag(Registers::Flags::AC, false);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.DAA();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x83);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(STC_CMA_CMC_DAA_Test, DAA_AdjustUpperNibble_UpperGreaterThan9) {
    // Nibble superior = 0xA (10 > 9)
    cpu.registers_m.setRegister(Registers::Register::A, 0xA5);
    cpu.registers_m.setFlag(Registers::Flags::AC, false);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.DAA();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x05);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(STC_CMA_CMC_DAA_Test, DAA_AdjustBothNibbles) {
    // Ambos nibbles necesitan ajuste
    cpu.registers_m.setRegister(Registers::Register::A, 0x9C); // 9 + C
    cpu.registers_m.setFlag(Registers::Flags::AC, false);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.DAA();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x02);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(STC_CMA_CMC_DAA_Test, DAA_BCDAddition_15Plus27) {
    // Simular: 15 + 27 = 42 en BCD
    // 0x15 + 0x27 = 0x3C (binario)
    cpu.registers_m.setRegister(Registers::Register::A, 0x3C);
    cpu.registers_m.setFlag(Registers::Flags::AC, false);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.DAA();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x42);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(STC_CMA_CMC_DAA_Test, DAA_BCDAddition_58Plus67) {
    // Simular: 58 + 67 = 125 en BCD (debe dar 0x25 con CY=1)
    // 0x58 + 0x67 = 0xBF (binario)
    cpu.registers_m.setRegister(Registers::Register::A, 0xBF);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.DAA();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x25);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(STC_CMA_CMC_DAA_Test, DAA_BCDAddition_99Plus01) {
    // Simular: 99 + 01 = 100 en BCD (debe dar 0x00 con CY=1)
    // 0x99 + 0x01 = 0x9A (binario)
    cpu.registers_m.setRegister(Registers::Register::A, 0x9A);
    cpu.registers_m.setFlag(Registers::Flags::AC, false);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.DAA();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(STC_CMA_CMC_DAA_Test, DAA_BCDAddition_09Plus08) {
    // Simular: 09 + 08 = 17 en BCD
    // 0x09 + 0x08 = 0x11 (binario, pero AC=1)
    cpu.registers_m.setRegister(Registers::Register::A, 0x11);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.DAA();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x17);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(STC_CMA_CMC_DAA_Test, DAA_UpdatesZeroFlag) {
    // Resultado es cero
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setFlag(Registers::Flags::AC, false);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.DAA();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(STC_CMA_CMC_DAA_Test, DAA_UpdatesSignFlag) {
    // Resultado tiene bit 7 establecido después del ajuste
    // 0xA0 tiene nibble superior > 9, se ajusta con +0x60 = 0x00 con CY=1
    // Mejor usar un valor que mantenga el bit 7 después del ajuste
    cpu.registers_m.setRegister(Registers::Register::A, 0x88); // nibble inferior = 8, superior = 8
    cpu.registers_m.setFlag(Registers::Flags::AC, false);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.DAA();
    
    // 0x88 no necesita ajuste, mantiene el bit 7 establecido
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(STC_CMA_CMC_DAA_Test, DAA_UpdatesParityFlag_Even) {
    // Resultado con paridad par
    cpu.registers_m.setRegister(Registers::Register::A, 0x03); // 0000 0011 (2 bits)
    cpu.registers_m.setFlag(Registers::Flags::AC, false);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.DAA();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(STC_CMA_CMC_DAA_Test, DAA_UpdatesParityFlag_Odd) {
    // Resultado con paridad impar
    cpu.registers_m.setRegister(Registers::Register::A, 0x07); // 0000 0111 (3 bits)
    cpu.registers_m.setFlag(Registers::Flags::AC, false);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.DAA();
    
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(STC_CMA_CMC_DAA_Test, DAA_DoesNotModifyOtherRegisters) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x9A);
    cpu.registers_m.setRegister(Registers::Register::B, 0x12);
    cpu.registers_m.setRegister(Registers::Register::C, 0x34);
    cpu.registers_m.setFlag(Registers::Flags::AC, false);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.DAA();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x12);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x34);
}

// ==================== Tests combinados ====================

TEST_F(STC_CMA_CMC_DAA_Test, Combined_STC_Then_CMC) {
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.STC();
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    cpu.CMC();
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(STC_CMA_CMC_DAA_Test, Combined_CMA_Twice) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xA5);
    
    cpu.CMA();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x5A);
    
    cpu.CMA();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xA5);
}

TEST_F(STC_CMA_CMC_DAA_Test, Combined_ADDThenDAA) {
    // Simular secuencia típica: ADD + DAA para aritmética BCD
    cpu.registers_m.setRegister(Registers::Register::A, 0x29);
    cpu.registers_m.setRegister(Registers::Register::B, 0x46);
    
    // Suma binaria: 0x29 + 0x46 = 0x6F
    uint8_t result = cpu.registers_m.getRegister(Registers::Register::A) + 
                     cpu.registers_m.getRegister(Registers::Register::B);
    cpu.registers_m.setRegister(Registers::Register::A, result);
    
    // Simular flags de ADD (esto normalmente lo haría ADD)
    cpu.registers_m.setFlag(Registers::Flags::AC, ((0x29 & 0x0F) + (0x46 & 0x0F)) > 0x0F);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.DAA();
    
    // Resultado BCD: 29 + 46 = 75
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x75);
}
