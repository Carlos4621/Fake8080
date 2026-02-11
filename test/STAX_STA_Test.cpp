#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"
#include <array>

class STAX_STA_Test : public ::testing::Test {
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

// ==================== Tests para STAX BC ====================

TEST_F(STAX_STA_Test, STAX_BC_BasicStore) {
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x42);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x3000);
    
    uint8_t cycles = cpu.STAX_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(rom[0x3000], 0x42);
    EXPECT_EQ(cycles, 7);
}

TEST_F(STAX_STA_Test, STAX_BC_ZeroValue) {
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x2000);
    
    cpu.STAX_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(rom[0x2000], 0x00);
}

TEST_F(STAX_STA_Test, STAX_BC_MaxValue) {
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x4000);
    
    cpu.STAX_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(rom[0x4000], 0xFF);
}

TEST_F(STAX_STA_Test, STAX_BC_LowAddress) {
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xAA);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x0010);
    
    cpu.STAX_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(rom[0x0010], 0xAA);
}

TEST_F(STAX_STA_Test, STAX_BC_HighAddress) {
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xBB);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0xFFFF);
    
    cpu.STAX_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(rom[0xFFFF], 0xBB);
}

// ==================== Tests para STAX DE ====================

TEST_F(STAX_STA_Test, STAX_DE_BasicStore) {
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x33);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x5000);
    
    uint8_t cycles = cpu.STAX_RR<Registers::CombinedRegister::DE>();
    
    EXPECT_EQ(rom[0x5000], 0x33);
    EXPECT_EQ(cycles, 7);
}

TEST_F(STAX_STA_Test, STAX_DE_DifferentAddress) {
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x77);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x1234);
    
    cpu.STAX_RR<Registers::CombinedRegister::DE>();
    
    EXPECT_EQ(rom[0x1234], 0x77);
}

// ==================== Tests para STA a16 ====================

TEST_F(STAX_STA_Test, STA_BasicStore) {
    rom[0] = 0x00; // Low byte de dirección
    rom[1] = 0x30; // High byte de dirección (0x3000)
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x55);
    
    uint8_t cycles = cpu.STA_a16();
    
    EXPECT_EQ(rom[0x3000], 0x55);
    EXPECT_EQ(cycles, 13);
}

TEST_F(STAX_STA_Test, STA_ZeroValue) {
    rom[0] = 0x00;
    rom[1] = 0x40;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    
    cpu.STA_a16();
    
    EXPECT_EQ(rom[0x4000], 0x00);
}

TEST_F(STAX_STA_Test, STA_MaxValue) {
    rom[0] = 0x00;
    rom[1] = 0x50;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    
    cpu.STA_a16();
    
    EXPECT_EQ(rom[0x5000], 0xFF);
}

TEST_F(STAX_STA_Test, STA_LowAddress) {
    rom[0] = 0x10;
    rom[1] = 0x00;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xCC);
    
    cpu.STA_a16();
    
    EXPECT_EQ(rom[0x0010], 0xCC);
}

TEST_F(STAX_STA_Test, STA_HighAddress) {
    rom[0] = 0xFF;
    rom[1] = 0xFF;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xDD);
    
    cpu.STA_a16();
    
    EXPECT_EQ(rom[0xFFFF], 0xDD);
}

TEST_F(STAX_STA_Test, STA_LittleEndianAddress) {
    rom[0] = 0x34;
    rom[1] = 0x12; // Dirección 0x1234
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xEE);
    
    cpu.STA_a16();
    
    EXPECT_EQ(rom[0x1234], 0xEE);
}

// ==================== Tests de preservación ====================

TEST_F(STAX_STA_Test, STAX_PreservesAllFlags) {
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x42);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x3000);
    
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.STAX_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(STAX_STA_Test, STA_PreservesAllFlags) {
    rom[0] = 0x00;
    rom[1] = 0x30;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x42);
    
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.STA_a16();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(STAX_STA_Test, STAX_PreservesAccumulator) {
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x42);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x3000);
    
    cpu.STAX_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x42);
}

TEST_F(STAX_STA_Test, STA_PreservesAccumulator) {
    rom[0] = 0x00;
    rom[1] = 0x30;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x42);
    
    cpu.STA_a16();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x42);
}

TEST_F(STAX_STA_Test, STAX_PreservesOtherRegisters) {
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xAA);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x3000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x1111);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2222);
    
    cpu.STAX_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x3000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x1111);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2222);
}

TEST_F(STAX_STA_Test, STA_PreservesOtherRegisters) {
    rom[0] = 0x00;
    rom[1] = 0x30;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xAA);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x1111);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x2222);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3333);
    
    cpu.STA_a16();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x1111);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x2222);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x3333);
}

// ==================== Tests de múltiples operaciones ====================

TEST_F(STAX_STA_Test, STAX_BC_MultipleStores) {
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x11);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x3000);
    cpu.STAX_RR<Registers::CombinedRegister::BC>();
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x22);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x3001);
    cpu.STAX_RR<Registers::CombinedRegister::BC>();
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x33);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x3002);
    cpu.STAX_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(rom[0x3000], 0x11);
    EXPECT_EQ(rom[0x3001], 0x22);
    EXPECT_EQ(rom[0x3002], 0x33);
}

TEST_F(STAX_STA_Test, STAX_DE_MultipleStores) {
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x44);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x4000);
    cpu.STAX_RR<Registers::CombinedRegister::DE>();
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x55);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x4001);
    cpu.STAX_RR<Registers::CombinedRegister::DE>();
    
    EXPECT_EQ(rom[0x4000], 0x44);
    EXPECT_EQ(rom[0x4001], 0x55);
}

TEST_F(STAX_STA_Test, STA_Sequential_PCIncrement) {
    rom[0] = 0x00;
    rom[1] = 0x30; // STA 0x3000
    rom[2] = 0x00;
    rom[3] = 0x40; // STA 0x4000
    rom[4] = 0x00;
    rom[5] = 0x50; // STA 0x5000
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xAA);
    cpu.STA_a16();
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xBB);
    cpu.STA_a16();
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xCC);
    cpu.STA_a16();
    
    EXPECT_EQ(rom[0x3000], 0xAA);
    EXPECT_EQ(rom[0x4000], 0xBB);
    EXPECT_EQ(rom[0x5000], 0xCC);
}

TEST_F(STAX_STA_Test, STAX_OverwritePreviousData) {
    cpu.setROM(rom);
    
    rom[0x3000] = 0xFF;
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x3000);
    cpu.STAX_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(rom[0x3000], 0x00);
}

TEST_F(STAX_STA_Test, STA_OverwritePreviousData) {
    rom[0] = 0x00;
    rom[1] = 0x30;
    rom[0x3000] = 0xFF;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);
    cpu.STA_a16();
    
    EXPECT_EQ(rom[0x3000], 0x00);
}

// ==================== Tests de casos prácticos ====================

TEST_F(STAX_STA_Test, RealisticUseCase_StoreToBuffer) {
    cpu.setROM(rom);
    
    // Almacenar datos en un buffer
    uint16_t bufferAddress = 0x6000;
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, bufferAddress);
    cpu.registers_m.setRegister(Registers::Register::A, 'H');
    cpu.STAX_RR<Registers::CombinedRegister::BC>();
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, bufferAddress + 1);
    cpu.registers_m.setRegister(Registers::Register::A, 'I');
    cpu.STAX_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(rom[0x6000], 'H');
    EXPECT_EQ(rom[0x6001], 'I');
}

TEST_F(STAX_STA_Test, RealisticUseCase_SaveStatus) {
    rom[0] = 0x00;
    rom[1] = 0x80; // Dirección de estado
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x01); // Estado: activo
    cpu.STA_a16();
    
    EXPECT_EQ(rom[0x8000], 0x01);
}

TEST_F(STAX_STA_Test, RealisticUseCase_FillArray) {
    cpu.setROM(rom);
    
    uint16_t arrayStart = 0x7000;
    uint8_t fillValue = 0xAA;
    
    cpu.registers_m.setRegister(Registers::Register::A, fillValue);
    
    for (int i = 0; i < 5; i++) {
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, arrayStart + i);
        cpu.STAX_RR<Registers::CombinedRegister::DE>();
    }
    
    for (int i = 0; i < 5; i++) {
        EXPECT_EQ(rom[arrayStart + i], fillValue);
    }
}

TEST_F(STAX_STA_Test, STAX_BC_DE_Comparison) {
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x12);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x8000);
    cpu.STAX_RR<Registers::CombinedRegister::BC>();
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x34);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x8001);
    cpu.STAX_RR<Registers::CombinedRegister::DE>();
    
    EXPECT_EQ(rom[0x8000], 0x12);
    EXPECT_EQ(rom[0x8001], 0x34);
}

TEST_F(STAX_STA_Test, BoundaryCondition_ConsecutiveAddresses) {
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x01);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x9000);
    cpu.STAX_RR<Registers::CombinedRegister::BC>();
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x02);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x9001);
    cpu.STAX_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(rom[0x9000], 0x01);
    EXPECT_EQ(rom[0x9001], 0x02);
}

TEST_F(STAX_STA_Test, BoundaryCondition_SameAddress) {
    rom[0] = 0x00;
    rom[1] = 0x90;
    rom[2] = 0x00;
    rom[3] = 0x90;
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xAA);
    cpu.STA_a16();
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xBB);
    cpu.STA_a16();
    
    EXPECT_EQ(rom[0x9000], 0xBB);
}

TEST_F(STAX_STA_Test, PatternTest_AlternatingValues) {
    cpu.setROM(rom);
    
    for (int i = 0; i < 10; i++) {
        uint8_t value = (i % 2 == 0) ? 0xAA : 0x55;
        cpu.registers_m.setRegister(Registers::Register::A, value);
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0xA000 + i);
        cpu.STAX_RR<Registers::CombinedRegister::BC>();
    }
    
    for (int i = 0; i < 10; i++) {
        uint8_t expected = (i % 2 == 0) ? 0xAA : 0x55;
        EXPECT_EQ(rom[0xA000 + i], expected);
    }
}

TEST_F(STAX_STA_Test, EdgeCase_StoreAtCodeArea) {
    rom[0] = 0x06;
    rom[1] = 0x00; // STA 0x0006
    cpu.setROM(rom);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x99);
    cpu.STA_a16();
    
    EXPECT_EQ(rom[0x0006], 0x99);
}

TEST_F(STAX_STA_Test, STAX_BC_SequentialBuffer) {
    cpu.setROM(rom);
    
    uint16_t base = 0xB000;
    uint8_t data[] = {0x10, 0x20, 0x30, 0x40, 0x50};
    
    for (size_t i = 0; i < sizeof(data); i++) {
        cpu.registers_m.setRegister(Registers::Register::A, data[i]);
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, base + i);
        cpu.STAX_RR<Registers::CombinedRegister::BC>();
    }
    
    for (size_t i = 0; i < sizeof(data); i++) {
        EXPECT_EQ(rom[base + i], data[i]);
    }
}

TEST_F(STAX_STA_Test, STA_MultipleAddresses) {
    rom[0] = 0x00;
    rom[1] = 0xC0;
    rom[2] = 0x10;
    rom[3] = 0xC0;
    rom[4] = 0x20;
    rom[5] = 0xC0;
    cpu.setROM(rom);
    
    uint8_t values[] = {0x11, 0x22, 0x33};
    
    for (size_t i = 0; i < sizeof(values); i++) {
        cpu.registers_m.setRegister(Registers::Register::A, values[i]);
        cpu.STA_a16();
    }
    
    EXPECT_EQ(rom[0xC000], 0x11);
    EXPECT_EQ(rom[0xC010], 0x22);
    EXPECT_EQ(rom[0xC020], 0x33);
}

TEST_F(STAX_STA_Test, CompareSTAX_vs_STA) {
    rom[0] = 0x00;
    rom[1] = 0xD0;
    cpu.setROM(rom);
    
    // STAX BC
    cpu.registers_m.setRegister(Registers::Register::A, 0xAA);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0xD000);
    cpu.STAX_RR<Registers::CombinedRegister::BC>();
    
    // STA (mismo valor, dirección adyacente)
    cpu.registers_m.setRegister(Registers::Register::A, 0xAA);
    cpu.STA_a16();
    
    EXPECT_EQ(rom[0xD000], 0xAA);
}
