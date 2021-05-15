#include "Test.h"

#include "VirtualPointer.h"

#include <cstdint>
#include <cstring>

using namespace std;
using namespace std::chrono;

using byte = uint8_t;

void initializeOriginalVector(vector<byte>& v)
{
	byte i = 0;
	for (auto& element : v) {
		element = i;
		++i;
	}
}

Test::Test(std::fstream& logsStream, std::ostream& avoidOptimizationStream) :
	m_avoidOptimizationStream(logsStream),
	m_logsStream(avoidOptimizationStream)
{
}

void Test::run()
{
	m_originalArray = vector<byte>(m_summaryPacketSize);
	m_decoderArray = vector<byte>(m_summaryPacketSize);
	m_intermediateBuffers = vector<vector<byte>>(m_depth);

	for(size_t i = 0; i < m_depth; ++i)
	{
		m_intermediateBuffers[i] = vector<byte>(m_summaryPacketSize);
	}

	initializeOriginalVector(m_originalArray);
	m_copyDuration = copyTest();

	initializeOriginalVector(m_originalArray);
	m_vptrDuration = vptrTest();
}

void Test::setDepth(const size_t depth)
{
	m_depth = depth;
}

void Test::setSummaryPacketsSize(const size_t summaryPacketsSize)
{
	m_summaryPacketSize = summaryPacketsSize;
}

void Test::setPacketsCount(const size_t packetsCount)
{
	m_packetsCount = packetsCount;
}

void Test::setHeaderSize(const size_t headerSize)
{
	m_headerSize = headerSize;
}

void Test::setPayloadSize(const size_t payloadSize)
{
	m_payloadSize = payloadSize;
}

std::chrono::duration<double> Test::getCopyDuration() const
{
	return m_copyDuration;
}

std::chrono::duration<double> Test::getVptrDuration() const
{
	return m_vptrDuration;
}

template<typename Ptr>
void copyToDecoderBuffer(std::vector<byte>& decoderBuffer, Ptr& lastLevel, size_t lastLevelSize)
{
	memcpy(decoderBuffer.data(), lastLevel, lastLevelSize);
}

size_t copyDeepDown(vector<byte>& to, vector<byte>& from, const size_t fromSize, const size_t headerSize, const size_t payloadSize)
{
	const auto minSize = fromSize - fromSize % (headerSize + payloadSize);
	size_t toId = 0;
	for (size_t fromId = 0; fromId < minSize - 1; toId += payloadSize, fromId += payloadSize)
	{
		fromId += headerSize;
		memcpy(to.data() + toId, from.data() + fromId, payloadSize);
	}
	return toId;
}

void copyOutside(vector<byte>& to, vector<byte>& from, const size_t toSize, const size_t headerSize, const size_t payloadSize)
{
	const auto minSize = toSize - toSize % (headerSize + payloadSize);
	
	for (size_t toId = 0, fromId = 0; toId < minSize - 1; toId += payloadSize, fromId += payloadSize)
	{
		toId += headerSize;
		/*const auto to_ = to.data() + toId;
		const auto from_ = from.data() + fromId;
		for (size_t i = 0; i < payloadSize; ++i)
		{
			to_[i] = from_[i];
		}*/
		memcpy(to.data() + toId, from.data() + fromId, payloadSize);
	}
}

void Test::recursiveCopy()
{
	const auto copiedSize = copyDeepDown(m_intermediateBuffers[0], m_originalArray, m_originalArray.size(), m_headerSize, m_payloadSize);

	if (m_depth > 1) 
	{
		recursiveCopy(1, 0, copiedSize);
	}
	else
	{
		for (auto& element : m_intermediateBuffers[0])
		{
			++element;
		}
	}

	copyOutside(m_originalArray, m_intermediateBuffers[0], m_originalArray.size(), m_headerSize, m_payloadSize);
}

void Test::recursiveCopy(const size_t idxTo, const size_t idxFrom, const size_t fromSize)
{
	const auto copiedSize = copyDeepDown(m_intermediateBuffers[idxTo], m_intermediateBuffers[idxFrom], fromSize, m_headerSize, m_payloadSize);

	if (m_depth > idxTo + 1)
	{
		recursiveCopy(idxTo + 1, idxTo, copiedSize);
	}
	else
	{
		for (size_t i = 0; i < copiedSize; ++i) {
			++m_intermediateBuffers[idxTo][i];
		}
	}

	copyOutside(m_intermediateBuffers[idxFrom], m_intermediateBuffers[idxTo], fromSize, m_headerSize, m_payloadSize);
}

void Test::recursiveVptr()
{
	VirtualPointer<byte> vptr{};

	const size_t minSize = m_summaryPacketSize - m_summaryPacketSize % static_cast<size_t>(m_headerSize + m_payloadSize);
	for (size_t fromId = 0; fromId < minSize - 1; fromId += m_payloadSize)
	{
		fromId += m_headerSize;
		vptr.addChunk(m_originalArray.data() + fromId, m_payloadSize);
	}

	if (m_depth > 1)
	{
		recursiveVptr(vptr, 1);
	}
	else
	{
		for (auto vp = vptr; vp.bytesRemaining(); ++vp)
		{
			++(*vp);
		}
		copyToDecoderBuffer(m_decoderArray, vptr, vptr.bytesRemaining());
	}
}

void Test::recursiveVptr(VirtualPointer<byte>& majorVptr, const size_t depth)
{
	VirtualPointer<byte> vptr{};
	
	for (auto fromVptr = majorVptr; fromVptr.bytesRemaining() >= static_cast<size_t>(m_headerSize + m_payloadSize); fromVptr += m_payloadSize)
	{
		fromVptr += m_headerSize;
		vptr.addChunk(fromVptr, m_payloadSize);
	}

	if (m_depth - 1 > depth)
	{
		recursiveVptr(vptr, depth + 1);
	}
	else
	{
		auto vp = vptr;
		for (; vp.bytesRemaining(); ++vp)
		{
			++(*vp);
		}
		copyToDecoderBuffer(m_decoderArray, vptr, vptr.bytesRemaining());
	}
}

Test::duration Test::copyTest()
{
	const auto start = steady_clock::now();
	for (size_t i = 0; i < m_packetsCount; ++i)
	{
		recursiveCopy();
	}
	const auto duration = steady_clock::now() - start;

	size_t originalArrayElementsSum = 0;
	for (auto& element : m_originalArray)
	{
		originalArrayElementsSum += element;
	}

	m_logsStream << "Copy test: depth = " << m_depth << "; packet bytesRemaining = " << m_summaryPacketSize << "; Elapsed time = " << duration.count() << endl;
	m_avoidOptimizationStream << "Copy test: depth = " << m_depth << "; packet bytesRemaining = " << m_summaryPacketSize << "; original array elements sum = " << originalArrayElementsSum << endl;
	return duration;
}

Test::duration Test::vptrTest()
{
	const auto start = steady_clock::now();
	for (size_t i = 0; i < m_packetsCount; ++i)
	{
		recursiveVptr();
	}
	const auto duration = steady_clock::now() - start;

	size_t originalArrayElementsSum = 0;
	for (auto& element : m_originalArray)
	{
		originalArrayElementsSum += element;
	}

	m_logsStream << "Vptr test: depth = " << m_depth << "; packet bytesRemaining = " << m_summaryPacketSize << "; Elapsed time = " << duration.count() << endl;
	m_avoidOptimizationStream << "Vptr test: depth = " << m_depth << "; packet bytesRemaining = " << m_summaryPacketSize << "; original array elements sum = " << originalArrayElementsSum << endl;
	return duration;
}
