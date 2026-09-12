#pragma once

#include <Arduino.h>

#include "ESPressio_PlatformGPIO.hpp"
#include "ESPressio_Platform_Arduino.hpp"

namespace ESPressio::Platform::Arduino {

template <typename TDomainTag = ESPressio::Platform::GPIO::DefaultDomain>
class GPIOController final
    : public ESPressio::Platform::GPIO::ProviderDeclaration<
          Backend,
          TDomainTag,
          ESPressio::Platform::CapabilitySet<
              ESPressio::Platform::Capability::GPIOPullUp<TDomainTag>>> {
public:
    IO::Result Configure(GPIO::Pin pin, const GPIO::Configuration& configuration) noexcept {
        const auto nativePin = static_cast<unsigned int>(pin);

        if (configuration.DirectionMode == GPIO::Direction::InputOutput)
            return IO::Result::Unsupported;

        if (configuration.DirectionMode == GPIO::Direction::Input) {
            if (configuration.Output == GPIO::OutputMode::OpenDrain)
                return IO::Result::Unsupported;
            switch (configuration.PullMode) {
                case GPIO::Pull::None:
                    pinMode(nativePin, INPUT);
                    return IO::Result::Ok;
                case GPIO::Pull::Up:
                    pinMode(nativePin, INPUT_PULLUP);
                    return IO::Result::Ok;
                case GPIO::Pull::Down:
#ifdef INPUT_PULLDOWN
                    pinMode(nativePin, INPUT_PULLDOWN);
                    return IO::Result::Ok;
#else
                    return IO::Result::Unsupported;
#endif
                case GPIO::Pull::UpDown:
                    return IO::Result::Unsupported;
            }
        }

        if (configuration.PullMode != GPIO::Pull::None)
            return IO::Result::Unsupported;

        if (configuration.ApplyInitialLevel)
            digitalWrite(nativePin, configuration.InitialLevel == GPIO::Level::High ? HIGH : LOW);

        if (configuration.Output == GPIO::OutputMode::OpenDrain) {
#ifdef OUTPUT_OPEN_DRAIN
            pinMode(nativePin, OUTPUT_OPEN_DRAIN);
            return IO::Result::Ok;
#else
            return IO::Result::Unsupported;
#endif
        }

        pinMode(nativePin, OUTPUT);
        return IO::Result::Ok;
    }

    IO::Result Read(GPIO::Pin pin, GPIO::Level& level) noexcept {
        level = digitalRead(static_cast<unsigned int>(pin)) == LOW
                    ? GPIO::Level::Low
                    : GPIO::Level::High;
        return IO::Result::Ok;
    }

    IO::Result Write(GPIO::Pin pin, GPIO::Level level) noexcept {
        digitalWrite(static_cast<unsigned int>(pin),
                     level == GPIO::Level::High ? HIGH : LOW);
        return IO::Result::Ok;
    }
};

} // namespace ESPressio::Platform::Arduino
