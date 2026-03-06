#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"

class ConditionalCALL_Test : public ::testing::Test {
protected:
    CPUTest cpu;
    std::array<uint8_t, 0x10000> memory{};

    void SetUp() override {
        memory.fill(0x00);
        cpu.mapMemory(memory);
        
        // Inicializar SP en una ubicación típica
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xF000);
    }
};

// ============================================================================
// Tests para CZ (Call if Zero)
// ============================================================================

TEST_F(ConditionalCALL_Test, CZ_CallsWhenZeroFlagSet) {
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.pc_m = 0x1000;
    
    memory[0x1000] = 0x50;  // Low byte
    memory[0x1001] = 0x30;  // High byte
    
    uint8_t cycles = cpu.CZ_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x3050);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFE);
    EXPECT_EQ(memory[0xEFFE], 0x02);  // Low byte of return address (0x1002)
    EXPECT_EQ(memory[0xEFFF], 0x10);  // High byte of return address
    EXPECT_EQ(cycles, 17);
}

TEST_F(ConditionalCALL_Test, CZ_DoesNotCallWhenZeroFlagClear) {
    cpu.registers_m.setFlag(Registers::Flags::Z, false);
    cpu.pc_m = 0x1000;
    
    memory[0x1000] = 0x50;
    memory[0x1001] = 0x30;
    
    uint8_t cycles = cpu.CZ_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x1002);  // Solo avanza PC
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);  // SP sin cambios
    EXPECT_EQ(cycles, 11);
}

// ============================================================================
// Tests para CNZ (Call if Not Zero)
// ============================================================================

TEST_F(ConditionalCALL_Test, CNZ_CallsWhenZeroFlagClear) {
    cpu.registers_m.setFlag(Registers::Flags::Z, false);
    cpu.pc_m = 0x2000;
    
    memory[0x2000] = 0x00;
    memory[0x2001] = 0x40;
    
    uint8_t cycles = cpu.CNZ_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x4000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFE);
    EXPECT_EQ(memory[0xEFFE], 0x02);
    EXPECT_EQ(memory[0xEFFF], 0x20);
    EXPECT_EQ(cycles, 17);
}

TEST_F(ConditionalCALL_Test, CNZ_DoesNotCallWhenZeroFlagSet) {
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.pc_m = 0x2000;
    
    memory[0x2000] = 0x00;
    memory[0x2001] = 0x40;
    
    uint8_t cycles = cpu.CNZ_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x2002);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
    EXPECT_EQ(cycles, 11);
}

// ============================================================================
// Tests para CC (Call if Carry)
// ============================================================================

TEST_F(ConditionalCALL_Test, CC_CallsWhenCarryFlagSet) {
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.pc_m = 0x3000;
    
    memory[0x3000] = 0x00;
    memory[0x3001] = 0x50;
    
    uint8_t cycles = cpu.CC_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x5000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFE);
    EXPECT_EQ(memory[0xEFFE], 0x02);
    EXPECT_EQ(memory[0xEFFF], 0x30);
    EXPECT_EQ(cycles, 17);
}

TEST_F(ConditionalCALL_Test, CC_DoesNotCallWhenCarryFlagClear) {
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    cpu.pc_m = 0x3000;
    
    memory[0x3000] = 0x00;
    memory[0x3001] = 0x50;
    
    uint8_t cycles = cpu.CC_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x3002);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
    EXPECT_EQ(cycles, 11);
}

// ============================================================================
// Tests para CNC (Call if No Carry)
// ============================================================================

TEST_F(ConditionalCALL_Test, CNC_CallsWhenCarryFlagClear) {
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    cpu.pc_m = 0x4000;
    
    memory[0x4000] = 0x00;
    memory[0x4001] = 0x60;
    
    uint8_t cycles = cpu.CNC_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x6000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFE);
    EXPECT_EQ(memory[0xEFFE], 0x02);
    EXPECT_EQ(memory[0xEFFF], 0x40);
    EXPECT_EQ(cycles, 17);
}

TEST_F(ConditionalCALL_Test, CNC_DoesNotCallWhenCarryFlagSet) {
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.pc_m = 0x4000;
    
    memory[0x4000] = 0x00;
    memory[0x4001] = 0x60;
    
    uint8_t cycles = cpu.CNC_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x4002);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
    EXPECT_EQ(cycles, 11);
}

// ============================================================================
// Tests para CM (Call if Minus/Negative)
// ============================================================================

TEST_F(ConditionalCALL_Test, CM_CallsWhenSignFlagSet) {
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.pc_m = 0x5000;
    
    memory[0x5000] = 0x00;
    memory[0x5001] = 0x70;
    
    uint8_t cycles = cpu.CM_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x7000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFE);
    EXPECT_EQ(memory[0xEFFE], 0x02);
    EXPECT_EQ(memory[0xEFFF], 0x50);
    EXPECT_EQ(cycles, 17);
}

TEST_F(ConditionalCALL_Test, CM_DoesNotCallWhenSignFlagClear) {
    cpu.registers_m.setFlag(Registers::Flags::S, false);
    cpu.pc_m = 0x5000;
    
    memory[0x5000] = 0x00;
    memory[0x5001] = 0x70;
    
    uint8_t cycles = cpu.CM_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x5002);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
    EXPECT_EQ(cycles, 11);
}

// ============================================================================
// Tests para CP (Call if Positive)
// ============================================================================

TEST_F(ConditionalCALL_Test, CP_CallsWhenSignFlagClear) {
    cpu.registers_m.setFlag(Registers::Flags::S, false);
    cpu.pc_m = 0x6000;
    
    memory[0x6000] = 0x00;
    memory[0x6001] = 0x80;
    
    uint8_t cycles = cpu.CP_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x8000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFE);
    EXPECT_EQ(memory[0xEFFE], 0x02);
    EXPECT_EQ(memory[0xEFFF], 0x60);
    EXPECT_EQ(cycles, 17);
}

TEST_F(ConditionalCALL_Test, CP_DoesNotCallWhenSignFlagSet) {
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.pc_m = 0x6000;
    
    memory[0x6000] = 0x00;
    memory[0x6001] = 0x80;
    
    uint8_t cycles = cpu.CP_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x6002);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
    EXPECT_EQ(cycles, 11);
}

// ============================================================================
// Tests para CPE (Call if Parity Even)
// ============================================================================

TEST_F(ConditionalCALL_Test, CPE_CallsWhenParityFlagSet) {
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.pc_m = 0x7000;
    
    memory[0x7000] = 0x00;
    memory[0x7001] = 0x90;
    
    uint8_t cycles = cpu.CPE_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x9000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFE);
    EXPECT_EQ(memory[0xEFFE], 0x02);
    EXPECT_EQ(memory[0xEFFF], 0x70);
    EXPECT_EQ(cycles, 17);
}

TEST_F(ConditionalCALL_Test, CPE_DoesNotCallWhenParityFlagClear) {
    cpu.registers_m.setFlag(Registers::Flags::P, false);
    cpu.pc_m = 0x7000;
    
    memory[0x7000] = 0x00;
    memory[0x7001] = 0x90;
    
    uint8_t cycles = cpu.CPE_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x7002);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
    EXPECT_EQ(cycles, 11);
}

// ============================================================================
// Tests para CPO (Call if Parity Odd)
// ============================================================================

TEST_F(ConditionalCALL_Test, CPO_CallsWhenParityFlagClear) {
    cpu.registers_m.setFlag(Registers::Flags::P, false);
    cpu.pc_m = 0x8000;
    
    memory[0x8000] = 0x00;
    memory[0x8001] = 0xA0;
    
    uint8_t cycles = cpu.CPO_a16();
    
    EXPECT_EQ(cpu.pc_m, 0xA000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFE);
    EXPECT_EQ(memory[0xEFFE], 0x02);
    EXPECT_EQ(memory[0xEFFF], 0x80);
    EXPECT_EQ(cycles, 17);
}

TEST_F(ConditionalCALL_Test, CPO_DoesNotCallWhenParityFlagSet) {
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.pc_m = 0x8000;
    
    memory[0x8000] = 0x00;
    memory[0x8001] = 0xA0;
    
    uint8_t cycles = cpu.CPO_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x8002);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
    EXPECT_EQ(cycles, 11);
}

// ============================================================================
// Tests de Preservación
// ============================================================================

TEST_F(ConditionalCALL_Test, ConditionalCall_PreservesRegistersWhenCalling) {
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xAA);
    cpu.registers_m.setRegister(Registers::Register::B, 0xBB);
    cpu.registers_m.setRegister(Registers::Register::C, 0xCC);
    cpu.registers_m.setRegister(Registers::Register::D, 0xDD);
    cpu.registers_m.setRegister(Registers::Register::E, 0xEE);
    cpu.registers_m.setRegister(Registers::Register::H, 0x11);
    cpu.registers_m.setRegister(Registers::Register::L, 0x22);
    
    cpu.pc_m = 0x1000;
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x30;
    
    cpu.CZ_a16();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xAA);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0xBB);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0xCC);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0xDD);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0xEE);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x11);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x22);
}

TEST_F(ConditionalCALL_Test, ConditionalCall_PreservesRegistersWhenNotCalling) {
    cpu.registers_m.setFlag(Registers::Flags::Z, false);
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xAA);
    cpu.registers_m.setRegister(Registers::Register::B, 0xBB);
    cpu.registers_m.setRegister(Registers::Register::C, 0xCC);
    cpu.registers_m.setRegister(Registers::Register::D, 0xDD);
    cpu.registers_m.setRegister(Registers::Register::E, 0xEE);
    cpu.registers_m.setRegister(Registers::Register::H, 0x11);
    cpu.registers_m.setRegister(Registers::Register::L, 0x22);
    
    cpu.pc_m = 0x1000;
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x30;
    
    cpu.CZ_a16();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xAA);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0xBB);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0xCC);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0xDD);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0xEE);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x11);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x22);
}

TEST_F(ConditionalCALL_Test, ConditionalCall_PreservesFlagsWhenCalling) {
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, false);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, false);
    
    cpu.pc_m = 0x1000;
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x30;
    
    cpu.CZ_a16();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(ConditionalCALL_Test, ConditionalCall_PreservesFlagsWhenNotCalling) {
    cpu.registers_m.setFlag(Registers::Flags::Z, false);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::P, false);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, false);
    
    cpu.pc_m = 0x1000;
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x30;
    
    cpu.CZ_a16();
    
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

// ============================================================================
// Tests de Casos Realistas
// ============================================================================

TEST_F(ConditionalCALL_Test, RealisticUseCase_ErrorHandling) {
    // Llamar a rutina de error solo si hubo un error (Z=1 indica error)
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.pc_m = 0x1000;
    
    memory[0x1000] = 0x00;  // Dirección de error handler
    memory[0x1001] = 0xE0;
    
    cpu.CZ_a16();
    
    EXPECT_EQ(cpu.pc_m, 0xE000);  // Saltó a error handler
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFE);
}

TEST_F(ConditionalCALL_Test, RealisticUseCase_ConditionalSubroutine) {
    // Llamar a subrutina solo si CY está activo
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.pc_m = 0x2000;
    
    memory[0x2000] = 0x00;
    memory[0x2001] = 0x50;
    
    cpu.CC_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x5000);
    
    // Retornar de la subrutina
    cpu.RET();
    EXPECT_EQ(cpu.pc_m, 0x2002);
}

TEST_F(ConditionalCALL_Test, RealisticUseCase_NegativeNumberProcessing) {
    // Llamar a rutina especial si el número es negativo (S=1)
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.pc_m = 0x3000;
    
    memory[0x3000] = 0x00;
    memory[0x3001] = 0x60;
    
    cpu.CM_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x6000);
}

TEST_F(ConditionalCALL_Test, RealisticUseCase_NestedConditionalCalls) {
    // Primera llamada condicional
    cpu.registers_m.setFlag(Registers::Flags::Z, false);
    cpu.pc_m = 0x1000;
    
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x20;
    
    cpu.CNZ_a16();
    EXPECT_EQ(cpu.pc_m, 0x2000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFE);
    
    // Segunda llamada condicional desde dentro de la primera
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    cpu.pc_m = 0x2000;
    
    memory[0x2000] = 0x00;
    memory[0x2001] = 0x30;
    
    cpu.CNC_a16();
    EXPECT_EQ(cpu.pc_m, 0x3000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFC);
    
    // Retornar de ambas
    cpu.RET();
    EXPECT_EQ(cpu.pc_m, 0x2002);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFE);
    
    cpu.RET();
    EXPECT_EQ(cpu.pc_m, 0x1002);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
}

// ============================================================================
// Tests de Integración
// ============================================================================

TEST_F(ConditionalCALL_Test, Integration_AllConditionalCallsTogether) {
    // Probar todas las llamadas condicionales en secuencia
    uint16_t originalSP = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    
    // CZ con Z=1
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.pc_m = 0x1000;
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x20;
    cpu.CZ_a16();
    EXPECT_EQ(cpu.pc_m, 0x2000);
    cpu.RET();
    
    // CNZ con Z=0
    cpu.registers_m.setFlag(Registers::Flags::Z, false);
    cpu.pc_m = 0x1010;
    memory[0x1010] = 0x00;
    memory[0x1011] = 0x30;
    cpu.CNZ_a16();
    EXPECT_EQ(cpu.pc_m, 0x3000);
    cpu.RET();
    
    // CC con CY=1
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.pc_m = 0x1020;
    memory[0x1020] = 0x00;
    memory[0x1021] = 0x40;
    cpu.CC_a16();
    EXPECT_EQ(cpu.pc_m, 0x4000);
    cpu.RET();
    
    // CNC con CY=0
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    cpu.pc_m = 0x1030;
    memory[0x1030] = 0x00;
    memory[0x1031] = 0x50;
    cpu.CNC_a16();
    EXPECT_EQ(cpu.pc_m, 0x5000);
    cpu.RET();
    
    // CM con S=1
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.pc_m = 0x1040;
    memory[0x1040] = 0x00;
    memory[0x1041] = 0x60;
    cpu.CM_a16();
    EXPECT_EQ(cpu.pc_m, 0x6000);
    cpu.RET();
    
    // CP con S=0
    cpu.registers_m.setFlag(Registers::Flags::S, false);
    cpu.pc_m = 0x1050;
    memory[0x1050] = 0x00;
    memory[0x1051] = 0x70;
    cpu.CP_a16();
    EXPECT_EQ(cpu.pc_m, 0x7000);
    cpu.RET();
    
    // CPE con P=1
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.pc_m = 0x1060;
    memory[0x1060] = 0x00;
    memory[0x1061] = 0x80;
    cpu.CPE_a16();
    EXPECT_EQ(cpu.pc_m, 0x8000);
    cpu.RET();
    
    // CPO con P=0
    cpu.registers_m.setFlag(Registers::Flags::P, false);
    cpu.pc_m = 0x1070;
    memory[0x1070] = 0x00;
    memory[0x1071] = 0x90;
    cpu.CPO_a16();
    EXPECT_EQ(cpu.pc_m, 0x9000);
    cpu.RET();
    
    // SP debe volver a su valor original
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), originalSP);
}

TEST_F(ConditionalCALL_Test, Integration_MixedCallsAndJumps) {
    // Mezclar CALLs condicionales con JMPs condicionales
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.pc_m = 0x1000;
    
    // JZ primero
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x20;
    cpu.JZ_a16();
    EXPECT_EQ(cpu.pc_m, 0x2000);
    
    // Luego CZ
    memory[0x2000] = 0x00;
    memory[0x2001] = 0x30;
    cpu.CZ_a16();
    EXPECT_EQ(cpu.pc_m, 0x3000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFE);
    
    // RET
    cpu.RET();
    EXPECT_EQ(cpu.pc_m, 0x2002);
}

// ============================================================================
// Tests de Edge Cases
// ============================================================================

TEST_F(ConditionalCALL_Test, EdgeCase_CallToZeroAddress) {
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.pc_m = 0x1000;
    
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x00;
    
    cpu.CZ_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x0000);
}

TEST_F(ConditionalCALL_Test, EdgeCase_CallToMaxAddress) {
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.pc_m = 0x1000;
    
    memory[0x1000] = 0xFF;
    memory[0x1001] = 0xFF;
    
    cpu.CZ_a16();
    
    EXPECT_EQ(cpu.pc_m, 0xFFFF);
}

TEST_F(ConditionalCALL_Test, EdgeCase_DeepNesting) {
    // Llamadas condicionales anidadas profundamente
    for (int i = 0; i < 10; ++i) {
        cpu.registers_m.setFlag(Registers::Flags::Z, true);
        cpu.pc_m = 0x1000 + (i * 0x100);
        
        memory[0x1000 + (i * 0x100)] = 0x00;
        memory[0x1001 + (i * 0x100)] = 0x20 + i;
        
        cpu.CZ_a16();
        EXPECT_EQ(cpu.pc_m, 0x2000 + (i * 0x100));
    }
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000 - (10 * 2));
    
    // Retornar de todas
    for (int i = 0; i < 10; ++i) {
        cpu.RET();
    }
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
}

TEST_F(ConditionalCALL_Test, EdgeCase_AlternatingConditions) {
    // Alternar entre llamar y no llamar
    for (int i = 0; i < 5; ++i) {
        cpu.registers_m.setFlag(Registers::Flags::Z, i % 2 == 0);
        cpu.pc_m = 0x1000;
        
        memory[0x1000] = 0x00;
        memory[0x1001] = 0x30;
        
        uint8_t cycles = cpu.CZ_a16();
        
        if (i % 2 == 0) {
            EXPECT_EQ(cpu.pc_m, 0x3000);
            EXPECT_EQ(cycles, 17);
            cpu.RET();  // Retornar para siguiente iteración
        } else {
            EXPECT_EQ(cpu.pc_m, 0x1002);
            EXPECT_EQ(cycles, 11);
        }
    }
}

// ============================================================================
// Tests de Stress
// ============================================================================

TEST_F(ConditionalCALL_Test, StressTest_ManyConditionalCalls) {
    // 50 llamadas y retornos condicionales
    for (int i = 0; i < 50; ++i) {
        cpu.registers_m.setFlag(Registers::Flags::Z, true);
        cpu.pc_m = 0x1000;
        
        memory[0x1000] = static_cast<uint8_t>(i & 0xFF);
        memory[0x1001] = 0x20;
        
        cpu.CZ_a16();
        
        uint16_t expectedPC = 0x2000 + (i & 0xFF);
        EXPECT_EQ(cpu.pc_m, expectedPC);
        
        cpu.RET();
        EXPECT_EQ(cpu.pc_m, 0x1002);
    }
}

TEST_F(ConditionalCALL_Test, StressTest_AllConditionsAllFlags) {
    // Probar todas las combinaciones de condiciones y flags
    std::vector<std::pair<uint8_t (CPUTest::*)(), Registers::Flags>> tests = {
        {&CPUTest::CZ_a16, Registers::Flags::Z},
        {&CPUTest::CNZ_a16, Registers::Flags::Z},
        {&CPUTest::CC_a16, Registers::Flags::CY},
        {&CPUTest::CNC_a16, Registers::Flags::CY},
        {&CPUTest::CM_a16, Registers::Flags::S},
        {&CPUTest::CP_a16, Registers::Flags::S},
        {&CPUTest::CPE_a16, Registers::Flags::P},
        {&CPUTest::CPO_a16, Registers::Flags::P}
    };
    
    for (size_t i = 0; i < tests.size(); ++i) {
        // Probar con flag en true
        cpu.registers_m.setFlag(tests[i].second, true);
        cpu.pc_m = 0x1000;
        memory[0x1000] = 0x00;
        memory[0x1001] = 0x30;
        
        (cpu.*tests[i].first)();
        
        // Limpiar (puede haber saltado o no)
        if (cpu.pc_m != 0x1002) {
            cpu.RET();
        }
        
        // Probar con flag en false
        cpu.registers_m.setFlag(tests[i].second, false);
        cpu.pc_m = 0x1000;
        
        (cpu.*tests[i].first)();
        
        // Limpiar
        if (cpu.pc_m != 0x1002) {
            cpu.RET();
        }
    }
}
