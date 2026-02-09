#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"
#include <array>

class LXI_Test : public ::testing::Test {
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
    }
};

// ==================== Tests para LXI BC, d16 ====================

TEST_F(LXI_Test, LXI_BC_BasicLoad) {
    // LXI BC, 0x1234 (little endian: 0x34, 0x12)
    rom[0] = 0x34; // Low byte
    rom[1] = 0x12; // High byte
    cpu.setROM(rom);
    
    uint8_t cycles = cpu.LXI_RR_d16<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x1234);
    EXPECT_EQ(cycles, 10);
}

TEST_F(LXI_Test, LXI_BC_ZeroValue) {
    rom[0] = 0x00;
    rom[1] = 0x00;
    cpu.setROM(rom);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x0000);
}

TEST_F(LXI_Test, LXI_BC_MaxValue) {
    rom[0] = 0xFF;
    rom[1] = 0xFF;
    cpu.setROM(rom);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0xFFFF);
}

TEST_F(LXI_Test, LXI_BC_LittleEndianVerification) {
    // Verificar que el orden es little endian
    rom[0] = 0x78; // Low byte
    rom[1] = 0x56; // High byte
    cpu.setROM(rom);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x5678);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x56); // High byte en B
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x78); // Low byte en C
}

// ==================== Tests para LXI DE, d16 ====================

TEST_F(LXI_Test, LXI_DE_BasicLoad) {
    rom[0] = 0xCD; // Low byte
    rom[1] = 0xAB; // High byte
    cpu.setROM(rom);
    
    uint8_t cycles = cpu.LXI_RR_d16<Registers::CombinedRegister::DE>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0xABCD);
    EXPECT_EQ(cycles, 10);
}

TEST_F(LXI_Test, LXI_DE_LittleEndianVerification) {
    rom[0] = 0x21; // Low byte
    rom[1] = 0x43; // High byte
    cpu.setROM(rom);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::DE>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x4321);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x43); // High byte
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x21); // Low byte
}

// ==================== Tests para LXI HL, d16 ====================

TEST_F(LXI_Test, LXI_HL_BasicLoad) {
    rom[0] = 0x00; // Low byte
    rom[1] = 0x20; // High byte
    cpu.setROM(rom);
    
    uint8_t cycles = cpu.LXI_RR_d16<Registers::CombinedRegister::HL>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2000);
    EXPECT_EQ(cycles, 10);
}

TEST_F(LXI_Test, LXI_HL_LittleEndianVerification) {
    rom[0] = 0x87; // Low byte
    rom[1] = 0x65; // High byte
    cpu.setROM(rom);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::HL>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x6587);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x65); // High byte
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x87); // Low byte
}

TEST_F(LXI_Test, LXI_HL_AddressPointer) {
    // Caso típico: cargar dirección de memoria
    rom[0] = 0x00; // Low byte
    rom[1] = 0x30; // High byte (0x3000)
    cpu.setROM(rom);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::HL>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x3000);
}

// ==================== Tests para LXI SP, d16 ====================

TEST_F(LXI_Test, LXI_SP_BasicLoad) {
    rom[0] = 0xF0; // Low byte
    rom[1] = 0xFF; // High byte
    cpu.setROM(rom);
    
    uint8_t cycles = cpu.LXI_RR_d16<Registers::CombinedRegister::SP>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xFFF0);
    EXPECT_EQ(cycles, 10);
}

TEST_F(LXI_Test, LXI_SP_StackInitialization) {
    // Caso típico: inicializar stack pointer al final de RAM
    rom[0] = 0xFF;
    rom[1] = 0xFF;
    cpu.setROM(rom);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::SP>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xFFFF);
}

// ==================== Tests de preservación ====================

TEST_F(LXI_Test, LXI_PreservesAllFlags) {
    rom[0] = 0x34;
    rom[1] = 0x12;
    cpu.setROM(rom);
    
    // Establecer todos los flags
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::BC>();
    
    // Verificar que ningún flag se modificó
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(LXI_Test, LXI_BC_DoesNotAffectOtherRegisters) {
    rom[0] = 0x34;
    rom[1] = 0x12;
    cpu.setROM(rom);
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x1111);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2222);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0x3333);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x1111);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2222);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x3333);
}

TEST_F(LXI_Test, LXI_OverwritesPreviousValue) {
    rom[0] = 0x78;
    rom[1] = 0x56;
    cpu.setROM(rom);
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0xFFFF);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x5678);
}

// ==================== Tests de PC increment (readNextTwoBytes lee 2 bytes) ====================

TEST_F(LXI_Test, LXI_Sequential_PCIncrement) {
    // PC se incrementa 2 veces por cada LXI (readNextTwoBytes)
    // Primera instrucción: LXI BC, 0x1234
    rom[0] = 0x34;
    rom[1] = 0x12;
    // Segunda instrucción: LXI DE, 0x5678
    rom[2] = 0x78;
    rom[3] = 0x56;
    // Tercera instrucción: LXI HL, 0x9ABC
    rom[4] = 0xBC;
    rom[5] = 0x9A;
    cpu.setROM(rom);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x1234);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::DE>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x5678);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::HL>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x9ABC);
}

TEST_F(LXI_Test, LXI_AllRegisterPairs_Sequential) {
    // Cargar todos los pares de registros en secuencia
    rom[0] = 0x11; rom[1] = 0x11; // BC = 0x1111
    rom[2] = 0x22; rom[3] = 0x22; // DE = 0x2222
    rom[4] = 0x33; rom[5] = 0x33; // HL = 0x3333
    rom[6] = 0x44; rom[7] = 0x44; // SP = 0x4444
    cpu.setROM(rom);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::BC>();
    cpu.LXI_RR_d16<Registers::CombinedRegister::DE>();
    cpu.LXI_RR_d16<Registers::CombinedRegister::HL>();
    cpu.LXI_RR_d16<Registers::CombinedRegister::SP>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x1111);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x2222);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x3333);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x4444);
}

// ==================== Tests de casos prácticos ====================

TEST_F(LXI_Test, RealisticUseCase_InitializePointers) {
    // Inicializar HL como puntero a datos y SP como stack
    rom[0] = 0x00; rom[1] = 0x30; // HL = 0x3000 (data area)
    rom[2] = 0xFF; rom[3] = 0xFF; // SP = 0xFFFF (top of stack)
    cpu.setROM(rom);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::HL>();
    cpu.LXI_RR_d16<Registers::CombinedRegister::SP>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x3000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xFFFF);
}

TEST_F(LXI_Test, RealisticUseCase_LoadBaseAndOffset) {
    // BC = base address, DE = offset
    rom[0] = 0x00; rom[1] = 0x20; // BC = 0x2000 (base)
    rom[2] = 0x00; rom[3] = 0x01; // DE = 0x0100 (offset)
    cpu.setROM(rom);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::BC>();
    cpu.LXI_RR_d16<Registers::CombinedRegister::DE>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x2000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x0100);
}

TEST_F(LXI_Test, RealisticUseCase_LoadCounters) {
    // Cargar contadores para bucles
    rom[0] = 0x00; rom[1] = 0x01; // BC = 0x0100 (256 iterations)
    rom[2] = 0x00; rom[3] = 0x00; // DE = 0x0000 (counter)
    cpu.setROM(rom);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::BC>();
    cpu.LXI_RR_d16<Registers::CombinedRegister::DE>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x0100);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x0000);
}

TEST_F(LXI_Test, BoundaryCondition_LowAddresses) {
    rom[0] = 0x00; rom[1] = 0x00; // Address 0x0000
    rom[2] = 0x01; rom[3] = 0x00; // Address 0x0001
    cpu.setROM(rom);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x0000);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::DE>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x0001);
}

TEST_F(LXI_Test, BoundaryCondition_HighAddresses) {
    rom[0] = 0xFF; rom[1] = 0xFF; // Address 0xFFFF
    rom[2] = 0xFE; rom[3] = 0xFF; // Address 0xFFFE
    cpu.setROM(rom);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::HL>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xFFFF);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::SP>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xFFFE);
}

TEST_F(LXI_Test, ByteOrderVerification_AllPairs) {
    // Verificar orden little endian en todos los pares
    rom[0] = 0x01; rom[1] = 0x02; // 0x0201
    rom[2] = 0x03; rom[3] = 0x04; // 0x0403
    rom[4] = 0x05; rom[5] = 0x06; // 0x0605
    rom[6] = 0x07; rom[7] = 0x08; // 0x0807
    cpu.setROM(rom);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x0201);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::DE>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x0403);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::HL>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0605);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::SP>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x0807);
}

TEST_F(LXI_Test, TypicalProgramStart) {
    // Secuencia típica al inicio de un programa
    // 1. Inicializar SP
    // 2. Cargar puntero a datos en HL
    rom[0] = 0x00; rom[1] = 0xFF; // SP = 0xFF00
    rom[2] = 0x00; rom[3] = 0x20; // HL = 0x2000
    cpu.setROM(rom);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::SP>();
    cpu.LXI_RR_d16<Registers::CombinedRegister::HL>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xFF00);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2000);
}

TEST_F(LXI_Test, RepeatedLoads_SameRegister) {
    // Cargar múltiples valores en el mismo registro
    rom[0] = 0x11; rom[1] = 0x11;
    rom[2] = 0x22; rom[3] = 0x22;
    rom[4] = 0x33; rom[5] = 0x33;
    cpu.setROM(rom);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x1111);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x2222);
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x3333);
}
