#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"

class INX_DCX_Test : public ::testing::Test {
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

// ==================== Tests para INX (Increment Register Pair) ====================

TEST_F(INX_DCX_Test, INX_BC_BasicIncrement) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x1000);
    
    uint8_t cycles = cpu.INX_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x1001);
    EXPECT_EQ(cycles, 5);
}

TEST_F(INX_DCX_Test, INX_DE_BasicIncrement) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x2000);
    
    uint8_t cycles = cpu.INX_RR<Registers::CombinedRegister::DE>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x2001);
    EXPECT_EQ(cycles, 5);
}

TEST_F(INX_DCX_Test, INX_HL_BasicIncrement) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3000);
    
    uint8_t cycles = cpu.INX_RR<Registers::CombinedRegister::HL>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x3001);
    EXPECT_EQ(cycles, 5);
}

TEST_F(INX_DCX_Test, INX_SP_BasicIncrement) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xFFF0);
    
    uint8_t cycles = cpu.INX_RR<Registers::CombinedRegister::SP>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xFFF1);
    EXPECT_EQ(cycles, 5);
}

TEST_F(INX_DCX_Test, INX_BC_FromZero) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x0000);
    
    cpu.INX_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x0001);
}

TEST_F(INX_DCX_Test, INX_BC_OverflowToZero) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0xFFFF);
    
    cpu.INX_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x0000);
}

TEST_F(INX_DCX_Test, INX_DE_AcrossByteCarry) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x10FF);
    
    cpu.INX_RR<Registers::CombinedRegister::DE>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x1100);
}

TEST_F(INX_DCX_Test, INX_HL_MaxValue) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xFFFE);
    
    cpu.INX_RR<Registers::CombinedRegister::HL>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xFFFF);
}

TEST_F(INX_DCX_Test, INX_DoesNotAffectFlags) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x1000);
    
    // Establecer todos los flags
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.INX_RR<Registers::CombinedRegister::BC>();
    
    // Verificar que ningún flag se modificó
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(INX_DCX_Test, INX_DoesNotAffectCarryOnOverflow) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0xFFFF);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.INX_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x0000);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY)); // CY no se afecta
}

TEST_F(INX_DCX_Test, INX_MultipleIncrements) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1000);
    
    cpu.INX_RR<Registers::CombinedRegister::HL>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1001);
    
    cpu.INX_RR<Registers::CombinedRegister::HL>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1002);
    
    cpu.INX_RR<Registers::CombinedRegister::HL>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1003);
}

TEST_F(INX_DCX_Test, INX_DoesNotAffectOtherRegisterPairs) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x1111);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x2222);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3333);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0x4444);
    
    cpu.INX_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x1112);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x2222);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x3333);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x4444);
}

// ==================== Tests para DCX (Decrement Register Pair) ====================

TEST_F(INX_DCX_Test, DCX_BC_BasicDecrement) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x1000);
    
    uint8_t cycles = cpu.DCX_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x0FFF);
    EXPECT_EQ(cycles, 5);
}

TEST_F(INX_DCX_Test, DCX_DE_BasicDecrement) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x2000);
    
    uint8_t cycles = cpu.DCX_RR<Registers::CombinedRegister::DE>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x1FFF);
    EXPECT_EQ(cycles, 5);
}

TEST_F(INX_DCX_Test, DCX_HL_BasicDecrement) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3000);
    
    uint8_t cycles = cpu.DCX_RR<Registers::CombinedRegister::HL>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2FFF);
    EXPECT_EQ(cycles, 5);
}

TEST_F(INX_DCX_Test, DCX_SP_BasicDecrement) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xFFF0);
    
    uint8_t cycles = cpu.DCX_RR<Registers::CombinedRegister::SP>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xFFEF);
    EXPECT_EQ(cycles, 5);
}

TEST_F(INX_DCX_Test, DCX_BC_ToZero) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x0001);
    
    cpu.DCX_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x0000);
}

TEST_F(INX_DCX_Test, DCX_BC_UnderflowToMax) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x0000);
    
    cpu.DCX_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0xFFFF);
}

TEST_F(INX_DCX_Test, DCX_DE_AcrossByteBorrow) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x1100);
    
    cpu.DCX_RR<Registers::CombinedRegister::DE>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x10FF);
}

TEST_F(INX_DCX_Test, DCX_HL_FromMaxValue) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xFFFF);
    
    cpu.DCX_RR<Registers::CombinedRegister::HL>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xFFFE);
}

TEST_F(INX_DCX_Test, DCX_DoesNotAffectFlags) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x1000);
    
    // Establecer todos los flags
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.DCX_RR<Registers::CombinedRegister::BC>();
    
    // Verificar que ningún flag se modificó
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(INX_DCX_Test, DCX_DoesNotAffectCarryOnUnderflow) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x0000);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.DCX_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0xFFFF);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY)); // CY no se afecta
}

TEST_F(INX_DCX_Test, DCX_MultipleDecrements) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1000);
    
    cpu.DCX_RR<Registers::CombinedRegister::HL>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0FFF);
    
    cpu.DCX_RR<Registers::CombinedRegister::HL>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0FFE);
    
    cpu.DCX_RR<Registers::CombinedRegister::HL>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0FFD);
}

TEST_F(INX_DCX_Test, DCX_DoesNotAffectOtherRegisterPairs) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x1111);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x2222);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3333);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0x4444);
    
    cpu.DCX_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x1110);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x2222);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x3333);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x4444);
}

// ==================== Tests de casos prácticos ====================

TEST_F(INX_DCX_Test, INX_DCX_Sequence) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1000);
    
    cpu.INX_RR<Registers::CombinedRegister::HL>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1001);
    
    cpu.INX_RR<Registers::CombinedRegister::HL>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1002);
    
    cpu.DCX_RR<Registers::CombinedRegister::HL>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1001);
    
    cpu.DCX_RR<Registers::CombinedRegister::HL>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1000);
}

TEST_F(INX_DCX_Test, PointerArithmetic_INX_HL) {
    // Simular incremento de puntero de memoria
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    
    // Avanzar 5 posiciones
    for (int i = 0; i < 5; i++) {
        cpu.INX_RR<Registers::CombinedRegister::HL>();
    }
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2005);
}

TEST_F(INX_DCX_Test, StackPointer_DCX_SP) {
    // Simular decremento de stack pointer
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xFFFF);
    
    cpu.DCX_RR<Registers::CombinedRegister::SP>();
    cpu.DCX_RR<Registers::CombinedRegister::SP>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xFFFD);
}

TEST_F(INX_DCX_Test, AllRegisterPairs_INX) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x1000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x2000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0x4000);
    
    cpu.INX_RR<Registers::CombinedRegister::BC>();
    cpu.INX_RR<Registers::CombinedRegister::DE>();
    cpu.INX_RR<Registers::CombinedRegister::HL>();
    cpu.INX_RR<Registers::CombinedRegister::SP>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x1001);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x2001);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x3001);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x4001);
}

TEST_F(INX_DCX_Test, AllRegisterPairs_DCX) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x1000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x2000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0x4000);
    
    cpu.DCX_RR<Registers::CombinedRegister::BC>();
    cpu.DCX_RR<Registers::CombinedRegister::DE>();
    cpu.DCX_RR<Registers::CombinedRegister::HL>();
    cpu.DCX_RR<Registers::CombinedRegister::SP>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x0FFF);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x1FFF);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2FFF);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x3FFF);
}

TEST_F(INX_DCX_Test, LoopCounter_DCX_BC) {
    // Simular contador de bucle descendente
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x0005);
    
    while (cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC) != 0) {
        cpu.DCX_RR<Registers::CombinedRegister::BC>();
    }
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x0000);
}

TEST_F(INX_DCX_Test, BoundaryCondition_0x7FFF_to_0x8000) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x7FFF);
    
    cpu.INX_RR<Registers::CombinedRegister::HL>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x8000);
}

TEST_F(INX_DCX_Test, BoundaryCondition_0x8000_to_0x7FFF) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x8000);
    
    cpu.DCX_RR<Registers::CombinedRegister::HL>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x7FFF);
}

TEST_F(INX_DCX_Test, RealisticUseCase_BufferTraversal) {
    // Simular recorrido de buffer con HL
    uint16_t bufferStart = 0x3000;
    uint16_t bufferEnd = 0x3010;
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, bufferStart);
    
    // Avanzar hasta el final del buffer
    while (cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL) < bufferEnd) {
        cpu.INX_RR<Registers::CombinedRegister::HL>();
    }
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), bufferEnd);
}
