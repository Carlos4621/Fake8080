#ifndef FAKE_8080_HEADER
#define FAKE_8080_HEADER

#include <cstdint>
#include <fstream>
#include <vector>
#include <string_view>
#include "CPU.hpp"

class Fake8080 {
public:
    Fake8080(std::string_view romPath);

private:
    std::vector<uint8_t> loadedROM_m;
    CPU cpu_m;

    void loadRom(std::string_view path);
};

#endif // !FAKE_8080_HEADER