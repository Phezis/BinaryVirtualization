#pragma once
#include <fstream>
#include <cstddef>
#include <chrono>
#include <vector>

template<typename T>
class VirtualPointer;

class Test final
{
public:
	Test(std::fstream& logsStream, std::ostream& avoidOptimizationStream);

	void run();

	void setDepth(std::size_t depth);
	void setSummaryPacketsSize(std::size_t summaryPacketsSize);
	void setPacketsCount(std::size_t packetsCount);
	void setHeaderSize(std::size_t headerSize);
	void setPayloadSize(std::size_t payloadSize);

	std::chrono::duration<double> getCopyDuration() const;
	std::chrono::duration<double> getVptrDuration() const;

	
	using duration = std::chrono::duration<double>;

private:
	std::fstream& m_avoidOptimizationStream;
	std::ostream& m_logsStream;

	std::size_t m_depth = 0;
	std::size_t m_summaryPacketSize = 0;
	std::size_t m_packetsCount = 0;
	std::size_t m_payloadSize = 0;
	std::size_t m_headerSize = 0;

	std::vector<uint8_t>				m_originalArray{};
	std::vector<uint8_t>				m_decoderArray{};
	std::vector<std::vector<uint8_t>>	m_intermediateBuffers{};

	duration m_copyDuration = duration(0.0);
	duration m_vptrDuration = duration(0.0);

private:
	void recursiveCopy();
	void recursiveCopy(std::size_t idxTo, std::size_t idxFrom, std::size_t fromSize);
	void recursiveVptr();
	void recursiveVptr(VirtualPointer<uint8_t>& majorVptr, std::size_t depth);
	duration copyTest();
	duration vptrTest();
};

