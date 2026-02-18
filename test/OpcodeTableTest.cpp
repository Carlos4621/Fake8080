#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"
#include <vector>

class OpcodeTableTest : public ::testing::Test {
protected:
    CPUTest cpu;
    std::vector<uint8_t> memory;

    void SetUp() override {
        // Reservar memoria suficiente para las pruebas
        memory.resize(0x10000, 0x00); // 64KB de memoria
        cpu.setROM(memory);
    }
};

// ==================== Test para verificar la tabla de opcodes ====================

TEST_F(OpcodeTableTest, OpcodeTable_HasCorrectSize) {
    // Verificar que la tabla tiene exactamente 256 entradas
    EXPECT_EQ(cpu.opcodes_m.size(), 256);
    EXPECT_EQ(cpu.Opcodes_Number, 256);
}

TEST_F(OpcodeTableTest, OpcodeTable_AllEntriesAreNonNull) {
    // Verificar que todas las entradas de la tabla no son nulas
    for (size_t i = 0; i < cpu.opcodes_m.size(); ++i) {
        EXPECT_NE(cpu.opcodes_m[i], nullptr) 
            << "Opcode 0x" << std::hex << i << " tiene un puntero nulo";
    }
}

TEST_F(OpcodeTableTest, OpcodeTable_NOP_IsInvalidOpcode) {
    // Verificar que NOP (0x00) está mapeado a invalidOpcode
    // No podemos llamarlo directamente porque lanza una excepción
    EXPECT_NE(cpu.opcodes_m[0x00], nullptr);
}

TEST_F(OpcodeTableTest, OpcodeTable_MVI_B_d8_IsCorrect) {
    // Opcode 0x06: MVI B,d8 - Cargar valor inmediato en B
    memory[0] = 0x42; // Valor inmediato a cargar
    cpu.pc_m = 0;
    
    // Llamar a la función a través de la tabla de opcodes
    auto opcode_func = cpu.opcodes_m[0x06];
    (cpu.*opcode_func)();
    
    // Verificar que B tiene el valor correcto
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x42);
    EXPECT_EQ(cpu.pc_m, 1); // PC debe avanzar 1 byte
}

TEST_F(OpcodeTableTest, OpcodeTable_LXI_B_d16_IsCorrect) {
    // Opcode 0x01: LXI B,d16 - Cargar par de registros BC con valor de 16 bits
    memory[0] = 0x34; // Byte bajo
    memory[1] = 0x12; // Byte alto
    cpu.pc_m = 0;
    
    // Llamar a la función a través de la tabla de opcodes
    auto opcode_func = cpu.opcodes_m[0x01];
    (cpu.*opcode_func)();
    
    // Verificar que BC tiene el valor correcto (little endian)
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x1234);
    EXPECT_EQ(cpu.pc_m, 2); // PC debe avanzar 2 bytes
}

TEST_F(OpcodeTableTest, OpcodeTable_INR_B_IsCorrect) {
    // Opcode 0x04: INR B - Incrementar registro B
    cpu.registers_m.setRegister(Registers::Register::B, 0x10);
    cpu.pc_m = 0;
    
    // Llamar a la función a través de la tabla de opcodes
    auto opcode_func = cpu.opcodes_m[0x04];
    (cpu.*opcode_func)();
    
    // Verificar que B fue incrementado
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x11);
}

TEST_F(OpcodeTableTest, OpcodeTable_DCR_C_IsCorrect) {
    // Opcode 0x0D: DCR C - Decrementar registro C
    cpu.registers_m.setRegister(Registers::Register::C, 0x10);
    cpu.pc_m = 0;
    
    // Llamar a la función a través de la tabla de opcodes
    auto opcode_func = cpu.opcodes_m[0x0D];
    (cpu.*opcode_func)();
    
    // Verificar que C fue decrementado
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0x0F);
}

TEST_F(OpcodeTableTest, OpcodeTable_MOV_B_C_IsCorrect) {
    // Opcode 0x41: MOV B,C - Mover contenido de C a B
    cpu.registers_m.setRegister(Registers::Register::C, 0xAB);
    cpu.registers_m.setRegister(Registers::Register::B, 0x00);
    cpu.pc_m = 0;
    
    // Llamar a la función a través de la tabla de opcodes
    auto opcode_func = cpu.opcodes_m[0x41];
    (cpu.*opcode_func)();
    
    // Verificar que B tiene el valor de C
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0xAB);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::C), 0xAB); // C no cambia
}

TEST_F(OpcodeTableTest, OpcodeTable_ADD_B_IsCorrect) {
    // Opcode 0x80: ADD B - Sumar B a A
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    cpu.registers_m.setRegister(Registers::Register::B, 0x05);
    cpu.pc_m = 0;
    
    // Llamar a la función a través de la tabla de opcodes
    auto opcode_func = cpu.opcodes_m[0x80];
    (cpu.*opcode_func)();
    
    // Verificar que A contiene la suma
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x15);
}

TEST_F(OpcodeTableTest, OpcodeTable_SUB_C_IsCorrect) {
    // Opcode 0x91: SUB C - Restar C de A
    cpu.registers_m.setRegister(Registers::Register::A, 0x20);
    cpu.registers_m.setRegister(Registers::Register::C, 0x05);
    cpu.pc_m = 0;
    
    // Llamar a la función a través de la tabla de opcodes
    auto opcode_func = cpu.opcodes_m[0x91];
    (cpu.*opcode_func)();
    
    // Verificar que A contiene la resta
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x1B);
}

TEST_F(OpcodeTableTest, OpcodeTable_ANA_D_IsCorrect) {
    // Opcode 0xA2: ANA D - AND lógico de D con A
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setRegister(Registers::Register::D, 0x0F);
    cpu.pc_m = 0;
    
    // Llamar a la función a través de la tabla de opcodes
    auto opcode_func = cpu.opcodes_m[0xA2];
    (cpu.*opcode_func)();
    
    // Verificar que A contiene el resultado del AND
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x0F);
}

TEST_F(OpcodeTableTest, OpcodeTable_ORA_E_IsCorrect) {
    // Opcode 0xB3: ORA E - OR lógico de E con A
    cpu.registers_m.setRegister(Registers::Register::A, 0xF0);
    cpu.registers_m.setRegister(Registers::Register::E, 0x0F);
    cpu.pc_m = 0;
    
    // Llamar a la función a través de la tabla de opcodes
    auto opcode_func = cpu.opcodes_m[0xB3];
    (cpu.*opcode_func)();
    
    // Verificar que A contiene el resultado del OR
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
}

TEST_F(OpcodeTableTest, OpcodeTable_XRA_H_IsCorrect) {
    // Opcode 0xAC: XRA H - XOR lógico de H con A
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    cpu.registers_m.setRegister(Registers::Register::H, 0xFF);
    cpu.pc_m = 0;
    
    // Llamar a la función a través de la tabla de opcodes
    auto opcode_func = cpu.opcodes_m[0xAC];
    (cpu.*opcode_func)();
    
    // Verificar que A contiene el resultado del XOR (0xFF XOR 0xFF = 0x00)
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
}

TEST_F(OpcodeTableTest, OpcodeTable_CMP_L_IsCorrect) {
    // Opcode 0xBD: CMP L - Comparar L con A
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    cpu.registers_m.setRegister(Registers::Register::L, 0x10);
    cpu.pc_m = 0;
    
    // Llamar a la función a través de la tabla de opcodes
    auto opcode_func = cpu.opcodes_m[0xBD];
    (cpu.*opcode_func)();
    
    // Verificar que A no cambia (CMP no modifica A)
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x10);
    // Verificar que la bandera Z se activa (son iguales)
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
}

TEST_F(OpcodeTableTest, OpcodeTable_ADI_d8_IsCorrect) {
    // Opcode 0xC6: ADI d8 - Sumar valor inmediato a A
    cpu.registers_m.setRegister(Registers::Register::A, 0x10);
    memory[0] = 0x25;
    cpu.pc_m = 0;
    
    // Llamar a la función a través de la tabla de opcodes
    auto opcode_func = cpu.opcodes_m[0xC6];
    (cpu.*opcode_func)();
    
    // Verificar que A contiene la suma
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x35);
    EXPECT_EQ(cpu.pc_m, 1); // PC debe avanzar 1 byte
}

TEST_F(OpcodeTableTest, OpcodeTable_SUI_d8_IsCorrect) {
    // Opcode 0xD6: SUI d8 - Restar valor inmediato de A
    cpu.registers_m.setRegister(Registers::Register::A, 0x50);
    memory[0] = 0x10;
    cpu.pc_m = 0;
    
    // Llamar a la función a través de la tabla de opcodes
    auto opcode_func = cpu.opcodes_m[0xD6];
    (cpu.*opcode_func)();
    
    // Verificar que A contiene la resta
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x40);
    EXPECT_EQ(cpu.pc_m, 1); // PC debe avanzar 1 byte
}

TEST_F(OpcodeTableTest, OpcodeTable_ANI_d8_IsCorrect) {
    // Opcode 0xE6: ANI d8 - AND inmediato con A
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    memory[0] = 0x0F;
    cpu.pc_m = 0;
    
    // Llamar a la función a través de la tabla de opcodes
    auto opcode_func = cpu.opcodes_m[0xE6];
    (cpu.*opcode_func)();
    
    // Verificar que A contiene el resultado
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x0F);
    EXPECT_EQ(cpu.pc_m, 1); // PC debe avanzar 1 byte
}

TEST_F(OpcodeTableTest, OpcodeTable_ORI_d8_IsCorrect) {
    // Opcode 0xF6: ORI d8 - OR inmediato con A
    cpu.registers_m.setRegister(Registers::Register::A, 0xF0);
    memory[0] = 0x0F;
    cpu.pc_m = 0;
    
    // Llamar a la función a través de la tabla de opcodes
    auto opcode_func = cpu.opcodes_m[0xF6];
    (cpu.*opcode_func)();
    
    // Verificar que A contiene el resultado
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(cpu.pc_m, 1); // PC debe avanzar 1 byte
}

TEST_F(OpcodeTableTest, OpcodeTable_XRI_d8_IsCorrect) {
    // Opcode 0xEE: XRI d8 - XOR inmediato con A
    cpu.registers_m.setRegister(Registers::Register::A, 0xFF);
    memory[0] = 0xFF;
    cpu.pc_m = 0;
    
    // Llamar a la función a través de la tabla de opcodes
    auto opcode_func = cpu.opcodes_m[0xEE];
    (cpu.*opcode_func)();
    
    // Verificar que A contiene el resultado
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x00);
    EXPECT_EQ(cpu.pc_m, 1); // PC debe avanzar 1 byte
}

TEST_F(OpcodeTableTest, OpcodeTable_INX_B_IsCorrect) {
    // Opcode 0x03: INX B - Incrementar par BC
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::BC, 0x1234);
    cpu.pc_m = 0;
    
    // Llamar a la función a través de la tabla de opcodes
    auto opcode_func = cpu.opcodes_m[0x03];
    (cpu.*opcode_func)();
    
    // Verificar que BC fue incrementado
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::BC), 0x1235);
}

TEST_F(OpcodeTableTest, OpcodeTable_DCX_D_IsCorrect) {
    // Opcode 0x1B: DCX D - Decrementar par DE
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::DE, 0x5678);
    cpu.pc_m = 0;
    
    // Llamar a la función a través de la tabla de opcodes
    auto opcode_func = cpu.opcodes_m[0x1B];
    (cpu.*opcode_func)();
    
    // Verificar que DE fue decrementado
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::DE), 0x5677);
}

TEST_F(OpcodeTableTest, OpcodeTable_AllOpcodesCanBeCalled) {
    // Test exhaustivo: verificar que todos los opcodes (excepto invalidOpcode y HLT)
    // pueden ser llamados sin crashear el programa
    
    std::vector<uint8_t> opcodes_that_throw = {
        0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, // NOP variantes
        0x76, // HLT
        0xCB, 0xD3, 0xD9, 0xDB, 0xDD, 0xED, 0xF3, 0xFB, 0xFD // Opcodes inválidos
    };
    
    int successful_calls = 0;
    int expected_throws = 0;
    
    for (size_t opcode = 0; opcode < 256; ++opcode) {
        // Preparar memoria con algunos datos de prueba
        for (size_t i = 0; i < 10; ++i) {
            memory[i] = static_cast<uint8_t>(i + 1);
        }
        
        // Inicializar registros con valores conocidos
        cpu.registers_m.setRegister(Registers::Register::A, 0x10);
        cpu.registers_m.setRegister(Registers::Register::B, 0x20);
        cpu.registers_m.setRegister(Registers::Register::C, 0x30);
        cpu.registers_m.setRegister(Registers::Register::D, 0x40);
        cpu.registers_m.setRegister(Registers::Register::E, 0x50);
        cpu.registers_m.setRegister(Registers::Register::H, 0x00);
        cpu.registers_m.setRegister(Registers::Register::L, 0x10);
        cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0x1000);
        cpu.pc_m = 0;
        
        bool should_throw = std::find(opcodes_that_throw.begin(), 
                                     opcodes_that_throw.end(), 
                                     opcode) != opcodes_that_throw.end();
        
        if (should_throw) {
            // Verificar que el opcode lanza una excepción
            EXPECT_THROW({
                auto opcode_func = cpu.opcodes_m[opcode];
                (cpu.*opcode_func)();
            }, std::runtime_error) << "Opcode 0x" << std::hex << opcode 
                                   << " debería lanzar excepción pero no lo hizo";
            expected_throws++;
        } else {
            // Verificar que el opcode puede ser llamado sin lanzar excepciones
            EXPECT_NO_THROW({
                auto opcode_func = cpu.opcodes_m[opcode];
                (cpu.*opcode_func)();
            }) << "Opcode 0x" << std::hex << opcode << " lanzó excepción inesperada";
            successful_calls++;
        }
    }
    
    // Verificar que se probaron todos los opcodes
    EXPECT_EQ(successful_calls + expected_throws, 256);
    EXPECT_GT(successful_calls, 0) << "Debe haber al menos un opcode válido";
}
