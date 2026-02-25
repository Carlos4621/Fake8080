#include <gtest/gtest.h>
#include "IOBus.hpp"
#include <array>

// Mock IOPort para testing
class MockIOPort : public IOPort {
private:
    uint8_t value_m{ 0 };
    mutable uint8_t lastReadValue_m{ 0 };
    mutable size_t readCount_m{ 0 };
    size_t writeCount_m{ 0 };

public:
    MockIOPort() = default;
    explicit MockIOPort(uint8_t initialValue) : value_m(initialValue) {}

    uint8_t in() const noexcept override {
        readCount_m++;
        lastReadValue_m = value_m;
        return value_m;
    }

    void out(uint8_t value) override {
        writeCount_m++;
        value_m = value;
    }

    // Métodos auxiliares para testing
    [[nodiscard]] uint8_t getValue() const { return value_m; }
    [[nodiscard]] size_t getReadCount() const { return readCount_m; }
    [[nodiscard]] size_t getWriteCount() const { return writeCount_m; }
    void setValue(uint8_t value) { value_m = value; }
    void resetCounters() {
        readCount_m = 0;
        writeCount_m = 0;
    }
};

class IOBusTest : public ::testing::Test {
protected:
    IOBus bus;
    MockIOPort device1;
    MockIOPort device2;
    MockIOPort device3;

    void SetUp() override {
        device1.setValue(0xAA);
        device2.setValue(0xBB);
        device3.setValue(0xCC);
    }
};

// ========== Tests de Construcción ==========

TEST_F(IOBusTest, ConstructorInitializesAllPortsAsDisconnected) {
    // Leer de puertos no conectados debe devolver 0xFF
    EXPECT_EQ(bus.in(0), 0xFF);
    EXPECT_EQ(bus.in(127), 0xFF);
    EXPECT_EQ(bus.in(255), 0xFF);
}

TEST_F(IOBusTest, DisconnectedPortsReturnDefaultValue) {
    // Verificar varios puertos desconectados
    for (uint16_t port = 0; port < 256; port += 17) {
        EXPECT_EQ(bus.in(static_cast<uint8_t>(port)), 0xFF)
            << "Puerto " << port << " debería devolver 0xFF cuando está desconectado";
    }
}

// ========== Tests de attach() ==========

TEST_F(IOBusTest, AttachDeviceToPort) {
    bus.attach(10, device1);
    
    EXPECT_EQ(bus.in(10), 0xAA);
}

TEST_F(IOBusTest, AttachMultipleDevicesToDifferentPorts) {
    bus.attach(0, device1);
    bus.attach(128, device2);
    bus.attach(255, device3);
    
    EXPECT_EQ(bus.in(0), 0xAA);
    EXPECT_EQ(bus.in(128), 0xBB);
    EXPECT_EQ(bus.in(255), 0xCC);
}

TEST_F(IOBusTest, AttachDeviceOverwritesPreviousDevice) {
    MockIOPort oldDevice(0x11);
    MockIOPort newDevice(0x22);
    
    bus.attach(50, oldDevice);
    EXPECT_EQ(bus.in(50), 0x11);
    
    bus.attach(50, newDevice);
    EXPECT_EQ(bus.in(50), 0x22);
}

// ========== Tests de in() ==========

TEST_F(IOBusTest, InReadsFromConnectedDevice) {
    bus.attach(42, device1);
    
    uint8_t result = bus.in(42);
    
    EXPECT_EQ(result, 0xAA);
    EXPECT_EQ(device1.getReadCount(), 1);
}

TEST_F(IOBusTest, InFromDisconnectedPortReturns0xFF) {
    // Puerto nunca conectado
    EXPECT_EQ(bus.in(100), 0xFF);
    
    // Puerto previamente conectado pero ahora apunta a nullptr
    bus.attach(101, device1);
    // Simulamos desconexión (en práctica esto no debería hacerse, 
    // pero verificamos el comportamiento defensivo)
    EXPECT_EQ(bus.in(101), 0xAA);
}

TEST_F(IOBusTest, InDoesNotAffectOtherPorts) {
    bus.attach(10, device1);
    bus.attach(20, device2);
    
    [[maybe_unused]] auto result = bus.in(10);
    
    EXPECT_EQ(device1.getReadCount(), 1);
    EXPECT_EQ(device2.getReadCount(), 0);
}

TEST_F(IOBusTest, MultipleInCallsIncrementReadCount) {
    bus.attach(5, device1);
    
    for (size_t i = 0; i < 5; ++i) {
        [[maybe_unused]] auto result = bus.in(5);
    }
    
    EXPECT_EQ(device1.getReadCount(), 5);
}

TEST_F(IOBusTest, InReturnsCurrentValueOfDevice) {
    bus.attach(30, device1);
    
    device1.setValue(0x10);
    EXPECT_EQ(bus.in(30), 0x10);
    
    device1.setValue(0x20);
    EXPECT_EQ(bus.in(30), 0x20);
    
    device1.setValue(0xFF);
    EXPECT_EQ(bus.in(30), 0xFF);
}

// ========== Tests de out() ==========

TEST_F(IOBusTest, OutWritesToConnectedDevice) {
    bus.attach(15, device1);
    
    bus.out(15, 0x55);
    
    EXPECT_EQ(device1.getValue(), 0x55);
    EXPECT_EQ(device1.getWriteCount(), 1);
}

TEST_F(IOBusTest, OutToDisconnectedPortDoesNothing) {
    // Escribir a puerto desconectado no debe causar crash
    bus.out(99, 0x42);
    
    // Verificar que el puerto sigue desconectado
    EXPECT_EQ(bus.in(99), 0xFF);
}

TEST_F(IOBusTest, OutDoesNotAffectOtherPorts) {
    bus.attach(10, device1);
    bus.attach(20, device2);
    
    bus.out(10, 0x77);
    
    EXPECT_EQ(device1.getValue(), 0x77);
    EXPECT_EQ(device1.getWriteCount(), 1);
    EXPECT_EQ(device2.getWriteCount(), 0);
    EXPECT_EQ(device2.getValue(), 0xBB); // Valor inicial no modificado
}

TEST_F(IOBusTest, MultipleOutCallsUpdateDevice) {
    bus.attach(8, device1);
    
    bus.out(8, 0x11);
    EXPECT_EQ(device1.getValue(), 0x11);
    
    bus.out(8, 0x22);
    EXPECT_EQ(device1.getValue(), 0x22);
    
    bus.out(8, 0x33);
    EXPECT_EQ(device1.getValue(), 0x33);
    
    EXPECT_EQ(device1.getWriteCount(), 3);
}

// ========== Tests de Integración ==========

TEST_F(IOBusTest, ReadWriteCycleOnSamePort) {
    bus.attach(50, device1);
    
    // Leer valor inicial
    EXPECT_EQ(bus.in(50), 0xAA);
    
    // Escribir nuevo valor
    bus.out(50, 0x77);
    
    // Leer valor actualizado
    EXPECT_EQ(bus.in(50), 0x77);
}

TEST_F(IOBusTest, MultipleDevicesOperateIndependently) {
    bus.attach(10, device1);
    bus.attach(20, device2);
    bus.attach(30, device3);
    
    // Escribir a cada dispositivo
    bus.out(10, 0x11);
    bus.out(20, 0x22);
    bus.out(30, 0x33);
    
    // Verificar que cada dispositivo tiene su valor correcto
    EXPECT_EQ(bus.in(10), 0x11);
    EXPECT_EQ(bus.in(20), 0x22);
    EXPECT_EQ(bus.in(30), 0x33);
    
    // Verificar contadores
    EXPECT_EQ(device1.getWriteCount(), 1);
    EXPECT_EQ(device2.getWriteCount(), 1);
    EXPECT_EQ(device3.getWriteCount(), 1);
}

TEST_F(IOBusTest, AllPortsCanBeUsed) {
    std::array<MockIOPort, 256> devices;
    
    // Conectar dispositivos a todos los puertos
    for (uint16_t port = 0; port < 256; ++port) {
        devices[port].setValue(static_cast<uint8_t>(port));
        bus.attach(static_cast<uint8_t>(port), devices[port]);
    }
    
    // Verificar todos los puertos
    for (uint16_t port = 0; port < 256; ++port) {
        EXPECT_EQ(bus.in(static_cast<uint8_t>(port)), static_cast<uint8_t>(port))
            << "Puerto " << port << " debe devolver " << port;
    }
}

TEST_F(IOBusTest, MixedConnectedAndDisconnectedPorts) {
    // Conectar solo puertos pares
    for (uint16_t port = 0; port < 256; port += 2) {
        device1.setValue(static_cast<uint8_t>(port));
        bus.attach(static_cast<uint8_t>(port), device1);
    }
    
    // Verificar puertos pares conectados y impares desconectados
    for (uint16_t port = 0; port < 256; ++port) {
        if (port % 2 == 0) {
            EXPECT_NE(bus.in(static_cast<uint8_t>(port)), 0xFF)
                << "Puerto par " << port << " debería estar conectado";
        } else {
            EXPECT_EQ(bus.in(static_cast<uint8_t>(port)), 0xFF)
                << "Puerto impar " << port << " debería estar desconectado";
        }
    }
}

TEST_F(IOBusTest, DeviceStatePersiststBetweenOperations) {
    bus.attach(100, device1);
    
    // Escribir un valor
    bus.out(100, 0x42);
    
    // Leer múltiples veces para verificar persistencia
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(bus.in(100), 0x42)
            << "Lectura " << i << " debería devolver el valor escrito";
    }
}

// ========== Tests de Casos Límite ==========

TEST_F(IOBusTest, Port0Works) {
    bus.attach(0, device1);
    bus.out(0, 0x00);
    EXPECT_EQ(bus.in(0), 0x00);
}

TEST_F(IOBusTest, Port255Works) {
    bus.attach(255, device1);
    bus.out(255, 0xFF);
    EXPECT_EQ(bus.in(255), 0xFF);
}

TEST_F(IOBusTest, WriteAndReadMaxValues) {
    bus.attach(50, device1);
    
    bus.out(50, 0xFF);
    EXPECT_EQ(bus.in(50), 0xFF);
    
    bus.out(50, 0x00);
    EXPECT_EQ(bus.in(50), 0x00);
}
