#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"

class JMP_Test : public ::testing::Test {
protected:
    CPUTest cpu;
    std::array<uint8_t, 0x10000> memory{}; // 64KB de memoria

    void SetUp() override {
        memory.fill(0x00);
        cpu.mapMemory(memory);
    }
};

// ============================================================================
// Tests Básicos
// ============================================================================

TEST_F(JMP_Test, JMP_BasicJump) {
    memory[0] = 0x12;  // Low byte de dirección 0x3456
    memory[1] = 0x34;  // High byte
    
    uint8_t cycles = cpu.JMP_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x3412);
    EXPECT_EQ(cycles, 10);
}

TEST_F(JMP_Test, JMP_ToZeroAddress) {
    memory[0] = 0x00;
    memory[1] = 0x00;
    
    uint8_t cycles = cpu.JMP_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x0000);
    EXPECT_EQ(cycles, 10);
}

TEST_F(JMP_Test, JMP_ToMaxAddress) {
    memory[0] = 0xFF;
    memory[1] = 0xFF;
    
    uint8_t cycles = cpu.JMP_a16();
    
    EXPECT_EQ(cpu.pc_m, 0xFFFF);
    EXPECT_EQ(cycles, 10);
}

TEST_F(JMP_Test, JMP_LowByteOnly) {
    memory[0] = 0xAB;
    memory[1] = 0x00;
    
    uint8_t cycles = cpu.JMP_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x00AB);
    EXPECT_EQ(cycles, 10);
}

TEST_F(JMP_Test, JMP_HighByteOnly) {
    memory[0] = 0x00;
    memory[1] = 0xCD;
    
    uint8_t cycles = cpu.JMP_a16();
    
    EXPECT_EQ(cpu.pc_m, 0xCD00);
    EXPECT_EQ(cycles, 10);
}

// ============================================================================
// Tests de Preservación de Flags
// ============================================================================

TEST_F(JMP_Test, JMP_PreservesAllFlags) {
    memory[0] = 0x00;
    memory[1] = 0x10;
    
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    
    cpu.JMP_a16();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(JMP_Test, JMP_PreservesFlagsClear) {
    memory[0] = 0x00;
    memory[1] = 0x20;
    
    cpu.registers_m.setFlag(Registers::Flags::Z, false);
    cpu.registers_m.setFlag(Registers::Flags::S, false);
    cpu.registers_m.setFlag(Registers::Flags::P, false);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    cpu.registers_m.setFlag(Registers::Flags::AC, false);
    
    cpu.JMP_a16();
    
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

// ============================================================================
// Tests de Preservación de Registros
// ============================================================================

TEST_F(JMP_Test, JMP_PreservesAllRegisters) {
    memory[0] = 0x00;
    memory[1] = 0x30;
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x12);
    cpu.registers_m.setRegister(Registers::Register::B, 0x34);
    cpu.registers_m.setRegister(Registers::Register::C, 0x56);
    cpu.registers_m.setRegister(Registers::Register::D, 0x78);
    cpu.registers_m.setRegister(Registers::Register::E, 0x9A);
    cpu.registers_m.setRegister(Registers::Register::H, 0xBC);
    cpu.registers_m.setRegister(Registers::Register::L, 0xDE);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xF000);
    
    cpu.JMP_a16();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x12);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x34);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x56);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x78);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x9A);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0xBC);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0xDE);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
}

// ============================================================================
// Tests de Saltos Consecutivos
// ============================================================================

TEST_F(JMP_Test, JMP_ConsecutiveJumps) {
    // Primer salto a 0x1000
    memory[0] = 0x00;
    memory[1] = 0x10;
    cpu.JMP_a16();
    EXPECT_EQ(cpu.pc_m, 0x1000);
    
    // Segundo salto a 0x2000
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x20;
    cpu.JMP_a16();
    EXPECT_EQ(cpu.pc_m, 0x2000);
    
    // Tercer salto a 0x3000
    memory[0x2000] = 0x00;
    memory[0x2001] = 0x30;
    cpu.JMP_a16();
    EXPECT_EQ(cpu.pc_m, 0x3000);
}

TEST_F(JMP_Test, JMP_BackwardJump) {
    // Establecer PC en 0x5000
    cpu.pc_m = 0x5000;
    
    // Saltar hacia atrás a 0x1000
    memory[0x5000] = 0x00;
    memory[0x5001] = 0x10;
    
    uint8_t cycles = cpu.JMP_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x1000);
    EXPECT_EQ(cycles, 10);
}

TEST_F(JMP_Test, JMP_ForwardJump) {
    // Establecer PC en 0x1000
    cpu.pc_m = 0x1000;
    
    // Saltar hacia adelante a 0x8000
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x80;
    
    uint8_t cycles = cpu.JMP_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x8000);
    EXPECT_EQ(cycles, 10);
}

// ============================================================================
// Casos de Uso Realistas
// ============================================================================

TEST_F(JMP_Test, RealisticUseCase_JumpToSubroutineArea) {
    // Salto común a área de subrutinas en 0x0100
    memory[0] = 0x00;
    memory[1] = 0x01;
    
    cpu.JMP_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x0100);
}

TEST_F(JMP_Test, RealisticUseCase_JumpToInterruptVector) {
    // Salto a vector de interrupción típico
    memory[0] = 0x38;  // RST 7 vector (0x0038)
    memory[1] = 0x00;
    
    cpu.JMP_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x0038);
}

TEST_F(JMP_Test, RealisticUseCase_JumpToHighMemory) {
    // Salto a área alta de memoria (ROM típica)
    memory[0] = 0x00;
    memory[1] = 0xF0;
    
    cpu.JMP_a16();
    
    EXPECT_EQ(cpu.pc_m, 0xF000);
}

// ============================================================================
// Tests de Patrones de Direcciones
// ============================================================================

TEST_F(JMP_Test, PatternTest_AlternatingBits) {
    memory[0] = 0xAA;  // 10101010
    memory[1] = 0x55;  // 01010101
    
    cpu.JMP_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x55AA);
}

TEST_F(JMP_Test, PatternTest_AllOnes) {
    memory[0] = 0xFF;
    memory[1] = 0xFF;
    
    cpu.JMP_a16();
    
    EXPECT_EQ(cpu.pc_m, 0xFFFF);
}

TEST_F(JMP_Test, PatternTest_LowNibbles) {
    memory[0] = 0x0F;
    memory[1] = 0x0F;
    
    cpu.JMP_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x0F0F);
}

TEST_F(JMP_Test, PatternTest_HighNibbles) {
    memory[0] = 0xF0;
    memory[1] = 0xF0;
    
    cpu.JMP_a16();
    
    EXPECT_EQ(cpu.pc_m, 0xF0F0);
}

// ============================================================================
// Tests de Condiciones de Frontera
// ============================================================================

TEST_F(JMP_Test, BoundaryCondition_PageBoundary) {
    // Salto a límite de página (0x0100, 0x0200, etc.)
    memory[0] = 0x00;
    memory[1] = 0x02;
    
    cpu.JMP_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x0200);
}

TEST_F(JMP_Test, BoundaryCondition_JustBeforeMax) {
    memory[0] = 0xFE;
    memory[1] = 0xFF;
    
    cpu.JMP_a16();
    
    EXPECT_EQ(cpu.pc_m, 0xFFFE);
}

TEST_F(JMP_Test, BoundaryCondition_JustAfterZero) {
    memory[0] = 0x01;
    memory[1] = 0x00;
    
    cpu.JMP_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x0001);
}

// ============================================================================
// Tests de Casos Especiales
// ============================================================================

TEST_F(JMP_Test, EdgeCase_JumpToCurrentPC) {
    // Saltar a la dirección donde está la instrucción JMP (loop infinito)
    cpu.pc_m = 0x1000;
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x10;
    
    cpu.JMP_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x1000);
}

TEST_F(JMP_Test, EdgeCase_JumpAfterArithmetic) {
    // Salto después de operación aritmética que afecta flags
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    memory[0] = 0x50;
    memory[1] = 0x40;
    
    cpu.JMP_a16();
    
    // El salto no debe afectar los flags
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_EQ(cpu.pc_m, 0x4050);
}

TEST_F(JMP_Test, EdgeCase_MultipleJumpsToSameLocation) {
    // Varios saltos a la misma ubicación
    memory[0] = 0x00;
    memory[1] = 0x50;
    cpu.JMP_a16();
    EXPECT_EQ(cpu.pc_m, 0x5000);
    
    cpu.pc_m = 0x2000;
    memory[0x2000] = 0x00;
    memory[0x2001] = 0x50;
    cpu.JMP_a16();
    EXPECT_EQ(cpu.pc_m, 0x5000);
    
    cpu.pc_m = 0x3000;
    memory[0x3000] = 0x00;
    memory[0x3001] = 0x50;
    cpu.JMP_a16();
    EXPECT_EQ(cpu.pc_m, 0x5000);
}

// ============================================================================
// Tests de Orden de Bytes (Little Endian)
// ============================================================================

TEST_F(JMP_Test, ByteOrder_LittleEndian) {
    // Verificar que se lee correctamente en formato little-endian
    memory[0] = 0x34;  // Low byte (parte baja de la dirección)
    memory[1] = 0x12;  // High byte (parte alta de la dirección)
    
    cpu.JMP_a16();
    
    // Debe formar 0x1234, NO 0x3412
    EXPECT_EQ(cpu.pc_m, 0x1234);
}

TEST_F(JMP_Test, ByteOrder_VerifyBytePositions) {
    memory[0] = 0xEF;  // Low byte
    memory[1] = 0xBE;  // High byte
    
    cpu.JMP_a16();
    
    // Debe formar 0xBEEF (0xDEADBEEF pero 16 bits)
    EXPECT_EQ(cpu.pc_m, 0xBEEF);
}

// ============================================================================
// Tests de Stress
// ============================================================================

TEST_F(JMP_Test, StressTest_ManySequentialJumps) {
    // Realizar muchos saltos secuenciales
    for (uint16_t i = 0; i < 100; ++i) {
        uint16_t targetAddr = i * 0x100;
        cpu.pc_m = i * 0x10;
        memory[i * 0x10] = static_cast<uint8_t>(targetAddr & 0xFF);
        memory[i * 0x10 + 1] = static_cast<uint8_t>((targetAddr >> 8) & 0xFF);
        
        cpu.JMP_a16();
        
        EXPECT_EQ(cpu.pc_m, targetAddr);
    }
}

TEST_F(JMP_Test, StressTest_JumpChain) {
    // Crear una cadena de saltos: 0x0000 -> 0x1000 -> 0x2000 -> 0x3000
    memory[0x0000] = 0x00;
    memory[0x0001] = 0x10;
    
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x20;
    
    memory[0x2000] = 0x00;
    memory[0x2001] = 0x30;
    
    cpu.pc_m = 0x0000;
    cpu.JMP_a16();
    EXPECT_EQ(cpu.pc_m, 0x1000);
    
    cpu.JMP_a16();
    EXPECT_EQ(cpu.pc_m, 0x2000);
    
    cpu.JMP_a16();
    EXPECT_EQ(cpu.pc_m, 0x3000);
}

// ============================================================================
// Tests de Saltos Condicionales
// ============================================================================

// JZ - Jump if Zero (Z=1)
TEST_F(JMP_Test, JZ_JumpsWhenZeroFlagSet) {
    memory[0] = 0x00;
    memory[1] = 0x20;
    
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    
    uint8_t cycles = cpu.JZ_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x2000);
    EXPECT_EQ(cycles, 10);
}

TEST_F(JMP_Test, JZ_DoesNotJumpWhenZeroFlagClear) {
    cpu.pc_m = 0x1000;
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x30;
    
    cpu.registers_m.setFlag(Registers::Flags::Z, false);
    
    uint8_t cycles = cpu.JZ_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x1002);
    EXPECT_EQ(cycles, 10);
}

// JNZ - Jump if Not Zero (Z=0)
TEST_F(JMP_Test, JNZ_JumpsWhenZeroFlagClear) {
    memory[0] = 0x00;
    memory[1] = 0x40;
    
    cpu.registers_m.setFlag(Registers::Flags::Z, false);
    
    uint8_t cycles = cpu.JNZ_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x4000);
    EXPECT_EQ(cycles, 10);
}

TEST_F(JMP_Test, JNZ_DoesNotJumpWhenZeroFlagSet) {
    cpu.pc_m = 0x2000;
    memory[0x2000] = 0x00;
    memory[0x2001] = 0x50;
    
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    
    uint8_t cycles = cpu.JNZ_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x2002);
    EXPECT_EQ(cycles, 10);
}

// JC - Jump if Carry (CY=1)
TEST_F(JMP_Test, JC_JumpsWhenCarryFlagSet) {
    memory[0] = 0x00;
    memory[1] = 0x60;
    
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.JC_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x6000);
    EXPECT_EQ(cycles, 10);
}

TEST_F(JMP_Test, JC_DoesNotJumpWhenCarryFlagClear) {
    cpu.pc_m = 0x3000;
    memory[0x3000] = 0x00;
    memory[0x3001] = 0x70;
    
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    uint8_t cycles = cpu.JC_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x3002);
    EXPECT_EQ(cycles, 10);
}

// JNC - Jump if No Carry (CY=0)
TEST_F(JMP_Test, JNC_JumpsWhenCarryFlagClear) {
    memory[0] = 0x00;
    memory[1] = 0x80;
    
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    uint8_t cycles = cpu.JNC_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x8000);
    EXPECT_EQ(cycles, 10);
}

TEST_F(JMP_Test, JNC_DoesNotJumpWhenCarryFlagSet) {
    cpu.pc_m = 0x4000;
    memory[0x4000] = 0x00;
    memory[0x4001] = 0x90;
    
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    uint8_t cycles = cpu.JNC_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x4002);
    EXPECT_EQ(cycles, 10);
}

// JP - Jump if Positive (S=0)
TEST_F(JMP_Test, JP_JumpsWhenSignFlagClear) {
    memory[0] = 0x00;
    memory[1] = 0xA0;
    
    cpu.registers_m.setFlag(Registers::Flags::S, false);
    
    uint8_t cycles = cpu.JP_a16();
    
    EXPECT_EQ(cpu.pc_m, 0xA000);
    EXPECT_EQ(cycles, 10);
}

TEST_F(JMP_Test, JP_DoesNotJumpWhenSignFlagSet) {
    cpu.pc_m = 0x5000;
    memory[0x5000] = 0x00;
    memory[0x5001] = 0xB0;
    
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    
    uint8_t cycles = cpu.JP_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x5002);
    EXPECT_EQ(cycles, 10);
}

// JM - Jump if Minus (S=1)
TEST_F(JMP_Test, JM_JumpsWhenSignFlagSet) {
    memory[0] = 0x00;
    memory[1] = 0xC0;
    
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    
    uint8_t cycles = cpu.JM_a16();
    
    EXPECT_EQ(cpu.pc_m, 0xC000);
    EXPECT_EQ(cycles, 10);
}

TEST_F(JMP_Test, JM_DoesNotJumpWhenSignFlagClear) {
    cpu.pc_m = 0x6000;
    memory[0x6000] = 0x00;
    memory[0x6001] = 0xD0;
    
    cpu.registers_m.setFlag(Registers::Flags::S, false);
    
    uint8_t cycles = cpu.JM_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x6002);
    EXPECT_EQ(cycles, 10);
}

// JPE - Jump if Parity Even (P=1)
TEST_F(JMP_Test, JPE_JumpsWhenParityFlagSet) {
    memory[0] = 0x00;
    memory[1] = 0xE0;
    
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    
    uint8_t cycles = cpu.JPE_a16();
    
    EXPECT_EQ(cpu.pc_m, 0xE000);
    EXPECT_EQ(cycles, 10);
}

TEST_F(JMP_Test, JPE_DoesNotJumpWhenParityFlagClear) {
    cpu.pc_m = 0x7000;
    memory[0x7000] = 0x00;
    memory[0x7001] = 0xF0;
    
    cpu.registers_m.setFlag(Registers::Flags::P, false);
    
    uint8_t cycles = cpu.JPE_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x7002);
    EXPECT_EQ(cycles, 10);
}

// JPO - Jump if Parity Odd (P=0)
TEST_F(JMP_Test, JPO_JumpsWhenParityFlagClear) {
    memory[0] = 0x34;
    memory[1] = 0x12;
    
    cpu.registers_m.setFlag(Registers::Flags::P, false);
    
    uint8_t cycles = cpu.JPO_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x1234);
    EXPECT_EQ(cycles, 10);
}

TEST_F(JMP_Test, JPO_DoesNotJumpWhenParityFlagSet) {
    cpu.pc_m = 0x8000;
    memory[0x8000] = 0x78;
    memory[0x8001] = 0x56;
    
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    
    uint8_t cycles = cpu.JPO_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x8002);
    EXPECT_EQ(cycles, 10);
}

// ============================================================================
// Tests de Combinaciones de Flags
// ============================================================================

TEST_F(JMP_Test, ConditionalJumps_PreserveOtherFlags) {
    memory[0] = 0x00;
    memory[1] = 0x10;
    
    // Establecer múltiples flags
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.registers_m.setFlag(Registers::Flags::S, false);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    
    cpu.JZ_a16();
    
    // Verificar que los otros flags no cambian
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
}

TEST_F(JMP_Test, ConditionalJumps_PreserveAllRegisters) {
    memory[0] = 0x00;
    memory[1] = 0x20;
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xAA);
    cpu.registers_m.setRegister(Registers::Register::B, 0xBB);
    cpu.registers_m.setRegister(Registers::Register::C, 0xCC);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xF000);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.JC_a16();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xAA);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0xBB);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0xCC);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
}

// ============================================================================
// Tests de Casos Realistas con Saltos Condicionales
// ============================================================================

TEST_F(JMP_Test, RealisticUseCase_LoopWithJNZ) {
    // Simular un bucle que se repite mientras contador != 0
    cpu.pc_m = 0x1000;
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x10;
    
    // Primera iteración: contador no es cero, salta
    cpu.registers_m.setFlag(Registers::Flags::Z, false);
    cpu.JNZ_a16();
    EXPECT_EQ(cpu.pc_m, 0x1000);  // Vuelve al inicio
    
    // Segunda iteración: contador es cero, no salta
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.JNZ_a16();
    EXPECT_EQ(cpu.pc_m, 0x1002);  // Sale del bucle
}

TEST_F(JMP_Test, RealisticUseCase_ErrorHandlingWithJC) {
    // Saltar a rutina de error si hay carry (overflow)
    memory[0] = 0x00;
    memory[1] = 0xFF;  // Dirección de manejo de error
    
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.JC_a16();
    
    EXPECT_EQ(cpu.pc_m, 0xFF00);  // Saltó a manejador de error
}

TEST_F(JMP_Test, RealisticUseCase_SignedComparisonWithJM) {
    // Saltar si el resultado es negativo
    memory[0] = 0x50;
    memory[1] = 0x20;
    
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.JM_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x2050);
}

// ============================================================================
// Tests de Secuencias de Saltos Condicionales
// ============================================================================

TEST_F(JMP_Test, ConditionalJumpChain) {
    // Secuencia de saltos condicionales
    
    // JZ no salta (Z=0)
    cpu.pc_m = 0x1000;
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x90;
    cpu.registers_m.setFlag(Registers::Flags::Z, false);
    cpu.JZ_a16();
    EXPECT_EQ(cpu.pc_m, 0x1002);
    
    // JNC salta (CY=0)
    memory[0x1002] = 0x00;
    memory[0x1003] = 0x20;
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    cpu.JNC_a16();
    EXPECT_EQ(cpu.pc_m, 0x2000);
    
    // JP salta (S=0)
    memory[0x2000] = 0x00;
    memory[0x2001] = 0x30;
    cpu.registers_m.setFlag(Registers::Flags::S, false);
    cpu.JP_a16();
    EXPECT_EQ(cpu.pc_m, 0x3000);
}

TEST_F(JMP_Test, AllConditionalJumps_SameCycleCount) {
    memory[0] = 0x00;
    memory[1] = 0x10;
    
    // Todos los saltos condicionales deben tomar 10 ciclos
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    EXPECT_EQ(cpu.JZ_a16(), 10);
    
    cpu.pc_m = 0;
    cpu.registers_m.setFlag(Registers::Flags::Z, false);
    EXPECT_EQ(cpu.JNZ_a16(), 10);
    
    cpu.pc_m = 0;
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    EXPECT_EQ(cpu.JC_a16(), 10);
    
    cpu.pc_m = 0;
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    EXPECT_EQ(cpu.JNC_a16(), 10);
    
    cpu.pc_m = 0;
    cpu.registers_m.setFlag(Registers::Flags::S, false);
    EXPECT_EQ(cpu.JP_a16(), 10);
    
    cpu.pc_m = 0;
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    EXPECT_EQ(cpu.JM_a16(), 10);
    
    cpu.pc_m = 0;
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    EXPECT_EQ(cpu.JPE_a16(), 10);
    
    cpu.pc_m = 0;
    cpu.registers_m.setFlag(Registers::Flags::P, false);
    EXPECT_EQ(cpu.JPO_a16(), 10);
}

// ============================================================================
// Tests de Edge Cases con Saltos Condicionales
// ============================================================================

TEST_F(JMP_Test, ConditionalJump_MultipleConsecutiveFalseBranches) {
    cpu.pc_m = 0x1000;
    
    // Varios saltos que no se toman
    memory[0x1000] = 0x00;
    memory[0x1001] = 0xAA;
    cpu.registers_m.setFlag(Registers::Flags::Z, false);
    cpu.JZ_a16();
    EXPECT_EQ(cpu.pc_m, 0x1002);
    
    memory[0x1002] = 0x00;
    memory[0x1003] = 0xBB;
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    cpu.JC_a16();
    EXPECT_EQ(cpu.pc_m, 0x1004);
    
    memory[0x1004] = 0x00;
    memory[0x1005] = 0xCC;
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.JP_a16();
    EXPECT_EQ(cpu.pc_m, 0x1006);
}

TEST_F(JMP_Test, ConditionalJump_AfterArithmeticOperation) {
    memory[0] = 0x00;
    memory[1] = 0x50;
    
    // Simular que una operación aritmética puso Z=1
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    cpu.registers_m.setFlag(Registers::Flags::S, false);
    
    cpu.JZ_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x5000);
    // Verificar que otros flags permanecen
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(JMP_Test, ConditionalJump_BothTrueFalseConditionsCovered) {
    // Asegurar que cada salto condicional se prueba en ambas direcciones
    
    memory[0] = 0x00;
    memory[1] = 0x10;
    
    // JZ: True y False
    cpu.pc_m = 0;
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.JZ_a16();
    EXPECT_EQ(cpu.pc_m, 0x1000);
    
    cpu.pc_m = 0;
    cpu.registers_m.setFlag(Registers::Flags::Z, false);
    cpu.JZ_a16();
    EXPECT_EQ(cpu.pc_m, 0x0002);
}
