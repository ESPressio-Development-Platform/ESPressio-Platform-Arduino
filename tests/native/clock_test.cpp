#include <type_traits>

#include "ESPressio_Platform_Arduino.hpp"

static unsigned long gMicros = 1234UL;
unsigned long micros() noexcept { return gMicros; }

using Clock = ESPressio::Platform::Arduino::MicrosClock;

static_assert(ESPressio::Platform::Clock::IsClockSourceV<Clock>);
static_assert(ESPressio::Platform::Clock::FrequencyHz<Clock> == 1'000'000ULL);
static_assert(!ESPressio::Platform::Clock::HasResolution<Clock>);
static_assert(!ESPressio::Platform::Clock::IsInterruptReadableClockSourceV<Clock>);

int main() {
    Clock clock;
    return clock.Now() == 1234ULL ? 0 : 1;
}
