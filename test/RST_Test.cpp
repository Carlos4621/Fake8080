#include <gtest/gtest.h>
#include "commons/Fake8080Test.hpp"

class RST_Test : public ::testing::Test {
protected:
    Fake8080Test cpu;
    std::array<uint8_t, 0x10000> memory{};  // 64KB de memoria

    void SetUp() override {
        memory.fill(0x00);
        cpu.mapMemory(memory);
    }
    
    // Helper para inicializar SP usando LXI SP
    void initializeSP(uint16_t spValue) {
        memory[0] = static_cast<uint8_t>(spValue & 0xFF);        // Low byte
        memory[1] = static_cast<uint8_t>((spValue >> 8) & 0xFF); // High byte
        cpu.LXI_RR_d16<Registers::CombinedRegister::SP>();
    }
};

// ============================================================================
// Tests de RST_N para cada vector (0-7)
// ============================================================================

TEST_F(RST_Test, RST_0_JumpsToAddress0x00) {
    initializeSP(0xF000);
    cpu.pc_m = 0x1234;
    
    uint8_t cycles = cpu.RST_N<0>();
    
    EXPECT_EQ(cpu.pc_m, 0x0000);  // Vector 0 -> 0x00
    EXPECT_EQ(cycles, 11);
    
    // Verificar que se guardó el return address en el stack
    uint16_t sp = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    EXPECT_EQ(sp, 0xEFFE);  // SP decrementó 2 bytes
    
    // Verificar el contenido del stack (PC original)
    EXPECT_EQ(memory[0xEFFE], 0x34);  // Low byte de 0x1234
    EXPECT_EQ(memory[0xEFFF], 0x12);  // High byte de 0x1234
}

TEST_F(RST_Test, RST_1_JumpsToAddress0x08) {
    initializeSP(0xF000);
    cpu.pc_m = 0x2000;
    
    uint8_t cycles = cpu.RST_N<1>();
    
    EXPECT_EQ(cpu.pc_m, 0x0008);  // Vector 1 -> 0x08
    EXPECT_EQ(cycles, 11);
    
    uint16_t sp = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    EXPECT_EQ(sp, 0xEFFE);
    
    EXPECT_EQ(memory[0xEFFE], 0x00);  // Low byte de 0x2000
    EXPECT_EQ(memory[0xEFFF], 0x20);  // High byte de 0x2000
}

TEST_F(RST_Test, RST_2_JumpsToAddress0x10) {
    initializeSP(0xF000);
    cpu.pc_m = 0x3456;
    
    uint8_t cycles = cpu.RST_N<2>();
    
    EXPECT_EQ(cpu.pc_m, 0x0010);  // Vector 2 -> 0x10
    EXPECT_EQ(cycles, 11);
    
    uint16_t sp = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    EXPECT_EQ(sp, 0xEFFE);
    
    EXPECT_EQ(memory[0xEFFE], 0x56);  // Low byte de 0x3456
    EXPECT_EQ(memory[0xEFFF], 0x34);  // High byte de 0x3456
}

TEST_F(RST_Test, RST_3_JumpsToAddress0x18) {
    initializeSP(0xF000);
    cpu.pc_m = 0x4567;
    
    uint8_t cycles = cpu.RST_N<3>();
    
    EXPECT_EQ(cpu.pc_m, 0x0018);  // Vector 3 -> 0x18
    EXPECT_EQ(cycles, 11);
    
    uint16_t sp = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    EXPECT_EQ(sp, 0xEFFE);
    
    EXPECT_EQ(memory[0xEFFE], 0x67);  // Low byte de 0x4567
    EXPECT_EQ(memory[0xEFFF], 0x45);  // High byte de 0x4567
}

TEST_F(RST_Test, RST_4_JumpsToAddress0x20) {
    initializeSP(0xF000);
    cpu.pc_m = 0x5678;
    
    uint8_t cycles = cpu.RST_N<4>();
    
    EXPECT_EQ(cpu.pc_m, 0x0020);  // Vector 4 -> 0x20
    EXPECT_EQ(cycles, 11);
    
    uint16_t sp = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    EXPECT_EQ(sp, 0xEFFE);
    
    EXPECT_EQ(memory[0xEFFE], 0x78);  // Low byte de 0x5678
    EXPECT_EQ(memory[0xEFFF], 0x56);  // High byte de 0x5678
}

TEST_F(RST_Test, RST_5_JumpsToAddress0x28) {
    initializeSP(0xF000);
    cpu.pc_m = 0x6789;
    
    uint8_t cycles = cpu.RST_N<5>();
    
    EXPECT_EQ(cpu.pc_m, 0x0028);  // Vector 5 -> 0x28
    EXPECT_EQ(cycles, 11);
    
    uint16_t sp = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    EXPECT_EQ(sp, 0xEFFE);
    
    EXPECT_EQ(memory[0xEFFE], 0x89);  // Low byte de 0x6789
    EXPECT_EQ(memory[0xEFFF], 0x67);  // High byte de 0x6789
}

TEST_F(RST_Test, RST_6_JumpsToAddress0x30) {
    initializeSP(0xF000);
    cpu.pc_m = 0x789A;
    
    uint8_t cycles = cpu.RST_N<6>();
    
    EXPECT_EQ(cpu.pc_m, 0x0030);  // Vector 6 -> 0x30
    EXPECT_EQ(cycles, 11);
    
    uint16_t sp = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    EXPECT_EQ(sp, 0xEFFE);
    
    EXPECT_EQ(memory[0xEFFE], 0x9A);  // Low byte de 0x789A
    EXPECT_EQ(memory[0xEFFF], 0x78);  // High byte de 0x789A
}

TEST_F(RST_Test, RST_7_JumpsToAddress0x38) {
    initializeSP(0xF000);
    cpu.pc_m = 0x89AB;
    
    uint8_t cycles = cpu.RST_N<7>();
    
    EXPECT_EQ(cpu.pc_m, 0x0038);  // Vector 7 -> 0x38
    EXPECT_EQ(cycles, 11);
    
    uint16_t sp = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    EXPECT_EQ(sp, 0xEFFE);
    
    EXPECT_EQ(memory[0xEFFE], 0xAB);  // Low byte de 0x89AB
    EXPECT_EQ(memory[0xEFFF], 0x89);  // High byte de 0x89AB
}

// ============================================================================
// Tests de preservación de flags
// ============================================================================

TEST_F(RST_Test, RST_PreservesAllFlags) {
    initializeSP(0xF000);
    cpu.pc_m = 0x1000;
    
    // Configurar todos los flags
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.registers_m.setFlag(Registers::Flags::S, false);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, false);
    
    cpu.RST_N<3>();
    
    // Verificar que todos los flags se mantienen
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

// ============================================================================
// Tests de preservación de registros
// ============================================================================

TEST_F(RST_Test, RST_PreservesAllRegisters) {
    initializeSP(0xF000);
    cpu.pc_m = 0x1000;
    
    // Configurar todos los registros con valores únicos
    cpu.registers_m.setRegister(Registers::Register::A, 0xAA);
    cpu.registers_m.setRegister(Registers::Register::B, 0xBB);
    cpu.registers_m.setRegister(Registers::Register::C, 0xCC);
    cpu.registers_m.setRegister(Registers::Register::D, 0xDD);
    cpu.registers_m.setRegister(Registers::Register::E, 0xEE);
    cpu.registers_m.setRegister(Registers::Register::H, 0x11);
    cpu.registers_m.setRegister(Registers::Register::L, 0x22);
    
    cpu.RST_N<5>();
    
    // Verificar que todos los registros se mantienen
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xAA);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0xBB);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0xCC);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0xDD);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0xEE);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x11);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x22);
}

// ============================================================================
// Tests de casos extremos con SP
// ============================================================================

TEST_F(RST_Test, RST_WithSPNearTop) {
    // SP cerca del tope de la memoria
    initializeSP(0xFFFF);
    cpu.pc_m = 0x5000;
    
    uint8_t cycles = cpu.RST_N<4>();
    
    EXPECT_EQ(cpu.pc_m, 0x0020);
    EXPECT_EQ(cycles, 11);
    
    uint16_t sp = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    EXPECT_EQ(sp, 0xFFFD);
    
    // Verificar que se guardó correctamente (con wrap-around)
    EXPECT_EQ(memory[0xFFFD], 0x00);  // Low byte de 0x5000
    EXPECT_EQ(memory[0xFFFE], 0x50);  // High byte de 0x5000
}

TEST_F(RST_Test, RST_WithSPAt0x0002) {
    // SP muy bajo
    initializeSP(0x0002);
    cpu.pc_m = 0x8000;
    
    uint8_t cycles = cpu.RST_N<7>();
    
    EXPECT_EQ(cpu.pc_m, 0x0038);
    EXPECT_EQ(cycles, 11);
    
    uint16_t sp = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    EXPECT_EQ(sp, 0x0000);
    
    EXPECT_EQ(memory[0x0000], 0x00);  // Low byte de 0x8000
    EXPECT_EQ(memory[0x0001], 0x80);  // High byte de 0x8000
}

// ============================================================================
// Tests de múltiples RST consecutivos
// ============================================================================

TEST_F(RST_Test, RST_MultipleConsecutiveCalls) {
    initializeSP(0xF000);
    cpu.pc_m = 0x1000;
    
    // Primer RST
    cpu.RST_N<0>();
    EXPECT_EQ(cpu.pc_m, 0x0000);
    uint16_t sp1 = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    EXPECT_EQ(sp1, 0xEFFE);
    
    // Segundo RST desde la nueva ubicación
    cpu.pc_m = 0x2000;
    cpu.RST_N<1>();
    EXPECT_EQ(cpu.pc_m, 0x0008);
    uint16_t sp2 = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    EXPECT_EQ(sp2, 0xEFFC);  // SP decrementó otros 2 bytes
    
    // Verificar ambos return addresses en el stack
    EXPECT_EQ(memory[0xEFFC], 0x00);  // Low byte de 0x2000
    EXPECT_EQ(memory[0xEFFD], 0x20);  // High byte de 0x2000
    EXPECT_EQ(memory[0xEFFE], 0x00);  // Low byte de 0x1000
    EXPECT_EQ(memory[0xEFFF], 0x10);  // High byte de 0x1000
}

TEST_F(RST_Test, RST_AllVectorsSequentially) {
    initializeSP(0xF000);
    
    for (int i = 0; i < 8; i++) {
        cpu.pc_m = 0x1000 + i * 0x100;  // PC diferente para cada RST
        
        uint8_t cycles = 0;
        switch(i) {
            case 0: cycles = cpu.RST_N<0>(); break;
            case 1: cycles = cpu.RST_N<1>(); break;
            case 2: cycles = cpu.RST_N<2>(); break;
            case 3: cycles = cpu.RST_N<3>(); break;
            case 4: cycles = cpu.RST_N<4>(); break;
            case 5: cycles = cpu.RST_N<5>(); break;
            case 6: cycles = cpu.RST_N<6>(); break;
            case 7: cycles = cpu.RST_N<7>(); break;
        }
        
        EXPECT_EQ(cpu.pc_m, i * 8);  // Vector i -> i*8
        EXPECT_EQ(cycles, 11);
        
        uint16_t sp = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
        EXPECT_EQ(sp, 0xF000 - (i + 1) * 2);  // SP decrementó 2 por cada RST
    }
}

// ============================================================================
// Tests de casos de borde con PC
// ============================================================================

TEST_F(RST_Test, RST_FromPCAt0x0000) {
    initializeSP(0xF000);
    cpu.pc_m = 0x0000;
    
    uint8_t cycles = cpu.RST_N<4>();
    
    EXPECT_EQ(cpu.pc_m, 0x0020);
    EXPECT_EQ(cycles, 11);
    
    // Verificar que se guardó 0x0000
    EXPECT_EQ(memory[0xEFFE], 0x00);
    EXPECT_EQ(memory[0xEFFF], 0x00);
}

TEST_F(RST_Test, RST_FromPCAtMaxAddress) {
    initializeSP(0xF000);
    cpu.pc_m = 0xFFFF;
    
    uint8_t cycles = cpu.RST_N<7>();
    
    EXPECT_EQ(cpu.pc_m, 0x0038);
    EXPECT_EQ(cycles, 11);
    
    // Verificar que se guardó 0xFFFF
    EXPECT_EQ(memory[0xEFFE], 0xFF);
    EXPECT_EQ(memory[0xEFFF], 0xFF);
}
