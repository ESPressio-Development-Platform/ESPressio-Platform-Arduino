#pragma once
#include <cstdint>
#define LOW 0
#define HIGH 1
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
unsigned long micros() noexcept;
inline void pinMode(unsigned int, int) noexcept {}
inline void digitalWrite(unsigned int, int) noexcept {}
inline int digitalRead(unsigned int) noexcept { return LOW; }
