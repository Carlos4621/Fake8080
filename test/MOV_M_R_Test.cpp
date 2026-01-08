#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"
#include <array>

class MOV_M_R_Test : public ::testing::Test {
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

// ==================== Tests MOV M, R para cada registro ====================

TEST_F(MOV_M_R_Test, MOV_M_A) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x42);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0010);
    
    uint8_t cycles = cpu.MOV_M_R<Registers::Register::A>();
    
    EXPECT_EQ(rom[0x0010], 0x42);
    EXPECT_EQ(cycles, 7);
}

TEST_F(MOV_M_R_Test, MOV_M_B) {
    cpu.registers_m.setRegister(Registers::Register::B, 0x55);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0020);
    
    uint8_t cycles = cpu.MOV_M_R<Registers::Register::B>();
    
    EXPECT_EQ(rom[0x0020], 0x55);
    EXPECT_EQ(cycles, 7);
}

TEST_F(MOV_M_R_Test, MOV_M_C) {
    cpu.registers_m.setRegister(Registers::Register::C, 0xAA);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0030);
    
    uint8_t cycles = cpu.MOV_M_R<Registers::Register::C>();
    
    EXPECT_EQ(rom[0x0030], 0xAA);
    EXPECT_EQ(cycles, 7);
}

TEST_F(MOV_M_R_Test, MOV_M_D) {
    cpu.registers_m.setRegister(Registers::Register::D, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0040);
    
    cpu.MOV_M_R<Registers::Register::D>();
    
    EXPECT_EQ(rom[0x0040], 0xFF);
}

TEST_F(MOV_M_R_Test, MOV_M_E) {
    cpu.registers_m.setRegister(Registers::Register::E, 0x12);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0050);
    
    cpu.MOV_M_R<Registers::Register::E>();
    
    EXPECT_EQ(rom[0x0050], 0x12);
}

TEST_F(MOV_M_R_Test, MOV_M_H) {
    // Caso especial: MOV M,H donde H es parte de la dirección HL
    cpu.registers_m.setRegister(Registers::Register::H, 0x34);
    cpu.registers_m.setRegister(Registers::Register::L, 0x60);
    
    uint8_t cycles = cpu.MOV_M_R<Registers::Register::H>();
    
    // HL = 0x3460, H contiene 0x34 que se escribe en esa dirección
    EXPECT_EQ(rom[0x3460], 0x34);
    EXPECT_EQ(cycles, 7);
}

TEST_F(MOV_M_R_Test, MOV_M_L) {
    // Caso especial: MOV M,L donde L es parte de la dirección HL
    cpu.registers_m.setRegister(Registers::Register::H, 0x00);
    cpu.registers_m.setRegister(Registers::Register::L, 0x78);
    
    cpu.MOV_M_R<Registers::Register::L>();
    
    // HL = 0x0078, L contiene 0x78 que se escribe en esa dirección
    EXPECT_EQ(rom[0x0078], 0x78);
}

// ==================== Tests con diferentes direcciones ====================

TEST_F(MOV_M_R_Test, MOV_M_A_AtAddressZero) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x99);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0000);
    
    cpu.MOV_M_R<Registers::Register::A>();
    
    EXPECT_EQ(rom[0x0000], 0x99);
}

TEST_F(MOV_M_R_Test, MOV_M_A_AtHighAddress) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x88);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x00FF);
    
    cpu.MOV_M_R<Registers::Register::A>();
    
    EXPECT_EQ(rom[0x00FF], 0x88);
}

TEST_F(MOV_M_R_Test, MOV_M_B_SequentialAddresses) {
    cpu.registers_m.setRegister(Registers::Register::B, 0x11);
    
    // Escribir en direcciones consecutivas
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0010);
    cpu.MOV_M_R<Registers::Register::B>();
    EXPECT_EQ(rom[0x0010], 0x11);
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0011);
    cpu.MOV_M_R<Registers::Register::B>();
    EXPECT_EQ(rom[0x0011], 0x11);
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0012);
    cpu.MOV_M_R<Registers::Register::B>();
    EXPECT_EQ(rom[0x0012], 0x11);
}

// ==================== Tests con valores especiales ====================

TEST_F(MOV_M_R_Test, MOV_M_A_ZeroValue) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0020);
    
    cpu.MOV_M_R<Registers::Register::A>();
    
    EXPECT_EQ(rom[0x0020], 0x00);
}

TEST_F(MOV_M_R_Test, MOV_M_A_MaxValue) {
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0030);
    
    cpu.MOV_M_R<Registers::Register::A>();
    
    EXPECT_EQ(rom[0x0030], 0xFF);
}

TEST_F(MOV_M_R_Test, MOV_M_B_Pattern) {
    cpu.registers_m.setRegister(Registers::Register::B, 0b10101010);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0040);
    
    cpu.MOV_M_R<Registers::Register::B>();
    
    EXPECT_EQ(rom[0x0040], 0xAA);
}

// ==================== Tests de preservación ====================

TEST_F(MOV_M_R_Test, MOV_M_PreservesAllFlags) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x42);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0010);
    
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.MOV_M_R<Registers::Register::A>();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(MOV_M_R_Test, MOV_M_PreservesSourceRegister) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x42);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0010);
    
    cpu.MOV_M_R<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x42);
    EXPECT_EQ(rom[0x0010], 0x42);
}

TEST_F(MOV_M_R_Test, MOV_M_PreservesHLRegister) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x42);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0050);
    
    cpu.MOV_M_R<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0050);
}

TEST_F(MOV_M_R_Test, MOV_M_DoesNotModifyOtherRegisters) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x11);
    cpu.registers_m.setRegister(Registers::Register::B, 0x22);
    cpu.registers_m.setRegister(Registers::Register::C, 0x33);
    cpu.registers_m.setRegister(Registers::Register::D, 0x44);
    cpu.registers_m.setRegister(Registers::Register::E, 0x55);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0010);
    
    cpu.MOV_M_R<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x11);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x22);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x33);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x44);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x55);
}

TEST_F(MOV_M_R_Test, MOV_M_OverwritesPreviousMemoryValue) {
    // Establecer un valor inicial en memoria
    rom[0x0020] = 0x99;
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x42);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0020);
    
    cpu.MOV_M_R<Registers::Register::A>();
    
    EXPECT_EQ(rom[0x0020], 0x42);
}

// ==================== Tests de casos típicos en programas 8080 ====================

TEST_F(MOV_M_R_Test, MOV_M_StoreArrayElement) {
    // Patrón típico: almacenar elemento en un array
    // HL apunta al inicio del array, almacenar valor en A
    cpu.registers_m.setRegister(Registers::Register::A, 0x42);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0010);
    
    cpu.MOV_M_R<Registers::Register::A>();
    
    EXPECT_EQ(rom[0x0010], 0x42);
}

TEST_F(MOV_M_R_Test, MOV_M_WriteSequentialData) {
    // Patrón típico: escribir datos secuenciales en memoria
    cpu.registers_m.setRegister(Registers::Register::A, 0x01);
    cpu.registers_m.setRegister(Registers::Register::B, 0x02);
    cpu.registers_m.setRegister(Registers::Register::C, 0x03);
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0010);
    cpu.MOV_M_R<Registers::Register::A>();
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0011);
    cpu.MOV_M_R<Registers::Register::B>();
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0012);
    cpu.MOV_M_R<Registers::Register::C>();
    
    EXPECT_EQ(rom[0x0010], 0x01);
    EXPECT_EQ(rom[0x0011], 0x02);
    EXPECT_EQ(rom[0x0012], 0x03);
}

TEST_F(MOV_M_R_Test, MOV_M_FillMemoryBlock) {
    // Patrón típico: llenar bloque de memoria con un valor
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    
    for (uint16_t addr = 0x0010; addr < 0x0020; ++addr) {
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, addr);
        cpu.MOV_M_R<Registers::Register::A>();
    }
    
    // Verificar que todas las posiciones tienen 0xFF
    for (uint16_t addr = 0x0010; addr < 0x0020; ++addr) {
        EXPECT_EQ(rom[addr], 0xFF);
    }
}

TEST_F(MOV_M_R_Test, MOV_M_SaveRegisterToStack) {
    // Patrón típico: guardar registro en stack (simulado con HL)
    cpu.registers_m.setRegister(Registers::Register::A, 0x42);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x00F0); // Dirección de stack
    
    cpu.MOV_M_R<Registers::Register::A>();
    
    EXPECT_EQ(rom[0x00F0], 0x42);
}

TEST_F(MOV_M_R_Test, MOV_M_WriteASCIIString) {
    // Patrón típico: escribir string ASCII en memoria
    const char* str = "ABC";
    uint16_t addr = 0x0020;
    
    for (int i = 0; str[i] != '\0'; ++i) {
        cpu.registers_m.setRegister(Registers::Register::A, str[i]);
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, addr + i);
        cpu.MOV_M_R<Registers::Register::A>();
    }
    
    EXPECT_EQ(rom[0x0020], 'A');
    EXPECT_EQ(rom[0x0021], 'B');
    EXPECT_EQ(rom[0x0022], 'C');
}

TEST_F(MOV_M_R_Test, MOV_M_CopyRegisterPairToMemory) {
    // Patrón típico: copiar par de registros a memoria
    cpu.registers_m.setRegister(Registers::Register::B, 0x12); // BC = 0x1234
    cpu.registers_m.setRegister(Registers::Register::C, 0x34);
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0010);
    cpu.MOV_M_R<Registers::Register::C>(); // Byte bajo primero
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0011);
    cpu.MOV_M_R<Registers::Register::B>(); // Byte alto después
    
    EXPECT_EQ(rom[0x0010], 0x34);
    EXPECT_EQ(rom[0x0011], 0x12);
}

TEST_F(MOV_M_R_Test, MOV_M_StoreCalculationResult) {
    // Patrón típico: almacenar resultado de cálculo
    // (simulando que A contiene el resultado)
    cpu.registers_m.setRegister(Registers::Register::A, 0x2A); // Resultado
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0030);
    
    cpu.MOV_M_R<Registers::Register::A>();
    
    EXPECT_EQ(rom[0x0030], 0x2A);
}

TEST_F(MOV_M_R_Test, MOV_M_WriteToIOBuffer) {
    // Patrón típico: escribir a buffer de I/O
    cpu.registers_m.setRegister(Registers::Register::A, 0x48); // 'H'
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0080); // Buffer address
    
    cpu.MOV_M_R<Registers::Register::A>();
    
    EXPECT_EQ(rom[0x0080], 0x48);
}

TEST_F(MOV_M_R_Test, MOV_M_UpdateLookupTable) {
    // Patrón típico: actualizar tabla de lookup
    cpu.registers_m.setRegister(Registers::Register::A, 0x05); // Nuevo valor
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0040); // Tabla base + offset
    
    cpu.MOV_M_R<Registers::Register::A>();
    
    EXPECT_EQ(rom[0x0040], 0x05);
}

TEST_F(MOV_M_R_Test, MOV_M_ClearMemoryLocation) {
    // Patrón típico: limpiar ubicación de memoria
    rom[0x0025] = 0xFF; // Valor inicial
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0025);
    
    cpu.MOV_M_R<Registers::Register::A>();
    
    EXPECT_EQ(rom[0x0025], 0x00);
}

TEST_F(MOV_M_R_Test, MOV_M_WriteAllRegistersToMemory) {
    // Patrón típico: guardar todos los registros en memoria (context save)
    cpu.registers_m.setRegister(Registers::Register::A, 0x11);
    cpu.registers_m.setRegister(Registers::Register::B, 0x22);
    cpu.registers_m.setRegister(Registers::Register::C, 0x33);
    cpu.registers_m.setRegister(Registers::Register::D, 0x44);
    cpu.registers_m.setRegister(Registers::Register::E, 0x55);
    
    uint16_t base = 0x0060;
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, base + 0);
    cpu.MOV_M_R<Registers::Register::A>();
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, base + 1);
    cpu.MOV_M_R<Registers::Register::B>();
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, base + 2);
    cpu.MOV_M_R<Registers::Register::C>();
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, base + 3);
    cpu.MOV_M_R<Registers::Register::D>();
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, base + 4);
    cpu.MOV_M_R<Registers::Register::E>();
    
    EXPECT_EQ(rom[0x0060], 0x11);
    EXPECT_EQ(rom[0x0061], 0x22);
    EXPECT_EQ(rom[0x0062], 0x33);
    EXPECT_EQ(rom[0x0063], 0x44);
    EXPECT_EQ(rom[0x0064], 0x55);
}
