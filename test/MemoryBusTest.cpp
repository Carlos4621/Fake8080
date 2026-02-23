#include <gtest/gtest.h>
#include "MemoryBus.hpp"
#include <array>
#include <vector>

class MemoryBusTest : public ::testing::Test {
protected:
    MemoryBus bus;
    std::array<uint8_t, 256> ramData{};
    std::array<uint8_t, 256> romData{};

    void SetUp() override {
        // Inicializar datos de RAM con valores incrementales
        for (size_t i = 0; i < ramData.size(); ++i) {
            ramData[i] = static_cast<uint8_t>(i);
        }
        
        // Inicializar datos de ROM con patrón específico
        for (size_t i = 0; i < romData.size(); ++i) {
            romData[i] = static_cast<uint8_t>(0xA0 + (i % 16));
        }
    }
};

// ========== Tests de OpenBusRegion ==========

TEST_F(MemoryBusTest, OpenBusRegionReturns0xFF) {
    OpenBusRegion openBus;
    EXPECT_EQ(openBus.read(0), 0xFF);
    EXPECT_EQ(openBus.read(100), 0xFF);
    EXPECT_EQ(openBus.read(0xFFFF), 0xFF);
}

TEST_F(MemoryBusTest, OpenBusRegionIgnoresWrites) {
    OpenBusRegion openBus;
    openBus.write(0, 0x42);
    EXPECT_EQ(openBus.read(0), 0xFF); // Debe retornar 0xFF, no 0x42
}

// ========== Tests de RamRegion ==========

TEST_F(MemoryBusTest, RamRegionReadWriteBasic) {
    RamRegion ram(ramData);
    
    // Leer valor inicial
    EXPECT_EQ(ram.read(0), 0);
    EXPECT_EQ(ram.read(10), 10);
    
    // Escribir y leer
    ram.write(0, 0x42);
    EXPECT_EQ(ram.read(0), 0x42);
    
    ram.write(10, 0xFF);
    EXPECT_EQ(ram.read(10), 0xFF);
}

TEST_F(MemoryBusTest, RamRegionMultipleReadsWrites) {
    RamRegion ram(ramData);
    
    for (uint16_t i = 0; i < 100; ++i) {
        ram.write(i, static_cast<uint8_t>(i * 2));
    }
    
    for (uint16_t i = 0; i < 100; ++i) {
        EXPECT_EQ(ram.read(i), static_cast<uint8_t>(i * 2));
    }
}

TEST_F(MemoryBusTest, RamRegionPreservesData) {
    RamRegion ram(ramData);
    
    ram.write(5, 0xAA);
    ram.write(6, 0xBB);
    ram.write(7, 0xCC);
    
    EXPECT_EQ(ram.read(5), 0xAA);
    EXPECT_EQ(ram.read(6), 0xBB);
    EXPECT_EQ(ram.read(7), 0xCC);
}

// ========== Tests de RomRegion ==========

TEST_F(MemoryBusTest, RomRegionReadBasic) {
    RomRegion rom(romData);
    
    EXPECT_EQ(rom.read(0), 0xA0);
    EXPECT_EQ(rom.read(1), 0xA1);
    EXPECT_EQ(rom.read(15), 0xAF);
    EXPECT_EQ(rom.read(16), 0xA0); // Patrón se repite
}

TEST_F(MemoryBusTest, RomRegionIgnoresWrites) {
    RomRegion rom(romData);
    
    uint8_t originalValue = rom.read(0);
    rom.write(0, 0xFF);
    EXPECT_EQ(rom.read(0), originalValue); // El valor no debe cambiar
}

TEST_F(MemoryBusTest, RomRegionMultipleWritesIgnored) {
    RomRegion rom(romData);
    
    for (uint16_t i = 0; i < 50; ++i) {
        uint8_t original = rom.read(i);
        rom.write(i, 0x00);
        rom.write(i, 0xFF);
        rom.write(i, 0x42);
        EXPECT_EQ(rom.read(i), original);
    }
}

// ========== Tests de MemoryBus - Sin mapeos ==========

TEST_F(MemoryBusTest, UnmappedAddressReturns0xFF) {
    // Sin mapeos, todas las direcciones deben retornar 0xFF
    EXPECT_EQ(bus.read(0), 0xFF);
    EXPECT_EQ(bus.read(0x1000), 0xFF);
    EXPECT_EQ(bus.read(0xFFFF), 0xFF);
}

TEST_F(MemoryBusTest, UnmappedAddressIgnoresWrites) {
    bus.write(0x1000, 0x42);
    EXPECT_EQ(bus.read(0x1000), 0xFF); // Debe retornar 0xFF
}

// ========== Tests de MemoryBus - Mapeo básico de RAM ==========

TEST_F(MemoryBusTest, SingleRamRegionMapping) {
    RamRegion ram(ramData);
    bus.map(0x0000, 0x00FF, ram);
    
    // Leer valores iniciales
    EXPECT_EQ(bus.read(0x0000), 0);
    EXPECT_EQ(bus.read(0x0010), 16);
    EXPECT_EQ(bus.read(0x00FF), 255);
    
    // Escribir y verificar
    bus.write(0x0000, 0x42);
    EXPECT_EQ(bus.read(0x0000), 0x42);
}

TEST_F(MemoryBusTest, PartialRamRegionMapping) {
    RamRegion ram(ramData);
    bus.map(0x1000, 0x10FF, ram); // Mapear en dirección diferente
    
    // Dirección no mapeada
    EXPECT_EQ(bus.read(0x0000), 0xFF);
    
    // Direcciones mapeadas
    EXPECT_EQ(bus.read(0x1000), 0);
    EXPECT_EQ(bus.read(0x1010), 16);
    
    bus.write(0x1000, 0x99);
    EXPECT_EQ(bus.read(0x1000), 0x99);
}

// ========== Tests de MemoryBus - Mapeo de ROM ==========

TEST_F(MemoryBusTest, RomRegionMapping) {
    RomRegion rom(romData);
    bus.map(0x0000, 0x00FF, rom);
    
    EXPECT_EQ(bus.read(0x0000), 0xA0);
    EXPECT_EQ(bus.read(0x0001), 0xA1);
    
    uint8_t originalValue = bus.read(0x0000);
    bus.write(0x0000, 0xFF);
    EXPECT_EQ(bus.read(0x0000), originalValue); // ROM no debe cambiar
}

TEST_F(MemoryBusTest, MixedRamAndRomRegions) {
    RamRegion ram(ramData);
    RomRegion rom(romData);
    
    // RAM en 0x0000-0x00FF, ROM en 0x1000-0x10FF
    bus.map(0x0000, 0x00FF, ram);
    bus.map(0x1000, 0x10FF, rom);
    
    // Verificar RAM (escribible)
    bus.write(0x0000, 0x12);
    EXPECT_EQ(bus.read(0x0000), 0x12);
    
    // Verificar ROM (no escribible)
    uint8_t romValue = bus.read(0x1000);
    bus.write(0x1000, 0xFF);
    EXPECT_EQ(bus.read(0x1000), romValue);
    
    // Verificar área no mapeada
    EXPECT_EQ(bus.read(0x2000), 0xFF);
}

// ========== Tests de MemoryBus - Mapeos superpuestos ==========

TEST_F(MemoryBusTest, OverlappingRegionsFirstWins) {
    std::array<uint8_t, 128> ram1{};
    std::array<uint8_t, 128> ram2{};
    
    // Inicializar con valores diferentes
    for (size_t i = 0; i < ram1.size(); ++i) {
        ram1[i] = 0x11;
        ram2[i] = 0x22;
    }
    
    RamRegion region1(ram1);
    RamRegion region2(ram2);
    
    // Mapear la misma dirección dos veces
    bus.map(0x0000, 0x007F, region1);
    bus.map(0x0000, 0x007F, region2); // Primer mapeo gana
    
    EXPECT_EQ(bus.read(0x0000), 0x11); // Debe leer de region1
    
    bus.write(0x0000, 0x99);
    EXPECT_EQ(bus.read(0x0000), 0x99);
    EXPECT_EQ(ram1[0], 0x99); // Se escribió en region1
    EXPECT_EQ(ram2[0], 0x22); // region2 no fue modificada
}

TEST_F(MemoryBusTest, OverlappingRegionsPartialOverlap) {
    std::array<uint8_t, 64> ram1{};
    std::array<uint8_t, 64> ram2{};
    
    for (size_t i = 0; i < ram1.size(); ++i) {
        ram1[i] = 0xAA;
        ram2[i] = 0xBB;
    }
    
    RamRegion region1(ram1);
    RamRegion region2(ram2);
    
    // region1: 0x0000-0x003F
    // region2: 0x0020-0x005F (superpone desde 0x0020-0x003F)
    bus.map(0x0000, 0x003F, region1);
    bus.map(0x0020, 0x005F, region2);
    
    // 0x0000-0x001F debe ir a region1
    EXPECT_EQ(bus.read(0x0000), 0xAA);
    EXPECT_EQ(bus.read(0x001F), 0xAA);
    
    // 0x0020-0x003F debe ir a region1 (primer mapeo gana)
    EXPECT_EQ(bus.read(0x0020), 0xAA);
    EXPECT_EQ(bus.read(0x003F), 0xAA);
    
    // 0x0040-0x005F debe ir a region2
    EXPECT_EQ(bus.read(0x0040), 0xBB);
    EXPECT_EQ(bus.read(0x005F), 0xBB);
    
    // Escribir en el área superpuesta
    bus.write(0x0020, 0x55);
    EXPECT_EQ(bus.read(0x0020), 0x55);
    EXPECT_EQ(ram1[32], 0x55); // Se escribió en region1
}

// ========== Tests de MemoryBus - Validación de parámetros ==========

TEST_F(MemoryBusTest, MapThrowsWhenEndBeforeStart) {
    RamRegion ram(ramData);
    EXPECT_THROW(bus.map(0x0100, 0x0000, ram), std::invalid_argument);
}

TEST_F(MemoryBusTest, MapAllowsEqualStartEnd) {
    std::array<uint8_t, 1> singleByte{0};
    RamRegion ram(singleByte);
    EXPECT_NO_THROW(bus.map(0x0100, 0x0100, ram));
    
    bus.write(0x0100, 0x42);
    EXPECT_EQ(bus.read(0x0100), 0x42);
}

// ========== Tests de MemoryBus - Casos límite ==========

TEST_F(MemoryBusTest, MapFullAddressSpace) {
    std::array<uint8_t, 65536> largeRam{};
    RamRegion ram(largeRam);
    
    bus.map(0x0000, 0xFFFF, ram);
    
    bus.write(0x0000, 0x11);
    bus.write(0x8000, 0x22);
    bus.write(0xFFFF, 0x33);
    
    EXPECT_EQ(bus.read(0x0000), 0x11);
    EXPECT_EQ(bus.read(0x8000), 0x22);
    EXPECT_EQ(bus.read(0xFFFF), 0x33);
}

TEST_F(MemoryBusTest, MapBoundaryConditions) {
    RamRegion ram(ramData);
    bus.map(0x0000, 0x00FF, ram);
    
    // Probar límites exactos
    bus.write(0x0000, 0xAA);
    bus.write(0x00FF, 0xBB);
    
    EXPECT_EQ(bus.read(0x0000), 0xAA);
    EXPECT_EQ(bus.read(0x00FF), 0xBB);
    
    // Fuera de límites
    EXPECT_EQ(bus.read(0x0100), 0xFF);
}

TEST_F(MemoryBusTest, MultipleSequentialMappings) {
    std::array<uint8_t, 64> ram1{}, ram2{}, ram3{}, ram4{};
    
    for (size_t i = 0; i < 64; ++i) {
        ram1[i] = 0x10;
        ram2[i] = 0x20;
        ram3[i] = 0x30;
        ram4[i] = 0x40;
    }
    
    RamRegion region1(ram1);
    RamRegion region2(ram2);
    RamRegion region3(ram3);
    RamRegion region4(ram4);
    
    // Mapear regiones secuencialmente
    bus.map(0x0000, 0x003F, region1);
    bus.map(0x0040, 0x007F, region2);
    bus.map(0x0080, 0x00BF, region3);
    bus.map(0x00C0, 0x00FF, region4);
    
    EXPECT_EQ(bus.read(0x0000), 0x10);
    EXPECT_EQ(bus.read(0x0040), 0x20);
    EXPECT_EQ(bus.read(0x0080), 0x30);
    EXPECT_EQ(bus.read(0x00C0), 0x40);
    
    // Escribir en cada región
    bus.write(0x0000, 0x01);
    bus.write(0x0040, 0x02);
    bus.write(0x0080, 0x03);
    bus.write(0x00C0, 0x04);
    
    EXPECT_EQ(bus.read(0x0000), 0x01);
    EXPECT_EQ(bus.read(0x0040), 0x02);
    EXPECT_EQ(bus.read(0x0080), 0x03);
    EXPECT_EQ(bus.read(0x00C0), 0x04);
}

TEST_F(MemoryBusTest, StressTestMultipleReadsWrites) {
    RamRegion ram(ramData);
    bus.map(0x0000, 0x00FF, ram);
    
    // Escribir patrón
    for (uint16_t addr = 0x0000; addr <= 0x00FF; ++addr) {
        bus.write(addr, static_cast<uint8_t>(addr ^ 0xFF));
    }
    
    // Verificar patrón
    for (uint16_t addr = 0x0000; addr <= 0x00FF; ++addr) {
        EXPECT_EQ(bus.read(addr), static_cast<uint8_t>(addr ^ 0xFF));
    }
}

TEST_F(MemoryBusTest, MirroredRegion) {
    RamRegion ram(ramData);
    
    // Mapear la misma región RAM en dos lugares diferentes
    bus.map(0x0000, 0x00FF, ram);
    bus.map(0x1000, 0x10FF, ram);
    
    // Escribir en la primera dirección
    bus.write(0x0000, 0x77);
    
    // Debería reflejarse en la segunda dirección (mismo RAM)
    EXPECT_EQ(bus.read(0x1000), 0x77);
    EXPECT_EQ(ramData[0], 0x77);
}

TEST_F(MemoryBusTest, ComplexMemoryLayout) {
    std::array<uint8_t, 256> lowRam{};
    std::array<uint8_t, 256> highRam{};
    std::array<const uint8_t, 128> biosRom = {
        0x31, 0xFF, 0xFF, 0xCD, 0x00, 0x20, 0x76 // Código de ejemplo
    };
    
    RamRegion lowRamRegion(lowRam);
    RamRegion highRamRegion(highRam);
    RomRegion biosRegion(biosRom);
    
    // Layout de memoria típico
    bus.map(0x0000, 0x00FF, lowRamRegion);     // RAM baja
    bus.map(0xE000, 0xE07F, biosRegion);       // BIOS ROM
    bus.map(0xFF00, 0xFFFF, highRamRegion);    // RAM alta
    
    // Verificar cada región
    bus.write(0x0000, 0xAA);
    EXPECT_EQ(bus.read(0x0000), 0xAA);
    
    EXPECT_EQ(bus.read(0xE000), 0x31); // ROM
    bus.write(0xE000, 0xFF);
    EXPECT_EQ(bus.read(0xE000), 0x31); // ROM no cambia
    
    bus.write(0xFF00, 0xCC);
    EXPECT_EQ(bus.read(0xFF00), 0xCC);
    
    // Área no mapeada
    EXPECT_EQ(bus.read(0x8000), 0xFF);
}

// ========== Tests adicionales de cobertura ==========

TEST_F(MemoryBusTest, EmptyBusDefaultConstructor) {
    MemoryBus emptyBus;
    
    // Todas las lecturas deben retornar 0xFF
    for (uint16_t addr = 0; addr < 100; ++addr) {
        EXPECT_EQ(emptyBus.read(addr), 0xFF);
    }
}

TEST_F(MemoryBusTest, WriteToOpenBusThroughBus) {
    // Sin mapeos, write debe usar openMemory_m
    bus.write(0x1234, 0x99);
    EXPECT_EQ(bus.read(0x1234), 0xFF); // Debe seguir siendo 0xFF
}

TEST_F(MemoryBusTest, MultipleOverlappingLayers) {
    std::array<uint8_t, 128> ram1{}, ram2{}, ram3{};
    
    for (size_t i = 0; i < 128; ++i) {
        ram1[i] = 0x11;
        ram2[i] = 0x22;
        ram3[i] = 0x33;
    }
    
    RamRegion region1(ram1);
    RamRegion region2(ram2);
    RamRegion region3(ram3);
    
    // Tres capas superpuestas
    bus.map(0x0000, 0x007F, region1);
    bus.map(0x0000, 0x007F, region2);
    bus.map(0x0000, 0x007F, region3); // Primer mapeo gana
    
    EXPECT_EQ(bus.read(0x0000), 0x11);
    
    bus.write(0x0010, 0xAB);
    EXPECT_EQ(ram1[16], 0xAB);
    EXPECT_EQ(ram2[16], 0x22); // No modificado
    EXPECT_EQ(ram3[16], 0x33); // No modificado
}
