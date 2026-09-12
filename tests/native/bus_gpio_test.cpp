#include "ESPressio_Platform_Arduino.hpp"

using namespace ESPressio::Platform;

struct RTCDeviceTag {};
struct SensorTag {};

using I2CDev = Arduino::I2CDevice<RTCDeviceTag, 0x68U, 400'000U>;
using SPIDev = Arduino::SPIDevice<SensorTag,
                                  8'000'000U,
                                  SPI::Mode::Mode3,
                                  SPI::BitOrder::MostSignificantFirst>;
using Gpio = Arduino::GPIOController<>;

static_assert(I2C::IsDeviceV<I2CDev, RTCDeviceTag>);
static_assert(SPI::IsDeviceV<SPIDev, SensorTag>);
static_assert(GPIO::IsControllerV<Gpio>);
static_assert(!GPIO::IsInterruptControllerV<Gpio>);

int main() {
    TwoWire wire;
    I2CDev i2c(wire);
    std::uint8_t writeData[2]{1U, 2U};
    std::uint8_t readData[2]{};
    if (i2c.WriteRead(IO::Bytes(writeData), IO::Bytes(readData)) != IO::Result::Ok)
        return 1;

    SPIClass spi;
    SPIDev spiDevice(spi);
    if (spiDevice.Transfer(writeData, readData, 2U) != IO::Result::Ok)
        return 2;

    Gpio gpio;
    GPIO::Configuration input{};
    input.DirectionMode = GPIO::Direction::Input;
    input.PullMode = GPIO::Pull::Up;
    if (gpio.Configure(1U, input) != IO::Result::Ok)
        return 3;

    return 0;
}
