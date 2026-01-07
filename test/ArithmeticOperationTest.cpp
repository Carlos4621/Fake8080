#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"

class ArithmeticOperationTest : public ::testing::Test {
protected:
    CPUTest cpu;
};

// ==================== Tests de ADD sin carry ====================

TEST_F(ArithmeticOperationTest, Add_BasicAddition_ReturnsCorrectResult) {
    uint8_t result = cpu.aritmeticOperation_8bits(0x10, 0x20, CPUTest::AritmeticOperation::ADD, false);
    EXPECT_EQ(result, 0x30);
}

TEST_F(ArithmeticOperationTest, Add_ZeroPlusZero_ReturnsZero) {
    uint8_t result = cpu.aritmeticOperation_8bits(0x00, 0x00, CPUTest::AritmeticOperation::ADD, false);
    EXPECT_EQ(result, 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(ArithmeticOperationTest, Add_WithOverflow_ReturnsWrappedValue) {
    uint8_t result = cpu.aritmeticOperation_8bits(0xFF, 0x01, CPUTest::AritmeticOperation::ADD, false);
    EXPECT_EQ(result, 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(ArithmeticOperationTest, Add_MaxValues_ReturnsWrappedValue) {
    uint8_t result = cpu.aritmeticOperation_8bits(0xFF, 0xFF, CPUTest::AritmeticOperation::ADD, false);
    EXPECT_EQ(result, 0xFE);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ArithmeticOperationTest, Add_ResultHasEvenParity_SetsParityFlag) {
    uint8_t result = cpu.aritmeticOperation_8bits(0x01, 0x02, CPUTest::AritmeticOperation::ADD, false);
    EXPECT_EQ(result, 0x03); // 0x03 = 00000011 (2 bits set = par)
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ArithmeticOperationTest, Add_ResultHasOddParity_ClearsParityFlag) {
    uint8_t result = cpu.aritmeticOperation_8bits(0x00, 0x01, CPUTest::AritmeticOperation::ADD, false);
    EXPECT_EQ(result, 0x01); // 0x01 = 00000001 (1 bit set = impar)
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ArithmeticOperationTest, Add_ResultIsNegative_SetsSignFlag) {
    uint8_t result = cpu.aritmeticOperation_8bits(0x7F, 0x01, CPUTest::AritmeticOperation::ADD, false);
    EXPECT_EQ(result, 0x80);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ArithmeticOperationTest, Add_CausesHalfCarry_SetsAuxiliaryCarryFlag) {
    uint8_t result = cpu.aritmeticOperation_8bits(0x0F, 0x01, CPUTest::AritmeticOperation::ADD, false);
    EXPECT_EQ(result, 0x10);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(ArithmeticOperationTest, Add_NoHalfCarry_ClearsAuxiliaryCarryFlag) {
    uint8_t result = cpu.aritmeticOperation_8bits(0x10, 0x20, CPUTest::AritmeticOperation::ADD, false);
    EXPECT_EQ(result, 0x30);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

// ==================== Tests de ADD con carry ====================

TEST_F(ArithmeticOperationTest, AddWithCarry_CarryFlagClear_AddsWithoutCarry) {
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    uint8_t result = cpu.aritmeticOperation_8bits(0x10, 0x20, CPUTest::AritmeticOperation::ADD, true);
    EXPECT_EQ(result, 0x30);
}

TEST_F(ArithmeticOperationTest, AddWithCarry_CarryFlagSet_AddsWithCarry) {
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    uint8_t result = cpu.aritmeticOperation_8bits(0x10, 0x20, CPUTest::AritmeticOperation::ADD, true);
    EXPECT_EQ(result, 0x31);
}

TEST_F(ArithmeticOperationTest, AddWithCarry_CarryFlagSet_CausesOverflow) {
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    uint8_t result = cpu.aritmeticOperation_8bits(0xFF, 0x00, CPUTest::AritmeticOperation::ADD, true);
    EXPECT_EQ(result, 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(ArithmeticOperationTest, AddWithCarry_CarryFlagSet_CausesHalfCarry) {
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    uint8_t result = cpu.aritmeticOperation_8bits(0x0E, 0x00, CPUTest::AritmeticOperation::ADD, true);
    EXPECT_EQ(result, 0x0F);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
    
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    result = cpu.aritmeticOperation_8bits(0x0F, 0x00, CPUTest::AritmeticOperation::ADD, true);
    EXPECT_EQ(result, 0x10);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

// ==================== Tests de SUB sin borrow ====================

TEST_F(ArithmeticOperationTest, Sub_BasicSubtraction_ReturnsCorrectResult) {
    uint8_t result = cpu.aritmeticOperation_8bits(0x30, 0x10, CPUTest::AritmeticOperation::SUB, false);
    EXPECT_EQ(result, 0x20);
}

TEST_F(ArithmeticOperationTest, Sub_SubtractFromItself_ReturnsZero) {
    uint8_t result = cpu.aritmeticOperation_8bits(0x50, 0x50, CPUTest::AritmeticOperation::SUB, false);
    EXPECT_EQ(result, 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ArithmeticOperationTest, Sub_SubtractZero_ReturnsSameValue) {
    uint8_t result = cpu.aritmeticOperation_8bits(0x42, 0x00, CPUTest::AritmeticOperation::SUB, false);
    EXPECT_EQ(result, 0x42);
}

TEST_F(ArithmeticOperationTest, Sub_WithBorrow_ReturnsWrappedValue) {
    uint8_t result = cpu.aritmeticOperation_8bits(0x00, 0x01, CPUTest::AritmeticOperation::SUB, false);
    EXPECT_EQ(result, 0xFF);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ArithmeticOperationTest, Sub_LargerFromSmaller_SetsBorrowFlag) {
    uint8_t result = cpu.aritmeticOperation_8bits(0x10, 0x20, CPUTest::AritmeticOperation::SUB, false);
    EXPECT_EQ(result, 0xF0);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ArithmeticOperationTest, Sub_ResultHasEvenParity_SetsParityFlag) {
    uint8_t result = cpu.aritmeticOperation_8bits(0x05, 0x02, CPUTest::AritmeticOperation::SUB, false);
    EXPECT_EQ(result, 0x03); // 0x03 = 00000011 (2 bits set = par)
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ArithmeticOperationTest, Sub_ResultHasOddParity_ClearsParityFlag) {
    uint8_t result = cpu.aritmeticOperation_8bits(0x02, 0x01, CPUTest::AritmeticOperation::SUB, false);
    EXPECT_EQ(result, 0x01); // 0x01 = 00000001 (1 bit set = impar)
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ArithmeticOperationTest, Sub_ResultIsNegative_SetsSignFlag) {
    uint8_t result = cpu.aritmeticOperation_8bits(0x00, 0x01, CPUTest::AritmeticOperation::SUB, false);
    EXPECT_EQ(result, 0xFF);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ArithmeticOperationTest, Sub_CausesHalfBorrow_SetsAuxiliaryCarryFlag) {
    uint8_t result = cpu.aritmeticOperation_8bits(0x10, 0x01, CPUTest::AritmeticOperation::SUB, false);
    EXPECT_EQ(result, 0x0F);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(ArithmeticOperationTest, Sub_NoHalfBorrow_ClearsAuxiliaryCarryFlag) {
    uint8_t result = cpu.aritmeticOperation_8bits(0x30, 0x10, CPUTest::AritmeticOperation::SUB, false);
    EXPECT_EQ(result, 0x20);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

// ==================== Tests de SUB con borrow (SBB) ====================

TEST_F(ArithmeticOperationTest, SubWithBorrow_CarryFlagClear_SubtractsWithoutBorrow) {
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    uint8_t result = cpu.aritmeticOperation_8bits(0x30, 0x10, CPUTest::AritmeticOperation::SUB, true);
    EXPECT_EQ(result, 0x20);
}

TEST_F(ArithmeticOperationTest, SubWithBorrow_CarryFlagSet_SubtractsWithBorrow) {
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    uint8_t result = cpu.aritmeticOperation_8bits(0x30, 0x10, CPUTest::AritmeticOperation::SUB, true);
    EXPECT_EQ(result, 0x1F);
}

TEST_F(ArithmeticOperationTest, SubWithBorrow_CarryFlagSet_CausesBorrow) {
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    uint8_t result = cpu.aritmeticOperation_8bits(0x00, 0x00, CPUTest::AritmeticOperation::SUB, true);
    EXPECT_EQ(result, 0xFF);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ArithmeticOperationTest, SubWithBorrow_CarryFlagSet_CausesHalfBorrow) {
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    uint8_t result = cpu.aritmeticOperation_8bits(0x10, 0x00, CPUTest::AritmeticOperation::SUB, true);
    EXPECT_EQ(result, 0x0F);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

// ==================== Tests de casos límite ====================

TEST_F(ArithmeticOperationTest, Add_MaxValuePlusOne_WrapsToZero) {
    uint8_t result = cpu.aritmeticOperation_8bits(0xFF, 0x01, CPUTest::AritmeticOperation::ADD, false);
    EXPECT_EQ(result, 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ArithmeticOperationTest, Sub_ZeroMinusOne_WrapsToMaxValue) {
    uint8_t result = cpu.aritmeticOperation_8bits(0x00, 0x01, CPUTest::AritmeticOperation::SUB, false);
    EXPECT_EQ(result, 0xFF);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ArithmeticOperationTest, Add_HalfMaxValues_NoOverflow) {
    uint8_t result = cpu.aritmeticOperation_8bits(0x7F, 0x7F, CPUTest::AritmeticOperation::ADD, false);
    EXPECT_EQ(result, 0xFE);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ArithmeticOperationTest, Sub_MaxValueMinusMaxValue_ReturnsZero) {
    uint8_t result = cpu.aritmeticOperation_8bits(0xFF, 0xFF, CPUTest::AritmeticOperation::SUB, false);
    EXPECT_EQ(result, 0x00);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

// ==================== Tests de todos los flags simultáneamente ====================

TEST_F(ArithmeticOperationTest, Add_SetsMultipleFlags) {
    // Ejemplo que debería establecer múltiples flags
    uint8_t result = cpu.aritmeticOperation_8bits(0xFF, 0x01, CPUTest::AritmeticOperation::ADD, false);
    EXPECT_EQ(result, 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));   // resultado es 0
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));  // overflow
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));  // half carry
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));   // 0 bits set = par
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));  // bit 7 = 0
}

TEST_F(ArithmeticOperationTest, Sub_SetsMultipleFlags) {
    uint8_t result = cpu.aritmeticOperation_8bits(0x00, 0x01, CPUTest::AritmeticOperation::SUB, false);
    EXPECT_EQ(result, 0xFF);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));  // resultado no es 0
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));  // borrow
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));  // half borrow
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));   // 8 bits set = par
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));   // bit 7 = 1
}

// ==================== Tests de verificación de estado limpio ====================

TEST_F(ArithmeticOperationTest, FlagsAreUpdatedCorrectlyOnEachOperation) {
    // Primera operación
    uint8_t result1 = cpu.aritmeticOperation_8bits(0xFF, 0x01, CPUTest::AritmeticOperation::ADD, false);
    EXPECT_EQ(result1, 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    // Segunda operación debería actualizar los flags correctamente
    uint8_t result2 = cpu.aritmeticOperation_8bits(0x10, 0x20, CPUTest::AritmeticOperation::ADD, false);
    EXPECT_EQ(result2, 0x30);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

// ==================== Tests de operaciones específicas del 8080 ====================

TEST_F(ArithmeticOperationTest, Add_TypicalSequence_WorksCorrectly) {
    // Simula ADD A, B donde A=0x3A y B=0xC9
    uint8_t result = cpu.aritmeticOperation_8bits(0x3A, 0xC9, CPUTest::AritmeticOperation::ADD, false);
    EXPECT_EQ(result, 0x03);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(ArithmeticOperationTest, Sub_TypicalSequence_WorksCorrectly) {
    // Simula SUB A, B donde A=0x3E y B=0x3E
    uint8_t result = cpu.aritmeticOperation_8bits(0x3E, 0x3E, CPUTest::AritmeticOperation::SUB, false);
    EXPECT_EQ(result, 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}
