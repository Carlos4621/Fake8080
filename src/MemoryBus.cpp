#include "MemoryBus.hpp"

const OpenBusRegion MemoryBus::openMemory_m{};

MemoryBus::MemoryBus() noexcept
    : regions_m{}
{
}

void MemoryBus::map(uint16_t start, uint16_t end, IMemRegion& region, uint16_t regionOffset) {
    if (end < start) {
        throw std::invalid_argument("Direcciones incorrectas. end < start");
    }

    regions_m.emplace_back(start, end, &region, regionOffset);
}

MemoryBus::RegionEntry* MemoryBus::findRegion(uint16_t address) noexcept {
    for (auto it = regions_m.rbegin(); it != regions_m.rend(); ++it) {
        if (address >= it->start && address <= it->end) {
            return &(*it);
        }
    }
    
    return nullptr;
}

uint8_t MemoryBus::read(uint16_t address) {
    const auto* const entry{ findRegion(address) };
    if (entry == nullptr || entry->region == nullptr) {
        return openMemory_m.read(0);
    }

    const uint16_t offset = entry->base + (address - entry->start);
    return entry->region->read(offset);
}

void MemoryBus::write(uint16_t address, uint8_t value) {
    const auto* const entry{ findRegion(address) };
    if (entry == nullptr || entry->region == nullptr) {
        return;
    }

    const uint16_t offset = entry->base + (address - entry->start);
    entry->region->write(offset, value);
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

uint8_t OpenBusRegion::read(uint16_t offset) const {
    (void)offset;
    return 0xFF;
}

void OpenBusRegion::write(uint16_t offset, uint8_t value) {
    // No se escribe en desconectado
    (void)offset;
    (void)value;
}
