#pragma once

#include "ESPressio_Platform.hpp"

namespace ESPressio::Platform::Arduino {

/** Compile-time identity of the Arduino framework backend. */
struct Backend final : ESPressio::Platform::Backend {};

/**
 * Convenience alias for declaring Arduino-backed providers without repeating
 * the backend identity. Concrete providers still choose their own capability
 * and requirement sets explicitly.
 */
template <typename TCapabilities,
          typename TRequirements = ESPressio::Platform::RequirementSet<>>
using ProviderDeclaration = ESPressio::Platform::ProviderDeclaration<
    Backend,
    TCapabilities,
    TRequirements>;

} // namespace ESPressio::Platform::Arduino

#include "ESPressio_Platform_ArduinoClock.hpp"
#include "ESPressio_Platform_ArduinoBus.hpp"
#include "ESPressio_Platform_ArduinoGPIO.hpp"
