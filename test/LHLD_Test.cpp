#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"
#include <array>

class LHLD_Test : public ::testing::Test {
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

// ==================== Tests básicos de LHLD ====================

TEST_F(LHLD_Test, LHLD_BasicLoad) {
    // LHLD 0x3000: Cargar HL desde dirección 0x3000
    // Dirección en little endian: 0x00, 0x30
    rom[0] = 0x00; // Low byte de dirección
    rom[1] = 0x30; // High byte de dirección
    
    // Datos en memoria: 0x1234
    rom[0x3000] = 0x34; // L
    rom[0x3001] = 0x12; // H
    cpu.setROM(rom);
    
    uint8_t cycles = cpu.LHLD_a16();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x34);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x12);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1234);
    EXPECT_EQ(cycles, 16);
}

TEST_F(LHLD_Test, LHLD_ZeroValue) {
    rom[0] = 0x00;
    rom[1] = 0x20;
    
    // Datos en memoria: 0x0000
    rom[0x2000] = 0x00;
    rom[0x2001] = 0x00;
    cpu.setROM(rom);
    
    cpu.LHLD_a16();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0000);
}

TEST_F(LHLD_Test, LHLD_MaxValue) {
    rom[0] = 0x00;
    rom[1] = 0x40;
    
    // Datos en memoria: 0xFFFF
    rom[0x4000] = 0xFF;
    rom[0x4001] = 0xFF;
    cpu.setROM(rom);
    
    cpu.LHLD_a16();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xFFFF);
}

TEST_F(LHLD_Test, LHLD_LittleEndianVerification) {
    // Verificar que L se carga desde dirección baja y H desde dirección alta
    rom[0] = 0x00;
    rom[1] = 0x50;
    
    rom[0x5000] = 0xCD; // L
    rom[0x5001] = 0xAB; // H
    cpu.setROM(rom);
    
    cpu.LHLD_a16();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0xCD);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0xAB);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xABCD);
}

// ==================== Tests de direcciones diferentes ====================

TEST_F(LHLD_Test, LHLD_LowAddress) {
    rom[0] = 0x10;
    rom[1] = 0x00;
    
    rom[0x0010] = 0x22;
    rom[0x0011] = 0x11;
    cpu.setROM(rom);
    
    cpu.LHLD_a16();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1122);
}

TEST_F(LHLD_Test, LHLD_HighAddress) {
    rom[0] = 0xFE;
    rom[1] = 0xFF;
    
    rom[0xFFFE] = 0x44;
    rom[0xFFFF] = 0x33;
    cpu.setROM(rom);
    
    cpu.LHLD_a16();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x3344);
}

TEST_F(LHLD_Test, LHLD_MiddleAddress) {
    rom[0] = 0x34;
    rom[1] = 0x12;
    
    rom[0x1234] = 0x66;
    rom[0x1235] = 0x55;
    cpu.setROM(rom);
    
    cpu.LHLD_a16();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x5566);
}

// ==================== Tests de preservación ====================

TEST_F(LHLD_Test, LHLD_PreservesAllFlags) {
    rom[0] = 0x00;
    rom[1] = 0x30;
    
    rom[0x3000] = 0x34;
    rom[0x3001] = 0x12;
    cpu.setROM(rom);
    
    // Establecer todos los flags
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.LHLD_a16();
    
    // Verificar que ningún flag se modificó
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(LHLD_Test, LHLD_OverwritesPreviousHL) {
    rom[0] = 0x00;
    rom[1] = 0x30;
    
    rom[0x3000] = 0x34;
    rom[0x3001] = 0x12;
    cpu.setROM(rom);
    
    // Establecer HL con valor previo
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xFFFF);
    
    cpu.LHLD_a16();
    
    // HL debe ser sobrescrito
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1234);
}

TEST_F(LHLD_Test, LHLD_PreservesOtherRegisters) {
    rom[0] = 0x00;
    rom[1] = 0x30;
    
    rom[0x3000] = 0x34;
    rom[0x3001] = 0x12;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xAA);
    cpu.registers_m.setRegister(Registers::Register::B, 0xBB);
    cpu.registers_m.setRegister(Registers::Register::C, 0xCC);
    cpu.registers_m.setRegister(Registers::Register::D, 0xDD);
    cpu.registers_m.setRegister(Registers::Register::E, 0xEE);
    
    cpu.LHLD_a16();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xAA);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0xBB);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0xCC);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0xDD);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0xEE);
}

// ==================== Tests de múltiples operaciones (PC increment) ====================

TEST_F(LHLD_Test, LHLD_Sequential_PCIncrement) {
    // Primera operación: LHLD 0x3000
    rom[0] = 0x00;
    rom[1] = 0x30;
    // Segunda operación: LHLD 0x4000
    rom[2] = 0x00;
    rom[3] = 0x40;
    // Tercera operación: LHLD 0x5000
    rom[4] = 0x00;
    rom[5] = 0x50;
    
    // Datos en memoria
    rom[0x3000] = 0x22;
    rom[0x3001] = 0x11;
    rom[0x4000] = 0x44;
    rom[0x4001] = 0x33;
    rom[0x5000] = 0x66;
    rom[0x5001] = 0x55;
    cpu.setROM(rom);
    
    cpu.LHLD_a16();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1122);
    
    cpu.LHLD_a16();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x3344);
    
    cpu.LHLD_a16();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x5566);
}

// ==================== Tests de casos prácticos ====================

TEST_F(LHLD_Test, RealisticUseCase_LoadPointer) {
    // Cargar un puntero guardado previamente
    rom[0] = 0x00;
    rom[1] = 0x80;
    
    rom[0x8000] = 0x00;
    rom[0x8001] = 0x30; // Puntero a 0x3000
    cpu.setROM(rom);
    
    cpu.LHLD_a16();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x3000);
}

TEST_F(LHLD_Test, RealisticUseCase_LoadCounter) {
    // Cargar un contador de 16 bits
    rom[0] = 0x10;
    rom[1] = 0x90;
    
    rom[0x9010] = 0x00;
    rom[0x9011] = 0x01; // Contador = 256
    cpu.setROM(rom);
    
    cpu.LHLD_a16();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0100);
}

TEST_F(LHLD_Test, RealisticUseCase_DataTable) {
    // Cargar valores desde una tabla de datos
    rom[0] = 0x00;
    rom[1] = 0x60;
    rom[2] = 0x02;
    rom[3] = 0x60;
    rom[4] = 0x04;
    rom[5] = 0x60;
    
    // Tabla de datos
    rom[0x6000] = 0x11;
    rom[0x6001] = 0x11;
    rom[0x6002] = 0x22;
    rom[0x6003] = 0x22;
    rom[0x6004] = 0x33;
    rom[0x6005] = 0x33;
    cpu.setROM(rom);
    
    cpu.LHLD_a16();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1111);
    
    cpu.LHLD_a16();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2222);
    
    cpu.LHLD_a16();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x3333);
}

TEST_F(LHLD_Test, RealisticUseCase_SHLD_LHLD_RoundTrip) {
    // Guardar con SHLD y recuperar con LHLD
    rom[0] = 0x00;
    rom[1] = 0x70; // SHLD 0x7000
    rom[2] = 0x00;
    rom[3] = 0x70; // LHLD 0x7000
    cpu.setROM(rom);
    
    // Guardar 0xABCD con SHLD
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xABCD);
    cpu.SHLD_a16();
    
    // Limpiar HL
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0000);
    
    // Recuperar con LHLD
    cpu.LHLD_a16();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xABCD);
}

TEST_F(LHLD_Test, BoundaryCondition_ConsecutiveAddresses) {
    // Cargar desde direcciones consecutivas
    rom[0] = 0x00;
    rom[1] = 0x70;
    rom[2] = 0x02;
    rom[3] = 0x70;
    
    rom[0x7000] = 0xBB;
    rom[0x7001] = 0xAA;
    rom[0x7002] = 0xDD;
    rom[0x7003] = 0xCC;
    cpu.setROM(rom);
    
    cpu.LHLD_a16();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xAABB);
    
    cpu.LHLD_a16();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xCCDD);
}

TEST_F(LHLD_Test, BoundaryCondition_SingleByteValues) {
    rom[0] = 0x00;
    rom[1] = 0x75;
    
    rom[0x7500] = 0xFF; // L
    rom[0x7501] = 0x00; // H
    cpu.setROM(rom);
    
    cpu.LHLD_a16();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0xFF);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x00);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x00FF);
}

TEST_F(LHLD_Test, EdgeCase_LoadFromCodeArea) {
    // Simular carga desde área de código
    rom[0] = 0x06;
    rom[1] = 0x00; // LHLD 0x0006
    
    // Datos "en el código"
    rom[0x0006] = 0x99;
    rom[0x0007] = 0x88;
    cpu.setROM(rom);
    
    cpu.LHLD_a16();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x8899);
}

TEST_F(LHLD_Test, PatternTest_AlternatingBytes) {
    rom[0] = 0x00;
    rom[1] = 0x77;
    
    rom[0x7700] = 0x55;
    rom[0x7701] = 0xAA;
    cpu.setROM(rom);
    
    cpu.LHLD_a16();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xAA55);
}

TEST_F(LHLD_Test, RepeatPattern_SameAddress) {
    rom[0] = 0x00;
    rom[1] = 0x78;
    rom[2] = 0x00;
    rom[3] = 0x78;
    
    rom[0x7800] = 0x44;
    rom[0x7801] = 0x33;
    cpu.setROM(rom);
    
    // Primera carga
    cpu.LHLD_a16();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x3344);
    
    // Segunda carga desde la misma dirección (debe dar el mismo resultado)
    cpu.LHLD_a16();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x3344);
}

TEST_F(LHLD_Test, AddressCalculation_LittleEndian) {
    // Verificar que la dirección se lee correctamente en little endian
    rom[0] = 0x34;
    rom[1] = 0x12; // Dirección = 0x1234
    
    rom[0x1234] = 0xCD;
    rom[0x1235] = 0xAB;
    cpu.setROM(rom);
    
    cpu.LHLD_a16();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xABCD);
}

TEST_F(LHLD_Test, LHLD_DifferentValuesInHL) {
    // Verificar que cada carga sobrescribe completamente HL
    rom[0] = 0x00;
    rom[1] = 0x80;
    rom[2] = 0x00;
    rom[3] = 0x81;
    
    rom[0x8000] = 0xFF;
    rom[0x8001] = 0xFF;
    rom[0x8100] = 0x01;
    rom[0x8101] = 0x00;
    cpu.setROM(rom);
    
    cpu.LHLD_a16();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xFFFF);
    
    cpu.LHLD_a16();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0001);
}

TEST_F(LHLD_Test, LHLD_ZeroToNonZero) {
    rom[0] = 0x00;
    rom[1] = 0x82;
    
    rom[0x8200] = 0x34;
    rom[0x8201] = 0x12;
    cpu.setROM(rom);
    
    // HL comienza en 0
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0000);
    
    cpu.LHLD_a16();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1234);
}

TEST_F(LHLD_Test, LHLD_WithAllBitPatterns) {
    rom[0] = 0x00;
    rom[1] = 0x83;
    
    // Patrón: todos los bits en L, ninguno en H
    rom[0x8300] = 0xFF;
    rom[0x8301] = 0x00;
    cpu.setROM(rom);
    
    cpu.LHLD_a16();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0xFF);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x00);
}
