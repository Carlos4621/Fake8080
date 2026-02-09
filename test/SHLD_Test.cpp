#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"
#include <array>

class SHLD_Test : public ::testing::Test {
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

// ==================== Tests básicos de SHLD ====================

TEST_F(SHLD_Test, SHLD_BasicStore) {
    // SHLD 0x3000: Almacenar HL en dirección 0x3000
    // Dirección en little endian: 0x00, 0x30
    rom[0] = 0x00; // Low byte de dirección
    rom[1] = 0x30; // High byte de dirección
    cpu.setROM(rom);
    
    // Configurar HL = 0x1234
    cpu.registers_m.setRegister(Registers::Register::H, 0x12);
    cpu.registers_m.setRegister(Registers::Register::L, 0x34);
    
    uint8_t cycles = cpu.SHLD_a16();
    
    // Verificar que L se almacenó en la dirección
    EXPECT_EQ(rom[0x3000], 0x34);
    // Verificar que H se almacenó en dirección + 1
    EXPECT_EQ(rom[0x3001], 0x12);
    EXPECT_EQ(cycles, 16);
}

TEST_F(SHLD_Test, SHLD_ZeroValue) {
    rom[0] = 0x00;
    rom[1] = 0x20;
    cpu.setROM(rom);
    
    // HL = 0x0000
    cpu.registers_m.setRegister(Registers::Register::H, 0x00);
    cpu.registers_m.setRegister(Registers::Register::L, 0x00);
    
    cpu.SHLD_a16();
    
    EXPECT_EQ(rom[0x2000], 0x00);
    EXPECT_EQ(rom[0x2001], 0x00);
}

TEST_F(SHLD_Test, SHLD_MaxValue) {
    rom[0] = 0x00;
    rom[1] = 0x40;
    cpu.setROM(rom);
    
    // HL = 0xFFFF
    cpu.registers_m.setRegister(Registers::Register::H, 0xFF);
    cpu.registers_m.setRegister(Registers::Register::L, 0xFF);
    
    cpu.SHLD_a16();
    
    EXPECT_EQ(rom[0x4000], 0xFF);
    EXPECT_EQ(rom[0x4001], 0xFF);
}

TEST_F(SHLD_Test, SHLD_LittleEndianVerification) {
    // Verificar que L se almacena en dirección baja y H en dirección alta
    rom[0] = 0x00;
    rom[1] = 0x50;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::H, 0xAB);
    cpu.registers_m.setRegister(Registers::Register::L, 0xCD);
    
    cpu.SHLD_a16();
    
    EXPECT_EQ(rom[0x5000], 0xCD); // L en dirección baja
    EXPECT_EQ(rom[0x5001], 0xAB); // H en dirección alta
}

// ==================== Tests de direcciones diferentes ====================

TEST_F(SHLD_Test, SHLD_LowAddress) {
    // Almacenar en dirección baja
    rom[0] = 0x10;
    rom[1] = 0x00;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::H, 0x11);
    cpu.registers_m.setRegister(Registers::Register::L, 0x22);
    
    cpu.SHLD_a16();
    
    EXPECT_EQ(rom[0x0010], 0x22);
    EXPECT_EQ(rom[0x0011], 0x11);
}

TEST_F(SHLD_Test, SHLD_HighAddress) {
    // Almacenar en dirección alta
    rom[0] = 0xFE;
    rom[1] = 0xFF;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::H, 0x33);
    cpu.registers_m.setRegister(Registers::Register::L, 0x44);
    
    cpu.SHLD_a16();
    
    EXPECT_EQ(rom[0xFFFE], 0x44);
    EXPECT_EQ(rom[0xFFFF], 0x33);
}

TEST_F(SHLD_Test, SHLD_MiddleAddress) {
    rom[0] = 0x34;
    rom[1] = 0x12;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::H, 0x55);
    cpu.registers_m.setRegister(Registers::Register::L, 0x66);
    
    cpu.SHLD_a16();
    
    EXPECT_EQ(rom[0x1234], 0x66);
    EXPECT_EQ(rom[0x1235], 0x55);
}

// ==================== Tests de preservación ====================

TEST_F(SHLD_Test, SHLD_PreservesAllFlags) {
    rom[0] = 0x00;
    rom[1] = 0x30;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::H, 0x12);
    cpu.registers_m.setRegister(Registers::Register::L, 0x34);
    
    // Establecer todos los flags
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.SHLD_a16();
    
    // Verificar que ningún flag se modificó
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(SHLD_Test, SHLD_PreservesHL) {
    rom[0] = 0x00;
    rom[1] = 0x30;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::H, 0x12);
    cpu.registers_m.setRegister(Registers::Register::L, 0x34);
    
    cpu.SHLD_a16();
    
    // HL no debe modificarse
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x12);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x34);
}

TEST_F(SHLD_Test, SHLD_PreservesOtherRegisters) {
    rom[0] = 0x00;
    rom[1] = 0x30;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xAA);
    cpu.registers_m.setRegister(Registers::Register::B, 0xBB);
    cpu.registers_m.setRegister(Registers::Register::C, 0xCC);
    cpu.registers_m.setRegister(Registers::Register::D, 0xDD);
    cpu.registers_m.setRegister(Registers::Register::E, 0xEE);
    cpu.registers_m.setRegister(Registers::Register::H, 0x12);
    cpu.registers_m.setRegister(Registers::Register::L, 0x34);
    
    cpu.SHLD_a16();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xAA);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0xBB);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0xCC);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0xDD);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0xEE);
}

// ==================== Tests de múltiples operaciones (PC increment) ====================

TEST_F(SHLD_Test, SHLD_Sequential_PCIncrement) {
    // Primera operación: SHLD 0x3000
    rom[0] = 0x00;
    rom[1] = 0x30;
    // Segunda operación: SHLD 0x4000
    rom[2] = 0x00;
    rom[3] = 0x40;
    // Tercera operación: SHLD 0x5000
    rom[4] = 0x00;
    rom[5] = 0x50;
    cpu.setROM(rom);
    
    // Primera SHLD
    cpu.registers_m.setRegister(Registers::Register::H, 0x11);
    cpu.registers_m.setRegister(Registers::Register::L, 0x22);
    cpu.SHLD_a16();
    EXPECT_EQ(rom[0x3000], 0x22);
    EXPECT_EQ(rom[0x3001], 0x11);
    
    // Segunda SHLD
    cpu.registers_m.setRegister(Registers::Register::H, 0x33);
    cpu.registers_m.setRegister(Registers::Register::L, 0x44);
    cpu.SHLD_a16();
    EXPECT_EQ(rom[0x4000], 0x44);
    EXPECT_EQ(rom[0x4001], 0x33);
    
    // Tercera SHLD
    cpu.registers_m.setRegister(Registers::Register::H, 0x55);
    cpu.registers_m.setRegister(Registers::Register::L, 0x66);
    cpu.SHLD_a16();
    EXPECT_EQ(rom[0x5000], 0x66);
    EXPECT_EQ(rom[0x5001], 0x55);
}

TEST_F(SHLD_Test, SHLD_OverwritePreviousData) {
    rom[0] = 0x00;
    rom[1] = 0x30;
    cpu.setROM(rom);
    
    // Establecer datos previos en memoria
    rom[0x3000] = 0xAA;
    rom[0x3001] = 0xBB;
    
    cpu.registers_m.setRegister(Registers::Register::H, 0x12);
    cpu.registers_m.setRegister(Registers::Register::L, 0x34);
    
    cpu.SHLD_a16();
    
    // Los datos previos deben ser sobrescritos
    EXPECT_EQ(rom[0x3000], 0x34);
    EXPECT_EQ(rom[0x3001], 0x12);
}

// ==================== Tests de casos prácticos ====================

TEST_F(SHLD_Test, RealisticUseCase_SavePointer) {
    // Guardar un puntero para uso posterior
    rom[0] = 0x00;
    rom[1] = 0x80; // Guardar en 0x8000 (área de datos guardados)
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::H, 0x30);
    cpu.registers_m.setRegister(Registers::Register::L, 0x00);
    
    cpu.SHLD_a16();
    
    EXPECT_EQ(rom[0x8000], 0x00);
    EXPECT_EQ(rom[0x8001], 0x30);
}

TEST_F(SHLD_Test, RealisticUseCase_SaveCounter) {
    // Guardar un contador de 16 bits
    rom[0] = 0x10;
    rom[1] = 0x90;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::H, 0x01);
    cpu.registers_m.setRegister(Registers::Register::L, 0x00); // Contador = 256
    
    cpu.SHLD_a16();
    
    EXPECT_EQ(rom[0x9010], 0x00);
    EXPECT_EQ(rom[0x9011], 0x01);
}

TEST_F(SHLD_Test, RealisticUseCase_DataTable) {
    // Guardar valores en una tabla de datos
    rom[0] = 0x00;
    rom[1] = 0x60;
    rom[2] = 0x02;
    rom[3] = 0x60;
    rom[4] = 0x04;
    rom[5] = 0x60;
    cpu.setROM(rom);
    
    // Entrada 0: 0x1111
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1111);
    cpu.SHLD_a16();
    
    // Entrada 1: 0x2222
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2222);
    cpu.SHLD_a16();
    
    // Entrada 2: 0x3333
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3333);
    cpu.SHLD_a16();
    
    EXPECT_EQ(rom[0x6000], 0x11);
    EXPECT_EQ(rom[0x6001], 0x11);
    EXPECT_EQ(rom[0x6002], 0x22);
    EXPECT_EQ(rom[0x6003], 0x22);
    EXPECT_EQ(rom[0x6004], 0x33);
    EXPECT_EQ(rom[0x6005], 0x33);
}

TEST_F(SHLD_Test, BoundaryCondition_ConsecutiveAddresses) {
    // Almacenar en direcciones consecutivas
    rom[0] = 0x00;
    rom[1] = 0x70;
    rom[2] = 0x02;
    rom[3] = 0x70;
    cpu.setROM(rom);
    
    // Primera SHLD en 0x7000 (ocupará 0x7000 y 0x7001)
    cpu.registers_m.setRegister(Registers::Register::H, 0xAA);
    cpu.registers_m.setRegister(Registers::Register::L, 0xBB);
    cpu.SHLD_a16();
    
    // Segunda SHLD en 0x7002 (ocupará 0x7002 y 0x7003)
    cpu.registers_m.setRegister(Registers::Register::H, 0xCC);
    cpu.registers_m.setRegister(Registers::Register::L, 0xDD);
    cpu.SHLD_a16();
    
    EXPECT_EQ(rom[0x7000], 0xBB);
    EXPECT_EQ(rom[0x7001], 0xAA);
    EXPECT_EQ(rom[0x7002], 0xDD);
    EXPECT_EQ(rom[0x7003], 0xCC);
}

TEST_F(SHLD_Test, BoundaryCondition_SingleByteValues) {
    rom[0] = 0x00;
    rom[1] = 0x75;
    cpu.setROM(rom);
    
    // H = 0x00, L = valor significativo
    cpu.registers_m.setRegister(Registers::Register::H, 0x00);
    cpu.registers_m.setRegister(Registers::Register::L, 0xFF);
    
    cpu.SHLD_a16();
    
    EXPECT_EQ(rom[0x7500], 0xFF);
    EXPECT_EQ(rom[0x7501], 0x00);
}

TEST_F(SHLD_Test, EdgeCase_AdjacentToCode) {
    // Simular almacenamiento cerca del código (después de las instrucciones)
    rom[0] = 0x06;
    rom[1] = 0x00; // Dirección 0x0006 (justo después de esta instrucción)
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::H, 0x88);
    cpu.registers_m.setRegister(Registers::Register::L, 0x99);
    
    cpu.SHLD_a16();
    
    EXPECT_EQ(rom[0x0006], 0x99);
    EXPECT_EQ(rom[0x0007], 0x88);
}

TEST_F(SHLD_Test, PatternTest_AlternatingBytes) {
    rom[0] = 0x00;
    rom[1] = 0x77;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::H, 0xAA);
    cpu.registers_m.setRegister(Registers::Register::L, 0x55);
    
    cpu.SHLD_a16();
    
    EXPECT_EQ(rom[0x7700], 0x55);
    EXPECT_EQ(rom[0x7701], 0xAA);
}

TEST_F(SHLD_Test, RepeatPattern_SameAddress) {
    rom[0] = 0x00;
    rom[1] = 0x78;
    rom[2] = 0x00;
    rom[3] = 0x78;
    cpu.setROM(rom);
    
    // Primera escritura
    cpu.registers_m.setRegister(Registers::Register::H, 0x11);
    cpu.registers_m.setRegister(Registers::Register::L, 0x22);
    cpu.SHLD_a16();
    
    // Segunda escritura en la misma dirección (debe sobrescribir)
    cpu.registers_m.setRegister(Registers::Register::H, 0x33);
    cpu.registers_m.setRegister(Registers::Register::L, 0x44);
    cpu.SHLD_a16();
    
    EXPECT_EQ(rom[0x7800], 0x44);
    EXPECT_EQ(rom[0x7801], 0x33);
}

TEST_F(SHLD_Test, AddressCalculation_LittleEndian) {
    // Verificar que la dirección se lee correctamente en little endian
    rom[0] = 0x34;
    rom[1] = 0x12; // Dirección = 0x1234
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::H, 0xAB);
    cpu.registers_m.setRegister(Registers::Register::L, 0xCD);
    
    cpu.SHLD_a16();
    
    EXPECT_EQ(rom[0x1234], 0xCD);
    EXPECT_EQ(rom[0x1235], 0xAB);
}
