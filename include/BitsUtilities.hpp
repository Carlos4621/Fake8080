#ifndef BITS_UTILITIES_HEADER
#define BITS_UTILITIES_HEADER

#include <cstdint>

/// @brief Obtiene el byte alto de un número de 16 bits
/// @param value Valor de extracción
/// @return Byte alto del valor
[[nodiscard]]
static constexpr uint8_t getHighByte(uint16_t value) noexcept {
    return static_cast<uint8_t>(value >> 8);
}

/// @brief Obtiene el byte bajo de un número de 16 bits
/// @param value Valor de extracción
/// @return Byte bajo del valor
[[nodiscard]]
static constexpr uint8_t getLowBytes(uint16_t value) noexcept {
    return static_cast<uint8_t>(value);
}

/// @brief Obtiene un bit de una posición en específico de un número
/// @param value Valor de extracción
/// @param position Bit n a extraer
/// @return Valor del bit extraido
[[nodiscard]]
static constexpr bool getBit(uint8_t value, uint8_t position) noexcept {
    return (value >> position) & 1;
}

/// @brief Establece un bit de una posición en específico de un número
/// @param value Valor de inserción
/// @param position Posición del bit
/// @param state Estado del bit a colocar
/// @return Valor modificado
[[nodiscard]]
static constexpr uint8_t setBit(uint8_t value, uint8_t position, bool state) noexcept {
    return state ? (value | (1 << position)) : (value & ~(1 << position));
}

#endif // !BITS_UTILITIES_HEADER