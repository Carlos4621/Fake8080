#include <gtest/gtest.h>
#include "Registers.hpp"

class RegistersTest : public ::testing::Test {
protected:
    Registers registers;

    void SetUp() override {
        // Los registros se inicializan automáticamente
    }
};

// Tests para registros individuales
TEST_F(RegistersTest, SetAndGetRegisterA) {
    registers.setRegister(Registers::Register::A, 0x42);
    EXPECT_EQ(registers.getRegister(Registers::Register::A), 0x42);
}

TEST_F(RegistersTest, SetAndGetRegisterB) {
    registers.setRegister(Registers::Register::B, 0xFF);
    EXPECT_EQ(registers.getRegister(Registers::Register::B), 0xFF);
}

TEST_F(RegistersTest, SetAndGetRegisterC) {
    registers.setRegister(Registers::Register::C, 0x00);
    EXPECT_EQ(registers.getRegister(Registers::Register::C), 0x00);
}

TEST_F(RegistersTest, SetAndGetRegisterD) {
    registers.setRegister(Registers::Register::D, 0xAB);
    EXPECT_EQ(registers.getRegister(Registers::Register::D), 0xAB);
}

TEST_F(RegistersTest, SetAndGetRegisterE) {
    registers.setRegister(Registers::Register::E, 0xCD);
    EXPECT_EQ(registers.getRegister(Registers::Register::E), 0xCD);
}

TEST_F(RegistersTest, SetAndGetRegisterH) {
    registers.setRegister(Registers::Register::H, 0x12);
    EXPECT_EQ(registers.getRegister(Registers::Register::H), 0x12);
}

TEST_F(RegistersTest, SetAndGetRegisterL) {
    registers.setRegister(Registers::Register::L, 0x34);
    EXPECT_EQ(registers.getRegister(Registers::Register::L), 0x34);
}

TEST_F(RegistersTest, SetAndGetRegisterW) {
    registers.setRegister(Registers::Register::W, 0x56);
    EXPECT_EQ(registers.getRegister(Registers::Register::W), 0x56);
}

TEST_F(RegistersTest, SetAndGetRegisterZ) {
    registers.setRegister(Registers::Register::Z, 0x78);
    EXPECT_EQ(registers.getRegister(Registers::Register::Z), 0x78);
}

TEST_F(RegistersTest, AllRegistersIndependent) {
    registers.setRegister(Registers::Register::A, 0x01);
    registers.setRegister(Registers::Register::B, 0x02);
    registers.setRegister(Registers::Register::C, 0x03);
    registers.setRegister(Registers::Register::D, 0x04);
    registers.setRegister(Registers::Register::E, 0x05);
    registers.setRegister(Registers::Register::H, 0x06);
    registers.setRegister(Registers::Register::L, 0x07);
    registers.setRegister(Registers::Register::W, 0x08);
    registers.setRegister(Registers::Register::Z, 0x09);

    EXPECT_EQ(registers.getRegister(Registers::Register::A), 0x01);
    EXPECT_EQ(registers.getRegister(Registers::Register::B), 0x02);
    EXPECT_EQ(registers.getRegister(Registers::Register::C), 0x03);
    EXPECT_EQ(registers.getRegister(Registers::Register::D), 0x04);
    EXPECT_EQ(registers.getRegister(Registers::Register::E), 0x05);
    EXPECT_EQ(registers.getRegister(Registers::Register::H), 0x06);
    EXPECT_EQ(registers.getRegister(Registers::Register::L), 0x07);
    EXPECT_EQ(registers.getRegister(Registers::Register::W), 0x08);
    EXPECT_EQ(registers.getRegister(Registers::Register::Z), 0x09);
}

// Tests para registros combinados
TEST_F(RegistersTest, SetAndGetCombinedRegisterBC) {
    registers.setCombinedRegister(Registers::CombinedRegister::BC, 0x1234);
    EXPECT_EQ(registers.getCombinedRegister(Registers::CombinedRegister::BC), 0x1234);
}

TEST_F(RegistersTest, SetAndGetCombinedRegisterDE) {
    registers.setCombinedRegister(Registers::CombinedRegister::DE, 0xABCD);
    EXPECT_EQ(registers.getCombinedRegister(Registers::CombinedRegister::DE), 0xABCD);
}

TEST_F(RegistersTest, SetAndGetCombinedRegisterHL) {
    registers.setCombinedRegister(Registers::CombinedRegister::HL, 0xFFFF);
    EXPECT_EQ(registers.getCombinedRegister(Registers::CombinedRegister::HL), 0xFFFF);
}

TEST_F(RegistersTest, CombinedRegisterBCSplitsCorrectly) {
    registers.setCombinedRegister(Registers::CombinedRegister::BC, 0x1234);
    EXPECT_EQ(registers.getRegister(Registers::Register::B), 0x12);
    EXPECT_EQ(registers.getRegister(Registers::Register::C), 0x34);
}

TEST_F(RegistersTest, CombinedRegisterDESplitsCorrectly) {
    registers.setCombinedRegister(Registers::CombinedRegister::DE, 0xABCD);
    EXPECT_EQ(registers.getRegister(Registers::Register::D), 0xAB);
    EXPECT_EQ(registers.getRegister(Registers::Register::E), 0xCD);
}

TEST_F(RegistersTest, CombinedRegisterHLSplitsCorrectly) {
    registers.setCombinedRegister(Registers::CombinedRegister::HL, 0x8765);
    EXPECT_EQ(registers.getRegister(Registers::Register::H), 0x87);
    EXPECT_EQ(registers.getRegister(Registers::Register::L), 0x65);
}

TEST_F(RegistersTest, IndividualRegistersFormCombinedRegisterBC) {
    registers.setRegister(Registers::Register::B, 0x12);
    registers.setRegister(Registers::Register::C, 0x34);
    EXPECT_EQ(registers.getCombinedRegister(Registers::CombinedRegister::BC), 0x1234);
}

TEST_F(RegistersTest, IndividualRegistersFormCombinedRegisterDE) {
    registers.setRegister(Registers::Register::D, 0xAB);
    registers.setRegister(Registers::Register::E, 0xCD);
    EXPECT_EQ(registers.getCombinedRegister(Registers::CombinedRegister::DE), 0xABCD);
}

TEST_F(RegistersTest, IndividualRegistersFormCombinedRegisterHL) {
    registers.setRegister(Registers::Register::H, 0xFF);
    registers.setRegister(Registers::Register::L, 0x00);
    EXPECT_EQ(registers.getCombinedRegister(Registers::CombinedRegister::HL), 0xFF00);
}

// Tests de valores límite
TEST_F(RegistersTest, CombinedRegisterMinValue) {
    registers.setCombinedRegister(Registers::CombinedRegister::BC, 0x0000);
    EXPECT_EQ(registers.getCombinedRegister(Registers::CombinedRegister::BC), 0x0000);
    EXPECT_EQ(registers.getRegister(Registers::Register::B), 0x00);
    EXPECT_EQ(registers.getRegister(Registers::Register::C), 0x00);
}

TEST_F(RegistersTest, CombinedRegisterMaxValue) {
    registers.setCombinedRegister(Registers::CombinedRegister::HL, 0xFFFF);
    EXPECT_EQ(registers.getCombinedRegister(Registers::CombinedRegister::HL), 0xFFFF);
    EXPECT_EQ(registers.getRegister(Registers::Register::H), 0xFF);
    EXPECT_EQ(registers.getRegister(Registers::Register::L), 0xFF);
}

TEST_F(RegistersTest, OverwriteRegisterValue) {
    registers.setRegister(Registers::Register::A, 0x11);
    EXPECT_EQ(registers.getRegister(Registers::Register::A), 0x11);
    
    registers.setRegister(Registers::Register::A, 0x22);
    EXPECT_EQ(registers.getRegister(Registers::Register::A), 0x22);
}

TEST_F(RegistersTest, OverwriteCombinedRegisterValue) {
    registers.setCombinedRegister(Registers::CombinedRegister::BC, 0x1234);
    EXPECT_EQ(registers.getCombinedRegister(Registers::CombinedRegister::BC), 0x1234);
    
    registers.setCombinedRegister(Registers::CombinedRegister::BC, 0x5678);
    EXPECT_EQ(registers.getCombinedRegister(Registers::CombinedRegister::BC), 0x5678);
}

TEST_F(RegistersTest, CombinedRegisterDoesNotAffectOtherRegisters) {
    registers.setRegister(Registers::Register::A, 0xFF);
    registers.setCombinedRegister(Registers::CombinedRegister::BC, 0x1234);
    
    EXPECT_EQ(registers.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(registers.getRegister(Registers::Register::B), 0x12);
    EXPECT_EQ(registers.getRegister(Registers::Register::C), 0x34);
}

// Tests para el registro SP (Stack Pointer)
TEST_F(RegistersTest, SetAndGetCombinedRegisterSP) {
    registers.setCombinedRegister(Registers::CombinedRegister::SP, 0xABCD);
    EXPECT_EQ(registers.getCombinedRegister(Registers::CombinedRegister::SP), 0xABCD);
}

TEST_F(RegistersTest, CombinedRegisterSPSplitsCorrectly) {
    registers.setCombinedRegister(Registers::CombinedRegister::SP, 0x1234);
    EXPECT_EQ(registers.getRegister(Registers::Register::SP_high), 0x12);
    EXPECT_EQ(registers.getRegister(Registers::Register::SP_low), 0x34);
}

TEST_F(RegistersTest, IndividualRegistersFormCombinedRegisterSP) {
    registers.setRegister(Registers::Register::SP_high, 0xFE);
    registers.setRegister(Registers::Register::SP_low, 0xDC);
    EXPECT_EQ(registers.getCombinedRegister(Registers::CombinedRegister::SP), 0xFEDC);
}

// Tests para el registro WZ
TEST_F(RegistersTest, SetAndGetCombinedRegisterWZ) {
    registers.setCombinedRegister(Registers::CombinedRegister::WZ, 0x5678);
    EXPECT_EQ(registers.getCombinedRegister(Registers::CombinedRegister::WZ), 0x5678);
}

TEST_F(RegistersTest, CombinedRegisterWZSplitsCorrectly) {
    registers.setCombinedRegister(Registers::CombinedRegister::WZ, 0x5678);
    EXPECT_EQ(registers.getRegister(Registers::Register::W), 0x56);
    EXPECT_EQ(registers.getRegister(Registers::Register::Z), 0x78);
}

TEST_F(RegistersTest, IndividualRegistersFormCombinedRegisterWZ) {
    registers.setRegister(Registers::Register::W, 0x56);
    registers.setRegister(Registers::Register::Z, 0x78);
    EXPECT_EQ(registers.getCombinedRegister(Registers::CombinedRegister::WZ), 0x5678);
}

// Tests para flags
TEST_F(RegistersTest, SetAndGetFlagS) {
    registers.setFlag(Registers::Flags::S, true);
    EXPECT_TRUE(registers.getFlag(Registers::Flags::S));
    
    registers.setFlag(Registers::Flags::S, false);
    EXPECT_FALSE(registers.getFlag(Registers::Flags::S));
}

TEST_F(RegistersTest, SetAndGetFlagZ) {
    registers.setFlag(Registers::Flags::Z, true);
    EXPECT_TRUE(registers.getFlag(Registers::Flags::Z));
    
    registers.setFlag(Registers::Flags::Z, false);
    EXPECT_FALSE(registers.getFlag(Registers::Flags::Z));
}

TEST_F(RegistersTest, SetAndGetFlagAC) {
    registers.setFlag(Registers::Flags::AC, true);
    EXPECT_TRUE(registers.getFlag(Registers::Flags::AC));
    
    registers.setFlag(Registers::Flags::AC, false);
    EXPECT_FALSE(registers.getFlag(Registers::Flags::AC));
}

TEST_F(RegistersTest, SetAndGetFlagP) {
    registers.setFlag(Registers::Flags::P, true);
    EXPECT_TRUE(registers.getFlag(Registers::Flags::P));
    
    registers.setFlag(Registers::Flags::P, false);
    EXPECT_FALSE(registers.getFlag(Registers::Flags::P));
}

TEST_F(RegistersTest, SetAndGetFlagCY) {
    registers.setFlag(Registers::Flags::CY, true);
    EXPECT_TRUE(registers.getFlag(Registers::Flags::CY));
    
    registers.setFlag(Registers::Flags::CY, false);
    EXPECT_FALSE(registers.getFlag(Registers::Flags::CY));
}

TEST_F(RegistersTest, AllFlagsIndependent) {
    registers.setFlag(Registers::Flags::S, true);
    registers.setFlag(Registers::Flags::Z, false);
    registers.setFlag(Registers::Flags::AC, true);
    registers.setFlag(Registers::Flags::P, false);
    registers.setFlag(Registers::Flags::CY, true);
    
    EXPECT_TRUE(registers.getFlag(Registers::Flags::S));
    EXPECT_FALSE(registers.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(registers.getFlag(Registers::Flags::AC));
    EXPECT_FALSE(registers.getFlag(Registers::Flags::P));
    EXPECT_TRUE(registers.getFlag(Registers::Flags::CY));
}

TEST_F(RegistersTest, FlagsStoredInRegisterF) {
    // S=1, Z=1, AC=1, P=1, CY=1 -> bits 7,6,4,2,0 = 0xD5
    // Bit 1 siempre es 1, entonces 0xD5 | 0x02 = 0xD7
    registers.setFlag(Registers::Flags::S, true);   // bit 7
    registers.setFlag(Registers::Flags::Z, true);   // bit 6
    registers.setFlag(Registers::Flags::AC, true);  // bit 4
    registers.setFlag(Registers::Flags::P, true);   // bit 2
    registers.setFlag(Registers::Flags::CY, true);  // bit 0
    
    EXPECT_EQ(registers.getRegister(Registers::Register::F), 0xD7);
}

TEST_F(RegistersTest, OverwriteFlagValue) {
    registers.setFlag(Registers::Flags::Z, true);
    EXPECT_TRUE(registers.getFlag(Registers::Flags::Z));
    
    registers.setFlag(Registers::Flags::Z, false);
    EXPECT_FALSE(registers.getFlag(Registers::Flags::Z));
    
    registers.setFlag(Registers::Flags::Z, true);
    EXPECT_TRUE(registers.getFlag(Registers::Flags::Z));
}

TEST_F(RegistersTest, SettingFlagDoesNotAffectOtherFlags) {
    registers.setFlag(Registers::Flags::S, true);
    registers.setFlag(Registers::Flags::CY, true);
    
    registers.setFlag(Registers::Flags::Z, true);
    
    EXPECT_TRUE(registers.getFlag(Registers::Flags::S));
    EXPECT_TRUE(registers.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(registers.getFlag(Registers::Flags::CY));
}

TEST_F(RegistersTest, ClearingFlagDoesNotAffectOtherFlags) {
    registers.setFlag(Registers::Flags::S, true);
    registers.setFlag(Registers::Flags::Z, true);
    registers.setFlag(Registers::Flags::CY, true);
    
    registers.setFlag(Registers::Flags::Z, false);
    
    EXPECT_TRUE(registers.getFlag(Registers::Flags::S));
    EXPECT_FALSE(registers.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(registers.getFlag(Registers::Flags::CY));
}

// Tests para el registro PSW (Program Status Word)
TEST_F(RegistersTest, SetAndGetCombinedRegisterPSW) {
    registers.setCombinedRegister(Registers::CombinedRegister::PSW, 0x12D7);
    EXPECT_EQ(registers.getCombinedRegister(Registers::CombinedRegister::PSW), 0x12D7);
}

TEST_F(RegistersTest, CombinedRegisterPSWSplitsCorrectly) {
    // PSW = A (high byte) + F (low byte)
    registers.setCombinedRegister(Registers::CombinedRegister::PSW, 0x42D5);
    EXPECT_EQ(registers.getRegister(Registers::Register::A), 0x42);
    EXPECT_EQ(registers.getRegister(Registers::Register::F), 0xD5);
}

TEST_F(RegistersTest, IndividualRegistersFormCombinedRegisterPSW) {
    registers.setRegister(Registers::Register::A, 0xFF);
    registers.setRegister(Registers::Register::F, 0xAA);
    EXPECT_EQ(registers.getCombinedRegister(Registers::CombinedRegister::PSW), 0xFFAA);
}

TEST_F(RegistersTest, PSWReflectsAccumulatorAndFlags) {
    // Establecer acumulador
    registers.setRegister(Registers::Register::A, 0x42);
    
    // Establecer flags: S=1, Z=1, AC=0, P=0, CY=1
    // Binario: 11000001 con bit 1 siempre en 1 = 11000011 = 0xC3
    registers.setFlag(Registers::Flags::S, true);
    registers.setFlag(Registers::Flags::Z, true);
    registers.setFlag(Registers::Flags::AC, false);
    registers.setFlag(Registers::Flags::P, false);
    registers.setFlag(Registers::Flags::CY, true);
    
    // PSW debe ser A (0x42) en byte alto, F (0xC3) en byte bajo
    EXPECT_EQ(registers.getCombinedRegister(Registers::CombinedRegister::PSW), 0x42C3);
}

TEST_F(RegistersTest, SettingPSWUpdatesAccumulatorAndFlags) {
    // PSW = 0xABD7
    // A = 0xAB (high byte)
    // F = 0xD7 (low byte) = 11010111
    // S=1 (bit 7), Z=1 (bit 6), AC=1 (bit 4), P=1 (bit 2), bit1=1, CY=1 (bit 0)
    registers.setCombinedRegister(Registers::CombinedRegister::PSW, 0xABD7);
    
    EXPECT_EQ(registers.getRegister(Registers::Register::A), 0xAB);
    EXPECT_TRUE(registers.getFlag(Registers::Flags::S));
    EXPECT_TRUE(registers.getFlag(Registers::Flags::Z));
    EXPECT_TRUE(registers.getFlag(Registers::Flags::AC));
    EXPECT_TRUE(registers.getFlag(Registers::Flags::P));
    EXPECT_TRUE(registers.getFlag(Registers::Flags::CY));
}

TEST_F(RegistersTest, PSWMinValue) {
    registers.setCombinedRegister(Registers::CombinedRegister::PSW, 0x0000);
    EXPECT_EQ(registers.getCombinedRegister(Registers::CombinedRegister::PSW), 0x0000);
    EXPECT_EQ(registers.getRegister(Registers::Register::A), 0x00);
    EXPECT_EQ(registers.getRegister(Registers::Register::F), 0x00);
}

TEST_F(RegistersTest, PSWMaxValue) {
    registers.setCombinedRegister(Registers::CombinedRegister::PSW, 0xFFFF);
    EXPECT_EQ(registers.getCombinedRegister(Registers::CombinedRegister::PSW), 0xFFFF);
    EXPECT_EQ(registers.getRegister(Registers::Register::A), 0xFF);
    EXPECT_EQ(registers.getRegister(Registers::Register::F), 0xFF);
}

TEST_F(RegistersTest, PSWDoesNotAffectOtherRegisters) {
    registers.setRegister(Registers::Register::B, 0x11);
    registers.setRegister(Registers::Register::C, 0x22);
    registers.setRegister(Registers::Register::H, 0x33);
    registers.setRegister(Registers::Register::L, 0x44);
    
    registers.setCombinedRegister(Registers::CombinedRegister::PSW, 0xABCD);
    
    EXPECT_EQ(registers.getRegister(Registers::Register::B), 0x11);
    EXPECT_EQ(registers.getRegister(Registers::Register::C), 0x22);
    EXPECT_EQ(registers.getRegister(Registers::Register::H), 0x33);
    EXPECT_EQ(registers.getRegister(Registers::Register::L), 0x44);
}

TEST_F(RegistersTest, PSWChangingAccumulatorUpdatesHighByte) {
    registers.setCombinedRegister(Registers::CombinedRegister::PSW, 0x12D5);
    EXPECT_EQ(registers.getCombinedRegister(Registers::CombinedRegister::PSW), 0x12D5);
    
    registers.setRegister(Registers::Register::A, 0xAB);
    EXPECT_EQ(registers.getCombinedRegister(Registers::CombinedRegister::PSW), 0xABD5);
}

TEST_F(RegistersTest, PSWChangingFlagsUpdatesLowByte) {
    registers.setCombinedRegister(Registers::CombinedRegister::PSW, 0x4202);
    EXPECT_EQ(registers.getCombinedRegister(Registers::CombinedRegister::PSW), 0x4202);
    
    // Cambiar todos los flags a 1 -> F = 0xD7
    registers.setFlag(Registers::Flags::S, true);
    registers.setFlag(Registers::Flags::Z, true);
    registers.setFlag(Registers::Flags::AC, true);
    registers.setFlag(Registers::Flags::P, true);
    registers.setFlag(Registers::Flags::CY, true);
    
    EXPECT_EQ(registers.getCombinedRegister(Registers::CombinedRegister::PSW), 0x42D7);
}

