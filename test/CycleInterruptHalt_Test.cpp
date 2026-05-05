#include <gtest/gtest.h>
#include "commons/Fake8080Test.hpp"
#include <vector>

class CycleInterruptHalt_Test : public ::testing::Test {
protected:
    Fake8080Test cpu;
    std::vector<uint8_t> memory;

    void SetUp() override {
        memory.resize(0x10000, 0x00);
        cpu.mapMemory(memory);
        cpu.pc_m = 0x0000;
    }
};

TEST_F(CycleInterruptHalt_Test, CycleFetchesOpcodeAndAdvancesPc) {
    memory[0x0000] = 0x06; // MVI B,d8
    memory[0x0001] = 0x42;

    cpu.cycle();

    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x42);
    EXPECT_EQ(cpu.pc_m, 0x0002);
}

TEST_F(CycleInterruptHalt_Test, InterruptPushesPcAndJumpsToVector) {
    cpu.pc_m = 0x1234;
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xFFFE);

    cpu.EI();
    cpu.requestInterrupt(2);
    cpu.cycle();

    EXPECT_EQ(cpu.pc_m, 0x0010);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xFFFC);
    EXPECT_EQ(memory[0xFFFC], 0x34);
    EXPECT_EQ(memory[0xFFFD], 0x12);

    memory[0x0010] = 0x00; // NOP
    cpu.cycle();
    EXPECT_EQ(cpu.pc_m, 0x0011);
}

TEST_F(CycleInterruptHalt_Test, HaltStopsExecutionUntilInterrupt) {
    cpu.registers_m.setRegister(Registers::Register::B, 0x00);
    cpu.registers_m.setCombinedRegister(Registers::CombinedRegister::SP, 0xFFFE);

    memory[0x0000] = 0x76; // HLT
    memory[0x0001] = 0x06; // MVI B,d8
    memory[0x0002] = 0x99;

    cpu.cycle();
    EXPECT_EQ(cpu.pc_m, 0x0001);

    cpu.cycle();
    EXPECT_EQ(cpu.pc_m, 0x0001);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::B), 0x00);

    cpu.EI();
    cpu.requestInterrupt(1);
    cpu.cycle();

    EXPECT_EQ(cpu.pc_m, 0x0008);
    EXPECT_EQ(cpu.registers_m.getCombinedRegister(Registers::CombinedRegister::SP), 0xFFFC);
    EXPECT_EQ(memory[0xFFFC], 0x01);
    EXPECT_EQ(memory[0xFFFD], 0x00);

    memory[0x0008] = 0x00; // NOP
    cpu.cycle();
    EXPECT_EQ(cpu.pc_m, 0x0009);
}
