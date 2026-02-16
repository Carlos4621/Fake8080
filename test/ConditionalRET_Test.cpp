#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"

class ConditionalRET_Test : public ::testing::Test {
protected:
    CPUTest cpu;
    std::array<uint8_t, 0x10000> memory{};

    void SetUp() override {
        memory.fill(0x00);
        cpu.setROM(memory);
        
        // Inicializar SP en una ubicación típica
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xF000);
    }
    
    // Helper para preparar una dirección de retorno en el stack
    void pushReturnAddress(uint16_t address) {
        uint16_t sp = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
        sp -= 2;
        memory[sp] = address & 0xFF;        // Low byte
        memory[sp + 1] = (address >> 8) & 0xFF;  // High byte
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, sp);
    }
};

// ============================================================================
// Tests para RZ (Return if Zero)
// ============================================================================

TEST_F(ConditionalRET_Test, RZ_ReturnsWhenZeroFlagSet) {
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    pushReturnAddress(0x3050);
    
    uint8_t cycles = cpu.RZ();
    
    EXPECT_EQ(cpu.pc_m, 0x3050);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
    EXPECT_EQ(cycles, 11);
}

TEST_F(ConditionalRET_Test, RZ_DoesNotReturnWhenZeroFlagClear) {
    cpu.registers_m.setFlag(Registers::Flags::Z, false);
    pushReturnAddress(0x3050);
    uint16_t originalSP = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    cpu.pc_m = 0x1000;
    
    uint8_t cycles = cpu.RZ();
    
    EXPECT_EQ(cpu.pc_m, 0x1000);  // PC sin cambios
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), originalSP);  // SP sin cambios
    EXPECT_EQ(cycles, 5);
}

// ============================================================================
// Tests para RNZ (Return if Not Zero)
// ============================================================================

TEST_F(ConditionalRET_Test, RNZ_ReturnsWhenZeroFlagClear) {
    cpu.registers_m.setFlag(Registers::Flags::Z, false);
    pushReturnAddress(0x4000);
    
    uint8_t cycles = cpu.RNZ();
    
    EXPECT_EQ(cpu.pc_m, 0x4000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
    EXPECT_EQ(cycles, 11);
}

TEST_F(ConditionalRET_Test, RNZ_DoesNotReturnWhenZeroFlagSet) {
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    pushReturnAddress(0x4000);
    uint16_t originalSP = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    cpu.pc_m = 0x2000;
    
    uint8_t cycles = cpu.RNZ();
    
    EXPECT_EQ(cpu.pc_m, 0x2000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), originalSP);
    EXPECT_EQ(cycles, 5);
}

// ============================================================================
// Tests para RC (Return if Carry)
// ============================================================================

TEST_F(ConditionalRET_Test, RC_ReturnsWhenCarryFlagSet) {
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    pushReturnAddress(0x5000);
    
    uint8_t cycles = cpu.RC();
    
    EXPECT_EQ(cpu.pc_m, 0x5000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
    EXPECT_EQ(cycles, 11);
}

TEST_F(ConditionalRET_Test, RC_DoesNotReturnWhenCarryFlagClear) {
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    pushReturnAddress(0x5000);
    uint16_t originalSP = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    cpu.pc_m = 0x3000;
    
    uint8_t cycles = cpu.RC();
    
    EXPECT_EQ(cpu.pc_m, 0x3000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), originalSP);
    EXPECT_EQ(cycles, 5);
}

// ============================================================================
// Tests para RNC (Return if No Carry)
// ============================================================================

TEST_F(ConditionalRET_Test, RNC_ReturnsWhenCarryFlagClear) {
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    pushReturnAddress(0x6000);
    
    uint8_t cycles = cpu.RNC();
    
    EXPECT_EQ(cpu.pc_m, 0x6000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
    EXPECT_EQ(cycles, 11);
}

TEST_F(ConditionalRET_Test, RNC_DoesNotReturnWhenCarryFlagSet) {
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    pushReturnAddress(0x6000);
    uint16_t originalSP = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    cpu.pc_m = 0x4000;
    
    uint8_t cycles = cpu.RNC();
    
    EXPECT_EQ(cpu.pc_m, 0x4000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), originalSP);
    EXPECT_EQ(cycles, 5);
}

// ============================================================================
// Tests para RM (Return if Minus/Negative)
// ============================================================================

TEST_F(ConditionalRET_Test, RM_ReturnsWhenSignFlagSet) {
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    pushReturnAddress(0x7000);
    
    uint8_t cycles = cpu.RM();
    
    EXPECT_EQ(cpu.pc_m, 0x7000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
    EXPECT_EQ(cycles, 11);
}

TEST_F(ConditionalRET_Test, RM_DoesNotReturnWhenSignFlagClear) {
    cpu.registers_m.setFlag(Registers::Flags::S, false);
    pushReturnAddress(0x7000);
    uint16_t originalSP = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    cpu.pc_m = 0x5000;
    
    uint8_t cycles = cpu.RM();
    
    EXPECT_EQ(cpu.pc_m, 0x5000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), originalSP);
    EXPECT_EQ(cycles, 5);
}

// ============================================================================
// Tests para RP (Return if Positive)
// ============================================================================

TEST_F(ConditionalRET_Test, RP_ReturnsWhenSignFlagClear) {
    cpu.registers_m.setFlag(Registers::Flags::S, false);
    pushReturnAddress(0x8000);
    
    uint8_t cycles = cpu.RP();
    
    EXPECT_EQ(cpu.pc_m, 0x8000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
    EXPECT_EQ(cycles, 11);
}

TEST_F(ConditionalRET_Test, RP_DoesNotReturnWhenSignFlagSet) {
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    pushReturnAddress(0x8000);
    uint16_t originalSP = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    cpu.pc_m = 0x6000;
    
    uint8_t cycles = cpu.RP();
    
    EXPECT_EQ(cpu.pc_m, 0x6000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), originalSP);
    EXPECT_EQ(cycles, 5);
}

// ============================================================================
// Tests para RPE (Return if Parity Even)
// ============================================================================

TEST_F(ConditionalRET_Test, RPE_ReturnsWhenParityFlagSet) {
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    pushReturnAddress(0x9000);
    
    uint8_t cycles = cpu.RPE();
    
    EXPECT_EQ(cpu.pc_m, 0x9000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
    EXPECT_EQ(cycles, 11);
}

TEST_F(ConditionalRET_Test, RPE_DoesNotReturnWhenParityFlagClear) {
    cpu.registers_m.setFlag(Registers::Flags::P, false);
    pushReturnAddress(0x9000);
    uint16_t originalSP = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    cpu.pc_m = 0x7000;
    
    uint8_t cycles = cpu.RPE();
    
    EXPECT_EQ(cpu.pc_m, 0x7000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), originalSP);
    EXPECT_EQ(cycles, 5);
}

// ============================================================================
// Tests para RPO (Return if Parity Odd)
// ============================================================================

TEST_F(ConditionalRET_Test, RPO_ReturnsWhenParityFlagClear) {
    cpu.registers_m.setFlag(Registers::Flags::P, false);
    pushReturnAddress(0xA000);
    
    uint8_t cycles = cpu.RPO();
    
    EXPECT_EQ(cpu.pc_m, 0xA000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
    EXPECT_EQ(cycles, 11);
}

TEST_F(ConditionalRET_Test, RPO_DoesNotReturnWhenParityFlagSet) {
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    pushReturnAddress(0xA000);
    uint16_t originalSP = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    cpu.pc_m = 0x8000;
    
    uint8_t cycles = cpu.RPO();
    
    EXPECT_EQ(cpu.pc_m, 0x8000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), originalSP);
    EXPECT_EQ(cycles, 5);
}

// ============================================================================
// Tests de Preservación
// ============================================================================

TEST_F(ConditionalRET_Test, ConditionalRet_PreservesRegistersWhenReturning) {
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    pushReturnAddress(0x3000);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xAA);
    cpu.registers_m.setRegister(Registers::Register::B, 0xBB);
    cpu.registers_m.setRegister(Registers::Register::C, 0xCC);
    cpu.registers_m.setRegister(Registers::Register::D, 0xDD);
    cpu.registers_m.setRegister(Registers::Register::E, 0xEE);
    cpu.registers_m.setRegister(Registers::Register::H, 0x11);
    cpu.registers_m.setRegister(Registers::Register::L, 0x22);
    
    cpu.RZ();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xAA);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0xBB);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0xCC);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0xDD);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0xEE);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x11);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x22);
}

TEST_F(ConditionalRET_Test, ConditionalRet_PreservesRegistersWhenNotReturning) {
    cpu.registers_m.setFlag(Registers::Flags::Z, false);
    pushReturnAddress(0x3000);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xAA);
    cpu.registers_m.setRegister(Registers::Register::B, 0xBB);
    cpu.registers_m.setRegister(Registers::Register::C, 0xCC);
    cpu.registers_m.setRegister(Registers::Register::D, 0xDD);
    cpu.registers_m.setRegister(Registers::Register::E, 0xEE);
    cpu.registers_m.setRegister(Registers::Register::H, 0x11);
    cpu.registers_m.setRegister(Registers::Register::L, 0x22);
    
    cpu.RZ();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xAA);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0xBB);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0xCC);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0xDD);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0xEE);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x11);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x22);
}

TEST_F(ConditionalRET_Test, ConditionalRet_PreservesFlagsWhenReturning) {
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, false);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, false);
    
    pushReturnAddress(0x3000);
    
    cpu.RZ();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(ConditionalRET_Test, ConditionalRet_PreservesFlagsWhenNotReturning) {
    cpu.registers_m.setFlag(Registers::Flags::Z, false);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, false);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, false);
    
    pushReturnAddress(0x3000);
    
    cpu.RZ();
    
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

// ============================================================================
// Tests de Casos Realistas
// ============================================================================

TEST_F(ConditionalRET_Test, RealisticUseCase_ErrorHandlingReturn) {
    // Retornar de rutina de error solo si hubo un error (Z=1)
    cpu.pc_m = 0x1000;
    memory[0x1000] = 0x00;
    memory[0x1001] = 0xE0;
    
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.CALL_a16();
    EXPECT_EQ(cpu.pc_m, 0xE000);
    
    // Retornar condicionalmente
    cpu.RZ();
    EXPECT_EQ(cpu.pc_m, 0x1002);
}

TEST_F(ConditionalRET_Test, RealisticUseCase_ConditionalSubroutineReturn) {
    // CALL seguido de RET condicional
    cpu.pc_m = 0x2000;
    memory[0x2000] = 0x00;
    memory[0x2001] = 0x50;
    
    cpu.CALL_a16();
    EXPECT_EQ(cpu.pc_m, 0x5000);
    
    // Retornar solo si CY está activo
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.RC();
    EXPECT_EQ(cpu.pc_m, 0x2002);
}

TEST_F(ConditionalRET_Test, RealisticUseCase_EarlyReturnOnError) {
    // Retorno anticipado si se detecta error
    cpu.pc_m = 0x3000;
    memory[0x3000] = 0x00;
    memory[0x3001] = 0x60;
    
    cpu.CALL_a16();
    EXPECT_EQ(cpu.pc_m, 0x6000);
    
    // Si hay error (Z=1), retornar inmediatamente
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.RZ();
    EXPECT_EQ(cpu.pc_m, 0x3002);
}

TEST_F(ConditionalRET_Test, RealisticUseCase_NestedCallsWithConditionalReturns) {
    // Primera llamada
    cpu.pc_m = 0x1000;
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x20;
    
    cpu.CALL_a16();
    EXPECT_EQ(cpu.pc_m, 0x2000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFE);
    
    // Segunda llamada desde dentro de la primera
    memory[0x2000] = 0x00;
    memory[0x2001] = 0x30;
    
    cpu.CALL_a16();
    EXPECT_EQ(cpu.pc_m, 0x3000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFC);
    
    // RET condicional de la segunda (retorna)
    cpu.registers_m.setFlag(Registers::Flags::Z, false);
    cpu.RNZ();
    EXPECT_EQ(cpu.pc_m, 0x2002);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFE);
    
    // RET condicional de la primera (retorna)
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    cpu.RNC();
    EXPECT_EQ(cpu.pc_m, 0x1002);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
}

// ============================================================================
// Tests de Integración
// ============================================================================

TEST_F(ConditionalRET_Test, Integration_AllConditionalReturnsTogether) {
    uint16_t originalSP = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    
    // RZ con Z=1
    pushReturnAddress(0x2000);
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.RZ();
    EXPECT_EQ(cpu.pc_m, 0x2000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), originalSP);
    
    // RNZ con Z=0
    pushReturnAddress(0x3000);
    cpu.registers_m.setFlag(Registers::Flags::Z, false);
    cpu.RNZ();
    EXPECT_EQ(cpu.pc_m, 0x3000);
    
    // RC con CY=1
    pushReturnAddress(0x4000);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.RC();
    EXPECT_EQ(cpu.pc_m, 0x4000);
    
    // RNC con CY=0
    pushReturnAddress(0x5000);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    cpu.RNC();
    EXPECT_EQ(cpu.pc_m, 0x5000);
    
    // RM con S=1
    pushReturnAddress(0x6000);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.RM();
    EXPECT_EQ(cpu.pc_m, 0x6000);
    
    // RP con S=0
    pushReturnAddress(0x7000);
    cpu.registers_m.setFlag(Registers::Flags::S, false);
    cpu.RP();
    EXPECT_EQ(cpu.pc_m, 0x7000);
    
    // RPE con P=1
    pushReturnAddress(0x8000);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.RPE();
    EXPECT_EQ(cpu.pc_m, 0x8000);
    
    // RPO con P=0
    pushReturnAddress(0x9000);
    cpu.registers_m.setFlag(Registers::Flags::P, false);
    cpu.RPO();
    EXPECT_EQ(cpu.pc_m, 0x9000);
    
    // SP debe volver a su valor original
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), originalSP);
}

TEST_F(ConditionalRET_Test, Integration_ConditionalCallsAndReturns) {
    // CZ seguido de RZ
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.pc_m = 0x1000;
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x30;
    
    cpu.CZ_a16();
    EXPECT_EQ(cpu.pc_m, 0x3000);
    
    cpu.RZ();
    EXPECT_EQ(cpu.pc_m, 0x1002);
    
    // CC seguido de RC
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.pc_m = 0x2000;
    memory[0x2000] = 0x00;
    memory[0x2001] = 0x40;
    
    cpu.CC_a16();
    EXPECT_EQ(cpu.pc_m, 0x4000);
    
    cpu.RC();
    EXPECT_EQ(cpu.pc_m, 0x2002);
}

TEST_F(ConditionalRET_Test, Integration_MixedConditionalAndUnconditional) {
    // CALL incondicional seguido de RET condicional
    cpu.pc_m = 0x1000;
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x30;
    
    cpu.CALL_a16();
    EXPECT_EQ(cpu.pc_m, 0x3000);
    
    // RET condicional (retorna)
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.RZ();
    EXPECT_EQ(cpu.pc_m, 0x1002);
    
    // CALL condicional seguido de RET incondicional
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.pc_m = 0x2000;
    memory[0x2000] = 0x00;
    memory[0x2001] = 0x40;
    
    cpu.CC_a16();
    EXPECT_EQ(cpu.pc_m, 0x4000);
    
    cpu.RET();
    EXPECT_EQ(cpu.pc_m, 0x2002);
}

// ============================================================================
// Tests de Edge Cases
// ============================================================================

TEST_F(ConditionalRET_Test, EdgeCase_ReturnToZeroAddress) {
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    pushReturnAddress(0x0000);
    
    cpu.RZ();
    
    EXPECT_EQ(cpu.pc_m, 0x0000);
}

TEST_F(ConditionalRET_Test, EdgeCase_ReturnToMaxAddress) {
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    pushReturnAddress(0xFFFF);
    
    cpu.RZ();
    
    EXPECT_EQ(cpu.pc_m, 0xFFFF);
}

TEST_F(ConditionalRET_Test, EdgeCase_DeepNesting) {
    // Anidar 10 llamadas
    for (int i = 0; i < 10; ++i) {
        cpu.pc_m = 0x1000 + (i * 0x100);
        memory[0x1000 + (i * 0x100)] = 0x00;
        memory[0x1001 + (i * 0x100)] = 0x20 + i;
        
        cpu.CALL_a16();
    }
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000 - (10 * 2));
    
    // Retornar condicionalmente de todas
    for (int i = 0; i < 10; ++i) {
        cpu.registers_m.setFlag(Registers::Flags::Z, true);
        cpu.RZ();
    }
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
}

TEST_F(ConditionalRET_Test, EdgeCase_AlternatingReturns) {
    // Alternar entre retornar y no retornar
    for (int i = 0; i < 5; ++i) {
        pushReturnAddress(0x3000);
        cpu.registers_m.setFlag(Registers::Flags::Z, i % 2 == 0);
        cpu.pc_m = 0x1000;
        
        uint8_t cycles = cpu.RZ();
        
        if (i % 2 == 0) {
            EXPECT_EQ(cpu.pc_m, 0x3000);
            EXPECT_EQ(cycles, 11);
        } else {
            EXPECT_EQ(cpu.pc_m, 0x1000);
            EXPECT_EQ(cycles, 5);
            // Limpiar stack para siguiente iteración
            cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 
                cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP) + 2);
        }
    }
}

TEST_F(ConditionalRET_Test, EdgeCase_ReturnWithoutCall) {
    // Ejecutar RET sin haber hecho CALL (stack ya tiene datos)
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    pushReturnAddress(0x5000);
    
    cpu.RZ();
    
    EXPECT_EQ(cpu.pc_m, 0x5000);
}

// ============================================================================
// Tests de Stress
// ============================================================================

TEST_F(ConditionalRET_Test, StressTest_ManyConditionalReturns) {
    // 50 llamadas y retornos condicionales
    for (int i = 0; i < 50; ++i) {
        cpu.pc_m = 0x1000;
        memory[0x1000] = static_cast<uint8_t>(i & 0xFF);
        memory[0x1001] = 0x20;
        
        cpu.CALL_a16();
        
        cpu.registers_m.setFlag(Registers::Flags::Z, true);
        cpu.RZ();
        
        EXPECT_EQ(cpu.pc_m, 0x1002);
    }
}

TEST_F(ConditionalRET_Test, StressTest_AllConditionsAllFlags) {
    // Probar todas las combinaciones de condiciones y flags
    std::vector<std::pair<uint8_t (CPUTest::*)(), Registers::Flags>> tests = {
        {&CPUTest::RZ, Registers::Flags::Z},
        {&CPUTest::RNZ, Registers::Flags::Z},
        {&CPUTest::RC, Registers::Flags::CY},
        {&CPUTest::RNC, Registers::Flags::CY},
        {&CPUTest::RM, Registers::Flags::S},
        {&CPUTest::RP, Registers::Flags::S},
        {&CPUTest::RPE, Registers::Flags::P},
        {&CPUTest::RPO, Registers::Flags::P}
    };
    
    for (size_t i = 0; i < tests.size(); ++i) {
        // Probar con flag en true
        pushReturnAddress(0x3000);
        cpu.registers_m.setFlag(tests[i].second, true);
        cpu.pc_m = 0x1000;
        
        (cpu.*tests[i].first)();
        
        // Resetear SP si retornó
        if (cpu.pc_m != 0x1000) {
            cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xF000);
        }
        
        // Probar con flag en false
        pushReturnAddress(0x3000);
        cpu.registers_m.setFlag(tests[i].second, false);
        cpu.pc_m = 0x1000;
        
        (cpu.*tests[i].first)();
        
        // Resetear SP
        if (cpu.pc_m != 0x1000) {
            cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xF000);
        } else {
            cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xF000);
        }
    }
}

TEST_F(ConditionalRET_Test, StressTest_RapidCallReturnSequences) {
    // Secuencias rápidas de CALL/RET condicionales
    for (int i = 0; i < 100; ++i) {
        cpu.pc_m = 0x1000;
        memory[0x1000] = 0x00;
        memory[0x1001] = 0x30;
        
        // CALL
        cpu.registers_m.setFlag(Registers::Flags::Z, true);
        cpu.CZ_a16();
        
        // RET inmediato
        cpu.RZ();
        
        EXPECT_EQ(cpu.pc_m, 0x1002);
        EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
    }
}

// ============================================================================
// Tests de Comparación de Ciclos
// ============================================================================

TEST_F(ConditionalRET_Test, CycleComparison_ReturnVsNotReturn) {
    // Retorno condicional (11 ciclos)
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    pushReturnAddress(0x3000);
    uint8_t cycles1 = cpu.RZ();
    EXPECT_EQ(cycles1, 11);
    
    // No retorno (5 ciclos)
    cpu.registers_m.setFlag(Registers::Flags::Z, false);
    pushReturnAddress(0x3000);
    uint8_t cycles2 = cpu.RZ();
    EXPECT_EQ(cycles2, 5);
    
    // Retorno condicional es más lento
    EXPECT_GT(cycles1, cycles2);
}

TEST_F(ConditionalRET_Test, CycleComparison_ConditionalVsUnconditionalRET) {
    // RET incondicional (10 ciclos)
    pushReturnAddress(0x3000);
    uint8_t unconditionalCycles = cpu.RET();
    EXPECT_EQ(unconditionalCycles, 10);
    
    // RET condicional que retorna (11 ciclos)
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    pushReturnAddress(0x3000);
    uint8_t conditionalCycles = cpu.RZ();
    EXPECT_EQ(conditionalCycles, 11);
    
    // RET condicional es 1 ciclo más lento
    EXPECT_EQ(conditionalCycles, unconditionalCycles + 1);
}
