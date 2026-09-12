#pragma once
#include <cstddef>
#include <cstdint>
class TwoWire {
public:
    void setClock(std::uint32_t) noexcept {}
    void beginTransmission(std::uint8_t) noexcept {}
    std::size_t write(const std::uint8_t*, std::size_t n) noexcept { return n; }
    std::uint8_t endTransmission(bool = true) noexcept { return 0; }
    std::uint8_t requestFrom(std::uint8_t, std::uint8_t n, std::uint8_t) noexcept {
        remaining_ = n;
        return n;
    }
    int read() noexcept {
        if (remaining_ == 0U) return -1;
        --remaining_;
        return 0x55;
    }
private:
    std::uint8_t remaining_{0U};
};
