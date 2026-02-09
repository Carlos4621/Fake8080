#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"
#include <array>

class MVI_Test : public ::testing::Test {
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
    }
};

// ==================== Tests MVI para cada registro ====================

TEST_F(MVI_Test, MVI_A_ImmediateValue) {
    // MVI A, 0x42
    rom[0] = 0x42; // Valor inmediato
    cpu.setROM(rom);
    
    uint8_t cycles = cpu.MVI_R_d8<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x42);
    EXPECT_EQ(cycles, 7);
}

TEST_F(MVI_Test, MVI_B_ImmediateValue) {
    rom[0] = 0x55;
    cpu.setROM(rom);
    
    uint8_t cycles = cpu.MVI_R_d8<Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x55);
    EXPECT_EQ(cycles, 7);
}

TEST_F(MVI_Test, MVI_C_ImmediateValue) {
    rom[0] = 0xAA;
    cpu.setROM(rom);
    
    uint8_t cycles = cpu.MVI_R_d8<Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0xAA);
    EXPECT_EQ(cycles, 7);
}

TEST_F(MVI_Test, MVI_D_ImmediateValue) {
    rom[0] = 0xFF;
    cpu.setROM(rom);
    
    uint8_t cycles = cpu.MVI_R_d8<Registers::Register::D>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0xFF);
    EXPECT_EQ(cycles, 7);
}

TEST_F(MVI_Test, MVI_E_ImmediateValue) {
    rom[0] = 0x12;
    cpu.setROM(rom);
    
    uint8_t cycles = cpu.MVI_R_d8<Registers::Register::E>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x12);
    EXPECT_EQ(cycles, 7);
}

TEST_F(MVI_Test, MVI_H_ImmediateValue) {
    rom[0] = 0x34;
    cpu.setROM(rom);
    
    uint8_t cycles = cpu.MVI_R_d8<Registers::Register::H>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x34);
    EXPECT_EQ(cycles, 7);
}

TEST_F(MVI_Test, MVI_L_ImmediateValue) {
    rom[0] = 0x78;
    cpu.setROM(rom);
    
    uint8_t cycles = cpu.MVI_R_d8<Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x78);
    EXPECT_EQ(cycles, 7);
}

// ==================== Tests con valores especiales ====================

TEST_F(MVI_Test, MVI_A_ZeroValue) {
    rom[0] = 0x00;
    cpu.setROM(rom);
    
    cpu.MVI_R_d8<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
}

TEST_F(MVI_Test, MVI_A_MaxValue) {
    rom[0] = 0xFF;
    cpu.setROM(rom);
    
    cpu.MVI_R_d8<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
}

TEST_F(MVI_Test, MVI_B_PowerOfTwo) {
    rom[0] = 0b10000000; // 128
    cpu.setROM(rom);
    
    cpu.MVI_R_d8<Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x80);
}

TEST_F(MVI_Test, MVI_C_Pattern) {
    rom[0] = 0b10101010; // Patrón alternado
    cpu.setROM(rom);
    
    cpu.MVI_R_d8<Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0xAA);
}

// ==================== Tests de preservación ====================

TEST_F(MVI_Test, MVI_PreservesAllFlags) {
    rom[0] = 0x42;
    cpu.setROM(rom);
    
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.MVI_R_d8<Registers::Register::A>();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(MVI_Test, MVI_DoesNotModifyOtherRegisters) {
    rom[0] = 0x99;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x11);
    cpu.registers_m.setRegister(Registers::Register::B, 0x22);
    cpu.registers_m.setRegister(Registers::Register::C, 0x33);
    cpu.registers_m.setRegister(Registers::Register::D, 0x44);
    cpu.registers_m.setRegister(Registers::Register::E, 0x55);
    cpu.registers_m.setRegister(Registers::Register::H, 0x66);
    cpu.registers_m.setRegister(Registers::Register::L, 0x77);
    
    cpu.MVI_R_d8<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x99);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x22);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x33);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x44);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x55);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x66);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x77);
}

TEST_F(MVI_Test, MVI_OverwritesPreviousValue) {
    rom[0] = 0xAB;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x12);
    
    cpu.MVI_R_d8<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xAB);
}

// ==================== Tests de secuencias de instrucciones ====================

TEST_F(MVI_Test, MVI_SequentialInstructions) {
    // Simular: MVI A, 0x11
    //          MVI B, 0x22
    //          MVI C, 0x33
    rom[0] = 0x11;
    rom[1] = 0x22;
    rom[2] = 0x33;
    cpu.setROM(rom);
    
    cpu.MVI_R_d8<Registers::Register::A>();
    cpu.MVI_R_d8<Registers::Register::B>();
    cpu.MVI_R_d8<Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x11);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x22);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x33);
}

TEST_F(MVI_Test, MVI_AllRegisters) {
    // Cargar un valor diferente en cada registro
    rom[0] = 0x01;
    rom[1] = 0x02;
    rom[2] = 0x03;
    rom[3] = 0x04;
    rom[4] = 0x05;
    rom[5] = 0x06;
    rom[6] = 0x07;
    cpu.setROM(rom);
    
    cpu.MVI_R_d8<Registers::Register::A>();
    cpu.MVI_R_d8<Registers::Register::B>();
    cpu.MVI_R_d8<Registers::Register::C>();
    cpu.MVI_R_d8<Registers::Register::D>();
    cpu.MVI_R_d8<Registers::Register::E>();
    cpu.MVI_R_d8<Registers::Register::H>();
    cpu.MVI_R_d8<Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x01);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x02);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x03);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x04);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x05);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x06);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x07);
}

TEST_F(MVI_Test, MVI_RepeatedLoads) {
    // Cargar múltiples valores en el mismo registro secuencialmente
    rom[0] = 0x10;
    rom[1] = 0x20;
    rom[2] = 0x30;
    cpu.setROM(rom);
    
    cpu.MVI_R_d8<Registers::Register::A>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x10);
    
    cpu.MVI_R_d8<Registers::Register::A>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x20);
    
    cpu.MVI_R_d8<Registers::Register::A>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x30);
}

// ==================== Tests de casos típicos en programas 8080 ====================

TEST_F(MVI_Test, MVI_InitializeCounter) {
    // Patrón típico: inicializar un contador
    rom[0] = 0x00; // Contador en 0
    cpu.setROM(rom);
    
    cpu.MVI_R_d8<Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x00);
}

TEST_F(MVI_Test, MVI_LoadLoopCount) {
    // Patrón típico: cargar número de iteraciones
    rom[0] = 0x0A; // 10 iteraciones
    cpu.setROM(rom);
    
    cpu.MVI_R_d8<Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x0A);
}

TEST_F(MVI_Test, MVI_SetupRegisterPair) {
    // Patrón típico: configurar un par de registros (HL como puntero)
    rom[0] = 0x20; // H = dirección alta
    rom[1] = 0x00; // L = dirección baja
    cpu.setROM(rom);
    
    cpu.MVI_R_d8<Registers::Register::H>();
    cpu.MVI_R_d8<Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x20);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x00);
    // HL apunta a 0x2000
}

TEST_F(MVI_Test, MVI_LoadMask) {
    // Patrón típico: cargar una máscara de bits
    rom[0] = 0b11110000;
    cpu.setROM(rom);
    
    cpu.MVI_R_d8<Registers::Register::B>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0xF0);
}

TEST_F(MVI_Test, MVI_LoadASCIICharacter) {
    // Patrón típico: cargar un carácter ASCII
    rom[0] = 'A'; // 0x41
    cpu.setROM(rom);
    
    cpu.MVI_R_d8<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x41);
}

TEST_F(MVI_Test, MVI_InitializeMultipleCounters) {
    // Patrón típico: inicializar múltiples contadores
    rom[0] = 0x08; // Contador 1
    rom[1] = 0x10; // Contador 2
    rom[2] = 0x20; // Contador 3
    cpu.setROM(rom);
    
    cpu.MVI_R_d8<Registers::Register::B>();
    cpu.MVI_R_d8<Registers::Register::C>();
    cpu.MVI_R_d8<Registers::Register::D>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x08);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x10);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x20);
}

TEST_F(MVI_Test, MVI_SetupBCDValues) {
    // Patrón típico: cargar valores BCD
    rom[0] = 0x99; // 99 en BCD
    cpu.setROM(rom);
    
    cpu.MVI_R_d8<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x99);
}

TEST_F(MVI_Test, MVI_LoadPortAddress) {
    // Patrón típico: cargar dirección de puerto para I/O
    rom[0] = 0x80; // Dirección de puerto
    cpu.setROM(rom);
    
    cpu.MVI_R_d8<Registers::Register::C>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x80);
}

TEST_F(MVI_Test, MVI_ClearRegisterViaZero) {
    // Patrón típico: limpiar un registro
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    
    rom[0] = 0x00;
    cpu.setROM(rom);
    
    cpu.MVI_R_d8<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
}

TEST_F(MVI_Test, MVI_LoadNegativeNumber) {
    // Patrón típico: cargar número "negativo" (en complemento a 2)
    rom[0] = 0xFF; // -1 en complemento a 2
    cpu.setROM(rom);
    
    cpu.MVI_R_d8<Registers::Register::A>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
}

TEST_F(MVI_Test, MVI_InitializeDataPointers) {
    // Patrón típico: inicializar múltiples punteros de datos
    rom[0] = 0x10; // DE = 0x1020
    rom[1] = 0x20;
    rom[2] = 0x30; // HL = 0x3040
    rom[3] = 0x40;
    cpu.setROM(rom);
    
    cpu.MVI_R_d8<Registers::Register::D>();
    cpu.MVI_R_d8<Registers::Register::E>();
    cpu.MVI_R_d8<Registers::Register::H>();
    cpu.MVI_R_d8<Registers::Register::L>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x10);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x20);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x30);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x40);
}

// ==================== Tests para MVI M, d8 (Move Immediate to Memory) ====================

TEST_F(MVI_Test, MVI_M_BasicOperation) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0] = 0x42; // Valor inmediato
    cpu.setROM(rom);
    
    uint8_t cycles = cpu.MVI_M_d8();
    
    EXPECT_EQ(rom[0x2000], 0x42);
    EXPECT_EQ(cycles, 10);
}

TEST_F(MVI_Test, MVI_M_ZeroValue) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0] = 0x00;
    cpu.setROM(rom);
    
    uint8_t cycles = cpu.MVI_M_d8();
    
    EXPECT_EQ(rom[0x2000], 0x00);
    EXPECT_EQ(cycles, 10);
}

TEST_F(MVI_Test, MVI_M_MaxValue) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0] = 0xFF;
    cpu.setROM(rom);
    
    uint8_t cycles = cpu.MVI_M_d8();
    
    EXPECT_EQ(rom[0x2000], 0xFF);
    EXPECT_EQ(cycles, 10);
}

TEST_F(MVI_Test, MVI_M_SignedNegative) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0] = 0x80; // -128 en complemento a 2
    cpu.setROM(rom);
    
    cpu.MVI_M_d8();
    
    EXPECT_EQ(rom[0x2000], 0x80);
}

TEST_F(MVI_Test, MVI_M_BitPattern) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0] = 0b10101010;
    cpu.setROM(rom);
    
    cpu.MVI_M_d8();
    
    EXPECT_EQ(rom[0x2000], 0xAA);
}

TEST_F(MVI_Test, MVI_M_PreservesHLRegister) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0] = 0x42;
    cpu.setROM(rom);
    
    cpu.MVI_M_d8();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2000);
}

TEST_F(MVI_Test, MVI_M_PreservesAllRegisters) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    cpu.registers_m.setRegister(Registers::Register::A, 0x11);
    cpu.registers_m.setRegister(Registers::Register::B, 0x22);
    cpu.registers_m.setRegister(Registers::Register::C, 0x33);
    cpu.registers_m.setRegister(Registers::Register::D, 0x44);
    cpu.registers_m.setRegister(Registers::Register::E, 0x55);
    rom[0] = 0x99;
    cpu.setROM(rom);
    
    cpu.MVI_M_d8();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x11);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x22);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x33);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x44);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x55);
}

TEST_F(MVI_Test, MVI_M_PreservesAllFlags) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0] = 0x42;
    cpu.setROM(rom);
    
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.MVI_M_d8();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(MVI_Test, MVI_M_OverwritesPreviousMemoryValue) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    rom[0x2000] = 0x11; // Valor previo en memoria
    rom[0] = 0x99;
    cpu.setROM(rom);
    
    cpu.MVI_M_d8();
    
    EXPECT_EQ(rom[0x2000], 0x99);
}

TEST_F(MVI_Test, MVI_M_AtDifferentAddresses) {
    // Colocar dos valores en ROM[0] y ROM[1] (PC se incrementa)
    rom[0] = 0xAA;
    rom[1] = 0xBB;
    cpu.setROM(rom);
    
    // Primera escritura en 0x1000 (lee rom[0], PC pasa a 1)
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1000);
    cpu.MVI_M_d8();
    EXPECT_EQ(rom[0x1000], 0xAA);
    
    // Segunda escritura en 0x3000 (lee rom[1], PC pasa a 2)
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3000);
    cpu.MVI_M_d8();
    EXPECT_EQ(rom[0x3000], 0xBB);
}

TEST_F(MVI_Test, MVI_M_SequentialWrites) {
    // Colocar valores en posiciones consecutivas de ROM (PC se incrementa)
    rom[0] = 0x10;
    rom[1] = 0x20;
    rom[2] = 0x30;
    cpu.setROM(rom);
    
    // Escribir en 0x2000 (lee rom[0], PC pasa a 1)
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    cpu.MVI_M_d8();
    EXPECT_EQ(rom[0x2000], 0x10);
    
    // Escribir en 0x2001 (lee rom[1], PC pasa a 2)
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2001);
    cpu.MVI_M_d8();
    EXPECT_EQ(rom[0x2001], 0x20);
    
    // Escribir en 0x2002 (lee rom[2], PC pasa a 3)
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2002);
    cpu.MVI_M_d8();
    EXPECT_EQ(rom[0x2002], 0x30);
}

TEST_F(MVI_Test, MVI_M_HighAddress) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xFFFF);
    rom[0] = 0x77;
    cpu.setROM(rom);
    
    cpu.MVI_M_d8();
    
    EXPECT_EQ(rom[0xFFFF], 0x77);
}

TEST_F(MVI_Test, MVI_M_LowAddress) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0000);
    rom[0] = 0x88;
    cpu.setROM(rom);
    
    cpu.MVI_M_d8();
    
    EXPECT_EQ(rom[0x0000], 0x88);
}

TEST_F(MVI_Test, MVI_M_InitializeArrayElements) {
    // Simular inicialización de array en memoria
    uint16_t baseAddress = 0x3000;
    uint8_t values[] = {0x10, 0x20, 0x30, 0x40, 0x50};
    
    // Colocar valores en ROM consecutivamente (PC se incrementa con cada llamada)
    for (int i = 0; i < 5; i++) {
        rom[i] = values[i];
    }
    cpu.setROM(rom);
    
    for (int i = 0; i < 5; i++) {
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, baseAddress + i);
        cpu.MVI_M_d8();
    }
    
    for (int i = 0; i < 5; i++) {
        EXPECT_EQ(rom[baseAddress + i], values[i]);
    }
}

TEST_F(MVI_Test, MVI_M_BufferInitialization) {
    // Inicializar buffer con valor específico
    uint16_t bufferStart = 0x4000;
    uint8_t fillValue = 0xCC;
    
    // Llenar los primeros 10 bytes de ROM con el valor (PC se incrementa en cada llamada)
    for (int i = 0; i < 10; i++) {
        rom[i] = fillValue;
    }
    cpu.setROM(rom);
    
    for (int i = 0; i < 10; i++) {
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, bufferStart + i);
        cpu.MVI_M_d8();
    }
    
    // Verificar que todas las ubicaciones tienen el valor
    for (int i = 0; i < 10; i++) {
        EXPECT_EQ(rom[bufferStart + i], fillValue);
    }
}

TEST_F(MVI_Test, MVI_M_StackAreaInitialization) {
    // Simular inicialización en área de stack
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xFFF0);
    rom[0] = 0xDD;
    cpu.setROM(rom);
    
    cpu.MVI_M_d8();
    
    EXPECT_EQ(rom[0xFFF0], 0xDD);
}

TEST_F(MVI_Test, MVI_M_RealisticUseCase_LookupTable) {
    // Crear lookup table en memoria
    uint16_t tableAddress = 0x5000;
    uint8_t lookupTable[] = {0x00, 0x01, 0x04, 0x09, 0x10}; // Cuadrados: 0², 1², 2², 3², 4²
    
    // Colocar valores en ROM consecutivamente (PC se incrementa con cada llamada)
    for (int i = 0; i < 5; i++) {
        rom[i] = lookupTable[i];
    }
    cpu.setROM(rom);
    
    for (int i = 0; i < 5; i++) {
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, tableAddress + i);
        cpu.MVI_M_d8();
    }
    
    // Verificar tabla
    for (int i = 0; i < 5; i++) {
        EXPECT_EQ(rom[tableAddress + i], lookupTable[i]);
    }
}

