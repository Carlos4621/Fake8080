#include "IOBus.hpp"

IOBus::IOBus() noexcept {
    ports_m.fill(nullptr);
}

void IOBus::attach(uint8_t port, IOPort &device) {
    ports_m[port] = &device;
}

uint8_t IOBus::in(uint8_t port) const noexcept {
    if (!ports_m[port]) {
        return Disconnected_Port_Return_Value;
    }
    
    return ports_m[port]->in();
}

void IOBus::out(uint8_t port, uint8_t value) noexcept {
    if (ports_m[port]) {
        ports_m[port]->out(value);
    }
}
