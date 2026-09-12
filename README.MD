# ESPressio-Platform-Arduino

Arduino framework backend for `ESPressio-Platform`.

The repository is a clean-slate target backend. It does not conform to legacy ESPressio platform contracts. Existing ESPressio consumers will later be refactored to consume the new Platform contracts.

Arduino providers may be composed alongside providers from FreeRTOS, ESP-IDF, hardware-specific libraries, or application-defined provider origins in the same firmware. The boundary remains strict: Arduino providers use Arduino framework APIs; ESP-IDF-specific implementation detail belongs in `ESPressio-Platform-ESP-IDF`.

## Clock

`Arduino::MicrosClock` is backed only by the portable Arduino `micros()` API. It provides `Capability::Clock` and `Capability::MonotonicClock`, with a 1 MHz tick scale and counter width derived from the selected Arduino core's actual `micros()` return type.

The provider intentionally does **not** publish `ClockResolutionNanoseconds`: Arduino cores may update `micros()` at a coarser platform-specific quantum. It also does not claim `InterruptReadableClock`, because ISR safety is not a portable Arduino guarantee.

## I2C

`Arduino::I2CDevice<Tag, Address, Frequency>` is a bound 7-bit I2C endpoint over `TwoWire`. Address and requested bus frequency are compile-time properties. The provider supports write, read, and repeated-START write/read operations without leaking `Wire` semantics into the generic Platform contract.

The adapter deliberately exposes only behavior that is portable across Arduino cores. Core-specific buffer capacities and implementation limits remain runtime/backend facts rather than being invented as universal Arduino guarantees.

## SPI

`Arduino::SPIDevice<Tag, Frequency, Mode, BitOrder>` is a bound SPI endpoint over `SPIClass`. It establishes an `SPISettings` transaction for each transfer and implements the generic full-duplex Platform transfer contract. Chip-select ownership is intentionally outside the minimum SPI contract so an application may use a composed GPIO provider or a future backend-specific CS extension.

## GPIO

`Arduino::GPIOController<DomainTag>` implements the generic digital GPIO contract with portable `pinMode`, `digitalRead`, and `digitalWrite` APIs. The generic provider statically advertises only portable pull-up support.

Some Arduino cores expose additional macros such as `INPUT_PULLDOWN` or `OUTPUT_OPEN_DRAIN`; the implementation can use those when present, but it does not advertise them as compile-time Platform guarantees because they are not part of the portable Arduino contract. The generic Arduino GPIO provider likewise does not claim interrupt support or bidirectional I/O.

Multiple GPIO domains can coexist in one Platform composition; the default Arduino controller uses `GPIO::DefaultDomain` while external GPIO hardware can use its own domain tag.
