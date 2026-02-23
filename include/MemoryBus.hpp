#ifndef MEMORY_BUS_HEADER
#define MEMORY_BUS_HEADER

#include <cstdint>
#include <span>
#include <stdexcept>
#include <vector>

/// @brief Clase padre de regiones de memoria
class IMemRegion {
public:
    virtual ~IMemRegion() = default;
    virtual uint8_t read(uint16_t offset) const = 0;
    virtual void write(uint16_t offset, uint8_t value) = 0;
    virtual uint16_t size() const noexcept = 0;
};

/// @brief Región de memoria desconectado
class OpenBusRegion : public IMemRegion {
public:

    /// @brief Constructor base
    /// @param size Tamaño del bus abierto. Por defecto 1
    explicit OpenBusRegion(uint16_t size = 0) noexcept;

    /// @brief "Intenta" leer un byte de la memoria
    /// @param offset Ignorado
    /// @return Siempre 0xFF
    [[nodiscard]]
    uint8_t read(uint16_t offset) const override;

    /// @brief "Intenta" escribir un byte en la memoria
    /// @param offset Ignorado
    /// @param value Ignorado
    void write(uint16_t offset, uint8_t value) override;

    /// @brief Devuelve el tamaño de la región
    /// @return Tamaño de la región
    uint16_t size() const noexcept override;

private:

    uint16_t size_m;
};

/// @brief Región de memoria modificable (RAM)
class RamRegion : public IMemRegion {
public:
    /// @brief Constructor base
    /// @param memory memoria designada
    explicit RamRegion(std::span<uint8_t> memory) noexcept;

    /// @brief Lee un byte de la memoria
    /// @param offset Dirección con offset a leer
    /// @return Byte leído
    [[nodiscard]]
    uint8_t read(uint16_t offset) const override;

    /// @brief Escribe un byte en la memoria
    /// @param offset Dirección con offset a escribir
    /// @param value Valor a escribir
    void write(uint16_t offset, uint8_t value) override;

    /// @brief Devuelve el tamaño de la región
    /// @return Tamaño de la región
    uint16_t size() const noexcept override;

private:
    std::span<uint8_t> memory_m;
};

/// @brief Región de memoria constante (ROM)
class RomRegion : public IMemRegion {
public:
    /// @brief Constructor base
    /// @param memory Memoria designada
    explicit RomRegion(std::span<const uint8_t> memory) noexcept;

    /// @brief Lee un byte en la memoria
    /// @param offset Dirección con offset de la memoria a leer
    /// @return Byte leído
    [[nodiscard]]
    uint8_t read(uint16_t offset) const override;

    /// @brief Intenta escribir en la memoria, no es posible en ROM
    /// @param offset Ignorado
    /// @param value Ignorado
    void write(uint16_t offset, uint8_t value) override;

    /// @brief Devuelve el tamaño de la región
    /// @return Tamaño de la región
    uint16_t size() const noexcept override;

private:
    std::span<const uint8_t> memory_m;
};

/// @brief Bus de memoria que debe ser pre-ensamblado para su uso con el CPU
class MemoryBus {
public:
    MemoryBus() noexcept;

    /// @brief Mapea un rango de direcciones [start, end] a una región
    /// @param start Dirección del inicio de la región
    /// @param end Dirección del final de la región
    /// @param region Región a colocar
    void map(uint16_t start, uint16_t end, IMemRegion& region);

    /// @brief Lee un byte de la región redireccionada de la memoria
    /// @param address Dirección a leer
    /// @return Byte leído de la región correspondiente
    [[nodiscard]]
    uint8_t read(uint16_t address) const;

    /// @brief Escribe un byte en la región redireccionada de la memoria
    /// @param address Dirección a leer
    /// @param value Valor a escribir
    void write(uint16_t address, uint8_t value);

private:
    struct RegionEntry {
        uint16_t start{};
        uint16_t end{};
        IMemRegion& region;
    };

    static OpenBusRegion Open_Bus;
    static constexpr RegionEntry Open_Memory{0, 0, Open_Bus};

    std::vector<RegionEntry> regions_m{};

    [[nodiscard]]
    RegionEntry findRegion(uint16_t address) const noexcept;
};

#endif // !MEMORY_BUS_HEADER
