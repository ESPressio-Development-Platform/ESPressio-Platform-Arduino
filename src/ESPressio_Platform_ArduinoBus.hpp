#pragma once

#include <cstddef>
#include <cstdint>

#include <SPI.h>
#include <Wire.h>

#include "ESPressio_PlatformBus.hpp"
#include "ESPressio_Platform_Arduino.hpp"

namespace ESPressio::Platform::Arduino {

namespace Detail {
inline IO::Result MapWireStatus(std::uint8_t status) noexcept {
    switch (status) {
        case 0U: return IO::Result::Ok;
        case 1U: return IO::Result::ResourceExhausted;
        case 2U: return IO::Result::NoDevice;
        case 5U: return IO::Result::Timeout;
        default: return IO::Result::IoError;
    }
}

constexpr std::uint8_t ArduinoSPIMode(SPI::Mode mode) noexcept {
    switch (mode) {
        case SPI::Mode::Mode0: return SPI_MODE0;
        case SPI::Mode::Mode1: return SPI_MODE1;
        case SPI::Mode::Mode2: return SPI_MODE2;
        case SPI::Mode::Mode3: return SPI_MODE3;
    }
    return SPI_MODE0;
}

constexpr std::uint8_t ArduinoBitOrder(SPI::BitOrder order) noexcept {
    return order == SPI::BitOrder::MostSignificantFirst ? MSBFIRST : LSBFIRST;
}
} // namespace Detail

template <typename TDeviceTag,
          std::uint16_t TAddress,
          std::uint32_t TFrequencyHz = 100'000U>
class I2CDevice final
    : public ESPressio::Platform::I2C::DeviceProviderDeclaration<
          Backend,
          TDeviceTag,
          TAddress,
          ESPressio::Platform::I2C::AddressWidth::Bits7,
          TFrequencyHz> {
public:
    explicit constexpr I2CDevice(TwoWire& wire) noexcept : wire_(wire) {}

    IO::Result Write(IO::ConstBuffer buffer) noexcept {
        if (!buffer.IsValid()) return IO::Result::InvalidArgument;
        wire_.setClock(TFrequencyHz);
        wire_.beginTransmission(static_cast<std::uint8_t>(TAddress));
        if (!buffer.Empty() && wire_.write(buffer.Data, buffer.Size) != buffer.Size) {
            (void)wire_.endTransmission(true);
            return IO::Result::ResourceExhausted;
        }
        return Detail::MapWireStatus(wire_.endTransmission(true));
    }

    IO::Result Read(IO::MutableBuffer buffer) noexcept {
        if (!buffer.IsValid()) return IO::Result::InvalidArgument;
        if (buffer.Size > 255U) return IO::Result::Unsupported;
        wire_.setClock(TFrequencyHz);
        const auto received = static_cast<std::size_t>(wire_.requestFrom(
            static_cast<std::uint8_t>(TAddress),
            static_cast<std::uint8_t>(buffer.Size),
            static_cast<std::uint8_t>(true)));
        if (received != buffer.Size)
            return received == 0U ? IO::Result::NoDevice : IO::Result::IoError;
        for (std::size_t i = 0; i < buffer.Size; ++i) {
            const int value = wire_.read();
            if (value < 0) return IO::Result::IoError;
            buffer.Data[i] = static_cast<std::uint8_t>(value);
        }
        return IO::Result::Ok;
    }

    IO::Result WriteRead(IO::ConstBuffer writeBuffer,
                         IO::MutableBuffer readBuffer) noexcept {
        if (!writeBuffer.IsValid() || !readBuffer.IsValid())
            return IO::Result::InvalidArgument;
        if (readBuffer.Size > 255U) return IO::Result::Unsupported;
        wire_.setClock(TFrequencyHz);
        wire_.beginTransmission(static_cast<std::uint8_t>(TAddress));
        if (!writeBuffer.Empty() &&
            wire_.write(writeBuffer.Data, writeBuffer.Size) != writeBuffer.Size) {
            (void)wire_.endTransmission(true);
            return IO::Result::ResourceExhausted;
        }
        auto result = Detail::MapWireStatus(wire_.endTransmission(false));
        if (result != IO::Result::Ok) return result;
        return Read(readBuffer);
    }

private:
    TwoWire& wire_;
};

template <typename TDeviceTag,
          std::uint32_t TFrequencyHz,
          ESPressio::Platform::SPI::Mode TMode = ESPressio::Platform::SPI::Mode::Mode0,
          ESPressio::Platform::SPI::BitOrder TBitOrder =
              ESPressio::Platform::SPI::BitOrder::MostSignificantFirst>
class SPIDevice final
    : public ESPressio::Platform::SPI::DeviceProviderDeclaration<
          Backend, TDeviceTag, TFrequencyHz, TMode, TBitOrder> {
public:
    explicit constexpr SPIDevice(SPIClass& spi) noexcept : spi_(spi) {}

    IO::Result Transfer(const std::uint8_t* transmit,
                        std::uint8_t* receive,
                        std::size_t size) noexcept {
        if (size != 0U && transmit == nullptr && receive == nullptr)
            return IO::Result::InvalidArgument;

        SPISettings settings(TFrequencyHz,
                             Detail::ArduinoBitOrder(TBitOrder),
                             Detail::ArduinoSPIMode(TMode));
        spi_.beginTransaction(settings);
        for (std::size_t i = 0; i < size; ++i) {
            const auto value = spi_.transfer(
                transmit != nullptr ? transmit[i] : std::uint8_t{0xFFU});
            if (receive != nullptr) receive[i] = value;
        }
        spi_.endTransaction();
        return IO::Result::Ok;
    }

private:
    SPIClass& spi_;
};

} // namespace ESPressio::Platform::Arduino
