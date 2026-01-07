#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"

class CPUFlagsTest : public ::testing::Test {
protected:
    CPUTest cpu;
};

// ==================== Tests para manageZeroFlag ====================

TEST_F(CPUFlagsTest, ManageZeroFlag_ValueIsZero_SetsFlag) {
    cpu.manageZeroFlag(0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(CPUFlagsTest, ManageZeroFlag_ValueIsNonZero_ClearsFlag) {
    cpu.manageZeroFlag(0x01);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    
    cpu.manageZeroFlag(0xFF);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    
    cpu.manageZeroFlag(0x80);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

// ==================== Tests para manageParityFlag ====================

TEST_F(CPUFlagsTest, ManageParityFlag_EvenNumberOfOnes_SetsFlag) {
    cpu.manageParityFlag(0x00); // 0 bits = par
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    
    cpu.manageParityFlag(0x03); // 2 bits = par
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    
    cpu.manageParityFlag(0x0F); // 4 bits = par
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    
    cpu.manageParityFlag(0x7E); // 6 bits = par
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    
    cpu.manageParityFlag(0xFF); // 8 bits = par
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(CPUFlagsTest, ManageParityFlag_OddNumberOfOnes_ClearsFlag) {
    cpu.manageParityFlag(0x01); // 1 bit = impar
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
    
    cpu.manageParityFlag(0x07); // 3 bits = impar
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
    
    cpu.manageParityFlag(0x1F); // 5 bits = impar
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
    
    cpu.manageParityFlag(0x7F); // 7 bits = impar
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
}

// ==================== Tests para manageSignedFlag ====================

TEST_F(CPUFlagsTest, ManageSignedFlag_BitSevenIsSet_SetsFlag) {
    cpu.manageSignedFlag(0x80);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    
    cpu.manageSignedFlag(0xFF);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    
    cpu.manageSignedFlag(0xC0);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(CPUFlagsTest, ManageSignedFlag_BitSevenIsClear_ClearsFlag) {
    cpu.manageSignedFlag(0x00);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
    
    cpu.manageSignedFlag(0x7F);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
    
    cpu.manageSignedFlag(0x01);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
}

// ==================== Tests para manageCarryFlag (ADD) ====================

TEST_F(CPUFlagsTest, ManageCarryFlag_Add_NoOverflow_ClearsFlag) {
    cpu.manageCarryFlag(0x10, 0x20, CPUTest::AritmeticOperation::ADD);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    cpu.manageCarryFlag(0x00, 0x00, CPUTest::AritmeticOperation::ADD);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    cpu.manageCarryFlag(0x7F, 0x7F, CPUTest::AritmeticOperation::ADD);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(CPUFlagsTest, ManageCarryFlag_Add_Overflow_SetsFlag) {
    cpu.manageCarryFlag(0xFF, 0x01, CPUTest::AritmeticOperation::ADD);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    cpu.manageCarryFlag(0xFF, 0xFF, CPUTest::AritmeticOperation::ADD);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    cpu.manageCarryFlag(0x80, 0x81, CPUTest::AritmeticOperation::ADD);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    cpu.manageCarryFlag(0xF0, 0x20, CPUTest::AritmeticOperation::ADD);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(CPUFlagsTest, ManageCarryFlag_Add_ExactOverflow_SetsFlag) {
    cpu.manageCarryFlag(0x80, 0x80, CPUTest::AritmeticOperation::ADD); // 0x100
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

// ==================== Tests para manageCarryFlag (SUB) ====================

TEST_F(CPUFlagsTest, ManageCarryFlag_Sub_NoBorrow_ClearsFlag) {
    cpu.manageCarryFlag(0x20, 0x10, CPUTest::AritmeticOperation::SUB);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    cpu.manageCarryFlag(0xFF, 0x00, CPUTest::AritmeticOperation::SUB);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    cpu.manageCarryFlag(0x50, 0x50, CPUTest::AritmeticOperation::SUB);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(CPUFlagsTest, ManageCarryFlag_Sub_Borrow_SetsFlag) {
    cpu.manageCarryFlag(0x10, 0x20, CPUTest::AritmeticOperation::SUB);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    cpu.manageCarryFlag(0x00, 0x01, CPUTest::AritmeticOperation::SUB);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    cpu.manageCarryFlag(0x7F, 0x80, CPUTest::AritmeticOperation::SUB);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

// ==================== Tests para manageAuxilaryCarryFlag (ADD) ====================

TEST_F(CPUFlagsTest, ManageAuxilaryCarryFlag_Add_NoHalfCarry_ClearsFlag) {
    cpu.manageAuxilaryCarryFlag(0x00, 0x00, CPUTest::AritmeticOperation::ADD);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
    
    cpu.manageAuxilaryCarryFlag(0x10, 0x20, CPUTest::AritmeticOperation::ADD);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
    
    cpu.manageAuxilaryCarryFlag(0x05, 0x09, CPUTest::AritmeticOperation::ADD);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(CPUFlagsTest, ManageAuxilaryCarryFlag_Add_HalfCarry_SetsFlag) {
    cpu.manageAuxilaryCarryFlag(0x0F, 0x01, CPUTest::AritmeticOperation::ADD);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
    
    cpu.manageAuxilaryCarryFlag(0x0F, 0x0F, CPUTest::AritmeticOperation::ADD);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
    
    cpu.manageAuxilaryCarryFlag(0x1F, 0x01, CPUTest::AritmeticOperation::ADD);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
    
    cpu.manageAuxilaryCarryFlag(0xAF, 0x05, CPUTest::AritmeticOperation::ADD);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(CPUFlagsTest, ManageAuxilaryCarryFlag_Add_ExactHalfCarry_SetsFlag) {
    cpu.manageAuxilaryCarryFlag(0x08, 0x08, CPUTest::AritmeticOperation::ADD); // nibble bajo = 0x10
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

// ==================== Tests para manageAuxilaryCarryFlag (SUB) ====================

TEST_F(CPUFlagsTest, ManageAuxilaryCarryFlag_Sub_NoHalfBorrow_ClearsFlag) {
    cpu.manageAuxilaryCarryFlag(0x20, 0x10, CPUTest::AritmeticOperation::SUB);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
    
    cpu.manageAuxilaryCarryFlag(0x0F, 0x05, CPUTest::AritmeticOperation::SUB);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
    
    cpu.manageAuxilaryCarryFlag(0xFF, 0xF0, CPUTest::AritmeticOperation::SUB);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(CPUFlagsTest, ManageAuxilaryCarryFlag_Sub_HalfBorrow_SetsFlag) {
    cpu.manageAuxilaryCarryFlag(0x00, 0x01, CPUTest::AritmeticOperation::SUB);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
    
    cpu.manageAuxilaryCarryFlag(0x10, 0x0F, CPUTest::AritmeticOperation::SUB);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
    
    cpu.manageAuxilaryCarryFlag(0x20, 0x01, CPUTest::AritmeticOperation::SUB);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

// ==================== Tests de integración ====================

TEST_F(CPUFlagsTest, MultipleFlagsCanBeSetIndependently) {
    cpu.manageZeroFlag(0x00);
    cpu.manageParityFlag(0xFF);
    cpu.manageSignedFlag(0x80);
    cpu.manageCarryFlag(0xFF, 0x01, CPUTest::AritmeticOperation::ADD);
    cpu.manageAuxilaryCarryFlag(0x0F, 0x01, CPUTest::AritmeticOperation::ADD);
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(CPUFlagsTest, FlagsCanBeCleared) {
    // Set all flags
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    
    // Clear them
    cpu.manageZeroFlag(0x01);
    cpu.manageParityFlag(0x01);
    cpu.manageSignedFlag(0x00);
    cpu.manageCarryFlag(0x10, 0x10, CPUTest::AritmeticOperation::ADD);
    cpu.manageAuxilaryCarryFlag(0x10, 0x10, CPUTest::AritmeticOperation::ADD);
    
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
}
