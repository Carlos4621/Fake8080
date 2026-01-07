#ifndef REGISTERS_HEADER
#define REGISTERS_HEADER

#include <cstdint>
#include <array>
#include <utility>

/// @brief Clase que representa los registros de un Intel 8080
class Registers {
public:
    Registers();

    /// @brief Registros de 8 bits
    enum class Register : uint8_t { A = 0, B, C, D, E, H, L, F, SP_high, SP_low };

    /// @brief Registros de 16 bits
    enum class CombinedRegister : uint8_t { BC = 0, DE, HL, SP };

    /// @brief Flags
    enum class Flags : uint8_t { S = 0, Z, AC, P, CY };

    /// @brief Obtiene el registro
    /// @param reg Registro a obtener
    /// @return Valor del registro
    [[nodiscard]]
    uint8_t getRegister(Register reg) const noexcept;

    /// @brief Obtiene los registros combinado
    /// @param reg Registros a obtener
    /// @return Valor de los registros
    [[nodiscard]]
    uint16_t getCombinedRegister(CombinedRegister reg) const noexcept;

    /// @brief Establece un valor al registro dado
    /// @param reg Registro a establecer
    /// @param value Valor a establecer
    void setRegister(Register reg, uint8_t value) noexcept;

    /// @brief Establece un valor al par de registros
    /// @param reg Registros a establecer
    /// @param value Valor a establecer
    void setCombinedRegister(CombinedRegister reg, uint16_t value) noexcept;

    /// @brief Obtiene la flag dada
    /// @param flag Flag a obtener
    /// @return Estado del flag
    [[nodiscard]]
    bool getFlag(Flags flag) const noexcept;

    /// @brief Establece el flag al valor dado
    /// @param flag Flag a establecer
    /// @param value Valor a establecer
    void setFlag(Flags flag, bool value) noexcept;

private:
    static constexpr uint8_t Byte_Shift{ 8 };
    static constexpr uint8_t Register_Number{ 9 };

    static constexpr uint8_t Initial_Flags_Register_Value{ 2 };

    static constexpr std::array<uint8_t, 5> FlagsShifts{ 7, 6, 4, 2, 0 };

    static constexpr std::array RegisterPairs {
        std::pair{Register::B, Register::C},
        std::pair{Register::D, Register::E},
        std::pair{Register::H, Register::L},
        std::pair{Register::SP_high, Register::SP_low}
    };

    std::array<uint8_t, Register_Number> registers_m{};

    /// @brief Combina dos registros en uno de 16 bits
    /// @param high Byte alto
    /// @param low Byte bajo
    /// @return Valor de 16 bits combinado
    [[nodiscard]]
    uint16_t combineRegisters(Register high, Register low) const noexcept;

    /// @brief Obtiene el byte alto de un número de 16 bits
    /// @param value Valor de extracción
    /// @return Byte alto del valor
    [[nodiscard]]
    static constexpr uint8_t getHighByte(uint16_t value) noexcept;

    /// @brief Obtiene el byte bajo de un número de 16 buts
    /// @param value Valor de extracción
    /// @return Byte bajo del valor
    [[nodiscard]]
    static constexpr uint8_t getLowBytes(uint16_t value) noexcept;

    /// @brief Obtiene un bit de una posición en específico de un número
    /// @param value Valor de extracción
    /// @param position Bit n a extraer
    /// @return Valor del bit extraido
    [[nodiscard]]
    static constexpr bool getBit(uint8_t value, uint8_t position) noexcept;

    /// @brief Establece un bit de una posición en específico de un número
    /// @param value Valor de inserción
    /// @param position Posición del bit
    /// @param state Estado del bit a colocar
    /// @return Valor modificado
    [[nodiscard]]
    static constexpr uint8_t setBit(uint8_t value, uint8_t position, bool state) noexcept;
};

#endif // !REGISTERS_HEADER