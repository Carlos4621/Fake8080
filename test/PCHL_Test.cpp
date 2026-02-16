#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"

class PCHL_Test : public ::testing::Test {
protected:
    CPUTest cpu;
    std::array<uint8_t, 0x10000> memory{};

    void SetUp() override {
        memory.fill(0x00);
        cpu.setROM(memory);
    }
};

// ============================================================================
// Tests Básicos
// ============================================================================

TEST_F(PCHL_Test, PCHL_BasicCopy) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3000);
    cpu.pc_m = 0x1000;
    
    uint8_t cycles = cpu.PCHL();
    
    EXPECT_EQ(cpu.pc_m, 0x3000);
    EXPECT_EQ(cycles, 5);
}

TEST_F(PCHL_Test, PCHL_CopyZeroValue) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0000);
    cpu.pc_m = 0xFFFF;
    
    uint8_t cycles = cpu.PCHL();
    
    EXPECT_EQ(cpu.pc_m, 0x0000);
    EXPECT_EQ(cycles, 5);
}

TEST_F(PCHL_Test, PCHL_CopyMaxValue) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xFFFF);
    cpu.pc_m = 0x0000;
    
    uint8_t cycles = cpu.PCHL();
    
    EXPECT_EQ(cpu.pc_m, 0xFFFF);
    EXPECT_EQ(cycles, 5);
}

TEST_F(PCHL_Test, PCHL_OverwritesPC) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1234);
    cpu.pc_m = 0x5678;
    
    cpu.PCHL();
    
    EXPECT_EQ(cpu.pc_m, 0x1234);
    // HL debe permanecer sin cambios
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1234);
}

TEST_F(PCHL_Test, PCHL_DifferentValues) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xABCD);
    cpu.pc_m = 0x0100;
    
    cpu.PCHL();
    
    EXPECT_EQ(cpu.pc_m, 0xABCD);
}

// ============================================================================
// Tests de Preservación
// ============================================================================

TEST_F(PCHL_Test, PCHL_PreservesHL) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    
    cpu.PCHL();
    
    // HL no debe cambiar
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2000);
}

TEST_F(PCHL_Test, PCHL_PreservesIndividualHLBytes) {
    cpu.registers_m.setRegister(Registers::Register::H, 0x30);
    cpu.registers_m.setRegister(Registers::Register::L, 0x40);
    
    cpu.PCHL();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x30);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x40);
    EXPECT_EQ(cpu.pc_m, 0x3040);
}

TEST_F(PCHL_Test, PCHL_PreservesAllFlags) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x5000);
    
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, false);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, false);
    
    cpu.PCHL();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(PCHL_Test, PCHL_PreservesOtherRegisters) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x6000);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xAA);
    cpu.registers_m.setRegister(Registers::Register::B, 0xBB);
    cpu.registers_m.setRegister(Registers::Register::C, 0xCC);
    cpu.registers_m.setRegister(Registers::Register::D, 0xDD);
    cpu.registers_m.setRegister(Registers::Register::E, 0xEE);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xF000);
    
    cpu.PCHL();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xAA);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0xBB);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0xCC);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0xDD);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0xEE);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
}

// ============================================================================
// Tests de Casos Realistas
// ============================================================================

TEST_F(PCHL_Test, RealisticUseCase_IndirectJump) {
    // Salto indirecto - PC se carga desde HL
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    cpu.pc_m = 0x1000;
    
    cpu.PCHL();
    
    EXPECT_EQ(cpu.pc_m, 0x2000);
}

TEST_F(PCHL_Test, RealisticUseCase_JumpTable) {
    // Simular tabla de saltos - cargar dirección desde tabla y saltar
    // Dirección de destino en HL
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x8000);
    
    cpu.PCHL();
    
    EXPECT_EQ(cpu.pc_m, 0x8000);
}

TEST_F(PCHL_Test, RealisticUseCase_ComputedJump) {
    // Salto calculado dinámicamente
    // HL contiene dirección calculada en runtime
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x4000);
    
    cpu.PCHL();
    
    EXPECT_EQ(cpu.pc_m, 0x4000);
}

TEST_F(PCHL_Test, RealisticUseCase_ReturnToCalculatedAddress) {
    // Retornar a una dirección calculada (alternativa a RET)
    uint16_t returnAddress = 0x1234;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, returnAddress);
    
    cpu.PCHL();
    
    EXPECT_EQ(cpu.pc_m, returnAddress);
}

TEST_F(PCHL_Test, RealisticUseCase_SwitchStatement) {
    // Implementar switch/case mediante tabla de saltos
    // Caso 0: saltar a 0x3000
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3000);
    cpu.PCHL();
    EXPECT_EQ(cpu.pc_m, 0x3000);
    
    // Caso 1: saltar a 0x3100
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3100);
    cpu.PCHL();
    EXPECT_EQ(cpu.pc_m, 0x3100);
    
    // Caso 2: saltar a 0x3200
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3200);
    cpu.PCHL();
    EXPECT_EQ(cpu.pc_m, 0x3200);
}

// ============================================================================
// Tests de Operaciones Múltiples
// ============================================================================

TEST_F(PCHL_Test, PCHL_MultipleTimes) {
    // Ejecutar PCHL varias veces con diferentes valores
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1000);
    cpu.PCHL();
    EXPECT_EQ(cpu.pc_m, 0x1000);
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    cpu.PCHL();
    EXPECT_EQ(cpu.pc_m, 0x2000);
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3000);
    cpu.PCHL();
    EXPECT_EQ(cpu.pc_m, 0x3000);
}

TEST_F(PCHL_Test, PCHL_AfterHLModification) {
    cpu.registers_m.setRegister(Registers::Register::H, 0x40);
    cpu.registers_m.setRegister(Registers::Register::L, 0x00);
    
    cpu.PCHL();
    EXPECT_EQ(cpu.pc_m, 0x4000);
    
    // Modificar HL
    cpu.registers_m.setRegister(Registers::Register::L, 0x50);
    
    cpu.PCHL();
    EXPECT_EQ(cpu.pc_m, 0x4050);
}

TEST_F(PCHL_Test, PCHL_WithIncrementedHL) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x5000);
    
    cpu.PCHL();
    EXPECT_EQ(cpu.pc_m, 0x5000);
    
    // Incrementar HL
    cpu.INX_RR<Registers::CombinedRegister::HL>();
    
    cpu.PCHL();
    EXPECT_EQ(cpu.pc_m, 0x5001);
}

TEST_F(PCHL_Test, PCHL_ChainedJumps) {
    // Simular una cadena de saltos indirectos
    for (int i = 0; i < 5; ++i) {
        uint16_t address = 0x1000 + (i * 0x100);
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, address);
        cpu.PCHL();
        EXPECT_EQ(cpu.pc_m, address);
    }
}

// ============================================================================
// Tests de Patrones de Valores
// ============================================================================

TEST_F(PCHL_Test, PatternTest_AlternatingBits) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xAAAA);
    
    cpu.PCHL();
    
    EXPECT_EQ(cpu.pc_m, 0xAAAA);
}

TEST_F(PCHL_Test, PatternTest_ComplementaryBits) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x5555);
    
    cpu.PCHL();
    
    EXPECT_EQ(cpu.pc_m, 0x5555);
}

TEST_F(PCHL_Test, PatternTest_LowNibbles) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0F0F);
    
    cpu.PCHL();
    
    EXPECT_EQ(cpu.pc_m, 0x0F0F);
}

TEST_F(PCHL_Test, PatternTest_HighNibbles) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xF0F0);
    
    cpu.PCHL();
    
    EXPECT_EQ(cpu.pc_m, 0xF0F0);
}

TEST_F(PCHL_Test, PatternTest_SingleBitSet) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0001);
    
    cpu.PCHL();
    
    EXPECT_EQ(cpu.pc_m, 0x0001);
}

TEST_F(PCHL_Test, PatternTest_SingleBitSetHigh) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x8000);
    
    cpu.PCHL();
    
    EXPECT_EQ(cpu.pc_m, 0x8000);
}

// ============================================================================
// Tests de Condiciones de Frontera
// ============================================================================

TEST_F(PCHL_Test, BoundaryCondition_PageBoundary) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0100);
    
    cpu.PCHL();
    
    EXPECT_EQ(cpu.pc_m, 0x0100);
}

TEST_F(PCHL_Test, BoundaryCondition_JustBeforeMax) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xFFFE);
    
    cpu.PCHL();
    
    EXPECT_EQ(cpu.pc_m, 0xFFFE);
}

TEST_F(PCHL_Test, BoundaryCondition_JustAfterZero) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0001);
    
    cpu.PCHL();
    
    EXPECT_EQ(cpu.pc_m, 0x0001);
}

TEST_F(PCHL_Test, BoundaryCondition_CodeSegmentStart) {
    // Direcciones típicas de inicio de segmentos de código
    uint16_t codeAddresses[] = {0x0000, 0x0100, 0x1000, 0x8000};
    
    for (uint16_t addr : codeAddresses) {
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, addr);
        cpu.PCHL();
        EXPECT_EQ(cpu.pc_m, addr);
    }
}

TEST_F(PCHL_Test, BoundaryCondition_HighMemory) {
    // Saltos a memoria alta
    uint16_t highAddresses[] = {0xC000, 0xD000, 0xE000, 0xF000};
    
    for (uint16_t addr : highAddresses) {
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, addr);
        cpu.PCHL();
        EXPECT_EQ(cpu.pc_m, addr);
    }
}

// ============================================================================
// Tests de Edge Cases
// ============================================================================

TEST_F(PCHL_Test, EdgeCase_SameValueTwice) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x7000);
    
    cpu.PCHL();
    EXPECT_EQ(cpu.pc_m, 0x7000);
    
    // Ejecutar de nuevo sin cambiar HL
    cpu.PCHL();
    EXPECT_EQ(cpu.pc_m, 0x7000);
}

TEST_F(PCHL_Test, EdgeCase_PCEqualsHL) {
    // PC y HL ya tienen el mismo valor
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x9000);
    cpu.pc_m = 0x9000;
    
    cpu.PCHL();
    
    EXPECT_EQ(cpu.pc_m, 0x9000);
}

TEST_F(PCHL_Test, EdgeCase_JumpToCurrentLocation) {
    // Saltar a la ubicación actual (bucle infinito potencial)
    cpu.pc_m = 0x2000;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    
    cpu.PCHL();
    
    EXPECT_EQ(cpu.pc_m, 0x2000);
}

TEST_F(PCHL_Test, EdgeCase_RapidChanges) {
    // Cambios rápidos de PC usando PCHL
    for (int i = 0; i < 10; ++i) {
        uint16_t value = 0x1000 + (i * 0x100);
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, value);
        cpu.PCHL();
        EXPECT_EQ(cpu.pc_m, value);
    }
}

TEST_F(PCHL_Test, EdgeCase_AfterJMPInstruction) {
    // PCHL después de JMP normal
    cpu.pc_m = 0x1000;
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x20;
    
    cpu.JMP_a16();
    EXPECT_EQ(cpu.pc_m, 0x2000);
    
    // Ahora usar PCHL
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3000);
    cpu.PCHL();
    EXPECT_EQ(cpu.pc_m, 0x3000);
}

// ============================================================================
// Tests de Ciclos
// ============================================================================

TEST_F(PCHL_Test, PCHL_TakesCorrectCycles) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xA000);
    
    uint8_t cycles = cpu.PCHL();
    
    EXPECT_EQ(cycles, 5);
}

TEST_F(PCHL_Test, PCHL_CyclesConsistent) {
    // Verificar que siempre toma 5 ciclos independientemente del valor
    uint16_t testValues[] = {0x0000, 0x1234, 0x8000, 0xFFFF};
    
    for (uint16_t value : testValues) {
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, value);
        uint8_t cycles = cpu.PCHL();
        EXPECT_EQ(cycles, 5);
    }
}

// ============================================================================
// Tests de Stress
// ============================================================================

TEST_F(PCHL_Test, StressTest_ManyExecutions) {
    for (int i = 0; i < 1000; ++i) {
        uint16_t value = static_cast<uint16_t>(i * 17);  // Patrón arbitrario
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, value);
        cpu.PCHL();
        EXPECT_EQ(cpu.pc_m, value);
    }
}

TEST_F(PCHL_Test, StressTest_AllPossibleHighBytes) {
    // Probar todos los valores posibles del byte alto
    for (int h = 0; h <= 0xFF; ++h) {
        cpu.registers_m.setRegister(Registers::Register::H, static_cast<uint8_t>(h));
        cpu.registers_m.setRegister(Registers::Register::L, 0x00);
        
        cpu.PCHL();
        
        uint16_t expected = static_cast<uint16_t>(h << 8);
        EXPECT_EQ(cpu.pc_m, expected);
    }
}

TEST_F(PCHL_Test, StressTest_AllMemoryPages) {
    // Saltar a cada página de memoria (256 páginas total)
    for (int page = 0; page < 256; ++page) {
        uint16_t address = static_cast<uint16_t>(page << 8);
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, address);
        cpu.PCHL();
        EXPECT_EQ(cpu.pc_m, address);
    }
}

// ============================================================================
// Tests de Integración con Otras Operaciones
// ============================================================================

TEST_F(PCHL_Test, Integration_LXI_Then_PCHL) {
    // Cargar HL con LXI y luego saltar con PCHL
    memory[0] = 0x50;  // Low byte
    memory[1] = 0xD0;  // High byte
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::HL>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xD050);
    
    cpu.PCHL();
    EXPECT_EQ(cpu.pc_m, 0xD050);
}

TEST_F(PCHL_Test, Integration_LHLD_Then_PCHL) {
    // Cargar HL desde memoria y luego saltar
    memory[0x1000] = 0x34;  // Low byte
    memory[0x1001] = 0x12;  // High byte
    
    memory[0] = 0x00;
    memory[1] = 0x10;
    
    cpu.LHLD_a16();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1234);
    
    cpu.PCHL();
    EXPECT_EQ(cpu.pc_m, 0x1234);
}

TEST_F(PCHL_Test, Integration_DAD_Then_PCHL) {
    // Sumar a HL y luego saltar
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x0100);
    
    cpu.DAD_RR<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1100);
    
    cpu.PCHL();
    EXPECT_EQ(cpu.pc_m, 0x1100);
}

TEST_F(PCHL_Test, Integration_INX_Then_PCHL) {
    // Incrementar HL y saltar (simular iterar por tabla)
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    
    for (int i = 0; i < 5; ++i) {
        cpu.PCHL();
        EXPECT_EQ(cpu.pc_m, 0x2000 + i);
        cpu.INX_RR<Registers::CombinedRegister::HL>();
    }
}

TEST_F(PCHL_Test, Integration_XCHG_Then_PCHL) {
    // Intercambiar DE y HL, luego saltar (dirección estaba en DE)
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x4000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x5000);
    
    cpu.XCHG();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x4000);
    
    cpu.PCHL();
    EXPECT_EQ(cpu.pc_m, 0x4000);
}

TEST_F(PCHL_Test, Integration_PCHL_Then_CALL) {
    // PCHL para saltar, luego CALL desde nueva ubicación
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xF000);
    
    cpu.PCHL();
    EXPECT_EQ(cpu.pc_m, 0x2000);
    
    // Ahora ejecutar CALL desde 0x2000
    memory[0x2000] = 0x00;
    memory[0x2001] = 0x30;
    cpu.CALL_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x3000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFE);
}

TEST_F(PCHL_Test, Integration_ConditionalJump_Then_PCHL) {
    // Salto condicional seguido de PCHL
    cpu.pc_m = 0x1000;
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x20;
    
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.JZ_a16();
    EXPECT_EQ(cpu.pc_m, 0x2000);
    
    // Ahora usar PCHL
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3000);
    cpu.PCHL();
    EXPECT_EQ(cpu.pc_m, 0x3000);
}

TEST_F(PCHL_Test, Integration_MultipleJumpMethods) {
    // Combinar diferentes métodos de salto
    // JMP directo
    cpu.pc_m = 0x1000;
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x20;
    cpu.JMP_a16();
    EXPECT_EQ(cpu.pc_m, 0x2000);
    
    // CALL
    memory[0x2000] = 0x00;
    memory[0x2001] = 0x30;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xF000);
    cpu.CALL_a16();
    EXPECT_EQ(cpu.pc_m, 0x3000);
    
    // PCHL
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x4000);
    cpu.PCHL();
    EXPECT_EQ(cpu.pc_m, 0x4000);
    
    // RET
    cpu.RET();
    EXPECT_EQ(cpu.pc_m, 0x2002);
}
