#include <gtest/gtest.h>
#include "commons/CPUTest.hpp"
#include <array>

class MockIOPort : public IOPort {
private:
    uint8_t value_m{ 0 };
    uint8_t lastWritten_m{ 0 };
    mutable size_t readCount_m{ 0 };
    size_t writeCount_m{ 0 };

public:
    explicit MockIOPort(uint8_t initialValue = 0) : value_m(initialValue) {}

    uint8_t in() const noexcept override {
        ++readCount_m;
        return value_m;
    }

    void out(uint8_t value) override {
        ++writeCount_m;
        lastWritten_m = value;
        value_m = value;
    }

    [[nodiscard]] size_t getReadCount() const { return readCount_m; }
    [[nodiscard]] size_t getWriteCount() const { return writeCount_m; }
    [[nodiscard]] uint8_t getLastWritten() const { return lastWritten_m; }
    void setValue(uint8_t value) { value_m = value; }
};

class IN_OUT_Test : public ::testing::Test {
protected:
    CPUTest cpu;
    MockIOPort device;
    std::array<uint8_t, 0x10000> rom;

    void SetUp() override {
        cpu.pc_m = 0x0000;
        cpu.registers_m.setRegister(Registers::Register::A, 0x00);
        cpu.registers_m.setFlag(Registers::Flags::S, false);
        cpu.registers_m.setFlag(Registers::Flags::Z, false);
        cpu.registers_m.setFlag(Registers::Flags::AC, false);
        cpu.registers_m.setFlag(Registers::Flags::P, false);
        cpu.registers_m.setFlag(Registers::Flags::CY, false);
        rom.fill(0x00);
    }
};

TEST_F(IN_OUT_Test, IN_ReadsFromPortIntoA) {
    device.setValue(0x3C);
    cpu.ioBus_test.attach(0x10, device);
    rom[0] = 0x10;
    cpu.mapMemory(rom);

    uint8_t cycles = cpu.IN_d8();

    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x3C);
    EXPECT_EQ(device.getReadCount(), 1u);
    EXPECT_EQ(cpu.pc_m, 0x0001);
    EXPECT_EQ(cycles, 10);
}

TEST_F(IN_OUT_Test, IN_DisconnectedPortReturnsFF) {
    rom[0] = 0x20;
    cpu.mapMemory(rom);

    cpu.IN_d8();

    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(cpu.pc_m, 0x0001);
}

TEST_F(IN_OUT_Test, IN_PreservesFlags) {
    device.setValue(0x55);
    cpu.ioBus_test.attach(0x05, device);
    rom[0] = 0x05;
    cpu.mapMemory(rom);

    cpu.registers_m.setFlag(Registers::Flags::S, true);
    cpu.registers_m.setFlag(Registers::Flags::Z, true);
    cpu.registers_m.setFlag(Registers::Flags::AC, true);
    cpu.registers_m.setFlag(Registers::Flags::P, true);
    cpu.registers_m.setFlag(Registers::Flags::CY, true);

    cpu.IN_d8();

    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::S));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::AC));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::P));
    EXPECT_TRUE(cpu.registers_m.getFlag(Registers::Flags::CY));
}

TEST_F(IN_OUT_Test, OUT_WritesAToPort) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x5A);
    cpu.ioBus_test.attach(0x33, device);
    rom[0] = 0x33;
    cpu.mapMemory(rom);

    uint8_t cycles = cpu.OUT_d8();

    EXPECT_EQ(device.getLastWritten(), 0x5A);
    EXPECT_EQ(device.getWriteCount(), 1u);
    EXPECT_EQ(cpu.pc_m, 0x0001);
    EXPECT_EQ(cycles, 10);
    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x5A);
}

TEST_F(IN_OUT_Test, OUT_DisconnectedPortDoesNothingToA) {
    cpu.registers_m.setRegister(Registers::Register::A, 0x7E);
    rom[0] = 0x80;
    cpu.mapMemory(rom);

    uint8_t cycles = cpu.OUT_d8();

    EXPECT_EQ(cpu.registers_m.getRegister(Registers::Register::A), 0x7E);
    EXPECT_EQ(cpu.pc_m, 0x0001);
    EXPECT_EQ(cycles, 10);
}
