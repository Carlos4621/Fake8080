#include "Fake8080.hpp"

Fake8080::Fake8080(std::string_view romPath) {
    loadRom(romPath);
    cpu_m.setROM(loadedROM_m);
}

void Fake8080::loadRom(std::string_view path) {
    std::ifstream romFile{ path.data(), std::ios::binary | std::ios::ate };

    if (!romFile) {
        throw std::runtime_error{ "Cant open ROM file" };
    }

    const auto fileSize{ romFile.tellg() };
    loadedROM_m.reserve(fileSize);

    romFile.seekg(std::ios::beg);
    for (size_t i{ 0 }; i < fileSize; ++i) {
        loadedROM_m.emplace_back(romFile.get());
    }
}
