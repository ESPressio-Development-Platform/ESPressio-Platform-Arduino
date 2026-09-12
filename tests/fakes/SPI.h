#pragma once
#include <cstdint>
#define MSBFIRST 1
#define LSBFIRST 0
#define SPI_MODE0 0x00
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C
class SPISettings {
public:
    SPISettings(std::uint32_t, std::uint8_t, std::uint8_t) noexcept {}
};
class SPIClass {
public:
    void beginTransaction(const SPISettings&) noexcept {}
    std::uint8_t transfer(std::uint8_t value) noexcept { return value; }
    void endTransaction() noexcept {}
};
