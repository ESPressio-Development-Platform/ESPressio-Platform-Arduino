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

## ByteStream

`Arduino::StreamByteStream<TStream, Tag>` is a non-owning adapter for Arduino Stream-like objects. It uses only the portable `available()`, `read()`, and `write(buffer, size)` surface and therefore works with `HardwareSerial`, network clients, files, or custom Stream-compatible types without imposing another inheritance hierarchy.

Reads consume only bytes reported as immediately available and return an explicit transferred-byte count. Writes likewise report the count accepted by the underlying Arduino stream. The adapter does not claim non-blocking write behavior because that is not a portable guarantee of every Arduino Stream implementation.

## Entropy

The generic Arduino backend intentionally does not provide `Capability::Entropy`. Arduino's portable `random()` API is a pseudo-random generator and does not constitute a portable hardware-entropy guarantee. A board-specific or hardware-specific library may provide the Entropy contract independently in the same composition.

## Synchronization and Queue

The generic Arduino framework backend intentionally does not provide `Capability::Synchronization` or `Capability::Queue`. The portable Arduino API does not define deterministic mutex, signal, or bounded queue primitives with static resource guarantees.

An Arduino-based firmware may compose the generic FreeRTOS Synchronization/Queue providers when its selected platform actually supplies FreeRTOS, or choose another backend/application-defined provider. This repository does not reach through Arduino into an underlying RTOS SDK because doing so would violate the backend boundary and make capability declarations board-core dependent.
