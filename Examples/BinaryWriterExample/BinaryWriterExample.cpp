#include <cstdlib>
#include <cstddef>
#include <iostream>
#include <BinaryWriter.h>
#include <VirtualPointer.h>

using std::size_t;

int main()
{
	try
	{
		// Test is correct if it runs on the little endian architecture
		constexpr auto chunkSize = 5;
		uint8_t chunk[chunkSize] = { 0 };
		BinaryWriter<uint8_t> writer(REVERSE_BYTES, !REVERSE_BITS);
		writer.setData(chunk, chunkSize);

		size_t storage = 0x00;
		writer.writeBits(8, storage);

		storage = 0x0F;
		writer.writeBits(4, storage);
		writer.writeBits(2, storage);
		writer.flush();
		// Second byte == 111111xx; x - undefined bit
		std::cout << "After flush 6 bits of the second byte is 0x" << std::hex
			<< static_cast<int>(chunk[1] >> 2) << std::dec << std::endl;
		writer.writeBits(1, storage);
		writer.writeBits(1, storage);
		writer.flush();
		std::cout << "After filling 2 bytes out of 5, " << writer.getRemainSize() << " bits remain" << std::endl;
		std::cout << "First byte was written as " << static_cast<int>(chunk[0]) << std::endl;
		std::cout << "Second byte was written as " << static_cast<int>(chunk[1]) << std::endl;

		VirtualPointer<uint8_t> vChunk{};
		vChunk.addChunk(chunk + 2, chunkSize - 2);

		writer.setData(vChunk, chunkSize - 2);
		storage = 0x030405;
		writer.writeBits(24, storage);
		
		std::cout << "Last 3 bytes were written as they are indexed: ";
		for(auto vptr = vChunk; vptr.bytesRemaining(); ++vptr)
		{
			std::cout << static_cast<int>(*vptr) << "; ";
		}
		std::cout << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Unexpected exception: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
