#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"
#include <array>

class PUSH_POP_Test : public ::testing::Test {
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
        
        // Inicializar SP en 0xF000 (área alta de memoria, típico para stack)
        // usando LXI SP, 0xF000
        rom[0] = 0x00;  // Low byte de 0xF000
        rom[1] = 0xF0;  // High byte de 0xF000
        cpu.setROM(rom);
        cpu.LXI_RR_d16<Registers::CombinedRegister::SP>();
    }
};

// ==================== PUSH BC Tests ====================

TEST_F(PUSH_POP_Test, PUSH_BC_BasicOperation) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x1234);
    
    uint8_t cycles = cpu.PUSH_RR<Registers::CombinedRegister::BC>();
    
    // SP debe decrementarse en 2
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFE);
    
    // Verificar que los datos se almacenaron en el stack (little endian en memoria de stack)
    // High byte (0x12) se almacena primero, luego low byte (0x34)
    EXPECT_EQ(cpu.rom_m[0xEFFF], 0x12);  // High byte en SP+1
    EXPECT_EQ(cpu.rom_m[0xEFFE], 0x34);  // Low byte en SP
    
    EXPECT_EQ(cycles, 11);
}

TEST_F(PUSH_POP_Test, PUSH_BC_ZeroValue) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x0000);
    
    uint8_t cycles = cpu.PUSH_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFE);
    EXPECT_EQ(cpu.rom_m[0xEFFF], 0x00);
    EXPECT_EQ(cpu.rom_m[0xEFFE], 0x00);
    EXPECT_EQ(cycles, 11);
}

TEST_F(PUSH_POP_Test, PUSH_BC_MaxValue) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0xFFFF);
    
    uint8_t cycles = cpu.PUSH_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFE);
    EXPECT_EQ(cpu.rom_m[0xEFFF], 0xFF);
    EXPECT_EQ(cpu.rom_m[0xEFFE], 0xFF);
    EXPECT_EQ(cycles, 11);
}

// ==================== PUSH DE Tests ====================

TEST_F(PUSH_POP_Test, PUSH_DE_BasicOperation) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x5678);
    
    uint8_t cycles = cpu.PUSH_RR<Registers::CombinedRegister::DE>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFE);
    EXPECT_EQ(cpu.rom_m[0xEFFF], 0x56);
    EXPECT_EQ(cpu.rom_m[0xEFFE], 0x78);
    EXPECT_EQ(cycles, 11);
}

// ==================== PUSH HL Tests ====================

TEST_F(PUSH_POP_Test, PUSH_HL_BasicOperation) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xABCD);
    
    uint8_t cycles = cpu.PUSH_RR<Registers::CombinedRegister::HL>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFE);
    EXPECT_EQ(cpu.rom_m[0xEFFF], 0xAB);
    EXPECT_EQ(cpu.rom_m[0xEFFE], 0xCD);
    EXPECT_EQ(cycles, 11);
}

// ==================== POP BC Tests ====================

TEST_F(PUSH_POP_Test, POP_BC_BasicOperation) {
    // Preparar datos en el stack
    rom[0xEFFE] = 0x34;  // Low byte
    rom[0xEFFF] = 0x12;  // High byte
    cpu.setROM(rom);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xEFFE);
    
    uint8_t cycles = cpu.POP_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x1234);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
    EXPECT_EQ(cycles, 10);
}

TEST_F(PUSH_POP_Test, POP_BC_ZeroValue) {
    rom[0xEFFE] = 0x00;
    rom[0xEFFF] = 0x00;
    cpu.setROM(rom);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xEFFE);
    
    uint8_t cycles = cpu.POP_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x0000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
    EXPECT_EQ(cycles, 10);
}

TEST_F(PUSH_POP_Test, POP_BC_MaxValue) {
    rom[0xEFFE] = 0xFF;
    rom[0xEFFF] = 0xFF;
    cpu.setROM(rom);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xEFFE);
    
    uint8_t cycles = cpu.POP_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0xFFFF);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
    EXPECT_EQ(cycles, 10);
}

// ==================== POP DE Tests ====================

TEST_F(PUSH_POP_Test, POP_DE_BasicOperation) {
    rom[0xEFFE] = 0x78;
    rom[0xEFFF] = 0x56;
    cpu.setROM(rom);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xEFFE);
    
    uint8_t cycles = cpu.POP_RR<Registers::CombinedRegister::DE>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x5678);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
    EXPECT_EQ(cycles, 10);
}

// ==================== POP HL Tests ====================

TEST_F(PUSH_POP_Test, POP_HL_BasicOperation) {
    rom[0xEFFE] = 0xCD;
    rom[0xEFFF] = 0xAB;
    cpu.setROM(rom);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xEFFE);
    
    uint8_t cycles = cpu.POP_RR<Registers::CombinedRegister::HL>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xABCD);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
    EXPECT_EQ(cycles, 10);
}

// ==================== PUSH/POP Round-trip Tests ====================

TEST_F(PUSH_POP_Test, PUSH_POP_BC_RoundTrip) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x1234);
    uint16_t originalSP = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    
    cpu.PUSH_RR<Registers::CombinedRegister::BC>();
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x0000);  // Clear BC
    cpu.POP_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x1234);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), originalSP);
}

TEST_F(PUSH_POP_Test, PUSH_POP_DE_RoundTrip) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x5678);
    uint16_t originalSP = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    
    cpu.PUSH_RR<Registers::CombinedRegister::DE>();
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x0000);
    cpu.POP_RR<Registers::CombinedRegister::DE>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x5678);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), originalSP);
}

TEST_F(PUSH_POP_Test, PUSH_POP_HL_RoundTrip) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xABCD);
    uint16_t originalSP = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    
    cpu.PUSH_RR<Registers::CombinedRegister::HL>();
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0000);
    cpu.POP_RR<Registers::CombinedRegister::HL>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xABCD);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), originalSP);
}

// ==================== Multiple PUSH Operations ====================

TEST_F(PUSH_POP_Test, MultiplePUSH_StackGrowsDown) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x1111);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x2222);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3333);
    
    cpu.PUSH_RR<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFE);
    
    cpu.PUSH_RR<Registers::CombinedRegister::DE>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFC);
    
    cpu.PUSH_RR<Registers::CombinedRegister::HL>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFA);
    
    // Verificar datos en stack
    EXPECT_EQ(cpu.rom_m[0xEFFF], 0x11);  // BC high
    EXPECT_EQ(cpu.rom_m[0xEFFE], 0x11);  // BC low
    EXPECT_EQ(cpu.rom_m[0xEFFD], 0x22);  // DE high
    EXPECT_EQ(cpu.rom_m[0xEFFC], 0x22);  // DE low
    EXPECT_EQ(cpu.rom_m[0xEFFB], 0x33);  // HL high
    EXPECT_EQ(cpu.rom_m[0xEFFA], 0x33);  // HL low
}

TEST_F(PUSH_POP_Test, MultiplePOP_LIFO_Order) {
    // Preparar stack con datos
    rom[0xEFFA] = 0x33; rom[0xEFFB] = 0x33;  // HL
    rom[0xEFFC] = 0x22; rom[0xEFFD] = 0x22;  // DE
    rom[0xEFFE] = 0x11; rom[0xEFFF] = 0x11;  // BC
    cpu.setROM(rom);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xEFFA);
    
    // Pop en orden LIFO (Last In, First Out)
    cpu.POP_RR<Registers::CombinedRegister::HL>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x3333);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFC);
    
    cpu.POP_RR<Registers::CombinedRegister::DE>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x2222);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFE);
    
    cpu.POP_RR<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x1111);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
}

// ==================== Flag Preservation Tests ====================

TEST_F(PUSH_POP_Test, PUSH_PreservesFlags) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x1234);
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, false);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, false);
    
    cpu.PUSH_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(PUSH_POP_Test, POP_PreservesFlags) {
    rom[0xEFFE] = 0x34;
    rom[0xEFFF] = 0x12;
    cpu.setROM(rom);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xEFFE);
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, false);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, false);
    
    cpu.POP_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

// ==================== Register Preservation Tests ====================

TEST_F(PUSH_POP_Test, PUSH_BC_PreservesOtherRegisters) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x1234);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x5678);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xABCD);
    cpu.registers_m.setRegister(Registers::Register::A, 0xEF);
    
    cpu.PUSH_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x1234);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x5678);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xABCD);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xEF);
}

TEST_F(PUSH_POP_Test, POP_DE_PreservesOtherRegisters) {
    rom[0xEFFE] = 0x78;
    rom[0xEFFF] = 0x56;
    cpu.setROM(rom);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xEFFE);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x1234);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xABCD);
    cpu.registers_m.setRegister(Registers::Register::A, 0xEF);
    
    cpu.POP_RR<Registers::CombinedRegister::DE>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x1234);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xABCD);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xEF);
}

// ==================== Realistic Use Cases ====================

TEST_F(PUSH_POP_Test, RealisticUseCase_FunctionCallSimulation) {
    // Simular guardar contexto antes de llamar a una función
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x1234);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x5678);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xABCD);
    
    // Guardar registros en stack
    cpu.PUSH_RR<Registers::CombinedRegister::BC>();
    cpu.PUSH_RR<Registers::CombinedRegister::DE>();
    cpu.PUSH_RR<Registers::CombinedRegister::HL>();
    
    // Simular uso de registros en la función
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x0000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x0000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0000);
    
    // Restaurar registros desde stack (en orden inverso)
    cpu.POP_RR<Registers::CombinedRegister::HL>();
    cpu.POP_RR<Registers::CombinedRegister::DE>();
    cpu.POP_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x1234);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x5678);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xABCD);
}

TEST_F(PUSH_POP_Test, RealisticUseCase_DataSwap) {
    // Intercambiar valores entre BC y DE usando el stack
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x1111);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x2222);
    
    cpu.PUSH_RR<Registers::CombinedRegister::BC>();
    cpu.PUSH_RR<Registers::CombinedRegister::DE>();
    cpu.POP_RR<Registers::CombinedRegister::BC>();
    cpu.POP_RR<Registers::CombinedRegister::DE>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x2222);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x1111);
}

// ==================== Edge Cases ====================

TEST_F(PUSH_POP_Test, EdgeCase_PUSHAtLowMemory) {
    // Inicializar SP en área baja de memoria
    rom[0] = 0x10;  // Low byte
    rom[1] = 0x00;  // High byte -> 0x0010
    cpu.setROM(rom);
    cpu.LXI_RR_d16<Registers::CombinedRegister::SP>();
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0xAAAA);
    
    cpu.PUSH_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x000E);
    EXPECT_EQ(cpu.rom_m[0x000F], 0xAA);
    EXPECT_EQ(cpu.rom_m[0x000E], 0xAA);
}

TEST_F(PUSH_POP_Test, EdgeCase_StackNearBoundary) {
    // SP cerca del límite de memoria
    rom[0] = 0x02;  // Low byte
    rom[1] = 0x00;  // High byte -> 0x0002
    cpu.setROM(rom);
    cpu.LXI_RR_d16<Registers::CombinedRegister::SP>();
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0xBBBB);
    
    cpu.PUSH_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x0000);
    EXPECT_EQ(cpu.rom_m[0x0001], 0xBB);
    EXPECT_EQ(cpu.rom_m[0x0000], 0xBB);
}

TEST_F(PUSH_POP_Test, EdgeCase_DeepStack) {
    // Probar múltiples PUSH/POP en secuencia
    for (int i = 0; i < 10; i++) {
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x0100 + i);
        cpu.PUSH_RR<Registers::CombinedRegister::BC>();
    }
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFEC);
    
    // Pop todos los valores
    for (int i = 9; i >= 0; i--) {
        cpu.POP_RR<Registers::CombinedRegister::BC>();
        EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x0100 + i);
    }
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
}

// ==================== Pattern Tests ====================

TEST_F(PUSH_POP_Test, PatternTest_AlternatingPushPop) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x1111);
    cpu.PUSH_RR<Registers::CombinedRegister::BC>();
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x2222);
    cpu.PUSH_RR<Registers::CombinedRegister::DE>();
    
    cpu.POP_RR<Registers::CombinedRegister::HL>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2222);
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x3333);
    cpu.PUSH_RR<Registers::CombinedRegister::DE>();
    
    cpu.POP_RR<Registers::CombinedRegister::HL>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x3333);
    
    cpu.POP_RR<Registers::CombinedRegister::HL>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1111);
}

TEST_F(PUSH_POP_Test, PatternTest_SameRegisterMultipleTimes) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0xAAAA);
    cpu.PUSH_RR<Registers::CombinedRegister::BC>();
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0xBBBB);
    cpu.PUSH_RR<Registers::CombinedRegister::BC>();
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0xCCCC);
    cpu.PUSH_RR<Registers::CombinedRegister::BC>();
    
    cpu.POP_RR<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0xCCCC);
    
    cpu.POP_RR<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0xBBBB);
    
    cpu.POP_RR<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0xAAAA);
}

// ==================== Byte Order Verification ====================

TEST_F(PUSH_POP_Test, ByteOrder_VerifyStackLayout) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x12AB);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x34CD);
    
    cpu.PUSH_RR<Registers::CombinedRegister::BC>();
    cpu.PUSH_RR<Registers::CombinedRegister::DE>();
    
    // Verificar orden de bytes en memoria
    // BC: 0x12AB -> [0xEFFF]=0x12, [0xEFFE]=0xAB
    EXPECT_EQ(cpu.rom_m[0xEFFF], 0x12);
    EXPECT_EQ(cpu.rom_m[0xEFFE], 0xAB);
    // DE: 0x34CD -> [0xEFFD]=0x34, [0xEFFC]=0xCD
    EXPECT_EQ(cpu.rom_m[0xEFFD], 0x34);
    EXPECT_EQ(cpu.rom_m[0xEFFC], 0xCD);
}
