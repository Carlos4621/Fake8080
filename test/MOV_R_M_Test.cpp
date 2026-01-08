#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"
#include <array>

class MOV_R_M_Test : public ::testing::Test {
protected:
    CPUTest cpu;
    std::array<uint8_t, 0x10000> rom;  // 64KB ROM (tamaño real del 8080)

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

// ==================== Tests MOV R, M para cada registro ====================

TEST_F(MOV_R_M_Test, MOV_A_M) {
    rom[0x0010] = 0x42;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0010);
    
    uint8_t cycles = cpu.MOV_R_M<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x42);
    EXPECT_EQ(cycles, 7);
}

TEST_F(MOV_R_M_Test, MOV_B_M) {
    rom[0x0020] = 0x55;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0020);
    
    uint8_t cycles = cpu.MOV_R_M<Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x55);
    EXPECT_EQ(cycles, 7);
}

TEST_F(MOV_R_M_Test, MOV_C_M) {
    rom[0x0030] = 0xAA;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0030);
    
    cpu.MOV_R_M<Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0xAA);
}

TEST_F(MOV_R_M_Test, MOV_D_M) {
    rom[0x0040] = 0xFF;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0040);
    
    cpu.MOV_R_M<Registers::Register::D>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0xFF);
}

TEST_F(MOV_R_M_Test, MOV_E_M) {
    rom[0x0050] = 0x12;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0050);
    
    cpu.MOV_R_M<Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x12);
}

TEST_F(MOV_R_M_Test, MOV_H_M) {
    rom[0x0060] = 0x34;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0060);
    
    cpu.MOV_R_M<Registers::Register::H>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x34);
}

TEST_F(MOV_R_M_Test, MOV_L_M) {
    rom[0x0070] = 0x78;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0070);
    
    cpu.MOV_R_M<Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x78);
}

// ==================== Tests con diferentes direcciones ====================

TEST_F(MOV_R_M_Test, MOV_A_M_FromAddressZero) {
    rom[0x0000] = 0x99;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0000);
    
    cpu.MOV_R_M<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x99);
}

TEST_F(MOV_R_M_Test, MOV_A_M_FromHighAddress) {
    rom[0xFFFF] = 0x88;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xFFFF);
    
    cpu.MOV_R_M<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x88);
}

TEST_F(MOV_R_M_Test, MOV_B_M_SequentialReads) {
    rom[0x0010] = 0x11;
    rom[0x0011] = 0x22;
    rom[0x0012] = 0x33;
    
    // Leer de direcciones consecutivas
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0010);
    cpu.MOV_R_M<Registers::Register::B>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x11);
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0011);
    cpu.MOV_R_M<Registers::Register::B>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x22);
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0012);
    cpu.MOV_R_M<Registers::Register::B>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x33);
}

// ==================== Tests con valores especiales ====================

TEST_F(MOV_R_M_Test, MOV_A_M_ZeroValue) {
    rom[0x0020] = 0x00;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0020);
    
    cpu.MOV_R_M<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
}

TEST_F(MOV_R_M_Test, MOV_A_M_MaxValue) {
    rom[0x0030] = 0xFF;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0030);
    
    cpu.MOV_R_M<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
}

TEST_F(MOV_R_M_Test, MOV_B_M_Pattern) {
    rom[0x0040] = 0b10101010;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0040);
    
    cpu.MOV_R_M<Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0xAA);
}

// ==================== Tests de preservación ====================

TEST_F(MOV_R_M_Test, MOV_M_PreservesAllFlags) {
    rom[0x0010] = 0x42;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0010);
    
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.MOV_R_M<Registers::Register::A>();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(MOV_R_M_Test, MOV_M_OverwritesDestinationRegister) {
    rom[0x0010] = 0x42;
    cpu.registers_m.setRegister(Registers::Register::A, 0x99);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0010);
    
    cpu.MOV_R_M<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x42);
}

TEST_F(MOV_R_M_Test, MOV_M_PreservesHLRegister) {
    rom[0x0050] = 0x42;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0050);
    
    cpu.MOV_R_M<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0050);
}

TEST_F(MOV_R_M_Test, MOV_M_DoesNotModifyOtherRegisters) {
    rom[0x0010] = 0x99;
    cpu.registers_m.setRegister(Registers::Register::B, 0x22);
    cpu.registers_m.setRegister(Registers::Register::C, 0x33);
    cpu.registers_m.setRegister(Registers::Register::D, 0x44);
    cpu.registers_m.setRegister(Registers::Register::E, 0x55);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0010);
    
    cpu.MOV_R_M<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x99);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x22);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x33);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x44);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x55);
}

TEST_F(MOV_R_M_Test, MOV_M_DoesNotModifyMemory) {
    rom[0x0020] = 0x42;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0020);
    
    cpu.MOV_R_M<Registers::Register::A>();
    
    EXPECT_EQ(rom[0x0020], 0x42);
}

// ==================== Tests de casos típicos en programas 8080 ====================

TEST_F(MOV_R_M_Test, MOV_M_LoadArrayElement) {
    // Patrón típico: cargar elemento de un array
    // HL apunta al elemento del array
    rom[0x0010] = 0x42;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0010);
    
    cpu.MOV_R_M<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x42);
}

TEST_F(MOV_R_M_Test, MOV_M_ReadSequentialData) {
    // Patrón típico: leer datos secuenciales de memoria
    rom[0x0010] = 0x01;
    rom[0x0011] = 0x02;
    rom[0x0012] = 0x03;
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0010);
    cpu.MOV_R_M<Registers::Register::A>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x01);
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0011);
    cpu.MOV_R_M<Registers::Register::B>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x02);
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0012);
    cpu.MOV_R_M<Registers::Register::C>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x03);
}

TEST_F(MOV_R_M_Test, MOV_M_LoadFromStack) {
    // Patrón típico: cargar desde stack (simulado con HL)
    rom[0x00F0] = 0x42;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x00F0);
    
    cpu.MOV_R_M<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x42);
}

TEST_F(MOV_R_M_Test, MOV_M_ReadASCIIString) {
    // Patrón típico: leer string ASCII de memoria
    rom[0x0020] = 'A';
    rom[0x0021] = 'B';
    rom[0x0022] = 'C';
    rom[0x0023] = '\0';
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0020);
    cpu.MOV_R_M<Registers::Register::A>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 'A');
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0021);
    cpu.MOV_R_M<Registers::Register::A>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 'B');
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0022);
    cpu.MOV_R_M<Registers::Register::A>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 'C');
}

TEST_F(MOV_R_M_Test, MOV_M_LoadRegisterPairFromMemory) {
    // Patrón típico: cargar par de registros desde memoria
    rom[0x0010] = 0x34; // Byte bajo
    rom[0x0011] = 0x12; // Byte alto
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0010);
    cpu.MOV_R_M<Registers::Register::C>();
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0011);
    cpu.MOV_R_M<Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x34);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x12);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x1234);
}

TEST_F(MOV_R_M_Test, MOV_M_LoadParameter) {
    // Patrón típico: cargar parámetro para cálculo
    rom[0x0030] = 0x2A;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0030);
    
    cpu.MOV_R_M<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x2A);
}

TEST_F(MOV_R_M_Test, MOV_M_ReadFromIOBuffer) {
    // Patrón típico: leer desde buffer de I/O
    rom[0x0080] = 0x48; // 'H'
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0080);
    
    cpu.MOV_R_M<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x48);
}

TEST_F(MOV_R_M_Test, MOV_M_ReadLookupTable) {
    // Patrón típico: leer valor de tabla de lookup
    rom[0x0040] = 0x05;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0040);
    
    cpu.MOV_R_M<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x05);
}

TEST_F(MOV_R_M_Test, MOV_M_LoadConstant) {
    // Patrón típico: cargar constante desde memoria
    rom[0x0025] = 0xFF;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0025);
    
    cpu.MOV_R_M<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
}

TEST_F(MOV_R_M_Test, MOV_M_LoadAllRegistersFromMemory) {
    // Patrón típico: restaurar todos los registros desde memoria (context restore)
    rom[0x0060] = 0x11;
    rom[0x0061] = 0x22;
    rom[0x0062] = 0x33;
    rom[0x0063] = 0x44;
    rom[0x0064] = 0x55;
    
    uint16_t base = 0x0060;
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, base + 0);
    cpu.MOV_R_M<Registers::Register::A>();
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, base + 1);
    cpu.MOV_R_M<Registers::Register::B>();
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, base + 2);
    cpu.MOV_R_M<Registers::Register::C>();
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, base + 3);
    cpu.MOV_R_M<Registers::Register::D>();
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, base + 4);
    cpu.MOV_R_M<Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x11);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x22);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x33);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x44);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x55);
}

TEST_F(MOV_R_M_Test, MOV_M_ReadMultipleTimes) {
    // Patrón típico: leer el mismo valor múltiples veces
    rom[0x0050] = 0x42;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0050);
    
    cpu.MOV_R_M<Registers::Register::A>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x42);
    
    cpu.MOV_R_M<Registers::Register::B>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x42);
    
    cpu.MOV_R_M<Registers::Register::C>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x42);
}

TEST_F(MOV_R_M_Test, MOV_M_ReadWithDifferentHLValues) {
    // Patrón típico: leer desde diferentes ubicaciones
    rom[0x0100] = 0xAA;
    rom[0x0200] = 0xBB;
    rom[0x0300] = 0xCC;
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0100);
    cpu.MOV_R_M<Registers::Register::A>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xAA);
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0200);
    cpu.MOV_R_M<Registers::Register::A>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xBB);
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0300);
    cpu.MOV_R_M<Registers::Register::A>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xCC);
}

TEST_F(MOV_R_M_Test, MOV_H_M_ModifiesHL) {
    // Caso especial: MOV H,M modifica H que es parte de HL
    rom[0x0060] = 0x12;
    cpu.registers_m.setRegister(Registers::Register::H, 0x00);
    cpu.registers_m.setRegister(Registers::Register::L, 0x60);
    
    cpu.MOV_R_M<Registers::Register::H>();
    
    // H ahora es 0x12, por lo que HL = 0x1260
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x12);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1260);
}

TEST_F(MOV_R_M_Test, MOV_L_M_ModifiesHL) {
    // Caso especial: MOV L,M modifica L que es parte de HL
    rom[0x0070] = 0x34;
    cpu.registers_m.setRegister(Registers::Register::H, 0x00);
    cpu.registers_m.setRegister(Registers::Register::L, 0x70);
    
    cpu.MOV_R_M<Registers::Register::L>();
    
    // L ahora es 0x34, por lo que HL = 0x0034
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x34);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0034);
}
