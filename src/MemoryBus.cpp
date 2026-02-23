#include "MemoryBus.hpp"

OpenBusRegion MemoryBus::Open_Bus{};

MemoryBus::MemoryBus() noexcept
    : regions_m{}
{
}

void MemoryBus::map(uint16_t start, uint16_t end, IMemRegion& region) {
    if (end < start) {
        throw std::invalid_argument{"Direcciones incorrectas. end < start"};
    }

    const uint16_t rangeSize = end - start + 1;
    if (region.size() != rangeSize) {
        throw std::invalid_argument{"La región debe ser del mismo tamaño que el rango de direcciones"};
    }
    
    regions_m.emplace_back(start, end, region);
}

MemoryBus::RegionEntry MemoryBus::findRegion(uint16_t address) noexcept {
    for (const auto& i : regions_m) {
        if (address >= i.start && address <= i.end) {
            return i;
        }
    }
    
    return Open_Memory;
}

uint8_t MemoryBus::read(uint16_t address) {
    const auto entry{ findRegion(address) };

    const uint16_t offset = address - entry.start;
    return entry.region.read(offset);
}

void MemoryBus::write(uint16_t address, uint8_t value) {
    auto entry{ findRegion(address) };

    const uint16_t offset = address - entry.start;
    entry.region.write(offset, value);
}

RamRegion::RamRegion(std::span<uint8_t> memory) noexcept
    : memory_m{ memory }
{
}

uint8_t RamRegion::read(uint16_t offset) const {
    return memory_m[offset];
}

void RamRegion::write(uint16_t offset, uint8_t value) {
    memory_m[offset] = value;
}

uint16_t RamRegion::size() const noexcept {
    return memory_m.size();
}

RomRegion::RomRegion(std::span<const uint8_t> memory) noexcept
    : memory_m{ memory }
{
}

uint8_t RomRegion::read(uint16_t offset) const {
    return memory_m[offset];
}

void RomRegion::write(uint16_t offset, uint8_t value) {
    // No se escribe en ROM
    (void)offset;
    (void)value;
}

uint16_t RomRegion::size() const noexcept {
    return memory_m.size();
}

OpenBusRegion::OpenBusRegion(uint16_t size) noexcept
    : size_m{ size }
{
}

uint8_t OpenBusRegion::read(uint16_t offset) const {
    (void)offset;
    return 0xFF;
}

void OpenBusRegion::write(uint16_t offset, uint8_t value) {
    // No se escribe en desconectado
    (void)offset;
    (void)value;
}

uint16_t OpenBusRegion::size() const noexcept {
    return size_m;
}
