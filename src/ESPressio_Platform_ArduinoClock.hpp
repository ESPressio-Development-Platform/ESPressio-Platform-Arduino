#pragma once

#include <Arduino.h>

#include <limits>
#include <type_traits>

#include "ESPressio_PlatformClock.hpp"
#include "ESPressio_Platform_Arduino.hpp"

namespace ESPressio::Platform::Arduino {

/**
 * Monotonic clock backed exclusively by the portable Arduino micros() API.
 *
 * The tick scale is always one microsecond. The effective resolution is
 * deliberately not advertised: Arduino cores are permitted to implement
 * micros() with a coarser update quantum than one microsecond.
 *
 * The native counter width follows the actual return type of micros() for the
 * selected Arduino core. Wrap is therefore explicit and handled by
 * Platform::Clock::Elapsed().
 */
class MicrosClock final
    : public Clock::MonotonicProviderDeclaration<
          Backend,
          1'000'000ULL,
          std::numeric_limits<decltype(::micros())>::digits> {
public:
    using NativeTick = decltype(::micros());

    static_assert(std::is_integral_v<NativeTick> &&
                      std::is_unsigned_v<NativeTick>,
                  "Arduino micros() must return an unsigned integral type");

    Clock::Tick Now() const noexcept {
        return static_cast<Clock::Tick>(::micros());
    }
};

static_assert(Clock::IsClockSourceV<MicrosClock>,
              "Arduino MicrosClock must satisfy the Platform Clock contract");

} // namespace ESPressio::Platform::Arduino
