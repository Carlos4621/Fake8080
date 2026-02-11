#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"
#include <array>

class XTHL_Test : public ::testing::Test {
protected:
    CPUTest cpu;
    std::array<uint8_t, 65536> rom{};

    void SetUp() override {
        rom.fill(0);
        cpu.setROM(rom);
        
        // Inicializar SP en 0xF000 usando LXI SP
        rom[0] = 0x00;  // Byte bajo de dirección para SP
        rom[1] = 0xF0;  // Byte alto de dirección para SP
        cpu.setROM(rom);
        cpu.LXI_RR_d16<Registers::CombinedRegister::SP>();
    }
};

// ==================== Tests básicos de XTHL ====================

TEST_F(XTHL_Test, XTHL_BasicExchange) {
    // Preparar HL con valores conocidos
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1234);
    
    // Preparar valores en el stack (SP apunta a 0xF000)
    rom[0xF000] = 0xAB;  // Byte bajo en stack
    rom[0xF001] = 0xCD;  // Byte alto en stack
    cpu.setROM(rom);
    
    uint8_t cycles = cpu.XTHL();
    
    // Verificar que HL ahora tiene los valores del stack
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xCDAB);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0xCD);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0xAB);
    
    // Verificar que el stack ahora tiene los valores originales de HL
    EXPECT_EQ(cpu.rom_m[0xF000], 0x34);  // L original
    EXPECT_EQ(cpu.rom_m[0xF001], 0x12);  // H original
    
    // Verificar SP no cambió
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
    
    // Verificar ciclos
    EXPECT_EQ(cycles, 18);
}

TEST_F(XTHL_Test, XTHL_ZeroValues) {
    // HL con cero
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0000);
    
    // Stack con cero
    rom[0xF000] = 0x00;
    rom[0xF001] = 0x00;
    cpu.setROM(rom);
    
    uint8_t cycles = cpu.XTHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0000);
    EXPECT_EQ(cpu.rom_m[0xF000], 0x00);
    EXPECT_EQ(cpu.rom_m[0xF001], 0x00);
    EXPECT_EQ(cycles, 18);
}

TEST_F(XTHL_Test, XTHL_MaxValues) {
    // HL con valor máximo
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xFFFF);
    
    // Stack con valor máximo
    rom[0xF000] = 0xFF;
    rom[0xF001] = 0xFF;
    cpu.setROM(rom);
    
    uint8_t cycles = cpu.XTHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xFFFF);
    EXPECT_EQ(cpu.rom_m[0xF000], 0xFF);
    EXPECT_EQ(cpu.rom_m[0xF001], 0xFF);
    EXPECT_EQ(cycles, 18);
}

TEST_F(XTHL_Test, XTHL_DifferentValues) {
    // HL = 0xABCD, Stack = 0x1234
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xABCD);
    
    rom[0xF000] = 0x34;
    rom[0xF001] = 0x12;
    cpu.setROM(rom);
    
    uint8_t cycles = cpu.XTHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1234);
    EXPECT_EQ(cpu.rom_m[0xF000], 0xCD);  // L original
    EXPECT_EQ(cpu.rom_m[0xF001], 0xAB);  // H original
    EXPECT_EQ(cycles, 18);
}

// ==================== Tests de intercambio de bytes ====================

TEST_F(XTHL_Test, XTHL_ByteOrderVerification) {
    // Verificar que L se intercambia con (SP) y H con (SP+1)
    cpu.registers_m.setRegister(Registers::Register::H, 0xAA);
    cpu.registers_m.setRegister(Registers::Register::L, 0xBB);
    
    rom[0xF000] = 0xCC;
    rom[0xF001] = 0xDD;
    cpu.setROM(rom);
    
    cpu.XTHL();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0xCC);  // L <- (SP)
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0xDD);  // H <- (SP+1)
    EXPECT_EQ(cpu.rom_m[0xF000], 0xBB);  // (SP) <- L original
    EXPECT_EQ(cpu.rom_m[0xF001], 0xAA);  // (SP+1) <- H original
}

TEST_F(XTHL_Test, XTHL_LittleEndianCorrect) {
    // Verificar little endian: HL = 0x1234 significa H=0x12, L=0x34
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1234);
    
    rom[0xF000] = 0x78;
    rom[0xF001] = 0x56;
    cpu.setROM(rom);
    
    cpu.XTHL();
    
    // HL debería ser ahora 0x5678
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x5678);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x56);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x78);
}

// ==================== Tests de preservación ====================

TEST_F(XTHL_Test, XTHL_PreservesAllFlags) {
    // Establecer todos los flags
    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1234);
    rom[0xF000] = 0xAB;
    rom[0xF001] = 0xCD;
    cpu.setROM(rom);
    
    cpu.XTHL();
    
    // Verificar que los flags no cambiaron
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(XTHL_Test, XTHL_PreservesSP) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1234);
    rom[0xF000] = 0xAB;
    rom[0xF001] = 0xCD;
    cpu.setROM(rom);
    
    uint16_t sp_before = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP);
    
    cpu.XTHL();
    
    // SP debe permanecer igual
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), sp_before);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xF000);
}

TEST_F(XTHL_Test, XTHL_PreservesOtherRegisters) {
    // Establecer otros registros
    cpu.registers_m.setRegister(Registers::Register::A, 0x11);
    cpu.registers_m.setRegister(Registers::Register::B, 0x22);
    cpu.registers_m.setRegister(Registers::Register::C, 0x33);
    cpu.registers_m.setRegister(Registers::Register::D, 0x44);
    cpu.registers_m.setRegister(Registers::Register::E, 0x55);
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1234);
    rom[0xF000] = 0xAB;
    rom[0xF001] = 0xCD;
    cpu.setROM(rom);
    
    cpu.XTHL();
    
    // Verificar que otros registros no cambiaron
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x11);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x22);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x33);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::D), 0x44);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::E), 0x55);
}

// ==================== Tests de casos especiales ====================

TEST_F(XTHL_Test, XTHL_DoubleExchange_RoundTrip) {
    // Doble XTHL debe restaurar valores originales
    uint16_t original_hl = 0x1234;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, original_hl);
    
    rom[0xF000] = 0xAB;
    rom[0xF001] = 0xCD;
    cpu.setROM(rom);
    
    uint8_t original_stack_low = rom[0xF000];
    uint8_t original_stack_high = rom[0xF001];
    
    // Primer intercambio
    cpu.XTHL();
    
    // Segundo intercambio
    cpu.XTHL();
    
    // Valores deben volver a sus posiciones originales
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), original_hl);
    EXPECT_EQ(cpu.rom_m[0xF000], original_stack_low);
    EXPECT_EQ(cpu.rom_m[0xF001], original_stack_high);
}

TEST_F(XTHL_Test, XTHL_AtLowMemory) {
    // SP en dirección baja
    rom[0] = 0x00;
    rom[1] = 0x01;  // SP = 0x0100
    cpu.setROM(rom);
    cpu.LXI_RR_d16<Registers::CombinedRegister::SP>();
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xABCD);
    rom[0x0100] = 0x12;
    rom[0x0101] = 0x34;
    cpu.setROM(rom);
    
    cpu.XTHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x3412);
    EXPECT_EQ(cpu.rom_m[0x0100], 0xCD);
    EXPECT_EQ(cpu.rom_m[0x0101], 0xAB);
}

TEST_F(XTHL_Test, XTHL_AtHighMemory) {
    // SP en dirección alta
    rom[0] = 0xFE;
    rom[1] = 0xFF;  // SP = 0xFFFE
    cpu.setROM(rom);
    cpu.LXI_RR_d16<Registers::CombinedRegister::SP>();
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1234);
    rom[0xFFFE] = 0xAB;
    rom[0xFFFF] = 0xCD;
    cpu.setROM(rom);
    
    cpu.XTHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xCDAB);
    EXPECT_EQ(cpu.rom_m[0xFFFE], 0x34);
    EXPECT_EQ(cpu.rom_m[0xFFFF], 0x12);
}

// ==================== Tests de operaciones múltiples ====================

TEST_F(XTHL_Test, XTHL_MultipleSequential) {
    // Realizar múltiples XTHL secuenciales en diferentes posiciones de stack
    
    // Primera operación en 0xF000
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1111);
    rom[0xF000] = 0xAA;
    rom[0xF001] = 0xBB;
    cpu.setROM(rom);
    cpu.XTHL();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xBBAA);
    
    // Cambiar SP y hacer otra operación
    rom[0] = 0x00;
    rom[1] = 0xE0;  // SP = 0xE000
    cpu.setROM(rom);
    cpu.LXI_RR_d16<Registers::CombinedRegister::SP>();
    
    rom[0xE000] = 0xCC;
    rom[0xE001] = 0xDD;
    cpu.setROM(rom);
    cpu.XTHL();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xDDCC);
}

TEST_F(XTHL_Test, XTHL_AfterPUSH) {
    // PUSH seguido de XTHL
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x1234);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x5678);
    
    // PUSH BC (SP quedará en 0xEFFE)
    cpu.PUSH_RR<Registers::CombinedRegister::BC>();
    
    // Ahora XTHL intercambiará HL con el valor recién pusheado
    cpu.XTHL();
    
    // HL debería tener ahora el valor de BC
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1234);
    
    // El stack debería tener el valor original de HL
    EXPECT_EQ(cpu.rom_m[0xEFFE], 0x78);  // L original
    EXPECT_EQ(cpu.rom_m[0xEFFF], 0x56);  // H original
}

TEST_F(XTHL_Test, XTHL_BeforePOP) {
    // XTHL seguido de POP para recuperar el valor intercambiado
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xABCD);
    
    rom[0xF000] = 0x12;
    rom[0xF001] = 0x34;
    cpu.setROM(rom);
    
    // XTHL pone 0xABCD en el stack y carga 0x3412 en HL
    cpu.XTHL();
    
    // POP BC debería recuperar el valor original de HL
    cpu.POP_RR<Registers::CombinedRegister::BC>();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0xABCD);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x3412);
}

// ==================== Tests de casos de uso realistas ====================

TEST_F(XTHL_Test, RealisticUseCase_TemporarySwap) {
    // Simular guardar temporalmente HL y cargar un valor del stack
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x2000);  // Puntero actual
    
    // Stack contiene otro puntero
    rom[0xF000] = 0x00;
    rom[0xF001] = 0x30;  // 0x3000
    cpu.setROM(rom);
    
    // Intercambiar
    cpu.XTHL();
    
    // Ahora HL apunta a 0x3000
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x3000);
    
    // Y 0x2000 está guardado en el stack
    EXPECT_EQ(cpu.rom_m[0xF000], 0x00);
    EXPECT_EQ(cpu.rom_m[0xF001], 0x20);
}

TEST_F(XTHL_Test, RealisticUseCase_StackManipulation) {
    // Modificar el top of stack sin hacer POP/PUSH
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xFFFF);
    
    rom[0xF000] = 0x11;
    rom[0xF001] = 0x22;
    cpu.setROM(rom);
    
    // XTHL para cargar valor del stack
    cpu.XTHL();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x2211);
    
    // Modificar HL
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x3344);
    
    // XTHL de nuevo para escribir de vuelta
    cpu.XTHL();
    
    // El stack debe tener el nuevo valor
    EXPECT_EQ(cpu.rom_m[0xF000], 0x44);
    EXPECT_EQ(cpu.rom_m[0xF001], 0x33);
}

TEST_F(XTHL_Test, RealisticUseCase_SaveReturnAddress) {
    // Simular guardar una dirección de retorno y cargar otra
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0150);  // Nueva dirección
    
    // Stack contiene dirección de retorno actual
    rom[0xF000] = 0x00;
    rom[0xF001] = 0x01;  // 0x0100
    cpu.setROM(rom);
    
    cpu.XTHL();
    
    // HL ahora tiene la dirección de retorno original
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0100);
    
    // Stack tiene la nueva dirección
    EXPECT_EQ(cpu.rom_m[0xF000], 0x50);
    EXPECT_EQ(cpu.rom_m[0xF001], 0x01);
}

// ==================== Tests de patrones de bits ====================

TEST_F(XTHL_Test, PatternTest_AlternatingBits) {
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0xAAAA);
    
    rom[0xF000] = 0x55;
    rom[0xF001] = 0x55;
    cpu.setROM(rom);
    
    cpu.XTHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x5555);
    EXPECT_EQ(cpu.rom_m[0xF000], 0xAA);
    EXPECT_EQ(cpu.rom_m[0xF001], 0xAA);
}

TEST_F(XTHL_Test, PatternTest_SingleBitSet) {
    // Probar con un solo bit establecido en diferentes posiciones
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x0001);
    
    rom[0xF000] = 0x80;
    rom[0xF001] = 0x00;
    cpu.setROM(rom);
    
    cpu.XTHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x0080);
    EXPECT_EQ(cpu.rom_m[0xF000], 0x01);
    EXPECT_EQ(cpu.rom_m[0xF001], 0x00);
}

// ==================== Tests de condiciones de frontera ====================

TEST_F(XTHL_Test, BoundaryCondition_SPAtZero) {
    // SP en dirección 0
    rom[0] = 0x00;
    rom[1] = 0x00;
    cpu.setROM(rom);
    cpu.LXI_RR_d16<Registers::CombinedRegister::SP>();
    
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1234);
    rom[0x0000] = 0xAB;
    rom[0x0001] = 0xCD;
    cpu.setROM(rom);
    
    cpu.XTHL();
    
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0xCDAB);
}

TEST_F(XTHL_Test, BoundaryCondition_ByteValues) {
    // Probar con valores de byte individuales significativos
    cpu.registers_m.setRegister(Registers::Register::H, 0x7F);  // Max signed positive
    cpu.registers_m.setRegister(Registers::Register::L, 0x80);  // Min signed negative
    
    rom[0xF000] = 0xFF;
    rom[0xF001] = 0x00;
    cpu.setROM(rom);
    
    cpu.XTHL();
    
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0xFF);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x00);
    EXPECT_EQ(cpu.rom_m[0xF000], 0x80);
    EXPECT_EQ(cpu.rom_m[0xF001], 0x7F);
}

TEST_F(XTHL_Test, EdgeCase_ImmediateReuse) {
    // Usar XTHL inmediatamente después de modificar HL
    cpu.registers_m.setRegister(Registers::Register::H, 0x12);
    cpu.registers_m.setRegister(Registers::Register::L, 0x34);
    
    rom[0xF000] = 0x56;
    rom[0xF001] = 0x78;
    cpu.setROM(rom);
    
    cpu.XTHL();
    
    // Verificar intercambio inmediato
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::H), 0x78);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::L), 0x56);
}

TEST_F(XTHL_Test, EdgeCase_ThreeWayRotation) {
    // Simular rotación de tres valores usando XTHL y registros
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, 0x1111);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x2222);
    
    rom[0xF000] = 0x33;
    rom[0xF001] = 0x33;
    cpu.setROM(rom);
    
    // HL <-> Stack: HL=0x3333, Stack=0x1111
    cpu.XTHL();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x3333);
    
    // Copiar DE a HL manualmente (simulación)
    uint16_t temp = cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::HL, temp);
    
    // HL <-> Stack: HL=0x1111, Stack=0x2222
    cpu.XTHL();
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::HL), 0x1111);
    EXPECT_EQ(cpu.rom_m[0xF000], 0x22);
    EXPECT_EQ(cpu.rom_m[0xF001], 0x22);
}
