#pragma once
#include <cstddef>
#include <cstdint>

using reverse_bytes_t = bool;
using reverse_bits_t = bool;

constexpr size_t BITS_IN_BYTE = 8;
constexpr reverse_bytes_t REVERSE_BYTES = true;
constexpr reverse_bits_t REVERSE_BITS = true;

template<typename T>
constexpr T multiplyBy8(T value)
{
	return value << 3;
}

template<typename T>
constexpr T divideBy8(T value)
{
	return value >> 3;
}

uint8_t reverseBitsInByte(uint8_t byte);
std::size_t reverseBytes(std::size_t val);
std::size_t reverseBits(std::size_t value);
std::size_t reverseAll(std::size_t value);