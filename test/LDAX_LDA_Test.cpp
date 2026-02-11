#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"
#include <array>

class LDAX_LDA_Test : public ::testing::Test {
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

// LDAX BC - Basic Load Tests
TEST_F(LDAX_LDA_Test, LDAX_BC_BasicLoad) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x2000);
    rom[0x2000] = 0x42;
    cpu.setROM(rom);
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);

    uint8_t cycles = cpu.LDAX_RR<Registers::CombinedRegister::BC>();

    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x42);
    EXPECT_EQ(cycles, 7);
}

TEST_F(LDAX_LDA_Test, LDAX_BC_ZeroValue) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x3000);
    rom[0x3000] = 0x00;
    cpu.setROM(rom);
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);

    uint8_t cycles = cpu.LDAX_RR<Registers::CombinedRegister::BC>();

    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_EQ(cycles, 7);
}

TEST_F(LDAX_LDA_Test, LDAX_BC_MaxValue) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x4000);
    rom[0x4000] = 0xFF;
    cpu.setROM(rom);
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);

    uint8_t cycles = cpu.LDAX_RR<Registers::CombinedRegister::BC>();

    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(cycles, 7);
}

TEST_F(LDAX_LDA_Test, LDAX_BC_LowAddress) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x0001);
    rom[0x0001] = 0xAB;
    cpu.setROM(rom);
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);

    uint8_t cycles = cpu.LDAX_RR<Registers::CombinedRegister::BC>();

    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xAB);
    EXPECT_EQ(cycles, 7);
}

TEST_F(LDAX_LDA_Test, LDAX_BC_HighAddress) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0xFFFE);
    rom[0xFFFE] = 0xCD;
    cpu.setROM(rom);
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);

    uint8_t cycles = cpu.LDAX_RR<Registers::CombinedRegister::BC>();

    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xCD);
    EXPECT_EQ(cycles, 7);
}

// LDAX DE Tests
TEST_F(LDAX_LDA_Test, LDAX_DE_BasicLoad) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x5000);
    rom[0x5000] = 0x77;
    cpu.setROM(rom);
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);

    uint8_t cycles = cpu.LDAX_RR<Registers::CombinedRegister::DE>();

    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x77);
    EXPECT_EQ(cycles, 7);
}

TEST_F(LDAX_LDA_Test, LDAX_DE_DifferentAddress) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x6789);
    rom[0x6789] = 0xEE;
    cpu.setROM(rom);
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);

    uint8_t cycles = cpu.LDAX_RR<Registers::CombinedRegister::DE>();

    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xEE);
    EXPECT_EQ(cycles, 7);
}

// LDA - Direct Load Tests
TEST_F(LDAX_LDA_Test, LDA_BasicLoad) {
    rom[0x0000] = 0x00;  // Low byte of address
    rom[0x0001] = 0x30;  // High byte of address
    rom[0x3000] = 0x55;  // Value to load
    cpu.setROM(rom);
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);

    uint8_t cycles = cpu.LDA_a16();

    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x55);
    EXPECT_EQ(cycles, 13);
}

TEST_F(LDAX_LDA_Test, LDA_ZeroValue) {
    rom[0x0000] = 0x00;  // Low byte
    rom[0x0001] = 0x40;  // High byte
    rom[0x4000] = 0x00;  // Value to load
    cpu.setROM(rom);
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);

    uint8_t cycles = cpu.LDA_a16();

    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_EQ(cycles, 13);
}

TEST_F(LDAX_LDA_Test, LDA_MaxValue) {
    rom[0x0000] = 0x00;  // Low byte
    rom[0x0001] = 0x50;  // High byte
    rom[0x5000] = 0xFF;  // Value to load
    cpu.setROM(rom);
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);

    uint8_t cycles = cpu.LDA_a16();

    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(cycles, 13);
}

TEST_F(LDAX_LDA_Test, LDA_LowAddress) {
    rom[0x0000] = 0x10;  // Low byte
    rom[0x0001] = 0x00;  // High byte
    rom[0x0010] = 0x88;  // Value to load
    cpu.setROM(rom);
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);

    uint8_t cycles = cpu.LDA_a16();

    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x88);
    EXPECT_EQ(cycles, 13);
}

TEST_F(LDAX_LDA_Test, LDA_HighAddress) {
    rom[0x0000] = 0xFE;  // Low byte
    rom[0x0001] = 0xFF;  // High byte
    rom[0xFFFE] = 0x99;  // Value to load
    cpu.setROM(rom);
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);

    uint8_t cycles = cpu.LDA_a16();

    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x99);
    EXPECT_EQ(cycles, 13);
}

TEST_F(LDAX_LDA_Test, LDA_LittleEndianAddress) {
    rom[0x0000] = 0x34;  // Low byte
    rom[0x0001] = 0x12;  // High byte (address is 0x1234)
    rom[0x1234] = 0xAA;  // Value to load
    cpu.setROM(rom);
    cpu.registers_m.setRegister(Registers::Register::A, 0x00);

    uint8_t cycles = cpu.LDA_a16();

    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xAA);
    EXPECT_EQ(cycles, 13);
}

// Flag Preservation Tests
TEST_F(LDAX_LDA_Test, LDAX_PreservesAllFlags) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x2000);
    rom[0x2000] = 0x42;
    cpu.setROM(rom);
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, false);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, false);

    cpu.LDAX_RR<Registers::CombinedRegister::BC>();

    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(LDAX_LDA_Test, LDA_PreservesAllFlags) {
    rom[0x0000] = 0x00;
    rom[0x0001] = 0x30;
    rom[0x3000] = 0x55;
    cpu.setROM(rom);
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, false);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, false);

    cpu.LDA_a16();

    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

// Register Preservation Tests
TEST_F(LDAX_LDA_Test, LDAX_PreservesPointerRegister) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x2000);
    rom[0x2000] = 0x42;
    cpu.setROM(rom);

    cpu.LDAX_RR<Registers::CombinedRegister::BC>();

    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x2000);
}

TEST_F(LDAX_LDA_Test, LDA_PreservesOtherRegisters) {
    rom[0x0000] = 0x00;
    rom[0x0001] = 0x30;
    rom[0x3000] = 0x55;
    cpu.setROM(rom);
    cpu.registers_m.setRegister(Registers::Register::B, 0x11);
    cpu.registers_m.setRegister(Registers::Register::C, 0x22);
    cpu.registers_m.setRegister(Registers::Register::D, 0x33);
    cpu.registers_m.setRegister(Registers::Register::E, 0x44);
    cpu.registers_m.setRegister(Registers::Register::H, 0x55);
    cpu.registers_m.setRegister(Registers::Register::L, 0x66);

    cpu.LDA_a16();

    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x11);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x22);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x33);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x44);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x55);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x66);
}

// Multiple Operations
TEST_F(LDAX_LDA_Test, LDAX_BC_MultipleLoads) {
    rom[0x2000] = 0x11;
    rom[0x2001] = 0x22;
    rom[0x2002] = 0x33;
    cpu.setROM(rom);

    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x2000);
    cpu.LDAX_RR<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x11);

    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x2001);
    cpu.LDAX_RR<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x22);

    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x2002);
    cpu.LDAX_RR<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x33);
}

TEST_F(LDAX_LDA_Test, LDAX_DE_MultipleLoads) {
    rom[0x3000] = 0xAA;
    rom[0x3001] = 0xBB;
    cpu.setROM(rom);

    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x3000);
    cpu.LDAX_RR<Registers::CombinedRegister::DE>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xAA);

    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x3001);
    cpu.LDAX_RR<Registers::CombinedRegister::DE>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xBB);
}

TEST_F(LDAX_LDA_Test, LDA_Sequential_PCIncrement) {
    rom[0x0000] = 0x00;  // Address 0x3000
    rom[0x0001] = 0x30;
    rom[0x0002] = 0x00;  // Address 0x4000
    rom[0x0003] = 0x40;
    rom[0x3000] = 0xAA;
    rom[0x4000] = 0xBB;
    cpu.setROM(rom);

    cpu.LDA_a16();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xAA);

    cpu.LDA_a16();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xBB);
}

// Realistic Use Cases
TEST_F(LDAX_LDA_Test, RealisticUseCase_LoadFromBuffer) {
    // Simular cargar datos de un buffer apuntado por BC
    rom[0x8000] = 0x48;  // 'H'
    rom[0x8001] = 0x69;  // 'i'
    cpu.setROM(rom);

    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x8000);
    cpu.LDAX_RR<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x48);

    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x8001);
    cpu.LDAX_RR<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x69);
}

TEST_F(LDAX_LDA_Test, RealisticUseCase_LoadStatus) {
    // Cargar un valor de estado desde una dirección fija
    rom[0x0000] = 0xFF;  // Address 0x80FF (status register)
    rom[0x0001] = 0x80;
    rom[0x80FF] = 0x03;  // Status value
    cpu.setROM(rom);

    cpu.LDA_a16();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x03);
}

TEST_F(LDAX_LDA_Test, RealisticUseCase_ReadArray) {
    // Leer elementos de un array usando LDAX DE
    rom[0x9000] = 0x10;
    rom[0x9001] = 0x20;
    rom[0x9002] = 0x30;
    cpu.setROM(rom);

    for (int i = 0; i < 3; i++) {
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x9000 + i);
        cpu.LDAX_RR<Registers::CombinedRegister::DE>();
        EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), (i + 1) * 0x10);
    }
}

// Comparison Tests
TEST_F(LDAX_LDA_Test, LDAX_BC_DE_Comparison) {
    rom[0x2000] = 0x42;
    rom[0x3000] = 0x42;
    cpu.setROM(rom);

    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x2000);
    cpu.LDAX_RR<Registers::CombinedRegister::BC>();
    uint8_t bcValue = cpu.registers_m.getRegister(Registers::Register::A);

    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x3000);
    cpu.LDAX_RR<Registers::CombinedRegister::DE>();
    uint8_t deValue = cpu.registers_m.getRegister(Registers::Register::A);

    EXPECT_EQ(bcValue, deValue);
}

// Boundary Conditions
TEST_F(LDAX_LDA_Test, BoundaryCondition_ConsecutiveAddresses) {
    rom[0x7FFE] = 0xFE;
    rom[0x7FFF] = 0xFF;
    cpu.setROM(rom);

    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x7FFE);
    cpu.LDAX_RR<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFE);

    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x7FFF);
    cpu.LDAX_RR<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
}

TEST_F(LDAX_LDA_Test, BoundaryCondition_LoadSameAddress) {
    rom[0x5000] = 0x7E;
    cpu.setROM(rom);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x5000);

    cpu.LDAX_RR<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x7E);

    cpu.LDAX_RR<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x7E);
}

// Pattern Tests
TEST_F(LDAX_LDA_Test, PatternTest_AlternatingValues) {
    rom[0x6000] = 0xAA;
    rom[0x6001] = 0x55;
    rom[0x6002] = 0xAA;
    rom[0x6003] = 0x55;
    cpu.setROM(rom);

    for (int i = 0; i < 4; i++) {
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x6000 + i);
        cpu.LDAX_RR<Registers::CombinedRegister::DE>();
        EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), (i % 2 == 0) ? 0xAA : 0x55);
    }
}

// Edge Cases
TEST_F(LDAX_LDA_Test, EdgeCase_LoadFromCodeArea) {
    rom[0x0000] = 0x05;  // Address 0x0005 (en área de código)
    rom[0x0001] = 0x00;
    rom[0x0005] = 0xC3;  // Posible opcode (JMP)
    cpu.setROM(rom);

    cpu.LDA_a16();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xC3);
}

TEST_F(LDAX_LDA_Test, LDAX_BC_SequentialBuffer) {
    // Cargar un buffer secuencial
    for (uint16_t i = 0; i < 5; i++) {
        rom[0xA000 + i] = 0x10 + i;
    }
    cpu.setROM(rom);

    uint16_t addr = 0xA000;
    for (int i = 0; i < 5; i++) {
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, addr++);
        cpu.LDAX_RR<Registers::CombinedRegister::BC>();
        EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x10 + i);
    }
}

TEST_F(LDAX_LDA_Test, LDA_MultipleAddresses) {
    rom[0x0000] = 0x00; rom[0x0001] = 0x10;  // Address 0x1000
    rom[0x0002] = 0x00; rom[0x0003] = 0x20;  // Address 0x2000
    rom[0x0004] = 0x00; rom[0x0005] = 0x30;  // Address 0x3000
    rom[0x1000] = 0x11;
    rom[0x2000] = 0x22;
    rom[0x3000] = 0x33;
    cpu.setROM(rom);

    cpu.LDA_a16();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x11);

    cpu.LDA_a16();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x22);

    cpu.LDA_a16();
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x33);
}

// Round-trip Test with STAX/STA
TEST_F(LDAX_LDA_Test, CompareLDAX_vs_LDA) {
    rom[0x0000] = 0x00;
    rom[0x0001] = 0x50;
    rom[0x5000] = 0x88;
    cpu.setROM(rom);

    // Load usando LDA
    cpu.LDA_a16();
    uint8_t ldaValue = cpu.registers_m.getRegister(Registers::Register::A);

    // Load usando LDAX
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x5000);
    cpu.LDAX_RR<Registers::CombinedRegister::BC>();
    uint8_t ldaxValue = cpu.registers_m.getRegister(Registers::Register::A);

    EXPECT_EQ(ldaValue, ldaxValue);
    EXPECT_EQ(ldaxValue, 0x88);
}
