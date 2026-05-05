# Fake8080

Emulador de la CPU Intel 8080 escrito en C++23, distribuido como biblioteca estática.

## Requisitos

- CMake ≥ 3.10
- Compilador con soporte C++23
- Google Test (se descarga automáticamente si no está instalado)

## Build

```bash
cmake -B build
cmake --build build
```

Para ejecutar los tests:

```bash
cd build && ctest
```

## Uso

La biblioteca expone tres componentes principales:

| Clase | Responsabilidad |
|---|---|
| `MemoryBus` | Mapa de memoria de 64 KB con regiones ROM/RAM |
| `IOBus` | Bus I/O con 256 puertos |
| `Fake8080` | CPU que consume ambos buses |

### API pública de `Fake8080`

```cpp
Fake8080(MemoryBus& memoryBus, IOBus& ioBus);

void cycle();                        // Ejecuta un ciclo de instrucción
void requestInterrupt(uint8_t n);   // Solicita interrupción RST n (0–7)
void reset();                        // Reinicia la CPU al estado inicial
```

### Ejemplo

El siguiente programa carga un valor en el registro B mediante `MVI B, 0x42` y lo ejecuta:

```cpp
#include "Fake8080.hpp"
#include "MemoryBus.hpp"
#include "IOBus.hpp"
#include <array>
#include <cstdint>

int main() {
    // Programa: MVI B, 0x42 ; HLT
    std::array<uint8_t, 3> rom_data = { 0x06, 0x42, 0x76 };
    RomRegion rom(rom_data);

    // 62 KB de RAM desde 0x0003 hasta 0xFFFF
    std::array<uint8_t, 0xFFFD> ram_data{};
    RamRegion ram(ram_data);

    MemoryBus mem;
    mem.map(0x0000, 0x0002, rom);
    mem.map(0x0003, 0xFFFF, ram);

    IOBus io;

    Fake8080 cpu(mem, io);

    cpu.cycle(); // MVI B, 0x42 → B = 0x42
    cpu.cycle(); // HLT         → CPU detenida hasta interrupción

    return 0;
}
```

Para compilar el ejemplo, activa la opción `FAKE8080_BUILD_EXAMPLE` y proporciona un `main.cpp`:

```bash
cmake -B build -DFAKE8080_BUILD_EXAMPLE=ON
cmake --build build --target fake8080_example
```

### Dispositivos I/O personalizados

Implementa `IOPort` para conectar periféricos:

```cpp
struct MyPort : IOPort {
    uint8_t in() const noexcept override { return 0xAB; }
    void out(uint8_t value) noexcept override { /* manejar escritura */ }
};

MyPort device;
io.attach(0x01, device); // conecta en el puerto 1
```
