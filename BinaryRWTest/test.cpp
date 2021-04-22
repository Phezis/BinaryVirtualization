#include "pch.h"
#include "ArraysTest.h"
#include "BinaryReader.h"
#include "BinaryWriter.h"

// input data is <bit endian> and <byte endian>
#define BB false, true
#define BL false, false
#define LB true, true
#define LL true, false

#define BB_SET reader.setReverseBits(false); reader.setReverseBytes(true);
#define BL_SET reader.setReverseBits(false); reader.setReverseBytes(false);
#define LB_SET reader.setReverseBits(true); reader.setReverseBytes(true);
#define LL_SET reader.setReverseBits(true); reader.setReverseBytes(false);

#if defined(_M_X64) || defined(__amd64__)
TEST(TestBinaryReader, TestLookAndSkip) {
	/*
		Huge test
		(consider that test are run on LE computer)
	*/
	// Test no reverse
	const size_t read1To32Count = 2048;
	const size_t maxBitsCount = 32;
	const size_t dataSize = (1 + 32) * 16 * 2048;

	BinaryReader<uint8_t> reader{ true };
	BB_SET
		reader.setData(packed_notReversed, dataSize);

	size_t container;

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.lookBits(j, container);
			reader.skipBits(j);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse bytes
	BL_SET
		reader.setData(packed_bytesReversed_64, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.lookBits(j, container);
			reader.skipBits(j);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse bits
	LB_SET
		reader.setData(packed_bitsReversed, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.lookBits(j, container);
			reader.skipBits(j);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse all
	LL_SET
		reader.setData(packed_allReversed_64, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.lookBits(j, container);
			reader.skipBits(j);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}
}
#else
TEST(TestBinaryReader, TestLookAndSkip) {
	/*
		Huge test
		(consider that test are run on LE computer)
	*/
	// Test no reverse
	const size_t read1To32Count = 2048;
	const size_t maxBitsCount = 32;
	const size_t dataSize = (1 + 32) * 16 * 2048;

	BinaryReader<uint8_t> reader{ true };
	BB_SET
		reader.setData(packed_notReversed, dataSize);

	size_t container;

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.lookBits(j, container);
			reader.skipBits(j);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse bytes
	BL_SET
		reader.setData(packed_bytesReversed, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.lookBits(j, container);
			reader.skipBits(j);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse bits
	LB_SET
		reader.setData(packed_bitsReversed, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.lookBits(j, container);
			reader.skipBits(j);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse all
	LL_SET
		reader.setData(packed_allReversed, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.lookBits(j, container);
			reader.skipBits(j);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}
}
#endif

TEST(TestBinaryReader, TestSkipping) {
	const size_t size = 256;
	uint8_t memory[size] = { 0 };
	BinaryReader<uint8_t> reader{ false };

	EXPECT_FALSE(reader.skipBits(1));

	reader.setData(memory, size);
	EXPECT_TRUE(reader.skipBits(0));
	EXPECT_TRUE(reader.skipBits(1));

	// test long skip
		// without any reverses
	BB_SET
		memory[size - 1] = uint8_t(0xC1);
	memory[size - 2] = 0;
	reader.setData(memory, size);
	EXPECT_TRUE(reader.skipBits((size - 1) * 8));
	size_t val = 0;
	reader.readBits(8, val);
	EXPECT_EQ(0xC1, val);
	EXPECT_FALSE(reader.skipBits(2));

	memory[size - 1] = 0;
	memory[size - 2] = uint8_t(0xC1);
	reader.setData(memory + 1, size - 2);
	EXPECT_TRUE(reader.skipBits((size - 3) * 8));
	val = 0;
	reader.readBits(8, val);
	EXPECT_EQ(0xC1, val);
	// reverse bytes
	// the test situation is no longer supported
	BL_SET
		memory[size - sizeof(size_t)] = uint8_t(0xC1);
	memory[size - 2] = 0;
	reader.setData(memory, size);
	EXPECT_TRUE(reader.skipBits((size - 1) * 8));
	val = 0;
	reader.readBits(8, val);
	EXPECT_EQ(0xC1, val);
	EXPECT_FALSE(reader.skipBits(2));

	/*reader.setData(memory + 1, bytesRemaining - 2);
	EXPECT_TRUE(reader.skipBits((bytesRemaining - 3) * 8));
	val = 0;
	EXPECT_TRUE(reader.readBits(8, val));
	EXPECT_EQ(0xC1, val);*/
	// reverse bits
	LB_SET
		memory[size - 1] = uint8_t(0xC1);
	memory[size - sizeof(size_t)] = 0;
	reader.setData(memory, size);
	EXPECT_TRUE(reader.skipBits((size - 1) * 8));
	val = 0;
	reader.readBits(8, val);
	EXPECT_EQ(0x83, val);
	EXPECT_FALSE(reader.skipBits(2));

	memory[size - 1] = 0;
	memory[size - 2] = uint8_t(0xC1);
	reader.setData(memory + 1, size - 2);
	EXPECT_TRUE(reader.skipBits((size - 3) * 8));
	val = 0;
	reader.readBits(8, val);
	EXPECT_EQ(0x83, val);
	// reverse all
	LL_SET
		memory[size - sizeof(size_t)] = uint8_t(0xC1);
	memory[size - 2] = 0;
	reader.setData(memory, size);
	EXPECT_TRUE(reader.skipBits((size - 1) * 8));
	val = 0;
	reader.readBits(8, val);
	EXPECT_EQ(0x83, val);
	EXPECT_FALSE(reader.skipBits(2));

	// the test situation is no longer supported
	/*reader.setData(memory + 1, bytesRemaining - 2);
	EXPECT_TRUE(reader.skipBits((bytesRemaining - 3) * 8));
	val = 0;
	reader.readBits(8, val);
	EXPECT_EQ(0x83, val);*/

	//
	reader.setData(memory, size);
	EXPECT_TRUE(reader.skipBits(size * 8));
	EXPECT_FALSE(reader.skipBits(1));

	reader.setData(memory, size);
	EXPECT_TRUE(reader.skipBits(7) && reader.skipBits(1));

	reader.setData(memory, size);
	EXPECT_FALSE(reader.skipBits(size * 8 + 1));

	EXPECT_FALSE(reader.skipBits(size * 8 * 16));

	// test one byte
	uint8_t a[] = { 0xC1, 0, 0, 0x83 };
	BB_SET
		reader.setData(a, 1);
	reader.skipBits(1);
	reader.readBits(7, val);
	EXPECT_EQ(0x41, val);
	BL_SET
		reader.setData(a, 1);
	reader.skipBits(1);
	reader.readBits(7, val);
	EXPECT_EQ(0x41, val);
	LB_SET
		reader.setData(a, 1);
	reader.skipBits(1);
	reader.readBits(7, val);
	EXPECT_EQ(0x03, val);
	LL_SET
		reader.setData(a, 1);
	reader.skipBits(1);
	reader.readBits(7, val);
	EXPECT_EQ(0x03, val);


	// test two bytes
	BB_SET
		reader.setData(a, 2);
	reader.skipBits(1);
	reader.readBits(15, val);
	EXPECT_EQ(0x4100, val);
	BL_SET
		reader.setData(a, 2);
	reader.skipBits(1);
	reader.readBits(15, val);
	EXPECT_EQ(0x00C1, val);
	LB_SET
		reader.setData(a, 2);
	reader.skipBits(1);
	reader.readBits(15, val);
	EXPECT_EQ(0x0300, val);
	LL_SET
		reader.setData(a, 2);
	reader.skipBits(1);
	reader.readBits(15, val);
	EXPECT_EQ(0x0083, val);

	// test four bytes
	BB_SET
		reader.setData(a, 4);
	reader.skipBits(17);
	reader.readBits(15, val);
	EXPECT_EQ(0x0083, val);
	BL_SET
		reader.setData(a, 4);
	reader.skipBits(17);
	reader.readBits(15, val);
	EXPECT_EQ(0x00C1, val);
	LB_SET
		reader.setData(a, 4);
	reader.skipBits(17);
	reader.readBits(15, val);
	EXPECT_EQ(0x00C1, val);
	LL_SET
		reader.setData(a, 4);
	reader.skipBits(17);
	reader.readBits(15, val);
	EXPECT_EQ(0x0083, val);
}

#if defined(_M_X64) || defined(__amd64__)
TEST(TestBinaryReader, TestLooking) {
	size_t size = sizeof(size_t) * 8;
	uint8_t onlyZeroes[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t onlyOnes[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	BinaryReader<uint8_t> reader{ false };
	BB_SET

		reader.setData(onlyZeroes, size);

	size_t value;

	EXPECT_FALSE(reader.lookBits(sizeof(size_t) * 8 + 1, value));

	value = 1;
	EXPECT_TRUE(reader.lookBits(0, value));
	EXPECT_EQ(0, value);

	value = -1;
	EXPECT_TRUE(reader.lookBits(1, value));
	EXPECT_EQ(0, value);

	value = 0xFF;
	EXPECT_TRUE(reader.lookBits(7, value));
	EXPECT_EQ(0, value);

	value = 0x1FF;
	EXPECT_TRUE(reader.lookBits(8, value));
	EXPECT_EQ(0, value);

	value = 0x3FF;
	EXPECT_TRUE(reader.lookBits(9, value));
	EXPECT_EQ(0, value);

	reader.setData(onlyOnes, size);

	EXPECT_TRUE(reader.lookBits(0, value));
	EXPECT_EQ(0, value);

	EXPECT_TRUE(reader.lookBits(1, value));
	EXPECT_EQ(1, value);

	EXPECT_TRUE(reader.lookBits(7, value));
	EXPECT_EQ(0x7F, value);

	EXPECT_TRUE(reader.lookBits(8, value));
	EXPECT_EQ(0xFF, value);

	EXPECT_TRUE(reader.lookBits(9, value));
	EXPECT_EQ(0x1FF, value);

	EXPECT_TRUE(reader.lookBits(15, value));
	EXPECT_EQ(0x7FFF, value);

	EXPECT_TRUE(reader.lookBits(16, value));
	EXPECT_EQ(0xFFFF, value);

	EXPECT_TRUE(reader.lookBits(17, value));
	EXPECT_EQ(0x1FFFF, value);

	EXPECT_TRUE(reader.lookBits(sizeof(size_t) * 8 - 1, value));
	EXPECT_EQ(0x7FFFFFFFFFFFFFFF, value);

	EXPECT_TRUE(reader.lookBits(sizeof(size_t) * 8, value));
	EXPECT_EQ(0xFFFFFFFFFFFFFFFF, value);

	EXPECT_TRUE(reader.skipBits(7));
	EXPECT_TRUE(reader.lookBits(8, value));
	EXPECT_EQ(0xFF, value);
}
#else
TEST(TestBinaryReader, TestLooking) {
	size_t bytesRemaining = sizeof(size_t) * 8;
	uint8_t onlyZeroes[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t onlyOnes[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	BinaryReader<uint8_t> reader{ false };
	BB_SET

		reader.setData(onlyZeroes, bytesRemaining);

	size_t value;

	EXPECT_FALSE(reader.lookBits(sizeof(size_t) * 8 + 1, value));

	value = 1;
	EXPECT_TRUE(reader.lookBits(0, value));
	EXPECT_EQ(0, value);

	value = -1;
	EXPECT_TRUE(reader.lookBits(1, value));
	EXPECT_EQ(0, value);

	value = 0xFF;
	EXPECT_TRUE(reader.lookBits(7, value));
	EXPECT_EQ(0, value);

	value = 0x1FF;
	EXPECT_TRUE(reader.lookBits(8, value));
	EXPECT_EQ(0, value);

	value = 0x3FF;
	EXPECT_TRUE(reader.lookBits(9, value));
	EXPECT_EQ(0, value);

	reader.setData(onlyOnes, bytesRemaining);

	EXPECT_TRUE(reader.lookBits(1, value));
	EXPECT_EQ(1, value);

	EXPECT_TRUE(reader.lookBits(7, value));
	EXPECT_EQ(0x7F, value);

	EXPECT_TRUE(reader.lookBits(8, value));
	EXPECT_EQ(0xFF, value);

	EXPECT_TRUE(reader.lookBits(9, value));
	EXPECT_EQ(0x1FF, value);

	EXPECT_TRUE(reader.lookBits(15, value));
	EXPECT_EQ(0x7FFF, value);

	EXPECT_TRUE(reader.lookBits(16, value));
	EXPECT_EQ(0xFFFF, value);

	EXPECT_TRUE(reader.lookBits(17, value));
	EXPECT_EQ(0x1FFFF, value);

	EXPECT_TRUE(reader.lookBits(sizeof(size_t) * 8 - 1, value));
	EXPECT_EQ(0x7FFFFFFF, value);

	EXPECT_TRUE(reader.lookBits(sizeof(size_t) * 8, value));
	EXPECT_EQ(0xFFFFFFFF, value);

	EXPECT_TRUE(reader.skipBits(7));
	EXPECT_TRUE(reader.lookBits(8, value));
	EXPECT_EQ(0xFF, value);
}
#endif


#if defined(_M_X64) || defined(__amd64__)
TEST(TestBinaryWriter, TestWriting) {

	uint8_t a[16] = { 0 };
	uint8_t b[16] = { 0 };
	uint8_t c[16] = { 0 };
	uint8_t d[18] = { 0 };
	BinaryWriter<uint8_t> writer{ false };
	writer.setData(a, 16);
	// Test like we are big endian
	writer.setReverseBits(false);
	writer.setReverseBytes(false);
	size_t val = 0xF0;
	for (uint8_t i = 0; i < 16; ++i) {
		EXPECT_TRUE(writer.writeBits(8, val));
	}
	EXPECT_EQ(0, writer.getRemainSize());
	EXPECT_FALSE(writer.writeBits(8, val));
	for (uint8_t i = 0; i < 16; ++i) {
		EXPECT_EQ(0xF0, a[i]);
	}

	writer.setData(b, 16);
	val = 0x0F0F0F0F;
	for (uint8_t i = 0; i < 4; ++i) {
		EXPECT_TRUE(writer.writeBits(32, val));
	}
	EXPECT_EQ(0, writer.getRemainSize());
	for (uint8_t i = 0; i < 16; ++i) {
		EXPECT_EQ(0x0F, b[i]);
	}

	writer.setData(c, 16);
	for (uint8_t i = 0; i < 32; ++i) {
		EXPECT_TRUE(writer.writeBits(2, 0x3));
		EXPECT_TRUE(writer.writeBits(2, 0));
	}
	EXPECT_EQ(0, writer.getRemainSize());
	for (uint8_t i = 0; i < 16; ++i) {
		EXPECT_EQ(0xCC, c[i]);
	}
	// next situation cannot be tested in little endian architecture
	writer.setReverseBytes(true);
	writer.setData(d, 17);
	for (uint8_t i = 0; i < 7; ++i) {
		EXPECT_TRUE(writer.writeBits(9, 0x155));
		EXPECT_TRUE(writer.writeBits(9, 0xAA));
	}
	EXPECT_TRUE(writer.writeBits(10, 0x2AA));
	EXPECT_EQ(0, writer.getRemainSize());
	EXPECT_FALSE(writer.writeBits(8, val));
	for (uint8_t i = 0; i < 17; ++i) {
		EXPECT_EQ(0xAA, d[i]);
	}
	// Test bit and byte swap works correctly
	std::fill(a, a + 16, 0);
	std::fill(b, b + 16, 0);
	std::fill(c, c + 16, 0);
	std::fill(d, d + 18, 0);
	writer.setReverseBytes(true);
	writer.setData(a, 16);
	val = 0xFF00FF00;
	for (uint8_t i = 0; i < 4; ++i) {
		EXPECT_TRUE(writer.writeBits(32, val));
	}
	EXPECT_EQ(0, writer.getRemainSize());
	for (uint8_t i = 0; i < 16; i += 2) {
		EXPECT_EQ(0xFF, a[i]);
		EXPECT_EQ(0x00, a[i + 1]);
	}
	writer.setReverseBytes(false);
	writer.setReverseBits(true);
	writer.setData(b, 16);
	val = 0xF0F0F0F0;
	for (uint8_t i = 0; i < 4; ++i) {
		EXPECT_TRUE(writer.writeBits(32, val));
	}
	EXPECT_EQ(0, writer.getRemainSize());
	for (uint8_t i = 0; i < 16; ++i) {
		EXPECT_EQ(0x0F, b[i]);
	}
	writer.setReverseBytes(true);
	writer.setData(c, 16);
	val = 0xF00FF00F;
	for (uint8_t i = 0; i < 4; ++i) {
		EXPECT_TRUE(writer.writeBits(32, val));
	}
	EXPECT_EQ(0, writer.getRemainSize());
	for (uint8_t i = 0; i < 16; i += 2) {
		EXPECT_EQ(0x0F, c[i]);
		EXPECT_EQ(0xF0, c[i + 1]);
	}



	/*
		Huge test
		(consider that test are run on LE computer)
	*/
	// Test no reverse
	const size_t read1To32Count = 2048;
	const size_t maxBitsCount = 32;
	const size_t dataSize = (1 + 32) * 16 * 2048;
	uint8_t* writingTo = new uint8_t[dataSize];

	writer.setReverseBits(false);
	writer.setReverseBytes(true);
	writer.setData(writingTo, dataSize);

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			writer.writeBits(j, notPackedBits[i * maxBitsCount + j - 1]);
		}
	}
	writer.flush();

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			EXPECT_EQ(size_t(packed_notReversed[i * maxBitsCount + j - 1]), writingTo[i * maxBitsCount + j - 1]);
		}
	}

	// Test reverse bytes
	writer.setReverseBytes(false);
	writer.setData(writingTo, dataSize);

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			writer.writeBits(j, notPackedBits[i * maxBitsCount + j - 1]);
		}
	}
	writer.flush();

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			EXPECT_EQ(size_t(packed_bytesReversed_64[i * maxBitsCount + j - 1]), writingTo[i * maxBitsCount + j - 1]);
		}
	}

	// Test reverse bits
	writer.setReverseBytes(true);
	writer.setReverseBits(true);
	writer.setData(writingTo, dataSize);

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			writer.writeBits(j, notPackedBits[i * maxBitsCount + j - 1]);
		}
	}
	writer.flush();

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			EXPECT_EQ(size_t(packed_bitsReversed[i * maxBitsCount + j - 1]), writingTo[i * maxBitsCount + j - 1]);
		}
	}

	// Test reverse all
	writer.setReverseBytes(false);
	writer.setData(writingTo, dataSize);

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			writer.writeBits(j, notPackedBits[i * maxBitsCount + j - 1]);
		}
	}
	writer.flush();

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			EXPECT_EQ(size_t(packed_allReversed_64[i * maxBitsCount + j - 1]), writingTo[i * maxBitsCount + j - 1]);
		}
	}
	delete[] writingTo;
}
#else
TEST(TestBinaryWriter, TestWriting) {

	uint8_t a[16] = { 0 };
	uint8_t b[16] = { 0 };
	uint8_t c[16] = { 0 };
	uint8_t d[18] = { 0 };
	BinaryWriter<uint8_t> writer{ false };
	writer.setData(a, 16);
	// Test like we are big endian
	writer.setReverseBits(false);
	writer.setReverseBytes(false);
	size_t val = 0xF0;
	for (uint8_t i = 0; i < 16; ++i) {
		EXPECT_TRUE(writer.writeBits(8, val));
	}
	EXPECT_EQ(0, writer.getRemainSize());
	EXPECT_FALSE(writer.writeBits(8, val));
	for (uint8_t i = 0; i < 16; ++i) {
		EXPECT_EQ(0xF0, a[i]);
	}

	writer.setData(b, 16);
	val = 0x0F0F0F0F;
	for (uint8_t i = 0; i < 4; ++i) {
		EXPECT_TRUE(writer.writeBits(32, val));
	}
	EXPECT_EQ(0, writer.getRemainSize());
	for (uint8_t i = 0; i < 16; ++i) {
		EXPECT_EQ(0x0F, b[i]);
	}

	writer.setData(c, 16);
	for (uint8_t i = 0; i < 32; ++i) {
		EXPECT_TRUE(writer.writeBits(2, 0x3));
		EXPECT_TRUE(writer.writeBits(2, 0));
	}
	EXPECT_EQ(0, writer.getRemainSize());
	for (uint8_t i = 0; i < 16; ++i) {
		EXPECT_EQ(0xCC, c[i]);
	}
	// next situation cannot be tested in little endian architecture
	writer.setReverseBytes(true);
	writer.setData(d, 17);
	for (uint8_t i = 0; i < 7; ++i) {
		EXPECT_TRUE(writer.writeBits(9, 0x155));
		EXPECT_TRUE(writer.writeBits(9, 0xAA));
	}
	EXPECT_TRUE(writer.writeBits(10, 0x2AA));
	EXPECT_EQ(0, writer.getRemainSize());
	EXPECT_FALSE(writer.writeBits(8, val));
	for (uint8_t i = 0; i < 17; ++i) {
		EXPECT_EQ(0xAA, d[i]);
	}
	// Test bit and byte swap works correctly
	std::fill(a, a + 16, 0);
	std::fill(b, b + 16, 0);
	std::fill(c, c + 16, 0);
	std::fill(d, d + 18, 0);
	writer.setReverseBytes(true);
	writer.setData(a, 16);
	val = 0xFF00FF00;
	for (uint8_t i = 0; i < 4; ++i) {
		EXPECT_TRUE(writer.writeBits(32, val));
	}
	EXPECT_EQ(0, writer.getRemainSize());
	for (uint8_t i = 0; i < 16; i += 2) {
		EXPECT_EQ(0xFF, a[i]);
		EXPECT_EQ(0x00, a[i + 1]);
	}
	writer.setReverseBytes(false);
	writer.setReverseBits(true);
	writer.setData(b, 16);
	val = 0xF0F0F0F0;
	for (uint8_t i = 0; i < 4; ++i) {
		EXPECT_TRUE(writer.writeBits(32, val));
	}
	EXPECT_EQ(0, writer.getRemainSize());
	for (uint8_t i = 0; i < 16; ++i) {
		EXPECT_EQ(0x0F, b[i]);
	}
	writer.setReverseBytes(true);
	writer.setData(c, 16);
	val = 0xF00FF00F;
	for (uint8_t i = 0; i < 4; ++i) {
		EXPECT_TRUE(writer.writeBits(32, val));
	}
	EXPECT_EQ(0, writer.getRemainSize());
	for (uint8_t i = 0; i < 16; i += 2) {
		EXPECT_EQ(0x0F, c[i]);
		EXPECT_EQ(0xF0, c[i + 1]);
	}



	/*
		Huge test
		(consider that test are run on LE computer)
	*/
	// Test no reverse
	const size_t read1To32Count = 2048;
	const size_t maxBitsCount = 32;
	const size_t dataSize = (1 + 32) * 16 * 2048;
	uint8_t* writingTo = new uint8_t[dataSize];

	writer.setReverseBits(false);
	writer.setReverseBytes(true);
	writer.setData(writingTo, dataSize);

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			writer.writeBits(j, notPackedBits[i * maxBitsCount + j - 1]);
		}
	}
	writer.flush();

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			EXPECT_EQ(size_t(packed_notReversed[i * maxBitsCount + j - 1]), writingTo[i * maxBitsCount + j - 1]);
		}
	}

	// Test reverse bytes
	writer.setReverseBytes(false);
	writer.setData(writingTo, dataSize);

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			writer.writeBits(j, notPackedBits[i * maxBitsCount + j - 1]);
		}
	}
	writer.flush();

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			EXPECT_EQ(size_t(packed_bytesReversed[i * maxBitsCount + j - 1]), writingTo[i * maxBitsCount + j - 1]);
		}
	}

	// Test reverse bits
	writer.setReverseBytes(true);
	writer.setReverseBits(true);
	writer.setData(writingTo, dataSize);

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			writer.writeBits(j, notPackedBits[i * maxBitsCount + j - 1]);
		}
	}
	writer.flush();

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			EXPECT_EQ(size_t(packed_bitsReversed[i * maxBitsCount + j - 1]), writingTo[i * maxBitsCount + j - 1]);
		}
	}

	// Test reverse all
	writer.setReverseBytes(false);
	writer.setData(writingTo, dataSize);

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			writer.writeBits(j, notPackedBits[i * maxBitsCount + j - 1]);
		}
	}
	writer.flush();

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			EXPECT_EQ(size_t(packed_allReversed[i * maxBitsCount + j - 1]), writingTo[i * maxBitsCount + j - 1]);
		}
	}
	delete[] writingTo;
}
#endif

TEST(TestBinaryWriter, TestFlush) {
	uint8_t a[20] = { 0 };
	BinaryWriter<uint8_t> writer{ false };
	writer.setData(a + 2, 16);
	// Now available only little endian test
	writer.setReverseBits(false);
	writer.setReverseBytes(true);
	size_t val = 0xF0;
	for (uint8_t i = 2; i < 18; ++i) {
		EXPECT_TRUE(writer.writeBits(8, val));
		writer.flush();
		EXPECT_EQ(0xF0, a[i]);
	}
	EXPECT_EQ(0, writer.getRemainSize());
	EXPECT_FALSE(writer.writeBits(8, val));
	for (uint8_t i = 2; i < 18; ++i) {
		EXPECT_EQ(0xF0, a[i]);
	}

	std::fill(a, a + 20, 0);
	writer.setData(a + 2, 16);

	for (uint8_t i = 2; i < 18; ++i) {
		EXPECT_TRUE(writer.writeBits(4, 0xF));
		writer.flush();
		EXPECT_EQ(0xF0, a[i]);
		EXPECT_TRUE(writer.writeBits(4, 0xF));
		writer.flush();
		EXPECT_EQ(0xFF, a[i]);
	}
	EXPECT_EQ(0, writer.getRemainSize());
	EXPECT_FALSE(writer.writeBits(8, val));
	for (uint8_t i = 2; i < 18; ++i) {
		EXPECT_EQ(0xFF, a[i]);
	}

	std::fill(a, a + 20, 0);
	writer.setData(a + 2, 16);

	for (uint8_t i = 2; i < 18; ++i) {
		EXPECT_TRUE(writer.writeBits(1, 0x1));
		writer.flush();
		EXPECT_EQ(0x80, a[i]);
		EXPECT_TRUE(writer.writeBits(1, 0x1));
		writer.flush();
		EXPECT_EQ(0xC0, a[i]);
		EXPECT_TRUE(writer.writeBits(1, 0x1));
		writer.flush();
		EXPECT_EQ(0xE0, a[i]);
		EXPECT_TRUE(writer.writeBits(1, 0x1));
		writer.flush();
		EXPECT_EQ(0xF0, a[i]);
		EXPECT_TRUE(writer.writeBits(1, 0x1));
		writer.flush();
		EXPECT_EQ(0xF8, a[i]);
		EXPECT_TRUE(writer.writeBits(1, 0x1));
		writer.flush();
		EXPECT_EQ(0xFC, a[i]);
		EXPECT_TRUE(writer.writeBits(1, 0x1));
		writer.flush();
		EXPECT_EQ(0xFE, a[i]);
		EXPECT_TRUE(writer.writeBits(1, 0x1));
		writer.flush();
		EXPECT_EQ(0xFF, a[i]);
	}
	EXPECT_EQ(0, writer.getRemainSize());
	EXPECT_FALSE(writer.writeBits(8, val));
	for (uint8_t i = 2; i < 18; ++i) {
		EXPECT_EQ(0xFF, a[i]);
	}
}

#if defined(_M_X64) || defined(__amd64__)
TEST(TestTogether, TestReadWrite) {
	// i have little endian so for tests sometime need reverse for simulating big or little endianes
	const size_t arrSize = 256;
	uint8_t a[arrSize] = { 0 };
	BinaryWriter<uint8_t> testWriter{ false };
	BinaryReader<uint8_t> testReader{ false };
	size_t val;
	// without any reverse
	testWriter.setData(a, arrSize);
	for (size_t i = 0; i < arrSize; i += 8) {
		EXPECT_TRUE(testWriter.writeBits(8, i));
		EXPECT_TRUE(testWriter.writeBits(8, i + 1));
		EXPECT_TRUE(testWriter.writeBits(8, i + 2));
		EXPECT_TRUE(testWriter.writeBits(8, i + 3));
		EXPECT_TRUE(testWriter.writeBits(8, i + 4));
		EXPECT_TRUE(testWriter.writeBits(8, i + 5));
		EXPECT_TRUE(testWriter.writeBits(8, i + 6));
		EXPECT_TRUE(testWriter.writeBits(8, i + 7));
		EXPECT_EQ(i + 7, a[i + 0]);
		EXPECT_EQ(i + 6, a[i + 1]);
		EXPECT_EQ(i + 5, a[i + 2]);
		EXPECT_EQ(i + 4, a[i + 3]);
		EXPECT_EQ(i + 3, a[i + 4]);
		EXPECT_EQ(i + 2, a[i + 5]);
		EXPECT_EQ(i + 1, a[i + 6]);
		EXPECT_EQ(i + 0, a[i + 7]);
	}
	testWriter.flush();
	testReader.setData(a, arrSize);
	for (size_t i = 0; i < arrSize; ++i) {
		if (i == arrSize - 9) {
			val = 0;
		}
		EXPECT_TRUE(testReader.readBits(8, val));
		EXPECT_EQ(i, val);
	}
	std::fill(a, a + arrSize, 0);

	// reverse bytes
	testWriter.setReverseBytes(true);
	testWriter.setReverseBits(false);
	testReader.setReverseBytes(true);
	testReader.setReverseBits(false);
	testWriter.setData(a, arrSize);
	for (size_t i = 0; i < arrSize; i += 8) {
		EXPECT_TRUE(testWriter.writeBits(8, i + 0));
		EXPECT_TRUE(testWriter.writeBits(8, i + 1));
		EXPECT_TRUE(testWriter.writeBits(8, i + 2));
		EXPECT_TRUE(testWriter.writeBits(8, i + 3));
		EXPECT_TRUE(testWriter.writeBits(8, i + 4));
		EXPECT_TRUE(testWriter.writeBits(8, i + 5));
		EXPECT_TRUE(testWriter.writeBits(8, i + 6));
		EXPECT_TRUE(testWriter.writeBits(8, i + 7));
		EXPECT_EQ(i + 0, a[i + 0]);
		EXPECT_EQ(i + 1, a[i + 1]);
		EXPECT_EQ(i + 2, a[i + 2]);
		EXPECT_EQ(i + 3, a[i + 3]);
		EXPECT_EQ(i + 4, a[i + 4]);
		EXPECT_EQ(i + 5, a[i + 5]);
		EXPECT_EQ(i + 6, a[i + 6]);
		EXPECT_EQ(i + 7, a[i + 7]);
	}
	testWriter.flush();
	testReader.setData(a, arrSize);
	for (size_t i = 0; i < arrSize; ++i) {
		EXPECT_TRUE(testReader.readBits(8, val));
		EXPECT_EQ(i, val);
	}
	std::fill(a, a + arrSize, 0);

	// reverse bits
	testWriter.setReverseBytes(false);
	testWriter.setReverseBits(true);
	testReader.setReverseBytes(false);
	testReader.setReverseBits(true);
	testWriter.setData(a, arrSize);
	for (size_t i = 0; i < arrSize; ++i) {
		EXPECT_TRUE(testWriter.writeBits(8, i));
	}
	testWriter.flush();
	testReader.setData(a, arrSize);
	for (size_t i = 0; i < arrSize; ++i) {
		EXPECT_TRUE(testReader.readBits(8, val));
		EXPECT_EQ(i, val);
	}
	std::fill(a, a + arrSize, 0);

	// reverse all
	testWriter.setReverseBytes(true);
	testWriter.setReverseBits(true);
	testReader.setReverseBytes(true);
	testReader.setReverseBits(true);
	testWriter.setData(a, arrSize);
	for (size_t i = 0; i < arrSize; ++i) {
		EXPECT_TRUE(testWriter.writeBits(8, i));
	}
	testWriter.flush();
	testReader.setData(a, arrSize);
	for (size_t i = 0; i < arrSize; ++i) {
		EXPECT_TRUE(testReader.readBits(8, val));
		EXPECT_EQ(i, val);
	}
	std::fill(a, a + arrSize, 0);


	/*
		Huge test
		(consider that test are run on LE computer)
	*/
	// Test no reverse
	// 1 .. 32 bits: 32 iterations write 66 bytes
	const size_t dataSize = (static_cast<size_t>(1 << 23) * 3 * 11) + 1;
	const size_t innerIterCount = (dataSize - 1) / 66 * 32;
	uint8_t* writingTo = new uint8_t[dataSize];

	BinaryReader<uint8_t> reader{ false };
	BinaryWriter<uint8_t> writer{ false };
	writer.setData(writingTo, dataSize);
	// reverse nothing
	writer.writeBits(1, 0);
	for (size_t i = 1; i < innerIterCount; ++i) {
		writer.writeBits((i - 1) % 32 + 1, i);
	}
	writer.writeBits(31, 0);
	writer.flush();
	reader.setData(writingTo, dataSize);
	reader.readBits(1, val);
	EXPECT_EQ(0, val);
	for (size_t i = 1; i < innerIterCount; ++i) {
		reader.readBits((i - 1) % 32 + 1, val);
		EXPECT_EQ(val, i & LITTLE_BITS[(i - 1) % 32 + 1]);
	}

	// Test reverse bytes
	writer.setReverseBytes(true);
	reader.setReverseBytes(true);
	writer.setData(writingTo, dataSize);

	writer.writeBits(1, 0);
	for (size_t i = 1; i < innerIterCount; ++i) {
		writer.writeBits((i - 1) % 32 + 1, i);
	}
	writer.writeBits(31, 0);
	writer.flush();
	reader.setData(writingTo, dataSize);
	reader.readBits(1, val);
	EXPECT_EQ(0, val);
	for (size_t i = 1; i < innerIterCount; ++i) {
		reader.readBits((i - 1) % 32 + 1, val);
		EXPECT_EQ(val, i & LITTLE_BITS[(i - 1) % 32 + 1]);
	}

	// Test reverse bits
	writer.setReverseBytes(false);
	reader.setReverseBytes(false);
	writer.setReverseBits(true);
	reader.setReverseBits(true);
	writer.setData(writingTo, dataSize);

	writer.writeBits(1, 0);
	for (size_t i = 1; i < innerIterCount; ++i) {
		writer.writeBits((i - 1) % 32 + 1, i);
	}
	writer.writeBits(31, 0);
	writer.flush();
	reader.setData(writingTo, dataSize);
	reader.readBits(1, val);
	EXPECT_EQ(0, val);
	for (size_t i = 1; i < innerIterCount; ++i) {
		reader.readBits((i - 1) % 32 + 1, val);
		EXPECT_EQ(val, i & LITTLE_BITS[(i - 1) % 32 + 1]);
	}

	// Test reverse all
	writer.setReverseBytes(true);
	reader.setReverseBytes(true);
	writer.setData(writingTo, dataSize);

	writer.writeBits(1, 0);
	for (size_t i = 1; i < innerIterCount; ++i) {
		writer.writeBits((i - 1) % 32 + 1, i);
	}
	writer.writeBits(31, 0);
	writer.flush();
	reader.setData(writingTo, dataSize);
	reader.readBits(1, val);
	EXPECT_EQ(0, val);
	for (size_t i = 1; i < innerIterCount; ++i) {
		reader.readBits((i - 1) % 32 + 1, val);
		EXPECT_EQ(val, i & LITTLE_BITS[(i - 1) % 32 + 1]);
	}

	delete[] writingTo;
}
#else
TEST(TestTogether, TestReadWrite) {
	// i have little endian so for tests sometime need reverse for simulating big or little endianes
	const size_t arrSize = 256;
	uint8_t a[arrSize] = { 0 };
	BinaryWriter<uint8_t> testWriter{ false };
	BinaryReader<uint8_t> testReader{ false };
	size_t val;
	// without any reverse
	testWriter.setData(a, arrSize);
	for (size_t i = 0; i < arrSize; i += 4) {
		EXPECT_TRUE(testWriter.writeBits(8, i));
		EXPECT_TRUE(testWriter.writeBits(8, i + 1));
		EXPECT_TRUE(testWriter.writeBits(8, i + 2));
		EXPECT_TRUE(testWriter.writeBits(8, i + 3));
		EXPECT_EQ(i + 3, a[i + 0]);
		EXPECT_EQ(i + 2, a[i + 1]);
		EXPECT_EQ(i + 1, a[i + 2]);
		EXPECT_EQ(i + 0, a[i + 3]);
	}
	testWriter.flush();
	testReader.setData(a, arrSize);
	for (size_t i = 0; i < arrSize; ++i) {
		EXPECT_TRUE(testReader.readBits(8, val));
		EXPECT_EQ(i, val);
	}
	std::fill(a, a + arrSize, 0);

	// reverse bytes
	testWriter.setReverseBytes(true);
	testWriter.setReverseBits(false);
	testReader.setReverseBytes(true);
	testReader.setReverseBits(false);
	testWriter.setData(a, arrSize);
	for (size_t i = 0; i < arrSize; i += 4) {
		EXPECT_TRUE(testWriter.writeBits(8, i));
		EXPECT_TRUE(testWriter.writeBits(8, i + 1));
		EXPECT_TRUE(testWriter.writeBits(8, i + 2));
		EXPECT_TRUE(testWriter.writeBits(8, i + 3));
		EXPECT_EQ(i + 0, a[i + 0]);
		EXPECT_EQ(i + 1, a[i + 1]);
		EXPECT_EQ(i + 2, a[i + 2]);
		EXPECT_EQ(i + 3, a[i + 3]);
	}
	testWriter.flush();
	testReader.setData(a, arrSize);
	for (size_t i = 0; i < arrSize; ++i) {
		EXPECT_TRUE(testReader.readBits(8, val));
		EXPECT_EQ(i, val);
	}
	std::fill(a, a + arrSize, 0);

	// reverse bits
	testWriter.setReverseBytes(false);
	testWriter.setReverseBits(true);
	testReader.setReverseBytes(false);
	testReader.setReverseBits(true);
	testWriter.setData(a, arrSize);
	for (size_t i = 0; i < arrSize; ++i) {
		EXPECT_TRUE(testWriter.writeBits(8, i));
	}
	testWriter.flush();
	testReader.setData(a, arrSize);
	for (size_t i = 0; i < arrSize; ++i) {
		EXPECT_TRUE(testReader.readBits(8, val));
		EXPECT_EQ(i, val);
	}
	std::fill(a, a + arrSize, 0);

	// reverse all
	testWriter.setReverseBytes(true);
	testWriter.setReverseBits(true);
	testReader.setReverseBytes(true);
	testReader.setReverseBits(true);
	testWriter.setData(a, arrSize);
	for (size_t i = 0; i < arrSize; ++i) {
		EXPECT_TRUE(testWriter.writeBits(8, i));
	}
	testWriter.flush();
	testReader.setData(a, arrSize);
	for (size_t i = 0; i < arrSize; ++i) {
		EXPECT_TRUE(testReader.readBits(8, val));
		EXPECT_EQ(i, val);
	}
	std::fill(a, a + arrSize, 0);


	/*
		Huge test
		(consider that test are run on LE computer)
	*/
	// Test no reverse
	// 1 .. 32 bits: 32 iterations write 66 bytes
	const size_t dataSize = (static_cast<size_t>(1 << 23) * 3 * 11) + 1;
	const size_t innerIterCount = (dataSize - 1) / 66 * 32;
	uint8_t* writingTo = new uint8_t[dataSize];

	BinaryReader<uint8_t> reader{ false };
	BinaryWriter<uint8_t> writer{ false };
	writer.setData(writingTo, dataSize);

	writer.writeBits(1, 0);
	for (size_t i = 1; i < innerIterCount; ++i) {
		writer.writeBits((i - 1) % 32 + 1, i);
	}
	writer.writeBits(31, 0);
	writer.flush();
	reader.setData(writingTo, dataSize);
	reader.readBits(1, val);
	EXPECT_EQ(0, val);
	for (size_t i = 1; i < innerIterCount; ++i) {
		reader.readBits((i - 1) % 32 + 1, val);
		EXPECT_EQ(val, i & LITTLE_BITS[(i - 1) % 32 + 1]);
	}

	// Test reverse bytes
	writer.setReverseBytes(true);
	reader.setReverseBytes(true);
	writer.setData(writingTo, dataSize);

	writer.writeBits(1, 0);
	for (size_t i = 1; i < innerIterCount; ++i) {
		writer.writeBits((i - 1) % 32 + 1, i);
	}
	writer.writeBits(31, 0);
	writer.flush();
	reader.setData(writingTo, dataSize);
	reader.readBits(1, val);
	EXPECT_EQ(0, val);
	for (size_t i = 1; i < innerIterCount; ++i) {
		reader.readBits((i - 1) % 32 + 1, val);
		EXPECT_EQ(val, i & LITTLE_BITS[(i - 1) % 32 + 1]);
	}

	// Test reverse bits
	writer.setReverseBytes(false);
	reader.setReverseBytes(false);
	writer.setReverseBits(true);
	reader.setReverseBits(true);
	writer.setData(writingTo, dataSize);

	writer.writeBits(1, 0);
	for (size_t i = 1; i < innerIterCount; ++i) {
		writer.writeBits((i - 1) % 32 + 1, i);
	}
	writer.writeBits(31, 0);
	writer.flush();
	reader.setData(writingTo, dataSize);
	reader.readBits(1, val);
	EXPECT_EQ(0, val);
	for (size_t i = 1; i < innerIterCount; ++i) {
		reader.readBits((i - 1) % 32 + 1, val);
		EXPECT_EQ(val, i & LITTLE_BITS[(i - 1) % 32 + 1]);
	}

	// Test reverse all
	writer.setReverseBytes(true);
	reader.setReverseBytes(true);
	writer.setData(writingTo, dataSize);

	writer.writeBits(1, 0);
	for (size_t i = 1; i < innerIterCount; ++i) {
		writer.writeBits((i - 1) % 32 + 1, i);
	}
	writer.writeBits(31, 0);
	writer.flush();
	reader.setData(writingTo, dataSize);
	reader.readBits(1, val);
	EXPECT_EQ(0, val);
	for (size_t i = 1; i < innerIterCount; ++i) {
		reader.readBits((i - 1) % 32 + 1, val);
		EXPECT_EQ(val, i & LITTLE_BITS[(i - 1) % 32 + 1]);
	}

	delete[] writingTo;
}
#endif

#ifdef __GNUC__
int main(int argc, char** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
#endif


#if defined(_M_X64) || defined(__amd64__)
TEST(TestBinaryReader, VirtualLookAndSkip) {
	// Test no reverse
	const size_t read1To32Count = 2048;
	const size_t maxBitsCount = 32;
	const size_t dataSize = (1 + 32) * 16 * 2048;

	VirtualPointer<uint8_t> vptr_BB;
	VirtualPointer<uint8_t> vptr_BL;
	VirtualPointer<uint8_t> vptr_LB;
	VirtualPointer<uint8_t> vptr_LL;

	for (size_t i = 0; i < dataSize; i += 8) {
		vptr_BB.addChunk(packed_notReversed + i, 8);
		vptr_BL.addChunk(packed_bytesReversed_64 + i, 8);
		vptr_LB.addChunk(packed_bitsReversed + i, 8);
		vptr_LL.addChunk(packed_allReversed_64 + i, 8);
	}

	BinaryReader<uint8_t> reader{ true };
	BB_SET
		reader.setData(vptr_BB, dataSize);

	size_t container;

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.lookBits(j, container);
			reader.skipBits(j);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse bytes
	BL_SET
		reader.setData(vptr_BL, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.lookBits(j, container);
			reader.skipBits(j);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse bits
	LB_SET
		reader.setData(vptr_LB, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.lookBits(j, container);
			reader.skipBits(j);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse all
	LL_SET
		reader.setData(vptr_LL, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.lookBits(j, container);
			reader.skipBits(j);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	VirtualPointer<uint8_t> vptr_BB_fragment;
	VirtualPointer<uint8_t> vptr_BL_fragment;
	VirtualPointer<uint8_t> vptr_LB_fragment;
	VirtualPointer<uint8_t> vptr_LL_fragment;

	for (size_t i = 0, j = 0; i < dataSize; i += 8, ++j) {
		vptr_BB_fragment.addChunk(packed_notReversed_inconsistent + i + j, 8);
		vptr_BL_fragment.addChunk(packed_bytesReversed_64_inconsistent + i + j, 8);
		vptr_LB_fragment.addChunk(packed_bitsReversed_inconsistent + i + j, 8);
		vptr_LL_fragment.addChunk(packed_allReversed_64_inconsistent + i + j, 8);
	}

	BB_SET
		reader.setData(vptr_BB_fragment, dataSize);


	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.lookBits(j, container);
			reader.skipBits(j);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse bytes
	BL_SET
		reader.setData(vptr_BL_fragment, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.lookBits(j, container);
			reader.skipBits(j);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse bits
	LB_SET
		reader.setData(vptr_LB_fragment, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.lookBits(j, container);
			reader.skipBits(j);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse all
	LL_SET
		reader.setData(vptr_LL_fragment, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.lookBits(j, container);
			reader.skipBits(j);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}
}
#else
TEST(TestBinaryReader, VirtualLookAndSkip) {
	// Test no reverse
	const size_t read1To32Count = 2048;
	const size_t maxBitsCount = 32;
	const size_t dataSize = (1 + 32) * 16 * 2048;

	VirtualPointer<uint8_t> vptr_BB;
	VirtualPointer<uint8_t> vptr_BL;
	VirtualPointer<uint8_t> vptr_LB;
	VirtualPointer<uint8_t> vptr_LL;

	for (size_t i = 0; i < dataSize; i += 8) {
		vptr_BB.addChunk(packed_notReversed + i, 8);
		vptr_BL.addChunk(packed_bytesReversed + i, 8);
		vptr_LB.addChunk(packed_bitsReversed + i, 8);
		vptr_LL.addChunk(packed_allReversed + i, 8);
	}

	BinaryReader<uint8_t> reader{ true };
	BB_SET
		reader.setData(vptr_BB, dataSize);

	size_t container;

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.lookBits(j, container);
			reader.skipBits(j);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse bytes
	BL_SET
		reader.setData(vptr_BL, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.lookBits(j, container);
			reader.skipBits(j);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse bits
	LB_SET
		reader.setData(vptr_LB, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.lookBits(j, container);
			reader.skipBits(j);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse all
	LL_SET
		reader.setData(vptr_LL, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.lookBits(j, container);
			reader.skipBits(j);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	VirtualPointer<uint8_t> vptr_BB_fragment;
	VirtualPointer<uint8_t> vptr_BL_fragment;
	VirtualPointer<uint8_t> vptr_LB_fragment;
	VirtualPointer<uint8_t> vptr_LL_fragment;

	for (size_t i = 0, j = 0; i < dataSize; i += 8, ++j) {
		vptr_BB_fragment.addChunk(packed_notReversed_inconsistent + i + j, 8);
		vptr_BL_fragment.addChunk(packed_bytesReversed_inconsistent + i + j, 8);
		vptr_LB_fragment.addChunk(packed_bitsReversed_inconsistent + i + j, 8);
		vptr_LL_fragment.addChunk(packed_allReversed_inconsistent + i + j, 8);
	}

	BB_SET
		reader.setData(vptr_BB_fragment, dataSize);


	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.lookBits(j, container);
			reader.skipBits(j);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse bytes
	BL_SET
		reader.setData(vptr_BL_fragment, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.lookBits(j, container);
			reader.skipBits(j);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse bits
	LB_SET
		reader.setData(vptr_LB_fragment, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.lookBits(j, container);
			reader.skipBits(j);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse all
	LL_SET
		reader.setData(vptr_LL_fragment, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.lookBits(j, container);
			reader.skipBits(j);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}
}
#endif


#if defined(_M_X64) || defined(__amd64__)
TEST(TestBinaryReader, VirtualRead) {
	// Test no reverse
	const size_t read1To32Count = 2048;
	const size_t maxBitsCount = 32;
	const size_t dataSize = (1 + 32) * 16 * 2048;

	VirtualPointer<uint8_t> vptr_BB;
	VirtualPointer<uint8_t> vptr_BL;
	VirtualPointer<uint8_t> vptr_LB;
	VirtualPointer<uint8_t> vptr_LL;

	for (size_t i = 0; i < dataSize; i += 8) {
		vptr_BB.addChunk(packed_notReversed + i, 8);
		vptr_BL.addChunk(packed_bytesReversed_64 + i, 8);
		vptr_LB.addChunk(packed_bitsReversed + i, 8);
		vptr_LL.addChunk(packed_allReversed_64 + i, 8);
	}

	BinaryReader<uint8_t> reader{ true };
	BB_SET
		reader.setData(vptr_BB, dataSize);

	size_t container;

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.readBits(j, container);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse bytes
	BL_SET
		reader.setData(vptr_BL, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.readBits(j, container);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse bits
	LB_SET
		reader.setData(vptr_LB, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.readBits(j, container);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse all
	LL_SET
		reader.setData(vptr_LL, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.readBits(j, container);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	VirtualPointer<uint8_t> vptr_BB_fragment;
	VirtualPointer<uint8_t> vptr_BL_fragment;
	VirtualPointer<uint8_t> vptr_LB_fragment;
	VirtualPointer<uint8_t> vptr_LL_fragment;

	for (size_t i = 0, j = 0; i < dataSize; i += 8, ++j) {
		vptr_BB_fragment.addChunk(packed_notReversed_inconsistent + i + j, 8);
		vptr_BL_fragment.addChunk(packed_bytesReversed_64_inconsistent + i + j, 8);
		vptr_LB_fragment.addChunk(packed_bitsReversed_inconsistent + i + j, 8);
		vptr_LL_fragment.addChunk(packed_allReversed_64_inconsistent + i + j, 8);
	}

	BB_SET
		reader.setData(vptr_BB_fragment, dataSize);


	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.readBits(j, container);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse bytes
	BL_SET
		reader.setData(vptr_BL_fragment, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.readBits(j, container);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse bits
	LB_SET
		reader.setData(vptr_LB_fragment, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.readBits(j, container);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse all
	LL_SET
		reader.setData(vptr_LL_fragment, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.readBits(j, container);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}
}
#else
TEST(TestBinaryReader, VirtualRead) {
	// Test no reverse
	const size_t read1To32Count = 2048;
	const size_t maxBitsCount = 32;
	const size_t dataSize = (1 + 32) * 16 * 2048;

	VirtualPointer<uint8_t> vptr_BB;
	VirtualPointer<uint8_t> vptr_BL;
	VirtualPointer<uint8_t> vptr_LB;
	VirtualPointer<uint8_t> vptr_LL;

	for (size_t i = 0; i < dataSize; i += 8) {
		vptr_BB.addChunk(packed_notReversed + i, 8);
		vptr_BL.addChunk(packed_bytesReversed + i, 8);
		vptr_LB.addChunk(packed_bitsReversed + i, 8);
		vptr_LL.addChunk(packed_allReversed + i, 8);
	}

	BinaryReader<uint8_t> reader{ true };
	BB_SET
		reader.setData(vptr_BB, dataSize);

	size_t container;

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.readBits(j, container);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse bytes
	BL_SET
		reader.setData(vptr_BL, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.readBits(j, container);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse bits
	LB_SET
		reader.setData(vptr_LB, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.readBits(j, container);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse all
	LL_SET
		reader.setData(vptr_LL, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.readBits(j, container);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	VirtualPointer<uint8_t> vptr_BB_fragment;
	VirtualPointer<uint8_t> vptr_BL_fragment;
	VirtualPointer<uint8_t> vptr_LB_fragment;
	VirtualPointer<uint8_t> vptr_LL_fragment;

	for (size_t i = 0, j = 0; i < dataSize; i += 8, ++j) {
		vptr_BB_fragment.addChunk(packed_notReversed_inconsistent + i + j, 8);
		vptr_BL_fragment.addChunk(packed_bytesReversed_inconsistent + i + j, 8);
		vptr_LB_fragment.addChunk(packed_bitsReversed_inconsistent + i + j, 8);
		vptr_LL_fragment.addChunk(packed_allReversed_inconsistent + i + j, 8);
	}

	BB_SET
		reader.setData(vptr_BB_fragment, dataSize);


	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.readBits(j, container);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse bytes
	BL_SET
		reader.setData(vptr_BL_fragment, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.readBits(j, container);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse bits
	LB_SET
		reader.setData(vptr_LB_fragment, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.readBits(j, container);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}

	// Test reverse all
	LL_SET
		reader.setData(vptr_LL_fragment, dataSize);
	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			reader.readBits(j, container);
			EXPECT_EQ(size_t(notPackedBits[i * maxBitsCount + j - 1]), container);
		}
	}
}
#endif

TEST(TestBinaryReader, VirtualReadSmall) {
	uint8_t data_BB[] = { 0xCA, 0, 0x35 };
	uint8_t data_BL[] = { 0x35, 0, 0xCA };
	uint8_t data_LB[] = { 0x53, 0, 0xAC };
	uint8_t data_LL[] = { 0xAC, 0, 0x53 };
	uint8_t answer[] = { 0xCA, 0x35 };

	VirtualPointer<uint8_t> vptr_BB;
	VirtualPointer<uint8_t> vptr_BL;
	VirtualPointer<uint8_t> vptr_LB;
	VirtualPointer<uint8_t> vptr_LL;

	for (size_t i = 0; i < 3; i += 2) {
		vptr_BB.addChunk(data_BB + i, 1);
		vptr_BL.addChunk(data_BL + i, 1);
		vptr_LB.addChunk(data_LB + i, 1);
		vptr_LL.addChunk(data_LL + i, 1);
	}

	BinaryReader<uint8_t> reader{ true };
	BB_SET
		reader.setData(vptr_BB, 2);

	size_t container;

	for (size_t i = 0; i < 2; ++i) {
		reader.readBits(8, container);
		EXPECT_EQ(size_t(answer[i]), container);
	}

	// Test reverse bytes
	BL_SET
		reader.setData(vptr_BL, 2);

	for (size_t i = 0; i < 2; ++i) {
		reader.readBits(8, container);
		EXPECT_EQ(size_t(answer[i]), container);
	}

	// Test reverse bits
	LB_SET
		reader.setData(vptr_LB, 2);

	for (size_t i = 0; i < 2; ++i) {
		reader.readBits(8, container);
		EXPECT_EQ(size_t(answer[i]), container);
	}

	// Test reverse all
	LL_SET
		reader.setData(vptr_LL, 2);

	for (size_t i = 0; i < 2; ++i) {
		reader.readBits(8, container);
		EXPECT_EQ(size_t(answer[i]), container);
	}
}


TEST(TestBinaryReader, VirtualReadNotAligned) {
	// Test is correct if it runs on the little endian architecture
	uint8_t chunk[] = { 0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
	uint8_t chunk2[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	constexpr auto chunkSize = 9;
	BinaryReader<uint8_t> reader(REVERSE_BYTES, !REVERSE_BITS);
	reader.setData(chunk, chunkSize);

	size_t storage;
	
	reader.readBits(12, storage);

	reader.readBits(3, storage);

	reader.readBits(1, storage);

	reader.skipBits(6);
	reader.lookBits(2, storage);
	reader.readBits(2, storage);
	reader.skipBits(40);
	reader.readBits(8, storage);

	VirtualPointer<uint8_t> vChunk{};
	vChunk.addChunk(chunk, chunkSize);
	vChunk.addChunk(chunk2, chunkSize);
	reader.setData(vChunk, chunkSize * 2);
	reader.skipBits(8 * 4);
	reader.readBits(8, storage);
	

	for (int i = 0; i < chunkSize + 2; ++i)
	{
		reader.readBits(8, storage);
	}
	EXPECT_TRUE(reader.readBits(8, storage));
	EXPECT_EQ(0xFF, storage);
}

#if defined(_M_X64) || defined(__amd64__)
TEST(TestBinaryWriter, TestWritingVirtual) {

	uint8_t a[32] = { 0 };
	uint8_t b[32] = { 0 };
	uint8_t c[32] = { 0 };
	uint8_t d[34] = { 0 };

	VirtualPointer<uint8_t> vptra;
	VirtualPointer<uint8_t> vptrb;
	VirtualPointer<uint8_t> vptrc;
	VirtualPointer<uint8_t> vptrd;

	for (size_t i = 0; i < 32; i += 2) {
		vptra.addChunk(a + i, 1);
		vptrb.addChunk(b + i, 1);
		vptrc.addChunk(c + i, 1);
		vptrd.addChunk(d + i, 1);
	}
	vptrd.addChunk(d + 32, 1);

	BinaryWriter<uint8_t> writer{ false };
	writer.setData(vptra, 16);
	// Test like we are big endian
	writer.setReverseBits(false);
	writer.setReverseBytes(false);
	size_t val = 0xF0;
	for (uint8_t i = 0; i < 16; ++i) {
		EXPECT_TRUE(writer.writeBits(8, val));
	}
	EXPECT_EQ(0, writer.getRemainSize());
	EXPECT_FALSE(writer.writeBits(8, val));
	for (uint8_t i = 0; i < 32; i += 2) {
		EXPECT_EQ(0xF0, a[i]);
		EXPECT_EQ(0x00, a[i + 1]);
	}

	writer.setData(vptrb, 16);
	val = 0x0F0F0F0F;
	for (uint8_t i = 0; i < 4; ++i) {
		EXPECT_TRUE(writer.writeBits(32, val));
	}
	EXPECT_EQ(0, writer.getRemainSize());
	for (uint8_t i = 0; i < 32; i += 2) {
		EXPECT_EQ(0x0F, b[i]);
		EXPECT_EQ(0x00, b[i + 1]);
	}

	writer.setData(vptrc, 16);
	for (uint8_t i = 0; i < 32; ++i) {
		EXPECT_TRUE(writer.writeBits(2, 0x3));
		EXPECT_TRUE(writer.writeBits(2, 0));
	}
	EXPECT_EQ(0, writer.getRemainSize());
	for (uint8_t i = 0; i < 32; i += 2) {
		EXPECT_EQ(0xCC, c[i]);
		EXPECT_EQ(0x00, c[i + 1]);
	}
	// next situation cannot be tested in little endian architecture
	writer.setReverseBytes(true);
	writer.setData(vptrd, 17);
	for (uint8_t i = 0; i < 7; ++i) {
		EXPECT_TRUE(writer.writeBits(9, 0x155));
		EXPECT_TRUE(writer.writeBits(9, 0xAA));
	}
	EXPECT_TRUE(writer.writeBits(10, 0x2AA));
	EXPECT_EQ(0, writer.getRemainSize());
	EXPECT_FALSE(writer.writeBits(8, val));
	for (uint8_t i = 0; i < 34; i += 2) {
		EXPECT_EQ(0xAA, d[i]);
		EXPECT_EQ(0x00, d[i + 1]);
	}
	// Test bit and byte swap works correctly
	std::fill(a, a + 32, 0);
	std::fill(b, b + 32, 0);
	std::fill(c, c + 32, 0);
	std::fill(d, d + 34, 0);
	writer.setReverseBytes(true);
	writer.setData(vptra, 16);
	val = 0xFF00FF00;
	for (uint8_t i = 0; i < 4; ++i) {
		EXPECT_TRUE(writer.writeBits(32, val));
	}
	EXPECT_EQ(0, writer.getRemainSize());
	for (uint8_t i = 0; i < 32; i += 4) {
		EXPECT_EQ(0xFF, a[i]);
		EXPECT_EQ(0x00, a[i + 1]);
		EXPECT_EQ(0x00, a[i + 2]);
		EXPECT_EQ(0x00, a[i + 3]);
	}
	writer.setReverseBytes(false);
	writer.setReverseBits(true);
	writer.setData(vptrb, 16);
	val = 0xF0F0F0F0;
	for (uint8_t i = 0; i < 4; ++i) {
		EXPECT_TRUE(writer.writeBits(32, val));
	}
	EXPECT_EQ(0, writer.getRemainSize());
	for (uint8_t i = 0; i < 32; i += 2) {
		EXPECT_EQ(0x0F, b[i]);
		EXPECT_EQ(0x00, b[i + 1]);
	}
	writer.setReverseBytes(true);
	writer.setData(vptrc, 16);
	val = 0xF00FF00F;
	for (uint8_t i = 0; i < 4; ++i) {
		EXPECT_TRUE(writer.writeBits(32, val));
	}
	EXPECT_EQ(0, writer.getRemainSize());
	for (uint8_t i = 0; i < 32; i += 4) {
		EXPECT_EQ(0x0F, c[i]);
		EXPECT_EQ(0x00, c[i + 1]);
		EXPECT_EQ(0xF0, c[i + 2]);
		EXPECT_EQ(0x00, c[i + 3]);
	}



	/*
		Huge test
	*/
	// Test no reverse
	const size_t read1To32Count = 2048;
	const size_t maxBitsCount = 32;
	const size_t dataSize = (1 + 32) * 16 * 2048;
	uint8_t* writingTo = new uint8_t[dataSize + dataSize / 8];

	VirtualPointer<uint8_t> vptr;

	for (size_t i = 0, j = 0; i < dataSize; i += 8, ++j) {
		vptr.addChunk(writingTo + i + j, 8);
	}

	writer.setReverseBits(false);
	writer.setReverseBytes(true);
	writer.setData(vptr, dataSize);

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			writer.writeBits(j, notPackedBits[i * maxBitsCount + j - 1]);
		}
	}
	writer.flush();

	for (size_t i = 0, e = (size_t)-1; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			if (!((i * maxBitsCount + j - 1) % 8)) {
				++e;
			}
			EXPECT_EQ(size_t(packed_notReversed[i * maxBitsCount + j - 1]), writingTo[i * maxBitsCount + j - 1 + e]);
		}
	}

	// Test reverse bytes
	writer.setReverseBytes(false);
	writer.setData(vptr, dataSize);

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			writer.writeBits(j, notPackedBits[i * maxBitsCount + j - 1]);
		}
	}
	writer.flush();

	for (size_t i = 0, e = (size_t)-1; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			if (!((i * maxBitsCount + j - 1) % 8)) {
				++e;
			}
			EXPECT_EQ(size_t(packed_bytesReversed_64[i * maxBitsCount + j - 1]), writingTo[i * maxBitsCount + j - 1 + e]);
		}
	}

	// Test reverse bits
	writer.setReverseBytes(true);
	writer.setReverseBits(true);
	writer.setData(vptr, dataSize);

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			writer.writeBits(j, notPackedBits[i * maxBitsCount + j - 1]);
		}
	}
	writer.flush();

	for (size_t i = 0, e = (size_t)-1; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			if (!((i * maxBitsCount + j - 1) % 8)) {
				++e;
			}
			EXPECT_EQ(size_t(packed_bitsReversed[i * maxBitsCount + j - 1]), writingTo[i * maxBitsCount + j - 1 + e]);
		}
	}

	// Test reverse all
	writer.setReverseBytes(false);
	writer.setData(vptr, dataSize);

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			writer.writeBits(j, notPackedBits[i * maxBitsCount + j - 1]);
		}
	}
	writer.flush();

	for (size_t i = 0, e = (size_t)-1; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			if (!((i * maxBitsCount + j - 1) % 8)) {
				++e;
			}
			EXPECT_EQ(size_t(packed_allReversed_64[i * maxBitsCount + j - 1]), writingTo[i * maxBitsCount + j - 1 + e]);
		}
	}
	delete[] writingTo;
}
#else
TEST(TestBinaryWriter, TestWritingVirtual) {

	uint8_t a[32] = { 0 };
	uint8_t b[32] = { 0 };
	uint8_t c[32] = { 0 };
	uint8_t d[34] = { 0 };

	VirtualPointer<uint8_t> vptra;
	VirtualPointer<uint8_t> vptrb;
	VirtualPointer<uint8_t> vptrc;
	VirtualPointer<uint8_t> vptrd;

	for (size_t i = 0; i < 32; i += 2) {
		vptra.addChunk(a + i, 1);
		vptrb.addChunk(b + i, 1);
		vptrc.addChunk(c + i, 1);
		vptrd.addChunk(d + i, 1);
	}
	vptrd.addChunk(d + 32, 1);

	BinaryWriter<uint8_t> writer{ false };
	writer.setData(vptra, 16);
	// Test like we are big endian
	writer.setReverseBits(false);
	writer.setReverseBytes(false);
	size_t val = 0xF0;
	for (uint8_t i = 0; i < 16; ++i) {
		EXPECT_TRUE(writer.writeBits(8, val));
	}
	EXPECT_EQ(0, writer.getRemainSize());
	EXPECT_FALSE(writer.writeBits(8, val));
	for (uint8_t i = 0; i < 32; i += 2) {
		EXPECT_EQ(0xF0, a[i]);
		EXPECT_EQ(0x00, a[i + 1]);
	}

	writer.setData(vptrb, 16);
	val = 0x0F0F0F0F;
	for (uint8_t i = 0; i < 4; ++i) {
		EXPECT_TRUE(writer.writeBits(32, val));
	}
	EXPECT_EQ(0, writer.getRemainSize());
	for (uint8_t i = 0; i < 32; i += 2) {
		EXPECT_EQ(0x0F, b[i]);
		EXPECT_EQ(0x00, b[i + 1]);
	}

	writer.setData(vptrc, 16);
	for (uint8_t i = 0; i < 32; ++i) {
		EXPECT_TRUE(writer.writeBits(2, 0x3));
		EXPECT_TRUE(writer.writeBits(2, 0));
	}
	EXPECT_EQ(0, writer.getRemainSize());
	for (uint8_t i = 0; i < 32; i += 2) {
		EXPECT_EQ(0xCC, c[i]);
		EXPECT_EQ(0x00, c[i + 1]);
	}
	// next situation cannot be tested in little endian architecture
	writer.setReverseBytes(true);
	writer.setData(vptrd, 17);
	for (uint8_t i = 0; i < 7; ++i) {
		EXPECT_TRUE(writer.writeBits(9, 0x155));
		EXPECT_TRUE(writer.writeBits(9, 0xAA));
	}
	EXPECT_TRUE(writer.writeBits(10, 0x2AA));
	EXPECT_EQ(0, writer.getRemainSize());
	EXPECT_FALSE(writer.writeBits(8, val));
	for (uint8_t i = 0; i < 34; i += 2) {
		EXPECT_EQ(0xAA, d[i]);
		EXPECT_EQ(0x00, d[i + 1]);
	}
	// Test bit and byte swap works correctly
	std::fill(a, a + 32, 0);
	std::fill(b, b + 32, 0);
	std::fill(c, c + 32, 0);
	std::fill(d, d + 34, 0);
	writer.setReverseBytes(true);
	writer.setData(vptra, 16);
	val = 0xFF00FF00;
	for (uint8_t i = 0; i < 4; ++i) {
		EXPECT_TRUE(writer.writeBits(32, val));
	}
	EXPECT_EQ(0, writer.getRemainSize());
	for (uint8_t i = 0; i < 32; i += 4) {
		EXPECT_EQ(0xFF, a[i]);
		EXPECT_EQ(0x00, a[i + 1]);
		EXPECT_EQ(0x00, a[i + 2]);
		EXPECT_EQ(0x00, a[i + 3]);
	}
	writer.setReverseBytes(false);
	writer.setReverseBits(true);
	writer.setData(vptrb, 16);
	val = 0xF0F0F0F0;
	for (uint8_t i = 0; i < 4; ++i) {
		EXPECT_TRUE(writer.writeBits(32, val));
	}
	EXPECT_EQ(0, writer.getRemainSize());
	for (uint8_t i = 0; i < 32; i += 2) {
		EXPECT_EQ(0x0F, b[i]);
		EXPECT_EQ(0x00, b[i + 1]);
	}
	writer.setReverseBytes(true);
	writer.setData(vptrc, 16);
	val = 0xF00FF00F;
	for (uint8_t i = 0; i < 4; ++i) {
		EXPECT_TRUE(writer.writeBits(32, val));
	}
	EXPECT_EQ(0, writer.getRemainSize());
	for (uint8_t i = 0; i < 32; i += 4) {
		EXPECT_EQ(0x0F, c[i]);
		EXPECT_EQ(0x00, c[i + 1]);
		EXPECT_EQ(0xF0, c[i + 2]);
		EXPECT_EQ(0x00, c[i + 3]);
	}



	/*
		Huge test
	*/
	// Test no reverse
	const size_t read1To32Count = 2048;
	const size_t maxBitsCount = 32;
	const size_t dataSize = (1 + 32) * 16 * 2048;
	uint8_t* writingTo = new uint8_t[dataSize + dataSize / 8];

	VirtualPointer<uint8_t> vptr;

	for (size_t i = 0, j = 0; i < dataSize; i += 8, ++j) {
		vptr.addChunk(writingTo + i + j, 8);
	}

	writer.setReverseBits(false);
	writer.setReverseBytes(true);
	writer.setData(vptr, dataSize);

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			writer.writeBits(j, notPackedBits[i * maxBitsCount + j - 1]);
		}
	}
	writer.flush();

	for (size_t i = 0, e = (size_t)-1; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			if (!((i * maxBitsCount + j - 1) % 8)) {
				++e;
			}
			EXPECT_EQ(size_t(packed_notReversed[i * maxBitsCount + j - 1]), writingTo[i * maxBitsCount + j - 1 + e]);
		}
	}

	// Test reverse bytes
	writer.setReverseBytes(false);
	writer.setData(vptr, dataSize);

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			writer.writeBits(j, notPackedBits[i * maxBitsCount + j - 1]);
		}
	}
	writer.flush();

	for (size_t i = 0, e = (size_t)-1; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			if (!((i * maxBitsCount + j - 1) % 8)) {
				++e;
			}
			EXPECT_EQ(size_t(packed_bytesReversed[i * maxBitsCount + j - 1]), writingTo[i * maxBitsCount + j - 1 + e]);
		}
	}

	// Test reverse bits
	writer.setReverseBytes(true);
	writer.setReverseBits(true);
	writer.setData(vptr, dataSize);

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			writer.writeBits(j, notPackedBits[i * maxBitsCount + j - 1]);
		}
	}
	writer.flush();

	for (size_t i = 0, e = (size_t)-1; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			if (!((i * maxBitsCount + j - 1) % 8)) {
				++e;
			}
			EXPECT_EQ(size_t(packed_bitsReversed[i * maxBitsCount + j - 1]), writingTo[i * maxBitsCount + j - 1 + e]);
		}
	}

	// Test reverse all
	writer.setReverseBytes(false);
	writer.setData(vptr, dataSize);

	for (size_t i = 0; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			writer.writeBits(j, notPackedBits[i * maxBitsCount + j - 1]);
		}
	}
	writer.flush();

	for (size_t i = 0, e = (size_t)-1; i < read1To32Count; ++i) {
		for (size_t j = 1; j <= maxBitsCount; ++j) {
			if (!((i * maxBitsCount + j - 1) % 8)) {
				++e;
			}
			EXPECT_EQ(size_t(packed_allReversed[i * maxBitsCount + j - 1]), writingTo[i * maxBitsCount + j - 1 + e]);
		}
	}
	delete[] writingTo;
}
#endif

TEST(TestBinaryWriter, VirtualWriteNotAligned) {
	// Test is correct if it runs on the little endian architecture
	uint8_t chunk[] = { 0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
	uint8_t chunk2[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	constexpr auto chunkSize = 9;
	BinaryWriter<uint8_t> writer(REVERSE_BYTES, !REVERSE_BITS);
	writer.setData(chunk, chunkSize);

	size_t storage = 0;

	VirtualPointer<uint8_t> vChunk{};
	vChunk.addChunk(chunk, chunkSize);
	vChunk.addChunk(chunk2, chunkSize);
	writer.setData(vChunk, chunkSize * 2);


	for (int i = 0; i < 2*chunkSize - 1; ++i)
	{
		writer.writeBits(8, storage);
	}
	storage = 0xFF;
	EXPECT_TRUE(writer.writeBits(8, storage));
	for(int i = 0; i < chunkSize - 1; ++i)
	{
		EXPECT_EQ(0x00, chunk[i]);
		EXPECT_EQ(0x00, chunk2[i]);
	}
	EXPECT_EQ(0x00, chunk[chunkSize - 1]);
	EXPECT_EQ(0xFF, chunk2[chunkSize - 1]);
}