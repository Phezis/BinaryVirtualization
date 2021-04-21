#include <cstdlib>
#include <cstddef>
#include <iostream>
#include <BinaryReader.h>
#include <VirtualPointer.h>

using std::size_t;

template<typename T, size_t N>
constexpr size_t sizeOfArray(const T(&)[N]) { return N; }

int main()
{
	try
	{
		// Test is correct if it runs on the little endian architecture
		uint8_t chunk[] = { 0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
		constexpr auto chunkSize = sizeOfArray(chunk);
		BinaryReader<uint8_t> reader(REVERSE_BYTES, !REVERSE_BITS);
		reader.setData(chunk, chunkSize);

		size_t storage;
		reader.readBits(12, storage);
		std::cout << "First 12 bits is " << storage << std::endl;
		
		reader.readBits(3, storage);
		std::cout << "Next 3 bits is " << storage << std::endl;
		
		reader.readBits(1, storage);
		std::cout << "Last bit of the first 16 bits is " << storage << std::endl;
		
		reader.skipBits(6);
		reader.lookBits(2, storage);
		std::cout << "Look at the last 2 bits of the 3rd byte. It is " << storage << std::endl;
		reader.readBits(2, storage);
		std::cout << "And read the last 2 bits of the 3rd byte. It is " << storage << " again" << std::endl;

		std::cout << "From the beginning 3 bytes were read. They are equal to "
			<< reader.getReadBitsCount() << " bits" << std::endl;
		reader.resetReadBitsCount();

		reader.skipBits(40);
		reader.readBits(8, storage);
		std::cout << "The last byte is " << storage << std::endl;
		std::cout << reader.getReadBitsCount() << " bits from the last checkpoint were read and skipped" << std::endl;

		VirtualPointer<uint8_t> vChunk{};
		vChunk.addChunk(chunk, chunkSize);

		reader.setData(vChunk, chunkSize);
		reader.skipBits(8 * 4);
		reader.readBits(8, storage);
		std::cout << "Usage of VirtualPointer in the BinaryReader is same: 5th byte is " << storage << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Unexpected exception: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
