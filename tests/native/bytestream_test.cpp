#include <cstddef>
#include <cstdint>

#include "ESPressio_Platform_Arduino.hpp"

namespace {

class FakeStream {
public:
    int available() noexcept { return static_cast<int>(remaining_); }

    int read() noexcept {
        if (remaining_ == 0U) return -1;
        const auto index = sizeof(input_) - remaining_;
        --remaining_;
        return input_[index];
    }

    std::size_t write(const std::uint8_t* data, std::size_t size) noexcept {
        lastWrite_ = size == 0U ? 0U : data[0];
        return size;
    }

    std::uint8_t LastWrite() const noexcept { return lastWrite_; }

private:
    std::uint8_t input_[3]{0x10U, 0x20U, 0x30U};
    std::size_t remaining_{3U};
    std::uint8_t lastWrite_{0U};
};

struct ConsoleTag {};
using Adapter = ESPressio::Platform::Arduino::StreamByteStream<FakeStream, ConsoleTag>;

static_assert(ESPressio::Platform::ByteStream::IsStreamV<Adapter, ConsoleTag>);

} // namespace

int main() {
    FakeStream native;
    Adapter stream(native);

    std::size_t available = 0U;
    if (stream.Available(available) != ESPressio::Platform::IO::Result::Ok || available != 3U)
        return 1;

    std::uint8_t input[2]{};
    const auto read = stream.Read(ESPressio::Platform::IO::Bytes(input));
    if (read.Status != ESPressio::Platform::IO::Result::Ok || read.Count != 2U)
        return 2;
    if (input[0] != 0x10U || input[1] != 0x20U) return 3;

    const std::uint8_t output[2]{0xA5U, 0x5AU};
    const auto write = stream.Write(ESPressio::Platform::IO::Bytes(output));
    if (!write.Completed(sizeof(output))) return 4;
    return native.LastWrite() == 0xA5U ? 0 : 5;
}
