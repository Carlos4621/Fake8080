#include <gtest/gtest.h>
#include "commons/Fake8080Test.hpp"

class CALL_RET_Test : public ::testing::Test {
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
// Tests Básicos de CALL
// ============================================================================

TEST_F(CALL_RET_Test, CALL_BasicCall) {
    initializeSP(0xF000);
    cpu.pc_m = 0x1000;
    
    memory[0x1000] = 0x00;  // Low byte de dirección 0x2000
    memory[0x1001] = 0x20;  // High byte
    
    uint8_t cycles = cpu.CALL_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x2000);  // PC apunta a la subrutina
    EXPECT_EQ(cycles, 17);
    
    // Verificar que se guardó el return address en el stack
    uint16_t sp = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    EXPECT_EQ(sp, 0xEFFE);  // SP decrementó 2 bytes
    
    // Verificar el contenido del stack (PC + 2 = 0x1002)
    EXPECT_EQ(memory[0xEFFE], 0x02);  // Low byte de 0x1002
    EXPECT_EQ(memory[0xEFFF], 0x10);  // High byte de 0x1002
}

TEST_F(CALL_RET_Test, CALL_ToZeroAddress) {
    initializeSP(0xF000);
    cpu.pc_m = 0x0500;
    
    memory[0x0500] = 0x00;
    memory[0x0501] = 0x00;
    
    uint8_t cycles = cpu.CALL_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x0000);
    EXPECT_EQ(cycles, 17);
    
    // Verificar stack contiene 0x0502 (PC después de leer 2 bytes)
    EXPECT_EQ(memory[0xEFFE], 0x02);
    EXPECT_EQ(memory[0xEFFF], 0x05);
}

TEST_F(CALL_RET_Test, CALL_ToMaxAddress) {
    initializeSP(0xF000);
    cpu.pc_m = 0x0000;
    
    memory[0x0000] = 0xFF;
    memory[0x0001] = 0xFF;
    
    uint8_t cycles = cpu.CALL_a16();
    
    EXPECT_EQ(cpu.pc_m, 0xFFFF);
    EXPECT_EQ(cycles, 17);
}

TEST_F(CALL_RET_Test, CALL_PreservesFlags) {
    initializeSP(0xF000);
    cpu.pc_m = 0x1000;
    
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x30;
    
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    cpu.registers_m.setFlag(Registers::Flags::S, false);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, false);
    
    cpu.CALL_a16();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
}

TEST_F(CALL_RET_Test, CALL_PreservesRegisters) {
    initializeSP(0xF000);
    cpu.pc_m = 0x2000;
    
    memory[0x2000] = 0x00;
    memory[0x2001] = 0x40;
    
    cpu.registers_m.setRegister(Registers::Register::A, 0xAA);
    cpu.registers_m.setRegister(Registers::Register::B, 0xBB);
    cpu.registers_m.setRegister(Registers::Register::C, 0xCC);
    cpu.registers_m.setRegister(Registers::Register::D, 0xDD);
    cpu.registers_m.setRegister(Registers::Register::E, 0xEE);
    cpu.registers_m.setRegister(Registers::Register::H, 0x11);
    cpu.registers_m.setRegister(Registers::Register::L, 0x22);
    
    cpu.CALL_a16();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xAA);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0xBB);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0xCC);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0xDD);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0xEE);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x11);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x22);
}

// ============================================================================
// Tests Básicos de RET
// ============================================================================

TEST_F(CALL_RET_Test, RET_BasicReturn) {
    initializeSP(0xEFFE);
    
    // Simular stack con dirección de retorno 0x3000
    memory[0xEFFE] = 0x00;  // Low byte
    memory[0xEFFF] = 0x30;  // High byte
    
    uint8_t cycles = cpu.RET();
    
    EXPECT_EQ(cpu.pc_m, 0x3000);
    EXPECT_EQ(cycles, 10);
    
    // SP debe incrementarse 2 bytes
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
}

TEST_F(CALL_RET_Test, RET_ToZeroAddress) {
    initializeSP(0xF000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xEFFE);
    
    memory[0xEFFE] = 0x00;
    memory[0xEFFF] = 0x00;
    
    uint8_t cycles = cpu.RET();
    
    EXPECT_EQ(cpu.pc_m, 0x0000);
    EXPECT_EQ(cycles, 10);
}

TEST_F(CALL_RET_Test, RET_ToMaxAddress) {
    initializeSP(0xF000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xEFFE);
    
    memory[0xEFFE] = 0xFF;
    memory[0xEFFF] = 0xFF;
    
    uint8_t cycles = cpu.RET();
    
    EXPECT_EQ(cpu.pc_m, 0xFFFF);
    EXPECT_EQ(cycles, 10);
}

TEST_F(CALL_RET_Test, RET_PreservesFlags) {
    initializeSP(0xEFFE);
    memory[0xEFFE] = 0x00;
    memory[0xEFFF] = 0x20;
    
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    
    cpu.RET();
    
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
}

TEST_F(CALL_RET_Test, RET_PreservesRegisters) {
    initializeSP(0xEFFE);
    memory[0xEFFE] = 0x00;
    memory[0xEFFF] = 0x50;
    
    cpu.registers_m.setRegister(Registers::Register::A, 0x12);
    cpu.registers_m.setRegister(Registers::Register::B, 0x34);
    cpu.registers_m.setRegister(Registers::Register::C, 0x56);
    
    cpu.RET();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x12);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x34);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x56);
}

// ============================================================================
// Tests de CALL + RET (Round Trip)
// ============================================================================

TEST_F(CALL_RET_Test, CALL_RET_RoundTrip) {
    initializeSP(0xF000);
    cpu.pc_m = 0x1000;
    
    memory[0x1000] = 0x00;  // Llamar a 0x2000
    memory[0x1001] = 0x20;
    
    cpu.CALL_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x2000);
    
    // Retornar
    cpu.RET();
    
    EXPECT_EQ(cpu.pc_m, 0x1002);  // Vuelve a la instrucción después del CALL
}

TEST_F(CALL_RET_Test, CALL_RET_NestedCalls) {
    initializeSP(0xF000);
    cpu.pc_m = 0x1000;
    
    // Primera llamada a 0x2000
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x20;
    cpu.CALL_a16();
    EXPECT_EQ(cpu.pc_m, 0x2000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFE);
    
    // Segunda llamada a 0x3000 desde 0x2000
    cpu.pc_m = 0x2000;
    memory[0x2000] = 0x00;
    memory[0x2001] = 0x30;
    cpu.CALL_a16();
    EXPECT_EQ(cpu.pc_m, 0x3000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFC);
    
    // Tercera llamada a 0x4000 desde 0x3000
    cpu.pc_m = 0x3000;
    memory[0x3000] = 0x00;
    memory[0x3001] = 0x40;
    cpu.CALL_a16();
    EXPECT_EQ(cpu.pc_m, 0x4000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFA);
    
    // Retornar de la tercera llamada
    cpu.RET();
    EXPECT_EQ(cpu.pc_m, 0x3002);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFC);
    
    // Retornar de la segunda llamada
    cpu.RET();
    EXPECT_EQ(cpu.pc_m, 0x2002);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFE);
    
    // Retornar de la primera llamada
    cpu.RET();
    EXPECT_EQ(cpu.pc_m, 0x1002);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
}

TEST_F(CALL_RET_Test, CALL_RET_MultipleRoundTrips) {
    initializeSP(0xF000);
    
    for (int i = 0; i < 5; ++i) {
        cpu.pc_m = 0x1000 + (i * 0x100);
        memory[cpu.pc_m] = 0x00;
        memory[cpu.pc_m + 1] = 0x50;
        
        cpu.CALL_a16();
        EXPECT_EQ(cpu.pc_m, 0x5000);
        
        cpu.RET();
        EXPECT_EQ(cpu.pc_m, 0x1002 + (i * 0x100));
    }
    
    // SP debe estar de vuelta en su posición original
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
}

// ============================================================================
// Tests de Casos Realistas
// ============================================================================

TEST_F(CALL_RET_Test, RealisticUseCase_SimpleSubroutine) {
    // Simular: main llama a función que suma dos números
    initializeSP(0xF000);
    cpu.pc_m = 0x0100;  // Programa principal
    
    // CALL suma_function en 0x0200
    memory[0x0100] = 0x00;
    memory[0x0101] = 0x02;
    cpu.CALL_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x0200);  // En la función
    
    // La función hace su trabajo y retorna
    cpu.RET();
    
    EXPECT_EQ(cpu.pc_m, 0x0102);  // De vuelta al main
}

TEST_F(CALL_RET_Test, RealisticUseCase_RecursiveFunction) {
    // Simular llamada recursiva simple (3 niveles)
    initializeSP(0xF000);
    cpu.pc_m = 0x2000;
    
    // Nivel 1
    memory[0x2000] = 0x00;
    memory[0x2001] = 0x20;  // Llamarse a sí misma
    cpu.CALL_a16();
    
    // Nivel 2
    cpu.pc_m = 0x2000;
    cpu.CALL_a16();
    
    // Nivel 3
    cpu.pc_m = 0x2000;
    cpu.CALL_a16();
    
    // Verificar depth del stack (3 llamadas = 6 bytes)
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xEFFA);
    
    // Unwinding: retornar 3 veces
    cpu.RET();
    cpu.RET();
    cpu.RET();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
}

TEST_F(CALL_RET_Test, RealisticUseCase_InterruptHandler) {
    // Simular interrupción: guardar contexto, llamar handler, restaurar
    initializeSP(0xF000);
    cpu.pc_m = 0x5000;  // Programa ejecutándose
    
    // Interrupción ocurre, salta a vector 0x0038
    memory[0x5000] = 0x38;
    memory[0x5001] = 0x00;
    cpu.CALL_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x0038);  // En el handler
    
    // Handler retorna
    cpu.RET();
    
    EXPECT_EQ(cpu.pc_m, 0x5002);  // Continúa ejecución normal
}

// ============================================================================
// Tests de Stack Overflow/Underflow
// ============================================================================

TEST_F(CALL_RET_Test, CALL_DecrementsSP) {
    initializeSP(0xF000);
    uint16_t initialSP = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    
    cpu.pc_m = 0x1000;
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x20;
    
    cpu.CALL_a16();
    
    uint16_t finalSP = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    EXPECT_EQ(finalSP, initialSP - 2);
}

TEST_F(CALL_RET_Test, RET_IncrementsSP) {
    initializeSP(0xEFFE);
    uint16_t initialSP = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    
    memory[0xEFFE] = 0x00;
    memory[0xEFFF] = 0x10;
    
    cpu.RET();
    
    uint16_t finalSP = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    EXPECT_EQ(finalSP, initialSP + 2);
}

TEST_F(CALL_RET_Test, DeepCallStack) {
    // Probar muchas llamadas anidadas
    initializeSP(0xF000);
    const int maxDepth = 100;
    
    for (int i = 0; i < maxDepth; ++i) {
        cpu.pc_m = 0x1000;
        memory[0x1000] = 0x00;
        memory[0x1001] = 0x20;
        cpu.CALL_a16();
    }
    
    // Verificar profundidad del stack
    uint16_t sp = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    EXPECT_EQ(sp, 0xF000 - (maxDepth * 2));
    
    // Retornar todas
    for (int i = 0; i < maxDepth; ++i) {
        cpu.RET();
    }
    
    // Stack debe estar de vuelta
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
}

// ============================================================================
// Tests de Patrones de Direcciones
// ============================================================================

TEST_F(CALL_RET_Test, CALL_PatternAlternatingBits) {
    initializeSP(0xF000);
    cpu.pc_m = 0x0000;
    
    memory[0x0000] = 0xAA;  // 10101010
    memory[0x0001] = 0x55;  // 01010101
    
    cpu.CALL_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x55AA);
}

TEST_F(CALL_RET_Test, CALL_PatternLittleEndian) {
    initializeSP(0xF000);
    cpu.pc_m = 0x0000;
    
    memory[0x0000] = 0x34;  // Low byte
    memory[0x0001] = 0x12;  // High byte
    
    cpu.CALL_a16();
    
    EXPECT_EQ(cpu.pc_m, 0x1234);
}

TEST_F(CALL_RET_Test, RET_PatternAlternatingBits) {
    initializeSP(0xEFFE);
    
    memory[0xEFFE] = 0xAA;
    memory[0xEFFF] = 0x55;
    
    cpu.RET();
    
    EXPECT_EQ(cpu.pc_m, 0x55AA);
}

// ============================================================================
// Tests de Ciclos
// ============================================================================

TEST_F(CALL_RET_Test, CALL_TakesCorrectCycles) {
    initializeSP(0xF000);
    cpu.pc_m = 0x1000;
    
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x20;
    
    uint8_t cycles = cpu.CALL_a16();
    
    EXPECT_EQ(cycles, 17);
}

TEST_F(CALL_RET_Test, RET_TakesCorrectCycles) {
    initializeSP(0xEFFE);
    memory[0xEFFE] = 0x00;
    memory[0xEFFF] = 0x30;
    
    uint8_t cycles = cpu.RET();
    
    EXPECT_EQ(cycles, 10);
}

TEST_F(CALL_RET_Test, CALL_RET_TotalCycles) {
    initializeSP(0xF000);
    cpu.pc_m = 0x1000;
    
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x20;
    
    uint8_t callCycles = cpu.CALL_a16();
    uint8_t retCycles = cpu.RET();
    
    EXPECT_EQ(callCycles + retCycles, 27);  // 17 + 10
}

// ============================================================================
// Tests de Edge Cases
// ============================================================================

TEST_F(CALL_RET_Test, CALL_WithPCAtMaxAddress) {
    initializeSP(0xF000);
    cpu.pc_m = 0xFFFE;
    
    memory[0xFFFE] = 0x00;
    memory[0xFFFF] = 0x10;
    
    cpu.CALL_a16();
    
    // Debe guardar 0x0000 en el stack (PC + 2 con overflow)
    EXPECT_EQ(memory[0xEFFE], 0x00);
    EXPECT_EQ(memory[0xEFFF], 0x00);
}

TEST_F(CALL_RET_Test, CALL_RET_SameAddress) {
    // Llamar y retornar a la misma dirección (loop infinito potencial)
    initializeSP(0xF000);
    cpu.pc_m = 0x2000;
    
    memory[0x2000] = 0x00;
    memory[0x2001] = 0x20;  // Llamar a 0x2000 (misma dirección)
    
    cpu.CALL_a16();
    EXPECT_EQ(cpu.pc_m, 0x2000);
    
    cpu.RET();
    EXPECT_EQ(cpu.pc_m, 0x2002);
}

TEST_F(CALL_RET_Test, CALL_PreservesStackContent) {
    initializeSP(0xF000);
    
    // Poner datos en el stack antes del CALL
    memory[0xEFFC] = 0xAA;
    memory[0xEFFD] = 0xBB;
    
    cpu.pc_m = 0x1000;
    memory[0x1000] = 0x00;
    memory[0x1001] = 0x30;
    
    cpu.CALL_a16();
    
    // Los datos anteriores deben seguir intactos
    EXPECT_EQ(memory[0xEFFC], 0xAA);
    EXPECT_EQ(memory[0xEFFD], 0xBB);
}

// ============================================================================
// Tests de Stress
// ============================================================================

TEST_F(CALL_RET_Test, StressTest_ManyCallsAndReturns) {
    initializeSP(0xF000);
    const int iterations = 50;
    
    for (int i = 0; i < iterations; ++i) {
        cpu.pc_m = 0x1000 + i;
        memory[cpu.pc_m] = 0x00;
        memory[cpu.pc_m + 1] = 0x50;
        
        uint16_t spBefore = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
        
        cpu.CALL_a16();
        EXPECT_EQ(cpu.pc_m, 0x5000);
        EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), spBefore - 2);
        
        cpu.RET();
        EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), spBefore);
    }
}

TEST_F(CALL_RET_Test, StressTest_DeepRecursion) {
    initializeSP(0xF000);
    const int depth = 200;
    
    // Llamadas profundas
    for (int i = 0; i < depth; ++i) {
        cpu.pc_m = 0x2000;
        memory[0x2000] = 0x00;
        memory[0x2001] = 0x20;
        cpu.CALL_a16();
    }
    
    uint16_t deepestSP = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    EXPECT_EQ(deepestSP, 0xF000 - (depth * 2));
    
    // Unwinding completo
    for (int i = 0; i < depth; ++i) {
        cpu.RET();
    }
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
}
