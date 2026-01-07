#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"

class ADD_ADC_SUB_SBB_CMP_Test : public ::testing::Test {
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

// ==================== Tests para ADD con cada registro ====================

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, ADD_B_BasicAddition) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    cpu.registers_m.setRegister(Registers::Register::B, 0x20);
    
    uint8_t cycles = cpu.ADD_R<Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x30);
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, ADD_C_WithOverflow) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setRegister(Registers::Register::C, 0x01);
    
    uint8_t cycles = cpu.ADD_R<Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, ADD_D_WithHalfCarry) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x0F);
    cpu.registers_m.setRegister(Registers::Register::D, 0x01);
    
    uint8_t cycles = cpu.ADD_R<Registers::Register::D>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x10);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, ADD_E_ResultNegative) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x7F);
    cpu.registers_m.setRegister(Registers::Register::E, 0x01);
    
    uint8_t cycles = cpu.ADD_R<Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x80);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, ADD_H_ZeroResult) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setRegister(Registers::Register::H, 0x00);
    
    uint8_t cycles = cpu.ADD_R<Registers::Register::H>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, ADD_L_ParityFlag) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x01);
    cpu.registers_m.setRegister(Registers::Register::L, 0x02);
    
    uint8_t cycles = cpu.ADD_R<Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x03);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, ADD_A_DoubleValue) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x42);
    
    uint8_t cycles = cpu.ADD_R<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x84);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

// ==================== Tests para ADC (ADD with Carry) con cada registro ====================

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, ADC_B_WithCarryClear) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    cpu.registers_m.setRegister(Registers::Register::B, 0x20);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    uint8_t cycles = cpu.ADC_R<Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x30);
    EXPECT_EQ(cycles, 4);
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, ADC_C_WithCarrySet) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    cpu.registers_m.setRegister(Registers::Register::C, 0x20);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.ADC_R<Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x31);
    EXPECT_EQ(cycles, 4);
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, ADC_D_CarryCausesOverflow) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setRegister(Registers::Register::D, 0x00);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.ADC_R<Registers::Register::D>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, ADC_E_CarryCausesHalfCarry) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x0F);
    cpu.registers_m.setRegister(Registers::Register::E, 0x00);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.ADC_R<Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x10);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, ADC_H_ComplexCase) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x88);
    cpu.registers_m.setRegister(Registers::Register::H, 0x88);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.ADC_R<Registers::Register::H>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x11);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, ADC_L_MaxValues) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setRegister(Registers::Register::L, 0xFF);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.ADC_R<Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, ADC_A_WithCarry) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x40);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.ADC_R<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x81);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

// ==================== Tests para SUB con cada registro ====================

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, SUB_B_BasicSubtraction) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x30);
    cpu.registers_m.setRegister(Registers::Register::B, 0x10);
    
    uint8_t cycles = cpu.SUB_R<Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x20);
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, SUB_C_ResultZero) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x50);
    cpu.registers_m.setRegister(Registers::Register::C, 0x50);
    
    uint8_t cycles = cpu.SUB_R<Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, SUB_D_WithBorrow) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setRegister(Registers::Register::D, 0x01);
    
    uint8_t cycles = cpu.SUB_R<Registers::Register::D>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, SUB_E_WithHalfBorrow) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    cpu.registers_m.setRegister(Registers::Register::E, 0x01);
    
    uint8_t cycles = cpu.SUB_R<Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x0F);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, SUB_H_LargerFromSmaller) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    cpu.registers_m.setRegister(Registers::Register::H, 0x20);
    
    uint8_t cycles = cpu.SUB_R<Registers::Register::H>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xF0);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, SUB_L_ParityCheck) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x05);
    cpu.registers_m.setRegister(Registers::Register::L, 0x02);
    
    uint8_t cycles = cpu.SUB_R<Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x03);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, SUB_A_ResultZero) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x42);
    
    uint8_t cycles = cpu.SUB_R<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

// ==================== Tests para SBB (SUB with Borrow) con cada registro ====================

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, SBB_B_WithCarryClear) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x30);
    cpu.registers_m.setRegister(Registers::Register::B, 0x10);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    uint8_t cycles = cpu.SBB_R<Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x20);
    EXPECT_EQ(cycles, 4);
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, SBB_C_WithCarrySet) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x30);
    cpu.registers_m.setRegister(Registers::Register::C, 0x10);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.SBB_R<Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x1F);
    EXPECT_EQ(cycles, 4);
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, SBB_D_CarryCausesBorrow) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setRegister(Registers::Register::D, 0x00);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.SBB_R<Registers::Register::D>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, SBB_E_CarryCausesHalfBorrow) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    cpu.registers_m.setRegister(Registers::Register::E, 0x00);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.SBB_R<Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x0F);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, SBB_H_ComplexCase) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x50);
    cpu.registers_m.setRegister(Registers::Register::H, 0x25);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.SBB_R<Registers::Register::H>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x2A);
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, SBB_L_EdgeCase) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x01);
    cpu.registers_m.setRegister(Registers::Register::L, 0x00);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.SBB_R<Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, SBB_A_WithCarry) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x40);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.SBB_R<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

// ==================== Tests para CMP (Compare - no store result) con cada registro ====================

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, CMP_B_Equal_NoStoreResult) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x50);
    cpu.registers_m.setRegister(Registers::Register::B, 0x50);
    
    uint8_t cycles = cpu.CMP_R<Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x50); // No cambió
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, CMP_C_AGreaterThanC_NoStoreResult) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x30);
    cpu.registers_m.setRegister(Registers::Register::C, 0x10);
    
    uint8_t cycles = cpu.CMP_R<Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x30); // No cambió
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, CMP_D_ALessThanD_NoStoreResult) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    cpu.registers_m.setRegister(Registers::Register::D, 0x20);
    
    uint8_t cycles = cpu.CMP_R<Registers::Register::D>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x10); // No cambió
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY)); // Borrow indica A < D
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, CMP_E_WithZero_NoStoreResult) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x42);
    cpu.registers_m.setRegister(Registers::Register::E, 0x00);
    
    uint8_t cycles = cpu.CMP_R<Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x42); // No cambió
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, CMP_H_NegativeResult_NoStoreResult) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setRegister(Registers::Register::H, 0x01);
    
    uint8_t cycles = cpu.CMP_R<Registers::Register::H>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00); // No cambió
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S)); // Resultado sería negativo
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, CMP_L_ParityCheck_NoStoreResult) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x05);
    cpu.registers_m.setRegister(Registers::Register::L, 0x02);
    
    uint8_t cycles = cpu.CMP_R<Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x05); // No cambió
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P)); // 0x03 tiene paridad par
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, CMP_A_CompareWithItself_NoStoreResult) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x99);
    
    uint8_t cycles = cpu.CMP_R<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x99); // No cambió
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

// ==================== Tests de casos límite ====================

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, AllRegisters_ADD_MaxValue) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setRegister(Registers::Register::B, 0x01);
    
    cpu.ADD_R<Registers::Register::B>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, AllRegisters_SUB_MinValue) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setRegister(Registers::Register::C, 0x01);
    
    cpu.SUB_R<Registers::Register::C>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

// ==================== Tests combinados para verificar independencia ====================

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, Sequential_ADD_Then_SUB) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    cpu.registers_m.setRegister(Registers::Register::B, 0x05);
    cpu.registers_m.setRegister(Registers::Register::C, 0x03);
    
    // ADD B
    cpu.ADD_R<Registers::Register::B>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x15);
    
    // SUB C
    cpu.SUB_R<Registers::Register::C>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x12);
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, Sequential_ADC_Chain) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFE);
    cpu.registers_m.setRegister(Registers::Register::B, 0x01);
    cpu.registers_m.setRegister(Registers::Register::C, 0x01);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    // ADD B (0xFE + 0x01 = 0xFF, no carry)
    cpu.ADD_R<Registers::Register::B>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    // ADC C (0xFF + 0x01 + 0 = 0x00, carry set)
    cpu.ADC_R<Registers::Register::C>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, CMP_DoesNotAffectRegisters_But_AffectsFlags) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x80);
    cpu.registers_m.setRegister(Registers::Register::B, 0x40);
    cpu.registers_m.setRegister(Registers::Register::C, 0x40);
    
    // CMP B (no modifica A)
    cpu.CMP_R<Registers::Register::B>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x80);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    
    // Ahora SUB C (modifica A)
    cpu.SUB_R<Registers::Register::C>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x40);
}

// ==================== Tests para todos los flags simultáneamente ====================

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, AllFlags_ADD_Scenario) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setRegister(Registers::Register::B, 0x01);
    
    cpu.ADD_R<Registers::Register::B>();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));   // Resultado = 0
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));   // Paridad par
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));  // Carry
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));  // Auxiliary carry
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));  // No negativo
}

TEST_F(ADD_ADC_SUB_SBB_CMP_Test, AllFlags_SUB_Scenario) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setRegister(Registers::Register::D, 0x01);
    
    cpu.SUB_R<Registers::Register::D>();
    
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));  // Resultado != 0
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));   // 0xFF tiene paridad par
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));  // Borrow
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));  // Auxiliary borrow
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));   // Negativo
}
