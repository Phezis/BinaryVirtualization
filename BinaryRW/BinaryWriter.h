#pragma once

#include "Reverser.h"
#include "BitMask.h"
#include "VirtualPointer.h"

#include <cstddef>
#include <cstdint>
#include <cassert>
#include <limits>

template <class T>
class BinaryWriter final
{
public:
	BinaryWriter();
	explicit BinaryWriter(reverse_bytes_t reverseBytes);
	BinaryWriter(reverse_bytes_t reverseBytes, reverse_bits_t reverseBits);

	BinaryWriter(const BinaryWriter& other) = default;
	BinaryWriter(BinaryWriter&& other) = default;

	~BinaryWriter() noexcept = default;

	BinaryWriter& operator=(const BinaryWriter & other) = default;
	BinaryWriter& operator=(BinaryWriter && other) = default;

	void setReverseBits(reverse_bits_t reverseBits);
	void setReverseBytes(reverse_bytes_t reverseBytes);

	void setData(T* address, std::size_t sizeInBytes);
	void setData(VirtualPointer<T>& address, std::size_t sizeInBytes);
	void setData(VirtualPointer<T>& address);
	bool writeBits(std::size_t count, std::size_t value);
	void flush();
	std::size_t getRemainSize() const;

private:
	bool				m_reverseBytes;
	bool				m_reverseBits;
	uint8_t*			m_typedData = nullptr;
	VirtualPointer<T>	m_vData{};
	std::size_t			m_remainDataSize = 0;
	std::size_t			m_cache = 0;
	uint16_t			m_bitPosInCache = 0;
};


template <class T>
BinaryWriter<T>::BinaryWriter() :
	m_reverseBytes(REVERSE_BYTES),
	m_reverseBits(!REVERSE_BITS)
{
}

template <class T>
BinaryWriter<T>::BinaryWriter(const reverse_bytes_t reverseBytes) :
	m_reverseBytes(reverseBytes),
	m_reverseBits(!REVERSE_BITS)
{
}

template <class T>
BinaryWriter<T>::BinaryWriter(const reverse_bytes_t reverseBytes, const reverse_bits_t reverseBits) :
	m_reverseBytes(reverseBytes),
	m_reverseBits(reverseBits)
{
}

template <class T>
void BinaryWriter<T>::setData(T* address, const std::size_t sizeInBytes)
{
	assert(nullptr != address);
	m_remainDataSize = multiplyBy8(sizeInBytes);
	m_cache = 0;
	m_typedData = static_cast<uint8_t*>(address);
	m_bitPosInCache = 0;
}

template <class T>
void BinaryWriter<T>::setData(VirtualPointer<T>& address, const std::size_t sizeInBytes)
{
	m_remainDataSize = multiplyBy8(sizeInBytes);
	m_cache = 0;
	m_typedData = nullptr;
	m_vData = address;
	m_bitPosInCache = 0;
}

template <class T>
void BinaryWriter<T>::setData(VirtualPointer<T>& address)
{
	setData(address, divideBy8(std::numeric_limits<std::size_t>::max()));
}

template <class T>
void BinaryWriter<T>::setReverseBits(const reverse_bits_t reverseBits)
{
	m_reverseBits = reverseBits;
}

template <class T>
void BinaryWriter<T>::setReverseBytes(const reverse_bytes_t reverseBytes)
{
	m_reverseBytes = reverseBytes;
}

template <class T>
bool BinaryWriter<T>::writeBits(const std::size_t count, std::size_t value)
{
	constexpr uint16_t sizeOfSizeTInBits = static_cast<uint16_t>(multiplyBy8(sizeof(std::size_t)));
	if (count > sizeOfSizeTInBits)
	{
		throw std::invalid_argument("Attempt to write more bits than size_t could contain");
	}
	if (count > m_remainDataSize)
	{
		return false;
	}

	value &= LITTLE_BITS[count];
	if (count + m_bitPosInCache > sizeOfSizeTInBits)
	{
		const uint16_t bitsToThisCache = sizeOfSizeTInBits - m_bitPosInCache;
		const auto bitsToNextCache = static_cast<uint16_t>(count - bitsToThisCache);
		m_cache |= (value >> bitsToNextCache) & LITTLE_BITS[bitsToThisCache];
		m_bitPosInCache = sizeOfSizeTInBits;
		flush();
		if (m_typedData)
		{
			m_typedData += sizeof(std::size_t);
		}
		else
		{
			m_vData += sizeof(std::size_t) / sizeof(T);
		}
		m_cache = (value & LITTLE_BITS[bitsToNextCache]) << (sizeOfSizeTInBits - bitsToNextCache);
		m_bitPosInCache = bitsToNextCache;
	}
	else
	{
		value <<= sizeOfSizeTInBits - m_bitPosInCache - count;
		m_cache |= value;
		m_bitPosInCache += static_cast<uint16_t>(count);
	}
	if (count == m_remainDataSize || m_bitPosInCache == sizeOfSizeTInBits)
	{
		flush();
		if (m_typedData)
		{
			m_typedData += sizeof(std::size_t);
		}
		else
		{
			m_vData += sizeof(std::size_t) / sizeof(T);
		}
		m_cache = 0;
		m_bitPosInCache = 0;
	}
	m_remainDataSize -= count;
	return true;
}

template <class T>
std::size_t BinaryWriter<T>::getRemainSize() const
{
	return m_remainDataSize;
}

template <class T>
void BinaryWriter<T>::flush()
{
	const auto cacheInBytes = reinterpret_cast<uint8_t*>(&m_cache);
	const auto lastByteToFlush = static_cast<uint16_t>(divideBy8(m_bitPosInCache + BITS_IN_BYTE - 1));
	if (m_typedData)
	{
		if (m_reverseBytes)
		{
			if (m_reverseBits)
			{
				for (auto i = 0; i < lastByteToFlush; ++i)
				{
					m_typedData[i] = reverseBitsInByte(cacheInBytes[sizeof(std::size_t) - i - 1]);
				}
			}
			else
			{
				for (auto i = 0; i < lastByteToFlush; ++i)
				{
					m_typedData[i] = cacheInBytes[sizeof(std::size_t) - i - 1];
				}
			}
		}
		else
		{
			if (m_reverseBits)
			{
				for (auto i = 0; i < lastByteToFlush; ++i)
				{
					m_typedData[i] = reverseBitsInByte(cacheInBytes[i]);
				}
			}
			else
			{
				for (auto i = 0; i < lastByteToFlush; ++i)
				{
					m_typedData[i] = cacheInBytes[i];
				}
			}
		}
	}
	else
	{
		if (sizeof(T) == 1)
		{
			if (m_reverseBytes)
			{
				if (m_reverseBits)
				{
					for (auto i = 0; i < lastByteToFlush; ++i)
					{
						m_vData[i] = reverseBitsInByte(cacheInBytes[sizeof(std::size_t) - i - 1]);
					}
				}
				else
				{
					for (auto i = 0; i < lastByteToFlush; ++i)
					{
						m_vData[i] = cacheInBytes[sizeof(std::size_t) - i - 1];
					}
				}
			}
			else
			{
				if (m_reverseBits)
				{
					for (auto i = 0; i < lastByteToFlush; ++i)
					{
						m_vData[i] = reverseBitsInByte(cacheInBytes[i]);
					}
				}
				else
				{
					for (auto i = 0; i < lastByteToFlush; ++i)
					{
						m_vData[i] = cacheInBytes[i];
					}
				}
			}
		}
		else
		{
			// TODO: make flush for a virtual pointer of a type that has sizeof > 1
		}
	}
}
