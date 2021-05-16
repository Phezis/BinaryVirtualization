#pragma once

#include "Exceptions.h"

#include <cstddef>
#include <cassert>
#include <vector>
#include <memory>
#include <cstring>
#include <functional>
#include <stdexcept>
#include <unordered_set>

template <typename T>
class VirtualPointer final
{
	using signed_size_t = std::ptrdiff_t;
public:
	VirtualPointer();
	VirtualPointer(const VirtualPointer& other);
	VirtualPointer(VirtualPointer&& other) noexcept;

	~VirtualPointer() noexcept;

	VirtualPointer& operator=(const VirtualPointer& other);
	VirtualPointer& operator=(VirtualPointer&& other) noexcept;

	VirtualPointer& operator++();
	VirtualPointer operator++(int);

	VirtualPointer& operator+=(std::size_t shift);

	VirtualPointer& operator--();
	VirtualPointer operator--(int);

	VirtualPointer& operator-=(std::size_t shift);

	T& operator[](std::size_t idx);
	const T& operator[](std::size_t idx) const;

	T& operator*();

	void addChunk(T* ptr, std::size_t length);
	void addChunk(const VirtualPointer& src, std::size_t count);

	std::size_t bytesRemaining() const;

	void clear();

	bool isOverflow() const;

	template<typename T, typename V>
	friend VirtualPointer<T>& memset(VirtualPointer<T>& dest, const V& value, std::size_t count);

	template<typename T>
	friend VirtualPointer<T>& memcpy(VirtualPointer<T>& dest, const VirtualPointer<T>& src, std::size_t count);

	template<typename T>
	friend VirtualPointer<T>& memcpy(VirtualPointer<T>& dest, const void* src, std::size_t count);

	template<typename T>
	friend void* memcpy(void* dest, const VirtualPointer<T>& src, std::size_t count);


	// Attention! All memmove functions can allocate an additional amount of memory of the count bytesRemaining!
	template<typename T>
	friend VirtualPointer<T>& memmove(VirtualPointer<T>& dest, const VirtualPointer<T>& src, std::size_t count);

	template<typename T>
	friend VirtualPointer<T>& memmove(VirtualPointer<T>& dest, const void* src, std::size_t count);

	template<typename T>
	friend void* memmove(void* dest, const VirtualPointer<T>& src, std::size_t count);


	template<typename T>
	friend int memcmp(const VirtualPointer<T>& dest, const VirtualPointer<T>& src, std::size_t count);

	template<typename T>
	friend int memcmp(const VirtualPointer<T>& dest, const void* src, std::size_t count);

	template<typename T>
	friend int memcmp(const void* dest, const VirtualPointer<T>& src, std::size_t count);

private:
	struct chunk;
	class chunksCollection;
	
	// contains all chunks with their sizes
	std::shared_ptr<chunksCollection> m_chunks;
	// contains the index of a current chunk in chunks collection
	std::size_t m_curChunkIdx = 0;
	// contains the index of a T-type atom
	// can be more than current chunk bytesRemaining
	signed_size_t m_curTIdx = 0;
	chunk m_currentChunk{};
	// contains the number of remaining bytes
	std::size_t m_byteFromStart = 0;



	struct chunk
	{
		T* chunkBeginning = nullptr;
		std::size_t chunkSize = 0;

		chunk() = default;
		chunk(const chunk& other) = default;
		chunk(chunk&& other) = default;
		chunk(T* chunkBeginning, std::size_t chunkSize);

		~chunk() = default;

		chunk& operator=(const chunk& other) = default;
		chunk& operator=(chunk&& other) = default;
	};

	class chunksCollection
	{
		std::vector<chunk> m_chunks{};
		std::size_t m_sizeInBytes = 0;

		std::unordered_set<VirtualPointer<T>*> m_pointersReferencingToChunks{};

	public:
		std::vector<chunk>& getChunks();

		void emplace(chunk&& chunk);
		void revalidateAllVirtualPointersIndexes();

		std::size_t chunksSizeInBytes() const;

		void addVirtualPointer(VirtualPointer<T>* virtualPointer);
		void removePointer(VirtualPointer<T>* virtualPointer);
	};

	friend chunksCollection;

private:

	void toNextElement();

	void toPrevElement();

	bool outOfRange() const;
	
	std::size_t currentChunkSize() const;
	T* currentChunkBeginning() const;

	void revalidateIndexes();

	
	void decreaseBytesFromStart(std::size_t value);
	void increaseBytesFromStart(std::size_t value);
};

template <typename T>
VirtualPointer<T> operator+(VirtualPointer<T> ptr, const std::size_t& shift);

template <typename T>
VirtualPointer<T> operator+(const std::size_t& shift, VirtualPointer<T> ptr);

template <typename T>
VirtualPointer<T> operator-(VirtualPointer<T> ptr, const std::size_t& shift);


template <typename T>
bool VirtualPointer<T>::outOfRange() const
{
	return m_chunks->getChunks().empty() || (m_curChunkIdx + 1 == m_chunks->getChunks().size() && static_cast<size_t>(m_curTIdx) >= currentChunkSize());
}

template <typename T>
std::size_t VirtualPointer<T>::currentChunkSize() const
{
	return m_currentChunk.chunkSize;
}

template <typename T>
T* VirtualPointer<T>::currentChunkBeginning() const
{
	return m_currentChunk.chunkBeginning;
}

template <typename T>
void VirtualPointer<T>::revalidateIndexes()
{
	while (m_curChunkIdx < m_chunks->getChunks().size() - 1 && static_cast<size_t>(m_curTIdx) >= currentChunkSize())
	{
		m_curTIdx -= currentChunkSize();
		++m_curChunkIdx;
	}
	m_currentChunk = m_chunks->getChunks()[m_curChunkIdx];
}

template <typename T>
void VirtualPointer<T>::decreaseBytesFromStart(const std::size_t value)
{
	m_byteFromStart -= value;
}

template <typename T>
void VirtualPointer<T>::increaseBytesFromStart(const std::size_t value)
{
	m_byteFromStart += value;
}

template <typename T>
T min(T f, T s)
{
	return f < s ? f : s;
}

template <typename T>
VirtualPointer<T>::VirtualPointer() :
	m_chunks(std::make_shared<chunksCollection>())
{
	m_chunks->addVirtualPointer(this);
}

template <typename T>
VirtualPointer<T>::VirtualPointer(const VirtualPointer& other) :
	m_chunks(other.m_chunks)
{
	m_curChunkIdx = other.m_curChunkIdx;
	m_curTIdx = other.m_curTIdx;
	m_currentChunk = other.m_currentChunk;
	m_byteFromStart = other.m_byteFromStart;

	m_chunks->addVirtualPointer(this);
}

template <typename T>
VirtualPointer<T>::VirtualPointer(VirtualPointer&& other) noexcept
{
	other.m_chunks->removePointer(&other);
	
	m_chunks = std::move(other.m_chunks);
	m_curChunkIdx = other.m_curChunkIdx;
	m_curTIdx = other.m_curTIdx;
	m_currentChunk = std::move(other.m_currentChunk);
	m_byteFromStart = other.m_byteFromStart;

	m_chunks->addVirtualPointer(this);
}

template <typename T>
VirtualPointer<T>::~VirtualPointer() noexcept
{
	if (nullptr != m_chunks) {
		m_chunks->removePointer(this);
	}
}

template <typename T>
VirtualPointer<T>& VirtualPointer<T>::operator=(const VirtualPointer& other)
{
	if(&other != this)
	{
		m_chunks->removePointer(this);

		m_chunks = other.m_chunks;
		m_curChunkIdx = other.m_curChunkIdx;
		m_curTIdx = other.m_curTIdx;
		m_currentChunk = other.m_currentChunk;
		m_byteFromStart = other.m_byteFromStart;

		m_chunks->addVirtualPointer(this);
	}
	return *this;
}

template <typename T>
VirtualPointer<T>& VirtualPointer<T>::operator=(VirtualPointer&& other) noexcept
{
	if (&other != this)
	{
		m_chunks->removePointer(this);
		other.m_chunks->removePointer(&other);

		m_chunks = std::move(other.m_chunks);
		m_curChunkIdx = other.m_curChunkIdx;
		m_curTIdx = other.m_curTIdx;
		m_currentChunk = std::move(other.m_currentChunk);
		m_byteFromStart = other.m_byteFromStart;

		m_chunks->addVirtualPointer(this);
	}
	return *this;
}

template <typename T>
VirtualPointer<T>& VirtualPointer<T>::operator++()
{
	toNextElement();
	return *this;
}

template <typename T>
VirtualPointer<T> VirtualPointer<T>::operator++(int)
{
	VirtualPointer<T> out = VirtualPointer<T>(*this);
	toNextElement();
	return out;
}

template <typename T>
VirtualPointer<T>& VirtualPointer<T>::operator+=(const std::size_t shift)
{
	std::size_t localShift = shift;
	increaseBytesFromStart(shift * sizeof(T));
	if (m_curTIdx + localShift >= currentChunkSize())
	{
		if (m_curChunkIdx + 1 >= m_chunks->getChunks().size())
		{
			m_curTIdx += localShift;
			return *this;
		}
		localShift -= currentChunkSize() - m_curTIdx;
		m_curTIdx = 0;
		auto chunk = m_chunks->getChunks().cbegin() + m_curChunkIdx;
		++chunk;
		for (; chunk != m_chunks->getChunks().cend() && localShift >= chunk->chunkSize; ++chunk)
		{
			localShift -= chunk->chunkSize;
		}
		m_curChunkIdx = chunk - m_chunks->getChunks().cbegin();
		if(chunk == m_chunks->getChunks().cend())
		{
			--m_curChunkIdx;
			m_currentChunk = m_chunks->getChunks()[m_curChunkIdx];
			localShift += m_currentChunk.chunkSize;
		}
		else
		{
			m_currentChunk = m_chunks->getChunks()[m_curChunkIdx];
		}
	}
	m_curTIdx += localShift;
	return *this;
}

template <typename T, typename V>
VirtualPointer<T>& memset(VirtualPointer<T>& dest, const V& value, std::size_t count)
{
	// need to be done because if not to do and the count is zero, then the next checks will fall with underflow
	if (!count)
	{
		return dest;
	}
	if (dest.m_chunks->getChunks().empty())
	{
		throw NullPointerException();
	}
	if (dest.outOfRange())
	{
		throw std::out_of_range("Attempt to go abroad the memory");
	}
	auto chunk = dest.m_chunks->getChunks().begin() + dest.m_curChunkIdx;
	T* ptr = chunk->chunkBeginning + dest.m_curTIdx;
	for (std::size_t i = dest.m_curTIdx; i < dest.currentChunkSize(); ++i)
	{
		*ptr = static_cast<T>(value);
		if (!(count - 1))
		{
			return dest;
		}
		--count;
		++ptr;
	}
	for (++chunk; chunk < dest.m_chunks->getChunks().end() - 1; ++chunk)
	{
		ptr = chunk->chunkBeginning;
		for (std::size_t i = 0; i < chunk->chunkSize; ++i)
		{
			*(ptr) = static_cast<T>(value);
			if (!(count - 1))
			{
				return dest;
			}
			--count;
			++ptr;
		}
	}
	if (chunk >= dest.m_chunks->getChunks().end())
	{
		throw std::out_of_range("Attempt to go abroad the memory");
	}
	ptr = chunk->chunkBeginning;
	for (std::size_t i = 0; i < chunk->chunkSize; ++i)
	{
		*(ptr) = static_cast<T>(value);
		if (!(count - 1))
		{
			return dest;
		}
		--count;
		++ptr;
	}
	throw std::out_of_range("Attempt to go abroad the memory");
}

template <typename T>
VirtualPointer<T>& memcpy(VirtualPointer<T>& dest, const VirtualPointer<T>& src, std::size_t count)
{
	if (!count)
	{
		return dest;
	}
	if (src.m_chunks->getChunks().empty() || dest.m_chunks->getChunks().empty())
	{
		throw NullPointerException();
	}
	if (dest.outOfRange() || src.outOfRange())
	{
		throw std::out_of_range("Attempt to go abroad the memory");
	}
	std::size_t curChunkIdx = dest.m_curChunkIdx;

	std::size_t curSrcChunkIdx = src.m_curChunkIdx;

	std::size_t blockMemLeft = dest.currentChunkSize() - dest.m_curTIdx;
	std::size_t srcBlockMemLeft = src.currentChunkSize() - src.m_curTIdx;

	T* destPtr = dest.m_chunks->getChunks()[curChunkIdx].chunkBeginning + dest.m_curTIdx;
	const T* srcPtr = src.m_chunks->getChunks()[curSrcChunkIdx].chunkBeginning + src.m_curTIdx;

	auto memoryOver = false;

	while (true)
	{
		if (memoryOver)
		{
			throw std::out_of_range("Attempt to go abroad the memory");
		}
		auto left = min(count, min(blockMemLeft, srcBlockMemLeft));
		memcpy(destPtr, srcPtr, left * sizeof(T));
		count -= left;
		if (!count)
		{
			return dest;
		}
		destPtr += left;
		srcPtr += left;
		blockMemLeft -= left;
		srcBlockMemLeft -= left;
		if (!blockMemLeft)
		{
			++curChunkIdx;
			if (dest.m_chunks->getChunks().size() <= curChunkIdx)
			{
				memoryOver = true;
			}
			else
			{
				destPtr = dest.m_chunks->getChunks()[curChunkIdx].chunkBeginning;
				blockMemLeft = dest.m_chunks->getChunks()[curChunkIdx].chunkSize;
			}
		}
		if (!srcBlockMemLeft)
		{
			++curSrcChunkIdx;
			if (src.m_chunks->getChunks().size() <= curSrcChunkIdx)
			{
				memoryOver = true;
			}
			else
			{
				srcPtr = src.m_chunks->getChunks()[curSrcChunkIdx].chunkBeginning;
				srcBlockMemLeft = src.m_chunks->getChunks()[curSrcChunkIdx].chunkSize;
			}
		}
	}
}

template <typename T>
VirtualPointer<T>& memcpy(VirtualPointer<T>& dest, const void* src, std::size_t count)
{
	if (!count)
	{
		return dest;
	}
	if (!src || dest.m_chunks->getChunks().empty())
	{
		throw NullPointerException();
	}
	if (dest.outOfRange())
	{
		throw std::out_of_range("Attempt to go abroad the memory");
	}
	std::size_t curChunkIdx = dest.m_curChunkIdx;

	std::size_t blockMemLeft = dest.currentChunkSize() - dest.m_curTIdx;

	T* destPtr = dest.m_chunks->getChunks()[curChunkIdx].chunkBeginning + dest.m_curTIdx;
	const T* srcPtr = static_cast<const T*>(src);

	auto memoryOver = false;

	while (true)
	{
		if (memoryOver)
		{
			throw std::out_of_range("Attempt to go abroad the memory");
		}
		auto left = min(count, blockMemLeft);
		memcpy(destPtr, srcPtr, left * sizeof(T));
		count -= left;
		if (!count)
		{
			return dest;
		}
		destPtr += left;
		srcPtr += left;
		blockMemLeft -= left;
		if (!blockMemLeft)
		{
			++curChunkIdx;
			if (dest.m_chunks->getChunks().size() <= curChunkIdx)
			{
				memoryOver = true;
			}
			else
			{
				destPtr = dest.m_chunks->getChunks()[curChunkIdx].chunkBeginning;
				blockMemLeft = dest.m_chunks->getChunks()[curChunkIdx].chunkSize;
			}
		}
	}
}

template <typename T>
void* memcpy(void* dest, const VirtualPointer<T>& src, std::size_t count)
{
	if (!count)
	{
		return dest;
	}
	if (!dest || src.m_chunks->getChunks().empty())
	{
		throw NullPointerException();
	}
	if (src.outOfRange())
	{
		throw std::out_of_range("Attempt to go abroad the memory");
	}

	std::size_t curSrcChunkIdx = src.m_curChunkIdx;

	std::size_t srcBlockMemLeft = src.currentChunkSize() - src.m_curTIdx;

	T* destPtr = static_cast<T*>(dest);
	const T* srcPtr = src.m_chunks->getChunks()[curSrcChunkIdx].chunkBeginning + src.m_curTIdx;

	auto memoryOver = false;

	while (true)
	{
		if (memoryOver)
		{
			throw std::out_of_range("Attempt to go abroad the memory");
		}
		std::size_t left = min(count, srcBlockMemLeft);
		memcpy(destPtr, srcPtr, left * sizeof(T));
		count -= left;
		if (!count)
		{
			return dest;
		}
		destPtr += left;
		srcPtr += left;
		srcBlockMemLeft -= left;
		if (!srcBlockMemLeft)
		{
			++curSrcChunkIdx;
			if (src.m_chunks->getChunks().size() <= curSrcChunkIdx)
			{
				memoryOver = true;
			}
			else
			{
				srcPtr = src.m_chunks->getChunks()[curSrcChunkIdx].chunkBeginning;
				srcBlockMemLeft = src.m_chunks->getChunks()[curSrcChunkIdx].chunkSize;
			}
		}
	}
}

template <typename T>
VirtualPointer<T>& memmove(VirtualPointer<T>& dest, const VirtualPointer<T>& src, std::size_t count)
{
	if (!count)
	{
		return dest;
	}
	if (dest.m_chunks->getChunks().empty() || src.m_chunks->getChunks().empty())
	{
		throw NullPointerException();
	}
	if (dest.outOfRange() || src.outOfRange())
	{
		throw std::out_of_range("Attempt to go abroad the memory");
	}
	T* extraMem = new T[count];
	if (!extraMem)
	{
		throw std::bad_alloc();
	}
	try
	{
		memcpy(extraMem, src, count);
		memcpy(dest, extraMem, count);
	}
	catch (std::exception&)
	{
		delete[] extraMem;
		throw;
	}
	delete[] extraMem;
	return dest;
}

template <typename T>
VirtualPointer<T>& memmove(VirtualPointer<T>& dest, const void* src, std::size_t count)
{
	if (!count)
	{
		return dest;
	}
	if (dest.m_chunks->getChunks().empty() || !src)
	{
		throw NullPointerException();
	}
	if (dest.outOfRange())
	{
		throw std::out_of_range("Attempt to go abroad the memory");
	}
	T* extraMem = new T[count];
	if (!extraMem)
	{
		throw std::bad_alloc();
	}
	try
	{
		memcpy(static_cast<void*>(extraMem), src, count * sizeof(T));
		memcpy(dest, extraMem, count);
	}
	catch (std::exception&)
	{
		delete[] extraMem;
		throw;
	}
	delete[] extraMem;
	return dest;
}

template <typename T>
void* memmove(void* dest, const VirtualPointer<T>& src, std::size_t count)
{
	if (!count)
	{
		return dest;
	}
	if (!dest || src.m_chunks->getChunks().empty())
	{
		throw NullPointerException();
	}
	if (src.outOfRange())
	{
		throw std::out_of_range("Attempt to go abroad the memory");
	}
	T* extraMem = new T[count];
	if (!extraMem)
	{
		throw std::bad_alloc();
	}
	try
	{
		memcpy(extraMem, src, count);
		memcpy(dest, static_cast<void*>(extraMem), count * sizeof(T));
	}
	catch (std::exception&)
	{
		delete[] extraMem;
		throw;
	}
	delete[] extraMem;
	return dest;
}

template <typename T>
VirtualPointer<T> operator+(VirtualPointer<T> ptr, const std::size_t& shift)
{
	return ptr += shift;
}

template <typename T>
VirtualPointer<T> operator+(const std::size_t& shift, VirtualPointer<T> ptr)
{
	return ptr += shift;
}

template <typename T>
VirtualPointer<T>& VirtualPointer<T>::operator--()
{
	toPrevElement();
	return *this;
}

template <typename T>
VirtualPointer<T> VirtualPointer<T>::operator--(int)
{
	VirtualPointer<T> out = VirtualPointer<T>(*this);
	toPrevElement();
	return out;
}

template <typename T>
VirtualPointer<T>& VirtualPointer<T>::operator-=(const std::size_t shift)
{
	std::size_t localShift = shift;
	decreaseBytesFromStart(shift * sizeof(T));
	while (static_cast<size_t>(m_curTIdx) < localShift)
	{
		if (m_curChunkIdx)
		{
			localShift -= m_curTIdx + 1;
			--m_curChunkIdx;
			m_currentChunk = m_chunks->getChunks()[m_curChunkIdx];
			m_curTIdx = currentChunkSize() - 1;
		}
		else
		{
			localShift -= m_curTIdx;
			m_curTIdx = -static_cast<signed_size_t>(localShift);
			return *this;
		}
	}
	m_curTIdx -= localShift;
	return *this;
}

template <typename T>
T& VirtualPointer<T>::operator[](std::size_t idx)
{
	std::size_t curChunkIdx = m_curChunkIdx;
	std::size_t curTIdx = m_curTIdx;
	std::size_t curChunkSize = currentChunkSize();
	if (curTIdx + idx >= curChunkSize)
	{
		idx -= curChunkSize - curTIdx;
		curTIdx = 0;
		++curChunkIdx;
		curChunkSize = m_chunks->getChunks()[curChunkIdx].chunkSize;
		while (idx >= curChunkSize)
		{
			idx -= curChunkSize;
			++curChunkIdx;
			curChunkSize = m_chunks->getChunks()[curChunkIdx].chunkSize;
		}
	}
	curTIdx += idx;
	return m_chunks->getChunks()[curChunkIdx].chunkBeginning[curTIdx];
}

template <typename T>
const T& VirtualPointer<T>::operator[](std::size_t idx) const
{
	VirtualPointer<T> tmp = *this;
	return *(tmp += idx);
}

template <typename T>
VirtualPointer<T> operator-(VirtualPointer<T> ptr, const std::size_t& shift)
{
	return ptr -= shift;
}

template <typename T>
T& VirtualPointer<T>::operator*()
{
	return *(currentChunkBeginning() + m_curTIdx);
}

template <typename T>
void VirtualPointer<T>::addChunk(T* ptr, std::size_t length)
{
	if (length)
	{
		if (nullptr != ptr)
		{
			bool tryShiftChunkIdx = false;
			if (outOfRange() && !m_chunks->getChunks().empty())
			{
				m_curTIdx -= currentChunkSize();
				tryShiftChunkIdx = true;
			}
			if (m_chunks->getChunks().empty())
			{
				tryShiftChunkIdx = false;
			}
			m_chunks->emplace(chunk(ptr, length));
			if (static_cast<size_t>(m_curTIdx) < currentChunkSize())
			{
				if (tryShiftChunkIdx)
				{
					++m_curChunkIdx;
					m_currentChunk = m_chunks->getChunks()[m_curChunkIdx];
				}
			}
		}
	}
	m_chunks->revalidateAllVirtualPointersIndexes();
}

template <typename T>
void VirtualPointer<T>::addChunk(const VirtualPointer& src, std::size_t count)
{
	if (src.outOfRange())
	{
		throw std::out_of_range("Attempt to add from outside of the memory");
	}
	std::size_t curSrcChunkIdx = src.m_curChunkIdx;
	std::size_t curSrcTIdx = src.m_curTIdx;
	auto curTIdx = m_curTIdx;
	auto curChunkIdx = m_curChunkIdx;
	auto curOutOfRange = outOfRange() && !m_chunks->getChunks().empty();
	auto again = true;
	while (again)
	{
		if (src.m_chunks->getChunks()[curSrcChunkIdx].chunkSize >= count + curSrcTIdx)
		{
			m_chunks->emplace(chunk(src.m_chunks->getChunks()[curSrcChunkIdx].chunkBeginning + curSrcTIdx, count));
			again = false;
		}
		else
		{
			m_chunks->emplace(chunk(
				src.m_chunks->getChunks()[curSrcChunkIdx].chunkBeginning + curSrcTIdx,
				src.m_chunks->getChunks()[curSrcChunkIdx].chunkSize - curSrcTIdx
			)
			);
			count -= src.m_chunks->getChunks()[curSrcChunkIdx].chunkSize - curSrcTIdx;
		}
		if (curOutOfRange)
		{
			if (src.m_chunks->getChunks()[curSrcChunkIdx].chunkSize - curSrcTIdx >= (curTIdx - currentChunkSize()))
			{
				// out of range and zero curTIdx both mean this was empty
				if (curTIdx)
				{
					curTIdx -= currentChunkSize();
				}
				else
				{
					// if this was empty, then current chunk index must be zero
					--curChunkIdx;
				}
			}
			else
			{
				curTIdx -= m_chunks->getChunks()[curChunkIdx].chunkSize;
			}
			++curChunkIdx;
			if (curTIdx < static_cast<signed_size_t>(currentChunkSize()))
			{
				curOutOfRange = false;
			}
		}
		++curSrcChunkIdx;
		curSrcTIdx = 0;
	}
	m_curTIdx = curTIdx;
	m_curChunkIdx = curChunkIdx;
	m_currentChunk = m_chunks->getChunks()[m_curChunkIdx];
	m_chunks->revalidateAllVirtualPointersIndexes();
}

template <typename T>
std::size_t VirtualPointer<T>::bytesRemaining() const
{
	return m_chunks->chunksSizeInBytes() - m_byteFromStart;
}

template <typename T>
inline void VirtualPointer<T>::clear()
{
	m_chunks = std::make_shared<chunksCollection>();
	m_curChunkIdx = 0;
	m_curTIdx = 0;
	m_currentChunk = chunk{};
	m_byteFromStart = 0;
}

template <typename T>
inline bool VirtualPointer<T>::isOverflow() const
{
	return outOfRange();
}

template <typename T>
int memcmp(const VirtualPointer<T>& dest, const VirtualPointer<T>& src, std::size_t count)
{
	if (!count)
	{
		return 0;
	}
	if (src.m_chunks->getChunks().empty() || dest.m_chunks->getChunks().empty())
	{
		throw NullPointerException();
	}
	if (dest.outOfRange() || src.outOfRange())
	{
		throw std::out_of_range("Attempt to go abroad the memory");
	}
	std::size_t curChunkIdx = dest.m_curChunkIdx;

	std::size_t curSrcChunkIdx = src.m_curChunkIdx;

	std::size_t blockMemLeft = dest.currentChunkSize() - dest.m_curTIdx;
	std::size_t srcBlockMemLeft = src.currentChunkSize() - src.m_curTIdx;

	T* destPtr = dest.m_chunks->getChunks()[curChunkIdx].chunkBeginning + dest.m_curTIdx;
	const T* srcPtr = src.m_chunks->getChunks()[curSrcChunkIdx].chunkBeginning + src.m_curTIdx;

	auto memoryOver = false;

	while (true)
	{
		if (memoryOver)
		{
			throw std::out_of_range("Attempt to go abroad the memory");
		}
		auto left = min(count, min(blockMemLeft, srcBlockMemLeft));
		const int result = memcmp(destPtr, srcPtr, left * sizeof(T));
		if (result)
		{
			return result;
		}
		count -= left;
		if (!count)
		{
			return 0;
		}
		destPtr += left;
		srcPtr += left;
		blockMemLeft -= left;
		srcBlockMemLeft -= left;
		if (!blockMemLeft)
		{
			++curChunkIdx;
			if (dest.m_chunks->getChunks().size() <= curChunkIdx)
			{
				memoryOver = true;
			}
			else
			{
				destPtr = dest.m_chunks->getChunks()[curChunkIdx].chunkBeginning;
				blockMemLeft = dest.m_chunks->getChunks()[curChunkIdx].chunkSize;
			}
		}
		if (!srcBlockMemLeft)
		{
			++curSrcChunkIdx;
			if (src.m_chunks->getChunks().size() <= curSrcChunkIdx)
			{
				memoryOver = true;
			}
			else
			{
				srcPtr = src.m_chunks->getChunks()[curSrcChunkIdx].chunkBeginning;
				srcBlockMemLeft = src.m_chunks->getChunks()[curSrcChunkIdx].chunkSize;
			}
		}
	}
}

template <typename T>
int memcmp(const VirtualPointer<T>& dest, const void* src, std::size_t count)
{
	if (!count)
	{
		return 0;
	}
	if (!src || dest.m_chunks->getChunks().empty())
	{
		throw NullPointerException();
	}
	if (dest.outOfRange())
	{
		throw std::out_of_range("Attempt to go abroad the memory");
	}
	std::size_t curChunkIdx = dest.m_curChunkIdx;

	std::size_t blockMemLeft = dest.currentChunkSize() - dest.m_curTIdx;

	T* destPtr = dest.m_chunks->getChunks()[curChunkIdx].chunkBeginning + dest.m_curTIdx;
	const T* srcPtr = static_cast<const T*>(src);

	auto memoryOver = false;

	while (true)
	{
		if (memoryOver)
		{
			throw std::out_of_range("Attempt to go abroad the memory");
		}
		auto left = min(count, blockMemLeft);
		const int result = memcmp(destPtr, srcPtr, left * sizeof(T));
		if (result)
		{
			return result;
		}
		count -= left;
		if (!count)
		{
			return 0;
		}
		destPtr += left;
		srcPtr += left;
		blockMemLeft -= left;
		if (!blockMemLeft)
		{
			++curChunkIdx;
			if (dest.m_chunks->getChunks().size() <= curChunkIdx)
			{
				memoryOver = true;
			}
			else
			{
				destPtr = dest.m_chunks->getChunks()[curChunkIdx].chunkBeginning;
				blockMemLeft = dest.m_chunks->getChunks()[curChunkIdx].chunkSize;
			}
		}
	}
}

template <typename T>
int memcmp(const void* dest, const VirtualPointer<T>& src, std::size_t count)
{
	if (!count)
	{
		return 0;
	}
	if (!dest || src.m_chunks->getChunks().empty())
	{
		throw NullPointerException();
	}
	if (src.outOfRange())
	{
		throw std::out_of_range("Attempt to go abroad the memory");
	}

	std::size_t curSrcChunkIdx = src.m_curChunkIdx;

	std::size_t srcBlockMemLeft = src.currentChunkSize() - src.m_curTIdx;

	const T* destPtr = static_cast<const T*>(dest);
	const T* srcPtr = src.m_chunks->getChunks()[curSrcChunkIdx].chunkBeginning + src.m_curTIdx;

	auto memoryOver = false;

	while (true)
	{
		if (memoryOver)
		{
			throw std::out_of_range("Attempt to go abroad the memory");
		}
		auto left = min(count, srcBlockMemLeft);
		const int result = memcmp(destPtr, srcPtr, left * sizeof(T));
		if (result)
		{
			return result;
		}
		count -= left;
		if (!count)
		{
			return 0;
		}
		destPtr += left;
		srcPtr += left;
		srcBlockMemLeft -= left;
		if (!srcBlockMemLeft)
		{
			++curSrcChunkIdx;
			if (src.m_chunks->getChunks().size() <= curSrcChunkIdx)
			{
				memoryOver = true;
			}
			else
			{
				srcPtr = src.m_chunks->getChunks()[curSrcChunkIdx].chunkBeginning;
				srcBlockMemLeft = src.m_chunks->getChunks()[curSrcChunkIdx].chunkSize;
			}
		}
	}
}

template <typename T>
VirtualPointer<T>::chunk::chunk(T* chunkBeginning, const std::size_t chunkSize) :
	chunkBeginning(chunkBeginning),
	chunkSize(chunkSize)
{
}

template <typename T>
std::vector<typename VirtualPointer<T>::chunk>& VirtualPointer<T>::chunksCollection::getChunks()
{
	return m_chunks;
}

template <typename T>
void VirtualPointer<T>::chunksCollection::emplace(chunk&& chunk)
{
	m_chunks.emplace_back(chunk);
	m_sizeInBytes += chunk.chunkSize * sizeof(T);
}

template <typename T>
void VirtualPointer<T>::chunksCollection::revalidateAllVirtualPointersIndexes()
{
	for (auto& virtualPointer : m_pointersReferencingToChunks)
	{
		virtualPointer->revalidateIndexes();
	}
}

template <typename T>
std::size_t VirtualPointer<T>::chunksCollection::chunksSizeInBytes() const
{
	return m_sizeInBytes;
}

template <typename T>
void VirtualPointer<T>::chunksCollection::addVirtualPointer(VirtualPointer<T>* virtualPointer)
{
	assert(nullptr != virtualPointer);
	m_pointersReferencingToChunks.insert(virtualPointer);
}

template <typename T>
void VirtualPointer<T>::chunksCollection::removePointer(VirtualPointer<T>* virtualPointer)
{
	auto virtualPointerIterator = m_pointersReferencingToChunks.find(virtualPointer);
	if(m_pointersReferencingToChunks.end() != virtualPointerIterator)
	{
		m_pointersReferencingToChunks.erase(virtualPointerIterator);
	}
}

template <typename T>
void VirtualPointer<T>::toNextElement()
{
	increaseBytesFromStart(sizeof(T));
	++m_curTIdx;
	/*for(; m_curTIdx >= static_cast<signed_size_t>(currentChunkSize()) && m_curChunkIdx + 1 < m_chunks->getChunks().size(); ++m_curChunkIdx)
	{
		m_curTIdx -= currentChunkSize();
		m_currentChunk = m_chunks->getChunks()[m_curChunkIdx + 1];
	}*/
	if (m_curTIdx >= static_cast<signed_size_t>(currentChunkSize()) && m_curChunkIdx + 1 < m_chunks->getChunks().size())
	{
		m_curTIdx = 0;
		++m_curChunkIdx;
		m_currentChunk = m_chunks->getChunks()[m_curChunkIdx];
	}
	/*while (m_curTIdx >= static_cast<signed_size_t>(currentChunkSize()) && m_curChunkIdx + 1 < m_chunks->getChunks().size())
	{
		m_curTIdx -= currentChunkSize();
		++m_curChunkIdx;
		m_currentChunk = m_chunks->getChunks()[m_curChunkIdx];
	}*/
}

template <typename T>
void VirtualPointer<T>::toPrevElement()
{
	decreaseBytesFromStart(sizeof(T));
	--m_curTIdx;
	while (m_curTIdx < 0 && 0 != m_curChunkIdx)
	{
		--m_curChunkIdx;
		m_currentChunk = m_chunks->getChunks()[m_curChunkIdx];
		m_curTIdx += currentChunkSize();
	}
}
