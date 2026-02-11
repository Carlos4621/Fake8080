#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"
#include <array>

class XCHG_Test : public ::testing::Test {
protected:
    CPUTest cpu;
    std::array<uint8_t, 65536> rom{};

    void SetUp() override {
        rom.fill(0);
        cpu.setROM(rom);
    }
};

// ==================== Tests básicos de XCHG ====================

TEST_F(XCHG_Test, XCHG_BasicExchange) {
    // Establecer valores en DE y HL
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x1234);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x5678);
    
    uint8_t cycles = cpu.XCHG();
    
    // Verificar el intercambio
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x5678);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1234);
    
    // Verificar registros individuales
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x56);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x78);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x12);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x34);
    
    // Verificar ciclos
    EXPECT_EQ(cycles, 5);
}

TEST_F(XCHG_Test, XCHG_ZeroValues) {
    // Ambos registros con cero
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x0000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0000);
    
    uint8_t cycles = cpu.XCHG();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x0000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0000);
    EXPECT_EQ(cycles, 5);
}

TEST_F(XCHG_Test, XCHG_MaxValues) {
    // Ambos registros con valor máximo
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0xFFFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xFFFF);
    
    uint8_t cycles = cpu.XCHG();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0xFFFF);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xFFFF);
    EXPECT_EQ(cycles, 5);
}

TEST_F(XCHG_Test, XCHG_OneZeroOneMax) {
    // DE = 0, HL = max
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x0000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xFFFF);
    
    cpu.XCHG();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0xFFFF);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0000);
}

TEST_F(XCHG_Test, XCHG_DifferentValues) {
    // Valores completamente diferentes
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0xABCD);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1234);
    
    cpu.XCHG();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x1234);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xABCD);
}

// ==================== Tests de bytes individuales ====================

TEST_F(XCHG_Test, XCHG_IndividualByteVerification) {
    // Verificar que D↔H y E↔L
    cpu.registers_m.setRegister(Registers::Register::D, 0xAA);
    cpu.registers_m.setRegister(Registers::Register::E, 0xBB);
    cpu.registers_m.setRegister(Registers::Register::H, 0xCC);
    cpu.registers_m.setRegister(Registers::Register::L, 0xDD);
    
    cpu.XCHG();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0xCC);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0xDD);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0xAA);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0xBB);
}

TEST_F(XCHG_Test, XCHG_HighByteExchange) {
    // Solo bytes altos diferentes
    cpu.registers_m.setRegister(Registers::Register::D, 0x12);
    cpu.registers_m.setRegister(Registers::Register::E, 0x00);
    cpu.registers_m.setRegister(Registers::Register::H, 0x34);
    cpu.registers_m.setRegister(Registers::Register::L, 0x00);
    
    cpu.XCHG();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x34);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x12);
}

TEST_F(XCHG_Test, XCHG_LowByteExchange) {
    // Solo bytes bajos diferentes
    cpu.registers_m.setRegister(Registers::Register::D, 0x00);
    cpu.registers_m.setRegister(Registers::Register::E, 0x12);
    cpu.registers_m.setRegister(Registers::Register::H, 0x00);
    cpu.registers_m.setRegister(Registers::Register::L, 0x34);
    
    cpu.XCHG();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x34);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x12);
}

// ==================== Tests de preservación ====================

TEST_F(XCHG_Test, XCHG_PreservesAllFlags) {
    // Establecer todos los flags
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x1234);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x5678);
    
    cpu.XCHG();
    
    // Verificar que los flags no cambiaron
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(XCHG_Test, XCHG_PreservesFlagsClear) {
    // Todos los flags en false
    cpu.registers_m.setFlag(Registers::Flags::S, false);
    cpu.registers_m.setFlag(Registers::Flags::Z, false);
    cpu.registers_m.setFlag(Registers::Flags::AC, false);
    cpu.registers_m.setFlag(Registers::Flags::P, false);
    cpu.registers_m.setFlag(Registers::Flags::CY, false);
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0xABCD);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1234);
    
    cpu.XCHG();
    
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::AC));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_FALSE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(XCHG_Test, XCHG_PreservesOtherRegisters) {
    // Establecer otros registros
    cpu.registers_m.setRegister(Registers::Register::A, 0x11);
    cpu.registers_m.setRegister(Registers::Register::B, 0x22);
    cpu.registers_m.setRegister(Registers::Register::C, 0x33);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xF000);
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x1234);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x5678);
    
    cpu.XCHG();
    
    // Verificar que otros registros no cambiaron
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x11);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x22);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x33);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
}

TEST_F(XCHG_Test, XCHG_PreservesBC) {
    // BC no debe cambiar
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0xAAAA);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x1234);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x5678);
    
    cpu.XCHG();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0xAAAA);
}

// ==================== Tests de operaciones múltiples ====================

TEST_F(XCHG_Test, XCHG_DoubleExchange_RoundTrip) {
    // Doble XCHG debe restaurar valores originales
    uint16_t original_de = 0x1234;
    uint16_t original_hl = 0x5678;
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, original_de);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, original_hl);
    
    // Primer intercambio
    cpu.XCHG();
    
    // Segundo intercambio
    cpu.XCHG();
    
    // Valores deben volver a sus posiciones originales
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), original_de);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), original_hl);
}

TEST_F(XCHG_Test, XCHG_MultipleSequential) {
    // Múltiples XCHG secuenciales
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x1111);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2222);
    
    // Primera
    cpu.XCHG();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x2222);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1111);
    
    // Segunda
    cpu.XCHG();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x1111);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2222);
    
    // Tercera
    cpu.XCHG();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x2222);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1111);
}

TEST_F(XCHG_Test, XCHG_ThreeWayRotation) {
    // Simular rotación de tres valores usando XCHG y BC
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x1111);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x2222);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3333);
    
    // Paso 1: XCHG - DE=0x3333, HL=0x2222
    cpu.XCHG();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x3333);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2222);
    
    // BC sigue siendo 0x1111
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x1111);
}

// ==================== Tests de casos de uso realistas ====================

TEST_F(XCHG_Test, RealisticUseCase_PointerSwap) {
    // Intercambiar dos punteros
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x2000);  // Puntero a datos
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3000);  // Puntero a buffer
    
    cpu.XCHG();
    
    // Ahora DE apunta al buffer y HL a los datos
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x3000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2000);
}

TEST_F(XCHG_Test, RealisticUseCase_SaveHLAndUseDE) {
    // Guardar HL en DE para operaciones temporales
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1234);  // Valor importante
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x0000);  // Vacío
    
    cpu.XCHG();
    
    // Ahora DE tiene el valor importante y HL está libre
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x1234);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0000);
    
    // Hacer operaciones con HL...
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x5678);
    
    // Restaurar con otro XCHG
    cpu.XCHG();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1234);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x5678);
}

TEST_F(XCHG_Test, RealisticUseCase_AlternatePointers) {
    // Alternar entre dos punteros para procesamiento
    uint16_t source = 0x1000;
    uint16_t dest = 0x2000;
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, source);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, dest);
    
    // Primera operación usa HL como source
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), source);
    
    // Intercambiar para usar DE como source
    cpu.XCHG();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), dest);
    
    // Intercambiar de vuelta
    cpu.XCHG();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), source);
}

// ==================== Tests de patrones de bits ====================

TEST_F(XCHG_Test, PatternTest_AlternatingBits) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0xAAAA);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x5555);
    
    cpu.XCHG();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x5555);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xAAAA);
}

TEST_F(XCHG_Test, PatternTest_ComplementaryValues) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x00FF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xFF00);
    
    cpu.XCHG();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0xFF00);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x00FF);
}

TEST_F(XCHG_Test, PatternTest_SingleBitSet) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x0001);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x8000);
    
    cpu.XCHG();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x8000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0001);
}

// ==================== Tests de condiciones límite ====================

TEST_F(XCHG_Test, BoundaryCondition_SignedBoundaries) {
    // Max signed positive (0x7FFF) y min signed negative (0x8000)
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x7FFF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x8000);
    
    cpu.XCHG();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x8000);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x7FFF);
}

TEST_F(XCHG_Test, BoundaryCondition_ByteBoundaries) {
    // Valores en límites de bytes: 0x00FF, 0xFF00
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x00FF);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xFF00);
    
    cpu.XCHG();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0xFF00);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x00FF);
}

TEST_F(XCHG_Test, BoundaryCondition_AdjacentValues) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x1000);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1001);
    
    cpu.XCHG();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x1001);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1000);
}

// ==================== Tests de casos especiales ====================

TEST_F(XCHG_Test, EdgeCase_SameValue) {
    // Ambos con el mismo valor
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0xABCD);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xABCD);
    
    cpu.XCHG();
    
    // Después del intercambio siguen siendo iguales
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0xABCD);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xABCD);
}

TEST_F(XCHG_Test, EdgeCase_ImmediateReuse) {
    // Usar XCHG inmediatamente después de establecer valores
    cpu.registers_m.setRegister(Registers::Register::D, 0x12);
    cpu.registers_m.setRegister(Registers::Register::E, 0x34);
    cpu.registers_m.setRegister(Registers::Register::H, 0x56);
    cpu.registers_m.setRegister(Registers::Register::L, 0x78);
    
    uint8_t cycles = cpu.XCHG();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x5678);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1234);
    EXPECT_EQ(cycles, 5);
}

TEST_F(XCHG_Test, EdgeCase_AfterArithmetic) {
    // Usar XCHG después de operaciones aritméticas (simulado)
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x0010);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0020);
    
    // Simular suma (HL = HL + DE = 0x0030)
    uint16_t sum = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL) + 
                   cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, sum);
    
    // XCHG para guardar resultado
    cpu.XCHG();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x0030);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0010);
}

TEST_F(XCHG_Test, StressTest_ManyExchanges) {
    // Realizar muchos intercambios consecutivos
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x1234);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x5678);
    
    // 100 intercambios (par = vuelta al original)
    for (int i = 0; i < 100; i++) {
        cpu.XCHG();
    }
    
    // Después de número par de intercambios, vuelven al original
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x1234);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x5678);
}

TEST_F(XCHG_Test, StressTest_OddExchanges) {
    // Realizar número impar de intercambios
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x1234);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x5678);
    
    // 101 intercambios (impar = valores intercambiados)
    for (int i = 0; i < 101; i++) {
        cpu.XCHG();
    }
    
    // Después de número impar de intercambios, están intercambiados
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x5678);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1234);
}
