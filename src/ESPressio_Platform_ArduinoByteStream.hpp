#pragma once

#include <cstddef>
#include <cstdint>

#include "ESPressio_PlatformByteStream.hpp"
#include "ESPressio_Platform_Arduino.hpp"

namespace ESPressio::Platform::Arduino {

/**
 * Non-owning adapter from an Arduino Stream-like object to Platform ByteStream.
 *
 * TStream is intentionally structural rather than fixed to the Stream base
 * class so HardwareSerial, Client, File and custom Arduino stream types can be
 * adapted without adding inheritance requirements of our own.
 */
template <typename TStream,
          typename TStreamTag = ESPressio::Platform::ByteStream::DefaultStream>
class StreamByteStream final
    : public ESPressio::Platform::ByteStream::ProviderDeclaration<
          Backend,
          TStreamTag> {
public:
    explicit constexpr StreamByteStream(TStream& stream) noexcept : stream_(&stream) {}

    IO::Result Available(std::size_t& count) noexcept {
        const int available = stream_->available();
        if (available < 0) {
            count = 0U;
            return IO::Result::IoError;
        }
        count = static_cast<std::size_t>(available);
        return IO::Result::Ok;
    }

    IO::TransferResult Read(IO::MutableBuffer buffer) noexcept {
        if (!buffer.IsValid()) return {IO::Result::InvalidArgument, 0U};
        if (buffer.Empty()) return {IO::Result::Ok, 0U};

        const int available = stream_->available();
        if (available < 0) return {IO::Result::IoError, 0U};

        const auto requested = static_cast<std::size_t>(available) < buffer.Size
                                   ? static_cast<std::size_t>(available)
                                   : buffer.Size;
        std::size_t count = 0U;
        while (count < requested) {
            const int value = stream_->read();
            if (value < 0) break;
            buffer.Data[count++] = static_cast<std::uint8_t>(value);
        }
        return {IO::Result::Ok, count};
    }

    IO::TransferResult Write(IO::ConstBuffer buffer) noexcept {
        if (!buffer.IsValid()) return {IO::Result::InvalidArgument, 0U};
        if (buffer.Empty()) return {IO::Result::Ok, 0U};

        const auto count = static_cast<std::size_t>(stream_->write(buffer.Data, buffer.Size));
        if (count > buffer.Size) return {IO::Result::IoError, 0U};
        return {IO::Result::Ok, count};
    }

    TStream& Native() noexcept { return *stream_; }

private:
    TStream* stream_;
};

} // namespace ESPressio::Platform::Arduino
