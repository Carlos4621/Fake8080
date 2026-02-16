#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"

class SPHL_Test : public ::testing::Test {
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

TEST_F(SPHL_Test, SPHL_BasicCopy) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xF000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0x0000);
    
    uint8_t cycles = cpu.SPHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
    EXPECT_EQ(cycles, 5);
}

TEST_F(SPHL_Test, SPHL_CopyZeroValue) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xFFFF);
    
    uint8_t cycles = cpu.SPHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x0000);
    EXPECT_EQ(cycles, 5);
}

TEST_F(SPHL_Test, SPHL_CopyMaxValue) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xFFFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0x0000);
    
    uint8_t cycles = cpu.SPHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xFFFF);
    EXPECT_EQ(cycles, 5);
}

TEST_F(SPHL_Test, SPHL_OverwritesSP) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1234);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0x5678);
    
    cpu.SPHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x1234);
    // HL debe permanecer sin cambios
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1234);
}

TEST_F(SPHL_Test, SPHL_DifferentValues) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xABCD);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0x0100);
    
    cpu.SPHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xABCD);
}

// ============================================================================
// Tests de Preservación
// ============================================================================

TEST_F(SPHL_Test, SPHL_PreservesHL) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    
    cpu.SPHL();
    
    // HL no debe cambiar
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2000);
}

TEST_F(SPHL_Test, SPHL_PreservesIndividualHLBytes) {
    cpu.registers_m.setRegister(Registers::Register::H, 0x30);
    cpu.registers_m.setRegister(Registers::Register::L, 0x40);
    
    cpu.SPHL();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x30);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x40);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x3040);
}

TEST_F(SPHL_Test, SPHL_PreservesAllFlags) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x5000);
    
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, false);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, false);
    
    cpu.SPHL();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(SPHL_Test, SPHL_PreservesOtherRegisters) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x6000);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xAA);
    cpu.registers_m.setRegister(Registers::Register::B, 0xBB);
    cpu.registers_m.setRegister(Registers::Register::C, 0xCC);
    cpu.registers_m.setRegister(Registers::Register::D, 0xDD);
    cpu.registers_m.setRegister(Registers::Register::E, 0xEE);
    
    cpu.SPHL();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xAA);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0xBB);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0xCC);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0xDD);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0xEE);
}

// ============================================================================
// Tests de Casos Realistas
// ============================================================================

TEST_F(SPHL_Test, RealisticUseCase_InitializeStack) {
    // Usar SPHL para inicializar el stack pointer
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xF000);
    
    cpu.SPHL();
    
    // Ahora SP está listo para operaciones de stack
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
    
    // Verificar que podemos usar el stack después
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x1234);
    cpu.PUSH_RR<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFE);
}

TEST_F(SPHL_Test, RealisticUseCase_DynamicStackRelocation) {
    // Mover el stack a una nueva ubicación durante ejecución
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0x8000);
    
    // Decidir mover el stack a 0xE000
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xE000);
    cpu.SPHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xE000);
}

TEST_F(SPHL_Test, RealisticUseCase_RestoreStackPointer) {
    // Guardar SP original en HL, hacer operaciones, restaurar
    uint16_t originalSP = 0xF000;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, originalSP);
    
    // Guardar SP en HL
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, originalSP);
    
    // Cambiar SP temporalmente
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0x2000);
    
    // Restaurar SP usando SPHL
    cpu.SPHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), originalSP);
}

TEST_F(SPHL_Test, RealisticUseCase_PrepareForCALL) {
    // Configurar stack antes de una serie de CALL
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xF800);
    cpu.SPHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF800);
    
    // Simular CALL (usaría el SP recién configurado)
    cpu.pc_m = 0x1000;
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x20;
    cpu.CALL_a16();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF7FE);
}

// ============================================================================
// Tests de Operaciones Múltiples
// ============================================================================

TEST_F(SPHL_Test, SPHL_MultipleTimes) {
    // Ejecutar SPHL varias veces con diferentes valores
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1000);
    cpu.SPHL();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x1000);
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);
    cpu.SPHL();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x2000);
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3000);
    cpu.SPHL();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x3000);
}

TEST_F(SPHL_Test, SPHL_AfterHLModification) {
    cpu.registers_m.setRegister(Registers::Register::H, 0x40);
    cpu.registers_m.setRegister(Registers::Register::L, 0x00);
    
    cpu.SPHL();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x4000);
    
    // Modificar HL
    cpu.registers_m.setRegister(Registers::Register::L, 0x50);
    
    cpu.SPHL();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x4050);
}

TEST_F(SPHL_Test, SPHL_WithIncrementedHL) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x5000);
    
    cpu.SPHL();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x5000);
    
    // Incrementar HL
    cpu.INX_RR<Registers::CombinedRegister::HL>();
    
    cpu.SPHL();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x5001);
}

// ============================================================================
// Tests de Patrones de Valores
// ============================================================================

TEST_F(SPHL_Test, PatternTest_AlternatingBits) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xAAAA);
    
    cpu.SPHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xAAAA);
}

TEST_F(SPHL_Test, PatternTest_ComplementaryBits) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x5555);
    
    cpu.SPHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x5555);
}

TEST_F(SPHL_Test, PatternTest_LowNibbles) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0F0F);
    
    cpu.SPHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x0F0F);
}

TEST_F(SPHL_Test, PatternTest_HighNibbles) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xF0F0);
    
    cpu.SPHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF0F0);
}

TEST_F(SPHL_Test, PatternTest_SingleBitSet) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0001);
    
    cpu.SPHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x0001);
}

TEST_F(SPHL_Test, PatternTest_SingleBitSetHigh) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x8000);
    
    cpu.SPHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x8000);
}

// ============================================================================
// Tests de Condiciones de Frontera
// ============================================================================

TEST_F(SPHL_Test, BoundaryCondition_PageBoundary) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0100);
    
    cpu.SPHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x0100);
}

TEST_F(SPHL_Test, BoundaryCondition_JustBeforeMax) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xFFFE);
    
    cpu.SPHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xFFFE);
}

TEST_F(SPHL_Test, BoundaryCondition_JustAfterZero) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0001);
    
    cpu.SPHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x0001);
}

TEST_F(SPHL_Test, BoundaryCondition_TypicalStackStart) {
    // Direcciones típicas de inicio de stack
    uint16_t stackAddresses[] = {0xF000, 0xE000, 0xD000, 0xC000};
    
    for (uint16_t addr : stackAddresses) {
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, addr);
        cpu.SPHL();
        EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), addr);
    }
}

// ============================================================================
// Tests de Edge Cases
// ============================================================================

TEST_F(SPHL_Test, EdgeCase_SameValueTwice) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x7000);
    
    cpu.SPHL();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x7000);
    
    // Ejecutar de nuevo sin cambiar HL
    cpu.SPHL();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x7000);
}

TEST_F(SPHL_Test, EdgeCase_SPEqualsHL) {
    // SP y HL ya tienen el mismo valor
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x9000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0x9000);
    
    cpu.SPHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0x9000);
}

TEST_F(SPHL_Test, EdgeCase_AfterStackOperations) {
    // SPHL después de operaciones de stack
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xF000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x1234);
    
    cpu.PUSH_RR<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFE);
    
    // Restaurar SP con SPHL
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xF000);
    cpu.SPHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
}

TEST_F(SPHL_Test, EdgeCase_RapidChanges) {
    // Cambios rápidos de SP usando SPHL
    for (int i = 0; i < 10; ++i) {
        uint16_t value = 0x1000 + (i * 0x100);
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, value);
        cpu.SPHL();
        EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), value);
    }
}

// ============================================================================
// Tests de Ciclos
// ============================================================================

TEST_F(SPHL_Test, SPHL_TakesCorrectCycles) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xA000);
    
    uint8_t cycles = cpu.SPHL();
    
    EXPECT_EQ(cycles, 5);
}

TEST_F(SPHL_Test, SPHL_CyclesConsistent) {
    // Verificar que siempre toma 5 ciclos independientemente del valor
    uint16_t testValues[] = {0x0000, 0x1234, 0x8000, 0xFFFF};
    
    for (uint16_t value : testValues) {
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, value);
        uint8_t cycles = cpu.SPHL();
        EXPECT_EQ(cycles, 5);
    }
}

// ============================================================================
// Tests de Stress
// ============================================================================

TEST_F(SPHL_Test, StressTest_ManyExecutions) {
    for (int i = 0; i < 1000; ++i) {
        uint16_t value = static_cast<uint16_t>(i * 17);  // Patrón arbitrario
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, value);
        cpu.SPHL();
        EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), value);
    }
}

TEST_F(SPHL_Test, StressTest_AllPossibleHighBytes) {
    // Probar todos los valores posibles del byte alto
    for (int h = 0; h <= 0xFF; ++h) {
        cpu.registers_m.setRegister(Registers::Register::H, static_cast<uint8_t>(h));
        cpu.registers_m.setRegister(Registers::Register::L, 0x00);
        
        cpu.SPHL();
        
        uint16_t expected = static_cast<uint16_t>(h << 8);
        EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), expected);
    }
}

// ============================================================================
// Tests de Integración con Otras Operaciones
// ============================================================================

TEST_F(SPHL_Test, Integration_SPHL_Then_PUSH_POP) {
    // Configurar SP con SPHL, luego usar stack
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xE000);
    cpu.SPHL();
    
    // PUSH
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0xABCD);
    cpu.PUSH_RR<Registers::CombinedRegister::DE>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xDFFE);
    
    // POP
    cpu.POP_RR<Registers::CombinedRegister::BC>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0xABCD);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xE000);
}

TEST_F(SPHL_Test, Integration_SPHL_Then_CALL_RET) {
    // Configurar SP con SPHL, luego CALL/RET
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xF800);
    cpu.SPHL();
    
    cpu.pc_m = 0x1000;
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x30;
    
    cpu.CALL_a16();
    EXPECT_EQ(cpu.pc_m, 0x3000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF7FE);
    
    cpu.RET();
    EXPECT_EQ(cpu.pc_m, 0x1002);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF800);
}

TEST_F(SPHL_Test, Integration_LXI_Then_SPHL) {
    // Cargar HL con LXI y luego copiar a SP con SPHL
    memory[0] = 0x50;  // Low byte
    memory[1] = 0xD0;  // High byte
    
    cpu.LXI_RR_d16<Registers::CombinedRegister::HL>();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xD050);
    
    cpu.SPHL();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xD050);
}
