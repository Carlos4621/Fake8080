#ifndef IO_PORT_HEADER
#define IO_PORT_HEADER

#include <array>
#include <cstdint>

/// @brief Clase que representa un dispositivo I/O, debe ser padre de todo dispositivo que use I/O de la CPU
struct IOPort {
    virtual ~IOPort() noexcept = default;
    virtual uint8_t in() const noexcept = 0;
    virtual void out(uint8_t value) = 0;
};

/// @brief Bus I/O de la CPU con 256 direcciones
class IOBus {
public:
    IOBus() noexcept;

    /// @brief Establece un dispositivo a un puerto específico
    /// @param port Puerto en donde se establecerá el dispositivo
    /// @param device Dispositivo a establecer
    void attach(uint8_t port, IOPort& device);

    /// @brief Lee el valor devuelto por el dispositivo establecido. Si no hay, devuelve 0xFF
    /// @param port Puerto del dispositivo a leer
    /// @return Valor devuelto por el dispositivo o 0xFF si no está conectado
    [[nodiscard]]
    uint8_t in(uint8_t port) const noexcept;

    /// @brief Escribe un valor en el puerto del dispositivo establecido. Si no hay, no hace nada
    /// @param port Puerto a escribir
    /// @param value Valor a escribir
    void out(uint8_t port, uint8_t value) noexcept;

private:
    static constexpr uint8_t Disconnected_Port_Return_Value{ 0xFF };
    static constexpr uint16_t Port_Number{ 256 };

    std::array<IOPort*, Port_Number> ports_m;
};

#endif // !IO_PORT_HEADER