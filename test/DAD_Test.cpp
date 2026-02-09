#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"

class DAD_Test : public ::testing::Test {
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

// ==================== Tests para DAD BC (Add BC to HL) ====================

TEST_F(DAD_Test, DAD_BC_BasicAddition) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x0500);
    
    uint8_t cycles = cpu.DAD_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1500);
    EXPECT_EQ(cycles, 10);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(DAD_Test, DAD_BC_AddZero) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1234);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x0000);
    
    cpu.DAD_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1234);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(DAD_Test, DAD_BC_WithCarry) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xFFFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x0001);
    
    cpu.DAD_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0000);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(DAD_Test, DAD_BC_LargeValues) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x8000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x8000);
    
    cpu.DAD_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0000);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(DAD_Test, DAD_BC_NoCarry_MaxWithoutOverflow) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x7FFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x7FFF);
    
    cpu.DAD_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xFFFE);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

// ==================== Tests para DAD DE (Add DE to HL) ====================

TEST_F(DAD_Test, DAD_DE_BasicAddition) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x0300);
    
    uint8_t cycles = cpu.DAD_RR<Registers::CombinedRegister::DE>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2300);
    EXPECT_EQ(cycles, 10);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(DAD_Test, DAD_DE_WithCarry) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xF000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x2000);
    
    cpu.DAD_RR<Registers::CombinedRegister::DE>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1000);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(DAD_Test, DAD_DE_BothZero) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x0000);
    
    cpu.DAD_RR<Registers::CombinedRegister::DE>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0000);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

// ==================== Tests para DAD HL (Add HL to HL - Double HL) ====================

TEST_F(DAD_Test, DAD_HL_DoubleHL) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1000);
    
    uint8_t cycles = cpu.DAD_RR<Registers::CombinedRegister::HL>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2000);
    EXPECT_EQ(cycles, 10);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(DAD_Test, DAD_HL_DoubleHL_WithCarry) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x8000);
    
    cpu.DAD_RR<Registers::CombinedRegister::HL>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0000);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(DAD_Test, DAD_HL_DoubleZero) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0000);
    
    cpu.DAD_RR<Registers::CombinedRegister::HL>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0000);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(DAD_Test, DAD_HL_DoubleMaxWithoutCarry) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x7FFF);
    
    cpu.DAD_RR<Registers::CombinedRegister::HL>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xFFFE);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

// ==================== Tests para DAD SP (Add SP to HL) ====================

TEST_F(DAD_Test, DAD_SP_BasicAddition) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0x0100);
    
    uint8_t cycles = cpu.DAD_RR<Registers::CombinedRegister::SP>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x3100);
    EXPECT_EQ(cycles, 10);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(DAD_Test, DAD_SP_WithCarry) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xFFFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0x0001);
    
    cpu.DAD_RR<Registers::CombinedRegister::SP>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0000);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(DAD_Test, DAD_SP_AddStackPointer) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xFFF0);
    
    cpu.DAD_RR<Registers::CombinedRegister::SP>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1FF0);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

// ==================== Tests de preservación de flags ====================

TEST_F(DAD_Test, DAD_PreservesOtherFlags) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x0500);
    
    // Establecer otros flags
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    
    cpu.DAD_RR<Registers::CombinedRegister::BC>();
    
    // Verificar que solo CY puede cambiar, los demás se preservan
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(DAD_Test, DAD_ClearsPreviousCarry) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x0500);
    
    // Establecer carry previo
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.DAD_RR<Registers::CombinedRegister::BC>();
    
    // DAD debe limpiar el carry si no hay overflow
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(DAD_Test, DAD_SetsCarryOnOverflow) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xFFFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x0001);
    
    // Carry inicialmente falso
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.DAD_RR<Registers::CombinedRegister::BC>();
    
    // DAD debe establecer carry en overflow
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

// ==================== Tests de preservación de registros ====================

TEST_F(DAD_Test, DAD_BC_PreservesBC) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x0500);
    
    cpu.DAD_RR<Registers::CombinedRegister::BC>();
    
    // BC no debe modificarse
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x0500);
}

TEST_F(DAD_Test, DAD_DE_PreservesDE) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x0300);
    
    cpu.DAD_RR<Registers::CombinedRegister::DE>();
    
    // DE no debe modificarse
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x0300);
}

TEST_F(DAD_Test, DAD_SP_PreservesSP) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0x0100);
    
    cpu.DAD_RR<Registers::CombinedRegister::SP>();
    
    // SP no debe modificarse
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x0100);
}

TEST_F(DAD_Test, DAD_BC_PreservesOtherRegisters) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x0500);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x1111);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0x2222);
    
    cpu.DAD_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x0500);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x1111);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x2222);
}

// ==================== Tests de casos prácticos ====================

TEST_F(DAD_Test, RealisticUseCase_PointerArithmetic) {
    // Simular avanzar puntero HL en múltiplos de BC
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x0010);
    
    // Avanzar 3 veces
    cpu.DAD_RR<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x3010);
    
    cpu.DAD_RR<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x3020);
    
    cpu.DAD_RR<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x3030);
}

TEST_F(DAD_Test, RealisticUseCase_AddressCalculation) {
    // Base address + offset
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000); // Base
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x0100); // Offset
    
    cpu.DAD_RR<Registers::CombinedRegister::DE>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2100);
}

TEST_F(DAD_Test, RealisticUseCase_MultiplyByTwo) {
    // DAD HL es la forma más rápida de multiplicar HL por 2
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0042);
    
    cpu.DAD_RR<Registers::CombinedRegister::HL>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0084);
}

TEST_F(DAD_Test, BoundaryCondition_CarryBoundary) {
    // Justo en el límite del carry
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xFFFE);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x0001);
    
    cpu.DAD_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xFFFF);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    
    // Una más para causar carry
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x0001);
    cpu.DAD_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0000);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(DAD_Test, SequentialAdditions) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x0100);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x0200);
    
    // HL = HL + BC
    cpu.DAD_RR<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1100);
    
    // HL = HL + DE
    cpu.DAD_RR<Registers::CombinedRegister::DE>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1300);
    
    // HL = HL + HL (double)
    cpu.DAD_RR<Registers::CombinedRegister::HL>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2600);
}

TEST_F(DAD_Test, MaxValueAdditions) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xFFFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0xFFFF);
    
    cpu.DAD_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xFFFE);
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(DAD_Test, ByteCarryPropagation) {
    // Verificar que el carry se propaga correctamente entre bytes
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x00FF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x0001);
    
    cpu.DAD_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0100);
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(DAD_Test, AllRegisterPairs_Sequential) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x0001);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x0002);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0x0004);
    
    // HL = 0 + 1 = 1
    cpu.DAD_RR<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0001);
    
    // HL = 1 + 2 = 3
    cpu.DAD_RR<Registers::CombinedRegister::DE>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0003);
    
    // HL = 3 + 4 = 7
    cpu.DAD_RR<Registers::CombinedRegister::SP>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0007);
    
    // HL = 7 + 7 = 14
    cpu.DAD_RR<Registers::CombinedRegister::HL>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x000E);
}
