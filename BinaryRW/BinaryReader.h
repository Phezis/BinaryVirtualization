#pragma once

#include "BitMask.h"
#include "Reverser.h"
#include "VirtualPointer.h"

#include <cstddef>
#include <cstdint>
#include <cassert>
#include <algorithm>


template <class T>
class BinaryReader {
public:
	explicit BinaryReader();
	explicit BinaryReader(reverse_bytes_t reverseBytes);
	explicit BinaryReader(reverse_bytes_t reverseBytes, reverse_bits_t reverseBits);

	void setReverseBits(reverse_bits_t val);
	void setReverseBytes(reverse_bytes_t val);

	void setData(const T* address, std::size_t sizeInBytes);
	void setData(const VirtualPointer<T>& address, std::size_t sizeInBytes);

	bool readBits(std::size_t count, std::size_t& value);
	template <class V>
	bool readBits(std::size_t count, V& value);
	bool lookBits(std::size_t count, std::size_t& value) const;
	bool skipBits(std::size_t count);

	T* getCurrentDataPtr();
	VirtualPointer<T> getCurrentVirtualDataPtr();

	std::size_t getReadBitsCount() const;
	void resetReadBitsCount();

private:
	reverse_bytes_t m_reverseBytes;
	reverse_bits_t m_reverseBits;
	// aligned by std::size_t data address
	const uint8_t* m_typedData = nullptr;
	VirtualPointer<T> m_vData{};
	// to return to the current data pointer excluding the cache
	std::size_t m_lastCacheSize = 0;
	// remain data size in bits
	uint64_t m_remainDataSize = 0;
	std::size_t m_cache = 0;
	uint16_t m_bitPos = 0;
	std::size_t m_readBitsCount = 0;
	
	static constexpr std::size_t BITNESS = multiplyBy8(sizeof(std::size_t));

	std::size_t getBytes(const uint8_t* from, std::size_t count) const;
	std::size_t getBytes(VirtualPointer<T> from, std::size_t count) const;
	void updateCache();
	// similar to updateCache() but can skip some caches, 
	// works a little slower and undefined in case of first set of cache
	void updateFarCache();
	void updateFarCache(std::size_t skipBitsAlignedSizeT);
	std::size_t lookNextCache() const;
};

template <class T>
template <class V>
bool BinaryReader<T>::readBits(std::size_t count, V& value) {
	if (static_cast<uint64_t>(count) > m_remainDataSize || count > BITNESS) {
		return false;
	}
	if (count + m_bitPos > BITNESS) {
		const auto bitsFromNextCache = static_cast<uint16_t>(count - (BITNESS - m_bitPos));
		if (bitsFromNextCache != BITNESS) {
			value = static_cast<V>((m_cache << bitsFromNextCache) & LITTLE_BITS[count]);
		}
		else {
			value = 0;
		}
		updateCache();
		m_bitPos = bitsFromNextCache;
		value |= static_cast<V>(m_cache >> (BITNESS - bitsFromNextCache));
	}
	else {
		value = static_cast<V>((m_cache >> (BITNESS - m_bitPos - count)) & LITTLE_BITS[count]);
		m_bitPos += static_cast<uint16_t>(count);
	}
	m_remainDataSize -= static_cast<uint64_t>(count);
	m_readBitsCount += count;
	return true;
}

template<typename T>
void BinaryReader<T>::setData(const VirtualPointer<T>& address, std::size_t sizeInBytes) {
	m_remainDataSize = multiplyBy8(static_cast<uint64_t>(sizeInBytes));
	m_cache = 0;
	m_typedData = nullptr;
	m_vData = address;
	m_bitPos = 0;
	updateCache();
	resetReadBitsCount();
}

template <class T>
BinaryReader<T>::BinaryReader() :
	m_reverseBytes(REVERSE_BYTES),
	m_reverseBits(!REVERSE_BITS)
{
}

template <class T>
BinaryReader<T>::BinaryReader(const reverse_bytes_t reverseBytes) :
	m_reverseBytes(reverseBytes),
	m_reverseBits(!REVERSE_BITS)
{
}

template <class T>
BinaryReader<T>::BinaryReader(reverse_bytes_t reverseBytes, reverse_bits_t reverseBits) {
	m_typedData = nullptr;
	m_reverseBytes = reverseBytes;
	m_reverseBits = reverseBits;
	m_remainDataSize = 0;
	resetReadBitsCount();
}

template <class T>
void BinaryReader<T>::setReverseBits(reverse_bits_t val) {
	m_reverseBits = val;
}

template <class T>
void BinaryReader<T>::setReverseBytes(reverse_bytes_t val) {
	m_reverseBytes = val;
}

template <class T>
void BinaryReader<T>::setData(const T* address, const std::size_t sizeInBytes) {
	assert(nullptr != address);
	m_remainDataSize = multiplyBy8(static_cast<uint64_t>(sizeInBytes));
	m_cache = 0;
	m_typedData = reinterpret_cast<const uint8_t*>(address);
	m_bitPos = 0;
	updateCache();
	resetReadBitsCount();
}

template <class T>
bool BinaryReader<T>::readBits(std::size_t count, std::size_t& value) {
	if (static_cast<uint64_t>(count) > m_remainDataSize || count > BITNESS) {
		return false;
	}
	if (count + m_bitPos > BITNESS) {
		const auto bitsFromNextCache = static_cast<uint16_t>(count - (BITNESS - m_bitPos));
		if (bitsFromNextCache != BITNESS) {
			value = (m_cache << bitsFromNextCache) & LITTLE_BITS[count];
		}
		else {
			value = 0;
		}
		updateCache();
		m_bitPos = bitsFromNextCache;
		value |= m_cache >> (BITNESS - bitsFromNextCache);
	}
	else {
		value = (m_cache >> (BITNESS - m_bitPos - count)) & LITTLE_BITS[count];
		m_bitPos += static_cast<uint16_t>(count);
	}
	m_remainDataSize -= static_cast<uint64_t>(count);
	m_readBitsCount += count;
	return true;
}

template <class T>
bool BinaryReader<T>::lookBits(std::size_t count, std::size_t& value) const {
	if (static_cast<uint64_t>(count) > m_remainDataSize || count > BITNESS) {
		return false;
	}
	if (count + m_bitPos > BITNESS) {
		const auto bitsFromNextCache = static_cast<uint16_t>(count - (BITNESS - m_bitPos));
		if (bitsFromNextCache != BITNESS) {
			value = (m_cache << bitsFromNextCache) & LITTLE_BITS[count];
		}
		else {
			value = 0;
		}
		std::size_t tmpCache;
		if (m_remainDataSize + m_bitPos < static_cast<uint64_t>(BITNESS << 1)) {
			tmpCache = getBytes(m_typedData, static_cast<std::size_t>(divideBy8(m_bitPos + m_remainDataSize - BITNESS)));
		}
		else {
			tmpCache = lookNextCache();
		}
		value |= tmpCache >> (BITNESS - bitsFromNextCache);
	}
	else {
		value = (m_cache >> (BITNESS - m_bitPos - count)) & LITTLE_BITS[count];
	}
	return true;
}

template <class T>
bool BinaryReader<T>::skipBits(std::size_t count) {
	if (static_cast<uint64_t>(count) > m_remainDataSize) {
		return false;
	}
	if (count + m_bitPos > BITNESS) {
		auto bitsFromNextCache = static_cast<uint16_t>(count - (BITNESS - m_bitPos));
		// skip bytes count in bits
		const auto skipBitsAlignedSizeT = bitsFromNextCache & ~LITTLE_BITS[ALIGN_BY_SIZE_T];
		bitsFromNextCache %= BITNESS;
		// skip bits count if from last not full byte
		updateFarCache(skipBitsAlignedSizeT);
		m_bitPos += bitsFromNextCache;
	}
	else {
		m_bitPos += static_cast<uint16_t>(count);
	}
	m_remainDataSize -= static_cast<uint64_t>(count);
	m_readBitsCount += count;
	return true;
}

template<class T>
inline T* BinaryReader<T>::getCurrentDataPtr() {
	return m_typedData - m_lastCacheSize + m_bitPos / 8;
}

template<class T>
inline VirtualPointer<T> BinaryReader<T>::getCurrentVirtualDataPtr() {
	return m_vData - m_lastCacheSize + (m_bitPos / 8 / sizeof(T));
}

template<class T>
inline std::size_t BinaryReader<T>::getReadBitsCount() const
{
	return m_readBitsCount;
}

template<class T>
inline void BinaryReader<T>::resetReadBitsCount() {
	m_readBitsCount = 0;
}

template <class T>
std::size_t BinaryReader<T>::getBytes(const uint8_t* from, std::size_t count) const {
	std::size_t out = 0;
	if (m_reverseBytes) {
		if (!m_reverseBits) {
			for (std::size_t i = 0; i < count; ++i) {
				out <<= BITS_IN_BYTE;
				out |= from[i] & LITTLE_BITS[BITS_IN_BYTE];
			}
		}
		else {
			for (std::size_t i = 0; i < count; ++i) {
				out <<= BITS_IN_BYTE;
				out |= reverseBitsInByte(from[i] & LITTLE_BITS[BITS_IN_BYTE]);
			}
		}
	}
	else {
		if (!m_reverseBits) {
			for (std::size_t i = 1; i <= count; ++i) {
				out <<= BITS_IN_BYTE;
				out |= from[count - i] & LITTLE_BITS[BITS_IN_BYTE];
			}
		}
		else {
			for (std::size_t i = 1; i <= count; ++i) {
				out <<= BITS_IN_BYTE;
				out |= reverseBitsInByte(from[count - i] & LITTLE_BITS[BITS_IN_BYTE]);
			}
		}
	}
	return out;
}

template <class T>
std::size_t BinaryReader<T>::getBytes(VirtualPointer<T> from, const std::size_t count) const {
	std::size_t out = 0;
	if (m_reverseBytes) {
		if (!m_reverseBits) {
			for (std::size_t i = 0; i < count; ++i) {
				out <<= BITS_IN_BYTE;
				out |= from[i] & LITTLE_BITS[BITS_IN_BYTE];
			}
		}
		else {
			for (std::size_t i = 0; i < count; ++i) {
				out <<= BITS_IN_BYTE;
				out |= reverseBitsInByte(from[i] & LITTLE_BITS[BITS_IN_BYTE]);
			}
		}
	}
	else {
		if (!m_reverseBits) {
			for (std::size_t i = 1; i <= count; ++i) {
				out <<= BITS_IN_BYTE;
				out |= from[count - i] & LITTLE_BITS[BITS_IN_BYTE];
			}
		}
		else {
			for (std::size_t i = 1; i <= count; ++i) {
				out <<= BITS_IN_BYTE;
				out |= reverseBitsInByte(from[count - i] & LITTLE_BITS[BITS_IN_BYTE]);
			}
		}
	}
	return out;
}

template <class T>
void BinaryReader<T>::updateCache() {
	std::size_t count;
	if (m_bitPos + m_remainDataSize >= static_cast<uint64_t>(BITNESS) * 2) {
		count = sizeof(std::size_t);
	}
	else {
		count = static_cast<std::size_t>(divideBy8((m_bitPos + m_remainDataSize) % BITNESS));
	}
	if (m_typedData) {
		m_cache = getBytes(m_typedData, count);
		m_typedData += count;
		m_lastCacheSize = count;
	}
	else {
		m_cache = getBytes(m_vData, count);
		m_vData += count;
		m_lastCacheSize = count;
	}
	m_bitPos = static_cast<uint16_t>(multiplyBy8(sizeof(std::size_t) - count));
}

template <class T>
void BinaryReader<T>::updateFarCache()
{
	updateFarCache(0);
}

template <class T>
void BinaryReader<T>::updateFarCache(std::size_t skipBitsAlignedSizeT) {
	// += 1 increase pointer on sizeof(std::size_t) bytes
	if (m_typedData) {
		m_typedData += ((divideBy8(skipBitsAlignedSizeT)));
	}
	else {
		m_vData += ((divideBy8(skipBitsAlignedSizeT)) / sizeof(T));
	}
	updateCache();
}

template <class T>
std::size_t BinaryReader<T>::lookNextCache() const {
	const auto availableBytesForPutIntoCache = static_cast<std::size_t>(divideBy8(m_remainDataSize + multiplyBy8(sizeof(std::size_t)) - m_bitPos));
	const auto bytesToCopyCount = std::min(sizeof(std::size_t), availableBytesForPutIntoCache);
	if (m_typedData) {
		return getBytes(m_typedData, bytesToCopyCount);
	}
	return getBytes(m_vData, bytesToCopyCount);
}