# ESPressio-Platform-Arduino

Arduino framework backend for `ESPressio-Platform`.

The repository is a clean-slate target backend. It does not conform to legacy ESPressio platform contracts. Existing ESPressio consumers will later be refactored to consume the new Platform contracts.

Arduino providers may be composed alongside providers from FreeRTOS, ESP-IDF, hardware-specific libraries, or application-defined provider origins in the same firmware. The boundary remains strict: Arduino providers use Arduino framework APIs; ESP-IDF-specific implementation detail belongs in `ESPressio-Platform-ESP-IDF`.

## Clock

`Arduino::MicrosClock` is the first concrete provider.

It uses only the portable Arduino `micros()` API and provides:

- `Capability::Clock`
- `Capability::MonotonicClock`
- `ClockFrequencyHz = 1,000,000`
- `ClockCounterWidthBits` derived from the selected Arduino core's actual `micros()` return type

The provider intentionally does **not** publish `ClockResolutionNanoseconds`. Although `micros()` reports values in microsecond units, Arduino cores may update that value at a coarser platform-specific quantum. Omitting the property prevents a consumer from accidentally treating unit scale as a guaranteed effective resolution.

It also does not claim `InterruptReadableClock`, because ISR safety is not a portable guarantee of the Arduino API contract across all supported cores.

Counter wrap is explicit and is handled by `ESPressio::Platform::Clock::Elapsed()`.
