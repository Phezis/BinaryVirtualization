#include "pch.h"
#include "Reverser.h"

using std::size_t;

#if _WIN32

#if defined(_M_X64) || defined(__amd64__)
size_t reverseBytes(const size_t val) {
	return static_cast<size_t>(_byteswap_uint64(val));
}
#else
size_t reverseBytes(const size_t val) {
	return static_cast<size_t>(_byteswap_ulong(val));
}
#endif

#else
#if defined(_M_X64) || defined(__amd64__)
size_t reverseBytes(const size_t val) {
	return static_cast<size_t>(__builtin_bswap64(val));
}
#else
size_t reverseBytes(const size_t val) {
	return static_cast<size_t>(__builtin_bswap32(val));
}
#endif

#endif


#if defined(_M_X64) || defined(__amd64__)
size_t reverseAll(size_t value) {
	value = (value & 0xFFFFFFFF00000000) >> 32 | (value & 0x00000000FFFFFFFF) << 32;
	value = (value & 0xFFFF0000FFFF0000) >> 16 | (value & 0x0000FFFF0000FFFF) << 16;
	value = (value & 0xFF00FF00FF00FF00) >> 8 | (value & 0x00FF00FF00FF00FF) << 8;
	value = (value & 0xF0F0F0F0F0F0F0F0) >> 4 | (value & 0x0F0F0F0F0F0F0F0F) << 4;
	value = (value & 0xCCCCCCCCCCCCCCCC) >> 2 | (value & 0x3333333333333333) << 2;
	return (value & 0xAAAAAAAAAAAAAAAA) >> 1 | (value & 0x5555555555555555) << 1;
}
size_t reverseBits(size_t value) {
	value = (value & 0xF0F0F0F0F0F0F0F0) >> 4 | (value & 0x0F0F0F0F0F0F0F0F) << 4;
	value = (value & 0xCCCCCCCCCCCCCCCC) >> 2 | (value & 0x3333333333333333) << 2;
	return (value & 0xAAAAAAAAAAAAAAAA) >> 1 | (value & 0x5555555555555555) << 1;

}
#else
size_t reverseAll(size_t value) {
	value = (value & 0xFFFF0000) >> 16 | (value & 0x0000FFFF) << 16;
	value = (value & 0xFF00FF00) >> 8 | (value & 0x00FF00FF) << 8;
	value = (value & 0xF0F0F0F0) >> 4 | (value & 0x0F0F0F0F) << 4;
	value = (value & 0xCCCCCCCC) >> 2 | (value & 0x33333333) << 2;
	return (value & 0xAAAAAAAA) >> 1 | (value & 0x55555555) << 1;
}
size_t reverseBits(size_t value) {
	value = (value & 0xF0F0F0F0) >> 4 | (value & 0x0F0F0F0F) << 4;
	value = (value & 0xCCCCCCCC) >> 2 | (value & 0x33333333) << 2;
	return (value & 0xAAAAAAAA) >> 1 | (value & 0x55555555) << 1;
}
#endif

uint8_t reverseBitsInByte(uint8_t byte) {
	byte = static_cast<uint8_t>((byte & 0xF0) >> 4) | static_cast<uint8_t>((byte & 0x0F) << 4);
	byte = static_cast<uint8_t>((byte & 0xCC) >> 2) | static_cast<uint8_t>((byte & 0x33) << 2);
	return static_cast<uint8_t>((byte & 0xAA) >> 1) | static_cast<uint8_t>((byte & 0x55) << 1);
}