# ESPressio-Platform-Arduino

Arduino framework backend for `ESPressio-Platform`.

The repository is being built as a clean-slate target backend. It does not conform to legacy ESPressio platform contracts. Existing ESPressio consumers will later be refactored to consume the new Platform contracts.

The initial capability groundwork defines only the distinct `ESPressio::Platform::Arduino::Backend` identity and the `Arduino::ProviderDeclaration` convenience alias. It deliberately does **not** claim any service capability until a corresponding Arduino concrete provider is implemented and validated.

Arduino providers may be composed alongside providers from FreeRTOS and ESP-IDF in the same firmware. The backend boundary must therefore remain strict: Arduino providers use Arduino framework APIs; ESP-IDF-specific implementation detail belongs in `ESPressio-Platform-ESP-IDF` instead of leaking into this repository.
