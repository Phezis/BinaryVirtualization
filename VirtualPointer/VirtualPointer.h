#pragma once

#include "Exceptions.h"

#include <cstddef>
#include <cassert>
#include <vector>
#include <memory>
#include <utility>
#include <cstring>
#include <functional>
#include <stdexcept>
#include <unordered_set>
#include <unordered_map>

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
	// contains all chunks with their sizes
	std::shared_ptr<std::vector<chunk>> m_chunks;
	T* m_pCurrentChunk = nullptr;
	// contains the index of a current chunk in chunks collection
	std::size_t m_curChunkIdx = 0;
	// contains the index of a T-type atom
	// can be more than current chunk bytesRemaining
	signed_size_t m_curTIdx = 0;
	// a bytesRemaining of the current chunk
	std::size_t m_curChunkSize = 0;
	// contains the number of remaining bytes
	mutable std::size_t m_bytesRemaining = 0;
	// Contains the last index of the vector of chunks.
	// Needed for maintain the correctness of the number of
	// remaining bytes in the case when another owner of
	// the vector of chunks added a new chunk
	mutable std::size_t m_endVectorIndex = 0;



	struct chunk
	{
		T* chunkBeginning;
		std::size_t chunkSize;

		chunk() = default;
		chunk(const chunk& other) = default;
		chunk(chunk&& other) = default;
		chunk(T* chunkBeginning, std::size_t chunkSize);

		~chunk() = default;

		chunk& operator=(const chunk& other) = default;
		chunk& operator=(chunk&& other) = default;
	};

	// Ensures correct indexing for all copies when
	// a new chunk is added to one of the copies
	using ReferencingCopies = std::unordered_set<VirtualPointer<T>*>;
	struct ReferencesToChunksCollection
	{
		std::unordered_map<std::shared_ptr<std::vector<chunk>>, ReferencingCopies> referencesToChunksCollections;

		void addReference(const std::shared_ptr<std::vector<chunk>>& chunks, VirtualPointer<T>* reference);
		void removeReference(const std::shared_ptr<std::vector<chunk>>& chunks, VirtualPointer<T>* reference);

		void validateContextForEachCopy(const std::shared_ptr<std::vector<chunk>>& chunks);
	};

	friend ReferencesToChunksCollection;

	static ReferencesToChunksCollection m_referencesToChunksCollection;


private:

	void toNextElement();

	void toPrevElement();

	bool outOfRange() const;

	// tries to revalidate current indexes 
	// and checks out of range
	bool outOfRangeWithRevalidateIndexes();

	void revalidateIndexes();

	void validateBytesRemaining() const;
	void increaseBytesRemaining(std::size_t value) const;
	void decreaseBytesRemaining(std::size_t value) const;
};

template <typename T>
VirtualPointer<T> operator+(VirtualPointer<T> ptr, const std::size_t& shift);

template <typename T>
VirtualPointer<T> operator+(const std::size_t& shift, VirtualPointer<T> ptr);

template <typename T>
VirtualPointer<T> operator-(VirtualPointer<T> ptr, const std::size_t& shift);

template<typename T>
typename VirtualPointer<T>::ReferencesToChunksCollection VirtualPointer<T>::m_referencesToChunksCollection{};


template <typename T>
bool VirtualPointer<T>::outOfRange() const
{
	return m_chunks->empty() || (m_curChunkIdx + 1 == m_chunks->size() && static_cast<size_t>(m_curTIdx) >= m_curChunkSize);
}


template <typename T>
void VirtualPointer<T>::revalidateIndexes()
{
	if (!m_pCurrentChunk && !m_chunks->empty())
	{
		m_pCurrentChunk = (*m_chunks)[m_curChunkIdx].chunkBeginning;
	}
	if (!m_curChunkSize && m_curChunkIdx < m_chunks->size())
	{
		m_curChunkSize = (*m_chunks)[m_curChunkIdx].chunkSize;
	}
	while (m_curChunkIdx < m_chunks->size() - 1 && static_cast<size_t>(m_curTIdx) >= m_curChunkSize)
	{
		m_curTIdx -= m_curChunkSize;
		++m_curChunkIdx;
		m_pCurrentChunk = (*m_chunks)[m_curChunkIdx].chunkBeginning;
		m_curChunkSize = (*m_chunks)[m_curChunkIdx].chunkSize;
	}
}

template <typename T>
void VirtualPointer<T>::validateBytesRemaining() const
{
	if (m_endVectorIndex == m_chunks->size())
	{
		return;
	}
	for (auto i = m_endVectorIndex; i < m_chunks->size(); ++i)
	{
		m_bytesRemaining += (*m_chunks)[i].chunkSize * sizeof(T);
	}
	m_endVectorIndex = m_chunks->size();
}

template <typename T>
void VirtualPointer<T>::increaseBytesRemaining(const std::size_t value) const
{
	m_bytesRemaining += value;
}

template <typename T>
void VirtualPointer<T>::decreaseBytesRemaining(const std::size_t value) const
{
	m_bytesRemaining -= value;
}

template <typename T>
bool VirtualPointer<T>::outOfRangeWithRevalidateIndexes()
{
	if (m_chunks->empty())
	{
		return true;
	}
	revalidateIndexes();
	return outOfRange();
}

template <typename T>
T min(T f, T s)
{
	return f < s ? f : s;
}

template <typename T>
VirtualPointer<T>::VirtualPointer() :
	m_chunks(std::make_shared<std::vector<chunk>>())
{
	m_referencesToChunksCollection.addReference(m_chunks, this);
}

template <typename T>
VirtualPointer<T>::VirtualPointer(const VirtualPointer& other) :
	m_chunks(other.m_chunks),
	m_pCurrentChunk(other.m_pCurrentChunk),
	m_curChunkIdx(other.m_curChunkIdx),
	m_curTIdx(other.m_curTIdx),
	m_curChunkSize(other.m_curChunkSize),
	m_bytesRemaining(other.m_bytesRemaining),
	m_endVectorIndex(other.m_endVectorIndex)
{
	m_referencesToChunksCollection.addReference(m_chunks, this);
}

template <typename T>
VirtualPointer<T>::VirtualPointer(VirtualPointer&& other) noexcept :
	m_chunks(std::move(other.m_chunks))
{
	m_referencesToChunksCollection.removeReference(m_chunks, &other);

	m_pCurrentChunk = other.m_pCurrentChunk;
	m_curChunkIdx = other.m_curChunkIdx;
	m_curTIdx = other.m_curTIdx;
	m_curChunkSize = other.m_curChunkSize;
	m_bytesRemaining = other.m_bytesRemaining;
	m_endVectorIndex = other.m_endVectorIndex;

	m_referencesToChunksCollection.addReference(m_chunks, this);
}

template <typename T>
VirtualPointer<T>::~VirtualPointer() noexcept
{
	if (nullptr != m_chunks) {
		try {
			m_referencesToChunksCollection.removeReference(m_chunks, this);
		}
		catch (const std::exception&) {}
	}
}

template <typename T>
VirtualPointer<T>& VirtualPointer<T>::operator=(const VirtualPointer& other)
{
	if (&other != this)
	{
		m_referencesToChunksCollection.removeReference(m_chunks, this);

		m_chunks = other.m_chunks;
		m_pCurrentChunk = other.m_pCurrentChunk;
		m_curChunkIdx = other.m_curChunkIdx;
		m_curTIdx = other.m_curTIdx;
		m_curChunkSize = other.m_curChunkSize;
		m_bytesRemaining = other.m_bytesRemaining;
		m_endVectorIndex = other.m_endVectorIndex;

		m_referencesToChunksCollection.addReference(m_chunks, this);
	}
	return *this;
}

template <typename T>
VirtualPointer<T>& VirtualPointer<T>::operator=(VirtualPointer&& other) noexcept
{
	if (&other != this)
	{
		m_referencesToChunksCollection.removeReference(other.m_chunks, &other);
		m_referencesToChunksCollection.removeReference(m_chunks, this);

		m_chunks = std::move(other.m_chunks);
		m_pCurrentChunk = other.m_pCurrentChunk;
		m_curChunkIdx = other.m_curChunkIdx;
		m_curTIdx = other.m_curTIdx;
		m_curChunkSize = other.m_curChunkSize;
		m_bytesRemaining = other.m_bytesRemaining;
		m_endVectorIndex = other.m_endVectorIndex;

		m_referencesToChunksCollection.addReference(m_chunks, this);
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
	decreaseBytesRemaining(shift * sizeof(T));
	if (m_curTIdx + localShift >= m_curChunkSize)
	{
		if (m_curChunkIdx + 1 >= m_chunks->size())
		{
			m_curTIdx += localShift;
			return *this;
		}
		localShift -= m_curChunkSize - m_curTIdx;
		m_curTIdx = 0;
		++m_curChunkIdx;
		auto chunk = m_chunks->cbegin() + m_curChunkIdx;
		m_curChunkSize = chunk->chunkSize;
		++chunk;
		for (; chunk != m_chunks->cend() && localShift >= m_curChunkSize; ++chunk)
		{
			localShift -= m_curChunkSize;
			m_curChunkSize = chunk->chunkSize;
		}
		m_curChunkIdx = chunk - m_chunks->cbegin() - 1;
		m_pCurrentChunk = (*m_chunks)[m_curChunkIdx].chunkBeginning;
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
	if (dest.m_chunks->empty())
	{
		throw NullPointerException();
	}
	if (dest.outOfRangeWithRevalidateIndexes())
	{
		throw std::out_of_range("Attempt to go abroad the memory");
	}
	auto chunk = dest.m_chunks->begin() + dest.m_curChunkIdx;
	T* ptr = chunk->chunkBeginning + dest.m_curTIdx;
	for (std::size_t i = dest.m_curTIdx; i < dest.m_curChunkSize; ++i)
	{
		*ptr = static_cast<T>(value);
		if (!(count - 1))
		{
			return dest;
		}
		--count;
		++ptr;
	}
	for (++chunk; chunk < dest.m_chunks->end() - 1; ++chunk)
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
	if (chunk >= dest.m_chunks->end())
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
	if (src.m_chunks->empty() || dest.m_chunks->empty())
	{
		throw NullPointerException();
	}
	if (dest.outOfRangeWithRevalidateIndexes() || src.outOfRange())
	{
		throw std::out_of_range("Attempt to go abroad the memory");
	}
	std::size_t curChunkIdx = dest.m_curChunkIdx;

	std::size_t curSrcChunkIdx = src.m_curChunkIdx;

	std::size_t blockMemLeft = dest.m_curChunkSize - dest.m_curTIdx;
	std::size_t srcBlockMemLeft = src.m_curChunkSize - src.m_curTIdx;

	T* destPtr = (*(dest.m_chunks))[curChunkIdx].chunkBeginning + dest.m_curTIdx;
	const T* srcPtr = (*(src.m_chunks))[curSrcChunkIdx].chunkBeginning + src.m_curTIdx;

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
			if (dest.m_chunks->size() <= curChunkIdx)
			{
				memoryOver = true;
			}
			else
			{
				destPtr = (*(dest.m_chunks))[curChunkIdx].chunkBeginning;
				blockMemLeft = (*(dest.m_chunks))[curChunkIdx].chunkSize;
			}
		}
		if (!srcBlockMemLeft)
		{
			++curSrcChunkIdx;
			if (src.m_chunks->size() <= curSrcChunkIdx)
			{
				memoryOver = true;
			}
			else
			{
				srcPtr = (*(src.m_chunks))[curSrcChunkIdx].chunkBeginning;
				srcBlockMemLeft = (*(src.m_chunks))[curSrcChunkIdx].chunkSize;
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
	if (!src || dest.m_chunks->empty())
	{
		throw NullPointerException();
	}
	if (dest.outOfRangeWithRevalidateIndexes())
	{
		throw std::out_of_range("Attempt to go abroad the memory");
	}
	std::size_t curChunkIdx = dest.m_curChunkIdx;

	std::size_t blockMemLeft = dest.m_curChunkSize - dest.m_curTIdx;

	T* destPtr = (*(dest.m_chunks))[curChunkIdx].chunkBeginning + dest.m_curTIdx;
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
			if (dest.m_chunks->size() <= curChunkIdx)
			{
				memoryOver = true;
			}
			else
			{
				destPtr = (*(dest.m_chunks))[curChunkIdx].chunkBeginning;
				blockMemLeft = (*(dest.m_chunks))[curChunkIdx].chunkSize;
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
	if (!dest || src.m_chunks->empty())
	{
		throw NullPointerException();
	}
	if (src.outOfRange())
	{
		throw std::out_of_range("Attempt to go abroad the memory");
	}

	std::size_t curSrcChunkIdx = src.m_curChunkIdx;

	std::size_t srcBlockMemLeft = src.m_curChunkSize - src.m_curTIdx;

	T* destPtr = static_cast<T*>(dest);
	const T* srcPtr = (*(src.m_chunks))[curSrcChunkIdx].chunkBeginning + src.m_curTIdx;

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
			if (src.m_chunks->size() <= curSrcChunkIdx)
			{
				memoryOver = true;
			}
			else
			{
				srcPtr = (*(src.m_chunks))[curSrcChunkIdx].chunkBeginning;
				srcBlockMemLeft = (*(src.m_chunks))[curSrcChunkIdx].chunkSize;
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
	if (dest.m_chunks->empty() || src.m_chunks->empty())
	{
		throw NullPointerException();
	}
	if (dest.outOfRangeWithRevalidateIndexes() || src.outOfRange())
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
	if (dest.m_chunks->empty() || !src)
	{
		throw NullPointerException();
	}
	if (dest.outOfRangeWithRevalidateIndexes())
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
	if (!dest || src.m_chunks->empty())
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
	increaseBytesRemaining(shift * sizeof(T));
	while (static_cast<size_t>(m_curTIdx) < localShift)
	{
		if (m_curChunkIdx)
		{
			localShift -= m_curTIdx + 1;
			--m_curChunkIdx;
			m_curChunkSize = (*m_chunks)[m_curChunkIdx].chunkSize;
			m_curTIdx = m_curChunkSize - 1;
		}
		else
		{
			localShift -= m_curTIdx;
			m_curTIdx = -static_cast<signed_size_t>(localShift);
			return *this;
		}
	}
	m_curTIdx -= localShift;
	m_pCurrentChunk = (*m_chunks)[m_curChunkIdx].chunkBeginning;
	return *this;
}

template <typename T>
T& VirtualPointer<T>::operator[](std::size_t idx)
{
	revalidateIndexes();
	std::size_t curChunkIdx = m_curChunkIdx;
	std::size_t curTIdx = m_curTIdx;
	std::size_t curChunkSize = m_curChunkSize;
	if (curTIdx + idx >= curChunkSize)
	{
		idx -= curChunkSize - curTIdx;
		curTIdx = 0;
		++curChunkIdx;
		curChunkSize = (*m_chunks)[curChunkIdx].chunkSize;
		while (idx >= curChunkSize)
		{
			idx -= curChunkSize;
			++curChunkIdx;
			curChunkSize = (*m_chunks)[curChunkIdx].chunkSize;
		}
	}
	curTIdx += idx;
	return (*m_chunks)[curChunkIdx].chunkBeginning[curTIdx];
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
	revalidateIndexes();
	return *(m_pCurrentChunk + m_curTIdx);
}

template <typename T>
void VirtualPointer<T>::addChunk(T* ptr, std::size_t length)
{
	if (length)
	{
		if (nullptr != ptr)
		{
			bool tryShiftChunkIdx = false;
			if (outOfRangeWithRevalidateIndexes())
			{
				m_curTIdx -= m_curChunkSize;
				m_curChunkSize = length;
				tryShiftChunkIdx = true;
			}
			if (m_chunks->empty())
			{
				tryShiftChunkIdx = false;
			}
			m_chunks->emplace_back(chunk(ptr, length));
			if (static_cast<size_t>(m_curTIdx) < m_curChunkSize)
			{
				if (tryShiftChunkIdx)
				{
					m_curChunkIdx++;
				}
			}
			m_pCurrentChunk = (*m_chunks)[m_curChunkIdx].chunkBeginning;
		}
	}
	m_referencesToChunksCollection.validateContextForEachCopy(m_chunks);
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
	auto curOutOfRange = outOfRangeWithRevalidateIndexes();
	auto again = true;
	const auto wasEmpty = m_chunks->empty();
	while (again)
	{
		if ((*(src.m_chunks))[curSrcChunkIdx].chunkSize >= count + curSrcTIdx)
		{
			m_chunks->emplace_back(chunk((*(src.m_chunks))[curSrcChunkIdx].chunkBeginning + curSrcTIdx, count));
			again = false;
		}
		else
		{
			m_chunks->emplace_back(chunk(
				(*(src.m_chunks))[curSrcChunkIdx].chunkBeginning + curSrcTIdx,
				(*(src.m_chunks))[curSrcChunkIdx].chunkSize - curSrcTIdx
			)
			);
			count -= (*(src.m_chunks))[curSrcChunkIdx].chunkSize - curSrcTIdx;
		}
		if (curOutOfRange)
		{
			if ((*(src.m_chunks))[curSrcChunkIdx].chunkSize - curSrcTIdx >= (curTIdx - m_curChunkSize))
			{
				// out of range and zero curTIdx both mean this was empty
				if (curTIdx)
				{
					curTIdx -= m_curChunkSize;
				}
				else
				{
					// if this was empty, then current chunk index must be zero
					--curChunkIdx;
				}
			}
			else
			{
				curTIdx -= (*m_chunks)[curChunkIdx].chunkSize;
			}
			++curChunkIdx;
			if (curTIdx < static_cast<signed_size_t>(m_curChunkSize))
			{
				curOutOfRange = false;
			}
		}
		++curSrcChunkIdx;
		curSrcTIdx = 0;
	}
	m_curTIdx = curTIdx;
	m_curChunkIdx = curChunkIdx;
	m_pCurrentChunk = (*m_chunks)[m_curChunkIdx].chunkBeginning;
	m_curChunkSize = (*m_chunks)[m_curChunkIdx].chunkSize;
	m_referencesToChunksCollection.validateContextForEachCopy(m_chunks);
}

template <typename T>
std::size_t VirtualPointer<T>::bytesRemaining() const
{
	return m_bytesRemaining;
}

template <typename T>
inline void VirtualPointer<T>::clear()
{
	m_referencesToChunksCollection.removeReference(m_chunks, this);
	m_chunks = std::make_shared<std::vector<chunk>>();
	m_pCurrentChunk = nullptr;
	m_curChunkIdx = 0;
	m_curTIdx = 0;
	m_curChunkSize = 0;
	m_bytesRemaining = 0;
	m_endVectorIndex = 0;
	m_referencesToChunksCollection.addReference(m_chunks, this);
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
	if (src.m_chunks->empty() || dest.m_chunks->empty())
	{
		throw NullPointerException();
	}
	if (dest.outOfRange() || src.outOfRange())
	{
		throw std::out_of_range("Attempt to go abroad the memory");
	}
	std::size_t curChunkIdx = dest.m_curChunkIdx;

	std::size_t curSrcChunkIdx = src.m_curChunkIdx;

	std::size_t blockMemLeft = dest.m_curChunkSize - dest.m_curTIdx;
	std::size_t srcBlockMemLeft = src.m_curChunkSize - src.m_curTIdx;

	T* destPtr = (*(dest.m_chunks))[curChunkIdx].chunkBeginning + dest.m_curTIdx;
	const T* srcPtr = (*(src.m_chunks))[curSrcChunkIdx].chunkBeginning + src.m_curTIdx;

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
			if (dest.m_chunks->size() <= curChunkIdx)
			{
				memoryOver = true;
			}
			else
			{
				destPtr = (*(dest.m_chunks))[curChunkIdx].chunkBeginning;
				blockMemLeft = (*(dest.m_chunks))[curChunkIdx].chunkSize;
			}
		}
		if (!srcBlockMemLeft)
		{
			++curSrcChunkIdx;
			if (src.m_chunks->size() <= curSrcChunkIdx)
			{
				memoryOver = true;
			}
			else
			{
				srcPtr = (*(src.m_chunks))[curSrcChunkIdx].chunkBeginning;
				srcBlockMemLeft = (*(src.m_chunks))[curSrcChunkIdx].chunkSize;
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
	if (!src || dest.m_chunks->empty())
	{
		throw NullPointerException();
	}
	if (dest.outOfRange())
	{
		throw std::out_of_range("Attempt to go abroad the memory");
	}
	std::size_t curChunkIdx = dest.m_curChunkIdx;

	std::size_t blockMemLeft = dest.m_curChunkSize - dest.m_curTIdx;

	T* destPtr = (*(dest.m_chunks))[curChunkIdx].chunkBeginning + dest.m_curTIdx;
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
			if (dest.m_chunks->size() <= curChunkIdx)
			{
				memoryOver = true;
			}
			else
			{
				destPtr = (*(dest.m_chunks))[curChunkIdx].chunkBeginning;
				blockMemLeft = (*(dest.m_chunks))[curChunkIdx].chunkSize;
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
	if (!dest || src.m_chunks->empty())
	{
		throw NullPointerException();
	}
	if (src.outOfRange())
	{
		throw std::out_of_range("Attempt to go abroad the memory");
	}

	std::size_t curSrcChunkIdx = src.m_curChunkIdx;

	std::size_t srcBlockMemLeft = src.m_curChunkSize - src.m_curTIdx;

	const T* destPtr = static_cast<const T*>(dest);
	const T* srcPtr = (*(src.m_chunks))[curSrcChunkIdx].chunkBeginning + src.m_curTIdx;

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
			if (src.m_chunks->size() <= curSrcChunkIdx)
			{
				memoryOver = true;
			}
			else
			{
				srcPtr = (*(src.m_chunks))[curSrcChunkIdx].chunkBeginning;
				srcBlockMemLeft = (*(src.m_chunks))[curSrcChunkIdx].chunkSize;
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
void VirtualPointer<T>::ReferencesToChunksCollection::addReference(const std::shared_ptr<std::vector<chunk>>& chunks,
	VirtualPointer<T>* reference)
{
	assert(nullptr != chunks.get());
	if (nullptr == reference)
	{
		return;
	}
	auto& references = referencesToChunksCollections[chunks];
	auto insertingStatus = references.insert(reference);
	assert(insertingStatus.second);
}

template <typename T>
void VirtualPointer<T>::ReferencesToChunksCollection::removeReference(const std::shared_ptr<std::vector<chunk>>& chunks,
	VirtualPointer<T>* reference)
{
	assert(nullptr != chunks.get());
	if (nullptr == reference)
	{
		return;
	}
	auto& references = referencesToChunksCollections[chunks];
	auto referenceIterator = references.find(reference);
	if (references.end() == referenceIterator)
	{
		return;
	}
	references.erase(referenceIterator);
	if (references.empty())
	{
		auto referencesToChunksCollection = referencesToChunksCollections.find(chunks);
		assert(referencesToChunksCollections.end() != referencesToChunksCollection);
		referencesToChunksCollections.erase(referencesToChunksCollection);
	}
}

template <typename T>
void VirtualPointer<T>::ReferencesToChunksCollection::validateContextForEachCopy(const std::shared_ptr<std::vector<chunk>>& chunks)
{
	auto& references = referencesToChunksCollections.at(chunks);
	for (auto& reference : references)
	{
		reference->validateBytesRemaining();
	}
}

template <typename T>
void VirtualPointer<T>::toNextElement()
{
	if (m_curTIdx + 1 < 0 || m_curTIdx + 1 < static_cast<signed_size_t>(m_curChunkSize) || m_curChunkIdx + 1 >= m_chunks->size())
	{
		++m_curTIdx;
	}
	else
	{
		m_curTIdx = 0;
		++m_curChunkIdx;
		const auto& pair = (*m_chunks)[m_curChunkIdx];
		m_pCurrentChunk = pair.chunkBeginning;
		m_curChunkSize = pair.chunkSize;
	}
	decreaseBytesRemaining(sizeof(T));
}

template <typename T>
void VirtualPointer<T>::toPrevElement()
{
	increaseBytesRemaining(sizeof(T));
	--m_curTIdx;
	while (m_curTIdx < 0 && 0 != m_curChunkIdx)
	{
		--m_curChunkIdx;
		m_pCurrentChunk = (*m_chunks)[m_curChunkIdx].chunkBeginning;
		m_curChunkSize = (*m_chunks)[m_curChunkIdx].chunkSize;
		m_curTIdx += m_curChunkSize;
	}
}
