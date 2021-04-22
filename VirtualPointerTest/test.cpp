#include "pch.h"
#include "gtest/gtest.h"
#include "VirtualPointer.h"

using std::size_t;

/*
*
*
*	Dereferensing: *ptr
*
*
*/


TEST(Dereferensing, dereferensedDataCorrectness) {
	const size_t count = 8;
	uint8_t arr8[count];
	uint16_t arr16[count];
	uint32_t arr32[count];
	for (size_t i = 0; i < count; ++i) {
		arr8[i] = static_cast<uint8_t>(i);
		arr16[i] = static_cast<uint16_t>(i);
		arr32[i] = static_cast<uint32_t>(i);
	}
	for (size_t i = 0; i < count; ++i) {
		VirtualPointer<uint8_t> ptr8{};
		VirtualPointer<uint16_t> ptr16{};
		VirtualPointer<uint32_t> ptr32{};
		ptr8.addChunk(arr8 + i, 1);
		ptr16.addChunk(arr16 + i, 1);
		ptr32.addChunk(arr32 + i, 1);
		EXPECT_EQ(i, *ptr8);
		EXPECT_EQ(i, *ptr16);
		EXPECT_EQ(i, *ptr32);
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[count];
	for (size_t i = 0; i < count; ++i) {
		arr64[i] = (uint64_t)i;
	}
	for (size_t i = 0; i < count; ++i) {
		VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
		ptr64.addChunk(arr64 + i, 1);
		EXPECT_EQ(i, *ptr64);
	}
#endif
}

TEST(Dereferensing, dereferensedChangedDataCorrectness) {
	const size_t count = 8;
	uint8_t arr8[count];
	uint16_t arr16[count];
	uint32_t arr32[count];
	for (size_t i = 0; i < count; ++i) {
		arr8[i] = (uint8_t)i;
		arr16[i] = (uint16_t)i;
		arr32[i] = (uint32_t)i;
	}
	for (size_t i = 0; i < count; ++i) {
		VirtualPointer<uint8_t> ptr8 = VirtualPointer<uint8_t>();
		VirtualPointer<uint16_t> ptr16 = VirtualPointer<uint16_t>();
		VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();
		ptr8.addChunk(arr8 + i, 1);
		ptr16.addChunk(arr16 + i, 1);
		ptr32.addChunk(arr32 + i, 1);
		arr8[i] = (uint8_t)(i + 1);
		arr16[i] = (uint16_t)(i + 1);
		arr32[i] = (uint32_t)(i + 1);
		EXPECT_EQ(i + 1, *ptr8);
		EXPECT_EQ(i + 1, *ptr16);
		EXPECT_EQ(i + 1, *ptr32);
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[count];
	for (size_t i = 0; i < count; ++i) {
		arr64[i] = (uint64_t)i;
	}
	for (size_t i = 0; i < count; ++i) {
		VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
		ptr64.addChunk(arr64 + i, 1);
		arr64[i] = (uint64_t)(i + 1);
		EXPECT_EQ(i + 1, *ptr64);
	}
#endif
}


/*
*
*
*	Prefix increment: ++ptr
*
*
*/

TEST(PrefixIncrement, outOfRangeException) {
	VirtualPointer<size_t> ptr = VirtualPointer<size_t>();
	ASSERT_NO_THROW(++ptr);
	size_t arr1[1] = { 0 };
	size_t arr2[2] = { 1, 2 };
	ptr.addChunk(arr1, 1);
	ptr.addChunk(arr2, 2);
	EXPECT_EQ(1, *ptr);
	ASSERT_NO_THROW(++ptr);
	EXPECT_EQ(2, *ptr);
	ASSERT_NO_THROW(++ptr);
}

TEST(PrefixIncrement, changedPointerCorrectness) {
	const size_t count = 8;
	uint8_t arr8[count];
	uint16_t arr16[count];
	uint32_t arr32[count];
	for (size_t i = 0; i < count; ++i) {
		arr8[i] = (uint8_t)i;
		arr16[i] = (uint16_t)i;
		arr32[i] = (uint32_t)i;
	}
	VirtualPointer<uint8_t> ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();
	ptr8.addChunk(arr8, count);
	ptr16.addChunk(arr16, count);
	ptr32.addChunk(arr32, count);
	for (size_t i = 0; i < count - 1; ++i) {
		EXPECT_EQ(i, *ptr8);
		EXPECT_EQ(i, *ptr16);
		EXPECT_EQ(i, *ptr32);
		++ptr8;
		++ptr16;
		++ptr32;
	}
	EXPECT_EQ(count - 1, *ptr8);
	EXPECT_EQ(count - 1, *ptr16);
	EXPECT_EQ(count - 1, *ptr32);
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[count];
	for (size_t i = 0; i < count; ++i) {
		arr64[i] = (uint64_t)i;
	}
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
	ptr64.addChunk(arr64, count);
	for (size_t i = 0; i < count - 1; ++i) {
		EXPECT_EQ(i, *ptr64);
		++ptr64;
	}
	EXPECT_EQ(count - 1, *ptr64);
#endif
}
TEST(PrefixIncrement, gettingAlreadyIncrementedResult) {
	const size_t count = 8;
	uint8_t arr8[count];
	uint16_t arr16[count];
	uint32_t arr32[count];
	for (size_t i = 0; i < count; ++i) {
		arr8[i] = (uint8_t)i;
		arr16[i] = (uint16_t)i;
		arr32[i] = (uint32_t)i;
	}
	VirtualPointer<uint8_t> ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();
	ptr8.addChunk(arr8, count);
	ptr16.addChunk(arr16, count);
	ptr32.addChunk(arr32, count);
	for (size_t i = 0; i < count - 1; ++i) {
		EXPECT_EQ(i + 1, *(++ptr8));
		EXPECT_EQ(i + 1, *(++ptr16));
		EXPECT_EQ(i + 1, *(++ptr32));
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[count];
	for (size_t i = 0; i < count; ++i) {
		arr64[i] = (uint64_t)i;
	}
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
	ptr64.addChunk(arr64, count);
	for (size_t i = 0; i < count - 1; ++i) {
		EXPECT_EQ(i + 1, *(++ptr64));
	}
#endif
}
TEST(PrefixIncrement, moveInDiscontinuousMemory) {
	const size_t memoryBlock = 8;
	const size_t blocksCount = 4;
	uint8_t arr8[memoryBlock * blocksCount];
	uint16_t arr16[memoryBlock * blocksCount];
	uint32_t arr32[memoryBlock * blocksCount];
	for (size_t i = 0; i < memoryBlock * blocksCount; ++i) {
		arr8[i] = (uint8_t)i;
		arr16[i] = (uint16_t)i;
		arr32[i] = (uint32_t)i;
	}
	VirtualPointer<uint8_t> ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();
	// first memory block
	ptr8.addChunk(arr8, memoryBlock);
	ptr16.addChunk(arr16, memoryBlock);
	ptr32.addChunk(arr32, memoryBlock);
	// third memory block
	ptr8.addChunk(arr8 + 2 * memoryBlock, memoryBlock);
	ptr16.addChunk(arr16 + 2 * memoryBlock, memoryBlock);
	ptr32.addChunk(arr32 + 2 * memoryBlock, memoryBlock);
	for (size_t i = 0; i < memoryBlock - 1; ++i) {
		EXPECT_EQ(i + 1, *(++ptr8));
		EXPECT_EQ(i + 1, *(++ptr16));
		EXPECT_EQ(i + 1, *(++ptr32));
	}
	for (size_t i = 0; i < memoryBlock - 1; ++i) {
		EXPECT_EQ(i + 2 * memoryBlock, *(++ptr8));
		EXPECT_EQ(i + 2 * memoryBlock, *(++ptr16));
		EXPECT_EQ(i + 2 * memoryBlock, *(++ptr32));
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[memoryBlock * blocksCount];
	for (size_t i = 0; i < memoryBlock * blocksCount; ++i) {
		arr64[i] = (uint64_t)i;
	}
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
	ptr64.addChunk(arr64, memoryBlock);
	ptr64.addChunk(arr64 + 2 * memoryBlock, memoryBlock);
	for (size_t i = 0; i < memoryBlock - 1; ++i) {
		EXPECT_EQ(i + 1, *(++ptr64));
	}
	for (size_t i = 0; i < memoryBlock - 1; ++i) {
		EXPECT_EQ(i + 2 * memoryBlock, *(++ptr64));
	}
#endif
}


/*
*
*
*	Postfix increment: ptr++
*
*
*/

TEST(PostfixIncrement, outOfRangeException) {
	VirtualPointer<size_t> ptr = VirtualPointer<size_t>();
	ASSERT_NO_THROW(ptr++);
	size_t arr1[1] = { 0 };
	size_t arr2[2] = { 1, 2 };
	ptr.addChunk(arr1, 1);
	ptr.addChunk(arr2, 2);
	EXPECT_EQ(1, *ptr);
	ASSERT_NO_THROW(ptr++);
	EXPECT_EQ(2, *ptr);
	EXPECT_EQ(2, *(ptr++));
}

TEST(PostfixIncrement, changedPointerCorrectness) {
	const size_t count = 8;
	uint8_t arr8[count];
	uint16_t arr16[count];
	uint32_t arr32[count];
	for (size_t i = 0; i < count; ++i) {
		arr8[i] = (uint8_t)i;
		arr16[i] = (uint16_t)i;
		arr32[i] = (uint32_t)i;
	}
	VirtualPointer<uint8_t> ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();
	ptr8.addChunk(arr8, count);
	ptr16.addChunk(arr16, count);
	ptr32.addChunk(arr32, count);
	for (size_t i = 0; i < count - 1; ++i) {
		EXPECT_EQ(i, *ptr8);
		EXPECT_EQ(i, *ptr16);
		EXPECT_EQ(i, *ptr32);
		ptr8++;
		ptr16++;
		ptr32++;
	}
	EXPECT_EQ(count - 1, *ptr8);
	EXPECT_EQ(count - 1, *ptr16);
	EXPECT_EQ(count - 1, *ptr32);
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[count];
	for (size_t i = 0; i < count; ++i) {
		arr64[i] = (uint64_t)i;
	}
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
	ptr64.addChunk(arr64, count);
	for (size_t i = 0; i < count - 1; ++i) {
		EXPECT_EQ(i, *ptr64);
		ptr64++;
	}
	EXPECT_EQ(count - 1, *ptr64);
#endif
}
TEST(PostfixIncrement, gettingStillNotIncrementedResult) {
	const size_t count = 8;
	uint8_t arr8[count];
	uint16_t arr16[count];
	uint32_t arr32[count];
	for (size_t i = 0; i < count; ++i) {
		arr8[i] = (uint8_t)i;
		arr16[i] = (uint16_t)i;
		arr32[i] = (uint32_t)i;
	}
	VirtualPointer<uint8_t> ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();
	ptr8.addChunk(arr8, count);
	ptr16.addChunk(arr16, count);
	ptr32.addChunk(arr32, count);
	for (size_t i = 0; i < count; ++i) {
		EXPECT_EQ(i, *(ptr8++));
		EXPECT_EQ(i, *(ptr16++));
		EXPECT_EQ(i, *(ptr32++));
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[count];
	for (size_t i = 0; i < count; ++i) {
		arr64[i] = (uint64_t)i;
	}
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
	ptr64.addChunk(arr64, count);
	for (size_t i = 0; i < count; ++i) {
		EXPECT_EQ(i, *(ptr64++));
	}
#endif
}

TEST(PostfixIncrement, moveInDiscontinuousMemory) {
	const size_t memoryBlock = 8;
	const size_t blocksCount = 4;
	uint8_t arr8[memoryBlock * blocksCount];
	uint16_t arr16[memoryBlock * blocksCount];
	uint32_t arr32[memoryBlock * blocksCount];
	for (size_t i = 0; i < memoryBlock * blocksCount; ++i) {
		arr8[i] = (uint8_t)i;
		arr16[i] = (uint16_t)i;
		arr32[i] = (uint32_t)i;
	}
	VirtualPointer<uint8_t> ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();
	// first memory block
	ptr8.addChunk(arr8, memoryBlock);
	ptr16.addChunk(arr16, memoryBlock);
	ptr32.addChunk(arr32, memoryBlock);
	// third memory block
	ptr8.addChunk(arr8 + 2 * memoryBlock, memoryBlock);
	ptr16.addChunk(arr16 + 2 * memoryBlock, memoryBlock);
	ptr32.addChunk(arr32 + 2 * memoryBlock, memoryBlock);
	for (size_t i = 0; i < memoryBlock - 1; ++i) {
		EXPECT_EQ(i, *(ptr8++));
		EXPECT_EQ(i, *(ptr16++));
		EXPECT_EQ(i, *(ptr32++));
	}
	EXPECT_EQ(memoryBlock - 1, *(ptr8++));
	EXPECT_EQ(memoryBlock - 1, *(ptr16++));
	EXPECT_EQ(memoryBlock - 1, *(ptr32++));
	for (size_t i = 1; i < memoryBlock - 1; ++i) {
		EXPECT_EQ(i - 1 + 2 * memoryBlock, *(ptr8++));
		EXPECT_EQ(i - 1 + 2 * memoryBlock, *(ptr16++));
		EXPECT_EQ(i - 1 + 2 * memoryBlock, *(ptr32++));
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[memoryBlock * blocksCount];
	for (size_t i = 0; i < memoryBlock * blocksCount; ++i) {
		arr64[i] = (uint64_t)i;
	}
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
	ptr64.addChunk(arr64, memoryBlock);
	ptr64.addChunk(arr64 + 2 * memoryBlock, memoryBlock);
	for (size_t i = 0; i < memoryBlock - 1; ++i) {
		EXPECT_EQ(i, *(ptr64++));
	}
	EXPECT_EQ(memoryBlock - 1, *(ptr64++));
	for (size_t i = 1; i < memoryBlock - 1; ++i) {
		EXPECT_EQ(i - 1 + 2 * memoryBlock, *(ptr64++));
	}
#endif
}


/*
*
*
*	Add and assignment: ptr += x
*
*
*/

TEST(AddAndAssignment, outOfRangeException) {
	VirtualPointer<size_t> ptr = VirtualPointer<size_t>();
	ASSERT_NO_THROW(ptr += 1);
	size_t arr1[1];
	size_t arr2[2];
	size_t arr128[128];
	size_t arr256[256];
	size_t arrInit[1] = { 64 };
	ptr.addChunk(arr1, 1);
	ASSERT_NO_THROW(ptr += 1);
	VirtualPointer<size_t> ptr2 = VirtualPointer<size_t>();
	ptr2.addChunk(arr2, 2);
	ASSERT_NO_THROW(ptr2 += 2);
	VirtualPointer<size_t> ptr128 = VirtualPointer<size_t>();
	ptr128.addChunk(arr128, 128);
	ASSERT_NO_THROW(ptr128 += 128);
	ptr128.addChunk(arr256, 256);
	ASSERT_NO_THROW(ptr128 += 128);
	ASSERT_NO_THROW(*ptr128);
	ASSERT_NO_THROW(ptr128 += 128);
	ptr128.addChunk(arrInit, 1);
	EXPECT_EQ(64, *ptr128);
}

TEST(AddAndAssignment, changedPointerCorrectness) {
	constexpr size_t count = 64;
	uint8_t arr8[count];
	uint16_t arr16[count];
	uint32_t arr32[count];
	for (size_t i = 0; i < count; ++i) {
		arr8[i] = static_cast<uint8_t>(i);
		arr16[i] = static_cast<uint16_t>(i);
		arr32[i] = static_cast<uint32_t>(i);
	}
	VirtualPointer<uint8_t> ptr8{};
	VirtualPointer<uint16_t> ptr16{};
	VirtualPointer<uint32_t> ptr32{};
	for (size_t i = 0; i < count / 8; ++i) {
		ptr8.addChunk(arr8 + i * 8, 8);
		ptr16.addChunk(arr16 + i * 8, 8);
		ptr32.addChunk(arr32 + i * 8, 8);
	}
	{
		EXPECT_EQ(0, *ptr8);
		EXPECT_EQ(0, *ptr16);
		EXPECT_EQ(0, *ptr32);
		ptr8 += 1;
		ptr16 += 1;
		ptr32 += 1;
		EXPECT_EQ(1, *ptr8);
		EXPECT_EQ(1, *ptr16);
		EXPECT_EQ(1, *ptr32);
		ptr8 += 2;
		ptr16 += 2;
		ptr32 += 2;
		EXPECT_EQ(3, *ptr8);
		EXPECT_EQ(3, *ptr16);
		EXPECT_EQ(3, *ptr32);
		ptr8 += 4;
		ptr16 += 4;
		ptr32 += 4;
		EXPECT_EQ(7, *ptr8);
		EXPECT_EQ(7, *ptr16);
		EXPECT_EQ(7, *ptr32);
		ptr8 += 1;
		ptr16 += 1;
		ptr32 += 1;
		EXPECT_EQ(8, *ptr8);
		EXPECT_EQ(8, *ptr16);
		EXPECT_EQ(8, *ptr32);
		ptr8 += 8;
		ptr16 += 8;
		ptr32 += 8;
		EXPECT_EQ(16, *ptr8);
		EXPECT_EQ(16, *ptr16);
		EXPECT_EQ(16, *ptr32);
		ptr8 += 17;
		ptr16 += 17;
		ptr32 += 17;
		EXPECT_EQ(33, *ptr8);
		EXPECT_EQ(33, *ptr16);
		EXPECT_EQ(33, *ptr32);
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[count];
	for (size_t i = 0; i < count; ++i) {
		arr64[i] = (uint64_t)i;
	}
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
	for (size_t i = 0; i < count / 8; ++i) {
		ptr64.addChunk(arr64 + i * 8, 8);
	}
	EXPECT_EQ(0, *ptr64);
	ptr64 += 1;
	EXPECT_EQ(1, *ptr64);
	ptr64 += 2;
	EXPECT_EQ(3, *ptr64);
	ptr64 += 4;
	EXPECT_EQ(7, *ptr64);
	ptr64 += 1;
	EXPECT_EQ(8, *ptr64);
	ptr64 += 8;
	EXPECT_EQ(16, *ptr64);
	ptr64 += 17;
	EXPECT_EQ(33, *ptr64);
#endif
}

TEST(AddAndAssignment, gettingAlreadyIncreasedResult) {
	const size_t count = 64;
	uint8_t arr8[count];
	uint16_t arr16[count];
	uint32_t arr32[count];
	for (size_t i = 0; i < count; ++i) {
		arr8[i] = (uint8_t)i;
		arr16[i] = (uint16_t)i;
		arr32[i] = (uint32_t)i;
	}
	VirtualPointer<uint8_t> ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();
	for (size_t i = 0; i < count / 8; ++i) {
		ptr8.addChunk(arr8 + i * 8, 8);
		ptr16.addChunk(arr16 + i * 8, 8);
		ptr32.addChunk(arr32 + i * 8, 8);
	}
	{
		EXPECT_EQ(1, *(ptr8 += 1));
		EXPECT_EQ(1, *(ptr16 += 1));
		EXPECT_EQ(1, *(ptr32 += 1));

		EXPECT_EQ(3, *(ptr8 += 2));
		EXPECT_EQ(3, *(ptr16 += 2));
		EXPECT_EQ(3, *(ptr32 += 2));

		EXPECT_EQ(7, *(ptr8 += 4));
		EXPECT_EQ(7, *(ptr16 += 4));
		EXPECT_EQ(7, *(ptr32 += 4));

		EXPECT_EQ(8, *(ptr8 += 1));
		EXPECT_EQ(8, *(ptr16 += 1));
		EXPECT_EQ(8, *(ptr32 += 1));

		EXPECT_EQ(16, *(ptr8 += 8));
		EXPECT_EQ(16, *(ptr16 += 8));
		EXPECT_EQ(16, *(ptr32 += 8));

		EXPECT_EQ(33, *(ptr8 += 17));
		EXPECT_EQ(33, *(ptr16 += 17));
		EXPECT_EQ(33, *(ptr32 += 17));
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[count];
	for (size_t i = 0; i < count; ++i) {
		arr64[i] = (uint64_t)i;
	}
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
	for (size_t i = 0; i < count / 8; ++i) {
		ptr64.addChunk(arr64 + i * 8, 8);
	}
	EXPECT_EQ(1, *(ptr64 += 1));
	EXPECT_EQ(3, *(ptr64 += 2));
	EXPECT_EQ(7, *(ptr64 += 4));
	EXPECT_EQ(8, *(ptr64 += 1));
	EXPECT_EQ(16, *(ptr64 += 8));
	EXPECT_EQ(33, *(ptr64 += 17));
#endif
}

TEST(AddAndAssignment, moveThrowDiscontinuousMemory) {
	const size_t count = 128;
	uint8_t arr8[count];
	uint16_t arr16[count];
	uint32_t arr32[count];
	for (size_t i = 0; i < count; ++i) {
		arr8[i] = (uint8_t)i;
		arr16[i] = (uint16_t)i;
		arr32[i] = (uint32_t)i;
	}
	VirtualPointer<uint8_t> ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();
	for (size_t i = 0; i < count / 16; ++i) {
		ptr8.addChunk(arr8 + i * 16, 8);
		ptr16.addChunk(arr16 + i * 16, 8);
		ptr32.addChunk(arr32 + i * 16, 8);
	}
	{
		EXPECT_EQ(0, *ptr8);
		EXPECT_EQ(0, *ptr16);
		EXPECT_EQ(0, *ptr32);
		ptr8 += 1;
		ptr16 += 1;
		ptr32 += 1;
		EXPECT_EQ(1, *ptr8);
		EXPECT_EQ(1, *ptr16);
		EXPECT_EQ(1, *ptr32);
		ptr8 += 2;
		ptr16 += 2;
		ptr32 += 2;
		EXPECT_EQ(3, *ptr8);
		EXPECT_EQ(3, *ptr16);
		EXPECT_EQ(3, *ptr32);
		ptr8 += 4;
		ptr16 += 4;
		ptr32 += 4;
		EXPECT_EQ(7, *ptr8);
		EXPECT_EQ(7, *ptr16);
		EXPECT_EQ(7, *ptr32);
		ptr8 += 1;
		ptr16 += 1;
		ptr32 += 1;
		EXPECT_EQ(16, *ptr8);
		EXPECT_EQ(16, *ptr16);
		EXPECT_EQ(16, *ptr32);
		ptr8 += 8;
		ptr16 += 8;
		ptr32 += 8;
		EXPECT_EQ(32, *ptr8);
		EXPECT_EQ(32, *ptr16);
		EXPECT_EQ(32, *ptr32);
		ptr8 += 17;
		ptr16 += 17;
		ptr32 += 17;
		EXPECT_EQ(65, *ptr8);
		EXPECT_EQ(65, *ptr16);
		EXPECT_EQ(65, *ptr32);
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[count];
	for (size_t i = 0; i < count; ++i) {
		arr64[i] = (uint64_t)i;
	}
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
	for (size_t i = 0; i < count / 16; ++i) {
		ptr64.addChunk(arr64 + i * 16, 8);
	}
	EXPECT_EQ(0, *ptr64);
	ptr64 += 1;
	EXPECT_EQ(1, *ptr64);
	ptr64 += 2;
	EXPECT_EQ(3, *ptr64);
	ptr64 += 4;
	EXPECT_EQ(7, *ptr64);
	ptr64 += 1;
	EXPECT_EQ(16, *ptr64);
	ptr64 += 8;
	EXPECT_EQ(32, *ptr64);
	ptr64 += 17;
	EXPECT_EQ(65, *ptr64);
#endif
}


/*
*
*
*	Prefix decrement: --ptr
*
*
*/

TEST(PrefixDecrement, outOfRangeException) {
	VirtualPointer<size_t> ptr = VirtualPointer<size_t>();
	ASSERT_NO_THROW(--ptr);
	size_t arr1[1] = { 0 };
	size_t arr2[2] = { 1, 2 };
	ptr.addChunk(arr1, 1);
	ASSERT_NO_THROW(ptr += 2);
	ptr.addChunk(arr2, 2);
	EXPECT_EQ(1, *ptr);
	ASSERT_NO_THROW(++ptr);
	EXPECT_EQ(2, *ptr);
	ASSERT_NO_THROW(++ptr);
	ASSERT_NO_THROW(--ptr);
	EXPECT_EQ(2, *ptr);
	ASSERT_NO_THROW(--ptr);
	EXPECT_EQ(1, *ptr);
	ASSERT_NO_THROW(--ptr);
	EXPECT_EQ(0, *ptr);
	ASSERT_NO_THROW(--ptr);
}

TEST(PrefixDecrement, changedPointerCorrectness) {
	const size_t count = 8;
	uint8_t arr8[count];
	uint16_t arr16[count];
	uint32_t arr32[count];
	for (size_t i = 0; i < count; ++i) {
		arr8[i] = (uint8_t)i;
		arr16[i] = (uint16_t)i;
		arr32[i] = (uint32_t)i;
	}
	VirtualPointer<uint8_t> ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();
	ptr8.addChunk(arr8, count);		ptr8 += count - 1;
	ptr16.addChunk(arr16, count);	ptr16 += count - 1;
	ptr32.addChunk(arr32, count);	ptr32 += count - 1;
	for (size_t i = 0; i < count; ++i) {
		EXPECT_EQ(count - i - 1, *ptr8);
		EXPECT_EQ(count - i - 1, *ptr16);
		EXPECT_EQ(count - i - 1, *ptr32);
		--ptr8;
		--ptr16;
		--ptr32;
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[count];
	for (size_t i = 0; i < count; ++i) {
		arr64[i] = (uint64_t)i;
	}
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
	ptr64.addChunk(arr64, count);	ptr64 += count - 1;
	for (size_t i = 0; i < count; ++i) {
		EXPECT_EQ(count - i - 1, *ptr64);
		--ptr64;
	}
#endif
}

TEST(PrefixDecrement, gettingAlreadyDecrementedResult) {
	const size_t count = 8;
	uint8_t arr8[count];
	uint16_t arr16[count];
	uint32_t arr32[count];
	for (size_t i = 0; i < count; ++i) {
		arr8[i] = (uint8_t)i;
		arr16[i] = (uint16_t)i;
		arr32[i] = (uint32_t)i;
	}
	VirtualPointer<uint8_t> ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();
	ptr8.addChunk(arr8, count);		ptr8 += count;
	ptr16.addChunk(arr16, count);	ptr16 += count;
	ptr32.addChunk(arr32, count);	ptr32 += count;
	for (size_t i = 0; i < count; ++i) {
		EXPECT_EQ(count - i - 1, *(--ptr8));
		EXPECT_EQ(count - i - 1, *(--ptr16));
		EXPECT_EQ(count - i - 1, *(--ptr32));
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[count];
	for (size_t i = 0; i < count; ++i) {
		arr64[i] = (uint64_t)i;
	}
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
	ptr64.addChunk(arr64, count);	ptr64 += count;
	for (size_t i = 0; i < count; ++i) {
		EXPECT_EQ(count - 1 - i, *(--ptr64));
	}
#endif
}

TEST(PrefixDecrement, moveInDiscontinuousMemory) {
	const size_t memoryBlock = 8;
	const size_t blocksCount = 4;
	uint8_t arr8[memoryBlock * blocksCount];
	uint16_t arr16[memoryBlock * blocksCount];
	uint32_t arr32[memoryBlock * blocksCount];
	for (size_t i = 0; i < memoryBlock * blocksCount; ++i) {
		arr8[i] = (uint8_t)i;
		arr16[i] = (uint16_t)i;
		arr32[i] = (uint32_t)i;
	}
	auto ptr8 = VirtualPointer<uint8_t>();
	auto ptr16 = VirtualPointer<uint16_t>();
	auto ptr32 = VirtualPointer<uint32_t>();
	// first memory block
	ptr8.addChunk(arr8, memoryBlock);
	ptr16.addChunk(arr16, memoryBlock);
	ptr32.addChunk(arr32, memoryBlock);
	// third memory block
	ptr8.addChunk(arr8 + 2 * memoryBlock, memoryBlock);	ptr8 += memoryBlock * blocksCount / 2;
	ptr16.addChunk(arr16 + 2 * memoryBlock, memoryBlock);	ptr16 += memoryBlock * blocksCount / 2;
	ptr32.addChunk(arr32 + 2 * memoryBlock, memoryBlock);	ptr32 += memoryBlock * blocksCount / 2;
	for (size_t i = 0; i < memoryBlock; ++i) {
		EXPECT_EQ(3 * memoryBlock - 1 - i, *(--ptr8));
		EXPECT_EQ(3 * memoryBlock - 1 - i, *(--ptr16));
		EXPECT_EQ(3 * memoryBlock - 1 - i, *(--ptr32));
	}
	for (size_t i = 0; i < memoryBlock; ++i) {
		EXPECT_EQ(memoryBlock - 1 - i, *(--ptr8));
		EXPECT_EQ(memoryBlock - 1 - i, *(--ptr16));
		EXPECT_EQ(memoryBlock - 1 - i, *(--ptr32));
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[memoryBlock * blocksCount];
	for (size_t i = 0; i < memoryBlock * blocksCount; ++i) {
		arr64[i] = (uint64_t)i;
	}
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
	ptr64.addChunk(arr64, memoryBlock);
	ptr64.addChunk(arr64 + 2 * memoryBlock, memoryBlock);	ptr64 += memoryBlock * blocksCount / 2;
	for (size_t i = 0; i < memoryBlock; ++i) {
		EXPECT_EQ(3 * memoryBlock - 1 - i, *(--ptr64));
	}
	for (size_t i = 0; i < memoryBlock; ++i) {
		EXPECT_EQ(memoryBlock - 1 - i, *(--ptr64));
	}
#endif
}


/*
*
*
*	Postfix decrement: ptr--
*
*
*/

TEST(PostfixDecrement, outOfRangeException) {
	VirtualPointer<size_t> ptr = VirtualPointer<size_t>();
	ASSERT_NO_THROW(ptr--);
	size_t arr1[1] = { 0 };
	size_t arr2[2] = { 1, 2 };
	ptr.addChunk(arr1, 1);
	ASSERT_NO_THROW(ptr += 2);
	ptr.addChunk(arr2, 2);
	EXPECT_EQ(1, *ptr);
	ASSERT_NO_THROW(++ptr);
	EXPECT_EQ(2, *ptr);
	ASSERT_NO_THROW(++ptr);
	ASSERT_NO_THROW(ptr--);
	EXPECT_EQ(2, *ptr);
	EXPECT_EQ(2, *(ptr--));
	EXPECT_EQ(1, *ptr);
	EXPECT_EQ(1, *(ptr--));
	EXPECT_EQ(0, *ptr);
	EXPECT_EQ(0, *(ptr--));
}

TEST(PostfixDecrement, changedPointerCorrectness) {
	const size_t count = 8;
	uint8_t arr8[count];
	uint16_t arr16[count];
	uint32_t arr32[count];
	for (size_t i = 0; i < count; ++i) {
		arr8[i] = (uint8_t)i;
		arr16[i] = (uint16_t)i;
		arr32[i] = (uint32_t)i;
	}
	VirtualPointer<uint8_t> ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();
	ptr8.addChunk(arr8, count);		ptr8 += count - 1;
	ptr16.addChunk(arr16, count);	ptr16 += count - 1;
	ptr32.addChunk(arr32, count);	ptr32 += count - 1;
	for (size_t i = 0; i < count; ++i) {
		EXPECT_EQ(count - 1 - i, *ptr8);
		EXPECT_EQ(count - 1 - i, *ptr16);
		EXPECT_EQ(count - 1 - i, *ptr32);
		ptr8--;
		ptr16--;
		ptr32--;
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[count];
	for (size_t i = 0; i < count; ++i) {
		arr64[i] = (uint64_t)i;
	}
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
	ptr64.addChunk(arr64, count);	ptr64 += count - 1;
	for (size_t i = 0; i < count; ++i) {
		EXPECT_EQ(count - 1 - i, *ptr64);
		ptr64--;
	}
#endif
}

TEST(PostfixDecrement, gettingStillNotDecrementedResult) {
	const size_t count = 8;
	uint8_t arr8[count];
	uint16_t arr16[count];
	uint32_t arr32[count];
	for (size_t i = 0; i < count; ++i) {
		arr8[i] = (uint8_t)i;
		arr16[i] = (uint16_t)i;
		arr32[i] = (uint32_t)i;
	}
	VirtualPointer<uint8_t> ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();
	ptr8.addChunk(arr8, count);		ptr8 += count - 1;
	ptr16.addChunk(arr16, count);	ptr16 += count - 1;
	ptr32.addChunk(arr32, count);	ptr32 += count - 1;
	for (size_t i = 0; i < count; ++i) {
		EXPECT_EQ(count - 1 - i, *(ptr8--));
		EXPECT_EQ(count - 1 - i, *(ptr16--));
		EXPECT_EQ(count - 1 - i, *(ptr32--));
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[count];
	for (size_t i = 0; i < count; ++i) {
		arr64[i] = (uint64_t)i;
	}
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
	ptr64.addChunk(arr64, count);	ptr64 += count - 1;
	for (size_t i = 0; i < count; ++i) {
		EXPECT_EQ(count - 1 - i, *(ptr64--));
	}
#endif
}

TEST(PostfixDecrement, moveInDiscontinuousMemory) {
	const size_t memoryBlock = 8;
	const size_t blocksCount = 4;
	uint8_t arr8[memoryBlock * blocksCount];
	uint16_t arr16[memoryBlock * blocksCount];
	uint32_t arr32[memoryBlock * blocksCount];
	for (size_t i = 0; i < memoryBlock * blocksCount; ++i) {
		arr8[i] = (uint8_t)i;
		arr16[i] = (uint16_t)i;
		arr32[i] = (uint32_t)i;
	}
	VirtualPointer<uint8_t> ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();
	// first memory block
	ptr8.addChunk(arr8, memoryBlock);
	ptr16.addChunk(arr16, memoryBlock);
	ptr32.addChunk(arr32, memoryBlock);
	// third memory block
	ptr8.addChunk(arr8 + 2 * memoryBlock, memoryBlock);
	ptr8 += memoryBlock * blocksCount / 2 - 1;
	ptr16.addChunk(arr16 + 2 * memoryBlock, memoryBlock);
	ptr16 += memoryBlock * blocksCount / 2 - 1;
	ptr32.addChunk(arr32 + 2 * memoryBlock, memoryBlock);
	ptr32 += memoryBlock * blocksCount / 2 - 1;
	for (size_t i = 0; i < memoryBlock; ++i) {
		EXPECT_EQ(3 * memoryBlock - 1 - i, *(ptr8--));
		EXPECT_EQ(3 * memoryBlock - 1 - i, *(ptr16--));
		EXPECT_EQ(3 * memoryBlock - 1 - i, *(ptr32--));
	}
	for (size_t i = 0; i < memoryBlock; ++i) {
		EXPECT_EQ(memoryBlock - 1 - i, *(ptr8--));
		EXPECT_EQ(memoryBlock - 1 - i, *(ptr16--));
		EXPECT_EQ(memoryBlock - 1 - i, *(ptr32--));
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[memoryBlock * blocksCount];
	for (size_t i = 0; i < memoryBlock * blocksCount; ++i) {
		arr64[i] = (uint64_t)i;
	}
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
	ptr64.addChunk(arr64, memoryBlock);
	ptr64.addChunk(arr64 + 2 * memoryBlock, memoryBlock);
	ptr64 += memoryBlock * blocksCount / 2 - 1;
	for (size_t i = 0; i < memoryBlock; ++i) {
		EXPECT_EQ(3 * memoryBlock - 1 - i, *(ptr64--));
	}
	for (size_t i = 0; i < memoryBlock; ++i) {
		EXPECT_EQ(memoryBlock - 1 - i, *(ptr64--));
	}
#endif
}


/*
*
*
*	Subtract and assignment: ptr -= x
*
*/


TEST(SubAndAssignment, changedPointerCorrectness) {
	const size_t count = 64;
	uint8_t arr8[count];
	uint16_t arr16[count];
	uint32_t arr32[count];
	for (size_t i = 0; i < count; ++i) {
		arr8[i] = (uint8_t)count - 1 - (uint8_t)i;
		arr16[i] = (uint16_t)count - 1 - (uint16_t)i;
		arr32[i] = (uint32_t)count - 1 - (uint32_t)i;
	}
	VirtualPointer<uint8_t> ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();
	for (size_t i = 0; i < count / 8; ++i) {
		ptr8.addChunk(arr8 + i * 8, 8);
		ptr16.addChunk(arr16 + i * 8, 8);
		ptr32.addChunk(arr32 + i * 8, 8);
	}
	{
		ptr8 += count - 1;
		ptr16 += count - 1;
		ptr32 += count - 1;
		EXPECT_EQ(0, *ptr8);
		EXPECT_EQ(0, *ptr16);
		EXPECT_EQ(0, *ptr32);
		ptr8 -= 1;
		ptr16 -= 1;
		ptr32 -= 1;
		EXPECT_EQ(1, *ptr8);
		EXPECT_EQ(1, *ptr16);
		EXPECT_EQ(1, *ptr32);
		ptr8 -= 2;
		ptr16 -= 2;
		ptr32 -= 2;
		EXPECT_EQ(3, *ptr8);
		EXPECT_EQ(3, *ptr16);
		EXPECT_EQ(3, *ptr32);
		ptr8 -= 4;
		ptr16 -= 4;
		ptr32 -= 4;
		EXPECT_EQ(7, *ptr8);
		EXPECT_EQ(7, *ptr16);
		EXPECT_EQ(7, *ptr32);
		ptr8 -= 1;
		ptr16 -= 1;
		ptr32 -= 1;
		EXPECT_EQ(8, *ptr8);
		EXPECT_EQ(8, *ptr16);
		EXPECT_EQ(8, *ptr32);
		ptr8 -= 8;
		ptr16 -= 8;
		ptr32 -= 8;
		EXPECT_EQ(16, *ptr8);
		EXPECT_EQ(16, *ptr16);
		EXPECT_EQ(16, *ptr32);
		ptr8 -= 17;
		ptr16 -= 17;
		ptr32 -= 17;
		EXPECT_EQ(33, *ptr8);
		EXPECT_EQ(33, *ptr16);
		EXPECT_EQ(33, *ptr32);
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[count];
	for (size_t i = 0; i < count; ++i) {
		arr64[i] = (uint64_t)count - 1 - (uint64_t)i;
	}
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
	for (size_t i = 0; i < count / 8; ++i) {
		ptr64.addChunk(arr64 + i * 8, 8);
	}
	ptr64 += count - 1;
	EXPECT_EQ(0, *ptr64);
	ptr64 -= 1;
	EXPECT_EQ(1, *ptr64);
	ptr64 -= 2;
	EXPECT_EQ(3, *ptr64);
	ptr64 -= 4;
	EXPECT_EQ(7, *ptr64);
	ptr64 -= 1;
	EXPECT_EQ(8, *ptr64);
	ptr64 -= 8;
	EXPECT_EQ(16, *ptr64);
	ptr64 -= 17;
	EXPECT_EQ(33, *ptr64);
#endif
}

TEST(SubAndAssignment, gettingAlreadyIncreasedResult) {
	const size_t count = 64;
	uint8_t arr8[count];
	uint16_t arr16[count];
	uint32_t arr32[count];
	for (size_t i = 0; i < count; ++i) {
		arr8[i] = (uint8_t)count - 1 - (uint8_t)i;
		arr16[i] = (uint16_t)count - 1 - (uint16_t)i;
		arr32[i] = (uint32_t)count - 1 - (uint32_t)i;
	}
	VirtualPointer<uint8_t> ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();
	for (size_t i = 0; i < count / 8; ++i) {
		ptr8.addChunk(arr8 + i * 8, 8);
		ptr16.addChunk(arr16 + i * 8, 8);
		ptr32.addChunk(arr32 + i * 8, 8);
	}
	{
		ptr8 += count - 1;
		ptr16 += count - 1;
		ptr32 += count - 1;

		EXPECT_EQ(1, *(ptr8 -= 1));
		EXPECT_EQ(1, *(ptr16 -= 1));
		EXPECT_EQ(1, *(ptr32 -= 1));

		EXPECT_EQ(3, *(ptr8 -= 2));
		EXPECT_EQ(3, *(ptr16 -= 2));
		EXPECT_EQ(3, *(ptr32 -= 2));

		EXPECT_EQ(7, *(ptr8 -= 4));
		EXPECT_EQ(7, *(ptr16 -= 4));
		EXPECT_EQ(7, *(ptr32 -= 4));

		EXPECT_EQ(8, *(ptr8 -= 1));
		EXPECT_EQ(8, *(ptr16 -= 1));
		EXPECT_EQ(8, *(ptr32 -= 1));

		EXPECT_EQ(16, *(ptr8 -= 8));
		EXPECT_EQ(16, *(ptr16 -= 8));
		EXPECT_EQ(16, *(ptr32 -= 8));

		EXPECT_EQ(33, *(ptr8 -= 17));
		EXPECT_EQ(33, *(ptr16 -= 17));
		EXPECT_EQ(33, *(ptr32 -= 17));
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[count];
	for (size_t i = 0; i < count; ++i) {
		arr64[i] = (uint64_t)count - 1 - (uint64_t)i;
	}
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
	for (size_t i = 0; i < count / 8; ++i) {
		ptr64.addChunk(arr64 + i * 8, 8);
	}
	ptr64 += count - 1;

	EXPECT_EQ(1, *(ptr64 -= 1));
	EXPECT_EQ(3, *(ptr64 -= 2));
	EXPECT_EQ(7, *(ptr64 -= 4));
	EXPECT_EQ(8, *(ptr64 -= 1));
	EXPECT_EQ(16, *(ptr64 -= 8));
	EXPECT_EQ(33, *(ptr64 -= 17));
#endif
}

TEST(SubAndAssignment, moveThrowDiscontinuousMemory) {
	const size_t count = 128;
	uint8_t arr8[count];
	uint16_t arr16[count];
	uint32_t arr32[count];
	for (size_t i = 0; i < count; ++i) {
		arr8[i] = (uint8_t)count - 1 - (uint8_t)i;
		arr16[i] = (uint16_t)count - 1 - (uint16_t)i;
		arr32[i] = (uint32_t)count - 1 - (uint32_t)i;
	}
	VirtualPointer<uint8_t> ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();
	for (size_t i = 0; i < count / 16; ++i) {
		ptr8.addChunk(8 + arr8 + i * 16, 8);
		ptr16.addChunk(8 + arr16 + i * 16, 8);
		ptr32.addChunk(8 + arr32 + i * 16, 8);
	}
	{
		ptr8 += count / 2 - 1;
		ptr16 += count / 2 - 1;
		ptr32 += count / 2 - 1;

		EXPECT_EQ(0, *ptr8);
		EXPECT_EQ(0, *ptr16);
		EXPECT_EQ(0, *ptr32);
		ptr8 -= 1;
		ptr16 -= 1;
		ptr32 -= 1;
		EXPECT_EQ(1, *ptr8);
		EXPECT_EQ(1, *ptr16);
		EXPECT_EQ(1, *ptr32);
		ptr8 -= 2;
		ptr16 -= 2;
		ptr32 -= 2;
		EXPECT_EQ(3, *ptr8);
		EXPECT_EQ(3, *ptr16);
		EXPECT_EQ(3, *ptr32);
		ptr8 -= 4;
		ptr16 -= 4;
		ptr32 -= 4;
		EXPECT_EQ(7, *ptr8);
		EXPECT_EQ(7, *ptr16);
		EXPECT_EQ(7, *ptr32);
		ptr8 -= 1;
		ptr16 -= 1;
		ptr32 -= 1;
		EXPECT_EQ(16, *ptr8);
		EXPECT_EQ(16, *ptr16);
		EXPECT_EQ(16, *ptr32);
		ptr8 -= 8;
		ptr16 -= 8;
		ptr32 -= 8;
		EXPECT_EQ(32, *ptr8);
		EXPECT_EQ(32, *ptr16);
		EXPECT_EQ(32, *ptr32);
		ptr8 -= 17;
		ptr16 -= 17;
		ptr32 -= 17;
		EXPECT_EQ(65, *ptr8);
		EXPECT_EQ(65, *ptr16);
		EXPECT_EQ(65, *ptr32);
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[count];
	for (size_t i = 0; i < count; ++i) {
		arr64[i] = (uint64_t)count - 1 - (uint64_t)i;
	}
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
	for (size_t i = 0; i < count / 16; ++i) {
		ptr64.addChunk(8 + arr64 + i * 16, 8);
	}
	ptr64 += count / 2 - 1;

	EXPECT_EQ(0, *ptr64);
	ptr64 -= 1;
	EXPECT_EQ(1, *ptr64);
	ptr64 -= 2;
	EXPECT_EQ(3, *ptr64);
	ptr64 -= 4;
	EXPECT_EQ(7, *ptr64);
	ptr64 -= 1;
	EXPECT_EQ(16, *ptr64);
	ptr64 -= 8;
	EXPECT_EQ(32, *ptr64);
	ptr64 -= 17;
	EXPECT_EQ(65, *ptr64);
#endif
}


/*
*
*
* Binary plus and minus: ptr + a, a + ptr, ptr - a, a - ptr
*
*
*/

TEST(BinaryPlusMinus, plus) {
	const size_t count = 128;
	uint8_t arr8[count];
	uint16_t arr16[count];
	uint32_t arr32[count];
	for (size_t i = 0; i < count; ++i) {
		arr8[i] = (uint8_t)i;
		arr16[i] = (uint16_t)i;
		arr32[i] = (uint32_t)i;
	}
	VirtualPointer<uint8_t> ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();

	ptr8.addChunk(arr8, 8);
	ptr8.addChunk(arr8 + 16, 16);
	ptr8.addChunk(arr8 + 40, 24);

	ptr16.addChunk(arr16, 8);
	ptr16.addChunk(arr16 + 16, 16);
	ptr16.addChunk(arr16 + 40, 24);

	ptr32.addChunk(arr32, 8);
	ptr32.addChunk(arr32 + 16, 16);
	ptr32.addChunk(arr32 + 40, 24);
	auto test = [&ptr8, &ptr16, &ptr32](const size_t shift, const size_t value) mutable -> void {
		EXPECT_EQ(value, *(ptr8 + shift));
		EXPECT_EQ(value, *(ptr16 + shift));
		EXPECT_EQ(value, *(ptr32 + shift));
		EXPECT_EQ(value, *(shift + ptr8));
		EXPECT_EQ(value, *(shift + ptr16));
		EXPECT_EQ(value, *(shift + ptr32));
		EXPECT_EQ(0, *ptr8);
		EXPECT_EQ(0, *ptr16);
		EXPECT_EQ(0, *ptr32);
	};
	{
		test(1, 1);
		test(2, 2);
		test(3, 3);
		test(7, 7);
		test(8, 16);
		test(9, 17);
		test(23, 31);
		test(24, 40);
		test(47, 63);
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[count];
	for (size_t i = 0; i < count; ++i) {
		arr64[i] = (uint64_t)i;
	}
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
	ptr64.addChunk(arr64, 8);
	ptr64.addChunk(arr64 + 16, 16);
	ptr64.addChunk(arr64 + 40, 24);

	auto extraTest = [&ptr64](int shift, int value) {
		EXPECT_EQ(value, *(ptr64 + shift));
		EXPECT_EQ(value, *(shift + ptr64));
		EXPECT_EQ(0, *ptr64);
	};
	extraTest(1, 1);
	extraTest(2, 2);
	extraTest(3, 3);
	extraTest(7, 7);
	extraTest(8, 16);
	extraTest(9, 17);
	extraTest(23, 31);
	extraTest(24, 40);
	extraTest(47, 63);
#endif
}

TEST(BinaryPlusMinus, minus) {
	const size_t count = 64;
	uint8_t arr8[count];
	uint16_t arr16[count];
	uint32_t arr32[count];
	for (size_t i = 0; i < count; ++i) {
		arr8[i] = (uint8_t)i;
		arr16[i] = (uint16_t)i;
		arr32[i] = (uint32_t)i;
	}
	VirtualPointer<uint8_t> ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();

	ptr8.addChunk(arr8, 8);
	ptr8.addChunk(arr8 + 16, 16);
	ptr8.addChunk(arr8 + 40, 24);

	ptr16.addChunk(arr16, 8);
	ptr16.addChunk(arr16 + 16, 16);
	ptr16.addChunk(arr16 + 40, 24);

	ptr32.addChunk(arr32, 8);
	ptr32.addChunk(arr32 + 16, 16);
	ptr32.addChunk(arr32 + 40, 24);

	ptr8 += 47;
	ptr16 += 47;
	ptr32 += 47;

	auto test = [&ptr8, &ptr16, &ptr32](const size_t shift, const size_t value) mutable -> void {
		EXPECT_EQ(value, *(ptr8 - shift));
		EXPECT_EQ(value, *(ptr16 - shift));
		EXPECT_EQ(value, *(ptr32 - shift));
		EXPECT_EQ(63, *ptr8);
		EXPECT_EQ(63, *ptr16);
		EXPECT_EQ(63, *ptr32);
	};
	{
		test(1, 62);
		test(2, 61);
		test(3, 60);
		test(7, 56);
		test(8, 55);
		test(15, 48);
		test(23, 40);
		test(24, 31);
		test(39, 16);
		test(40, 7);
		test(47, 0);
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[count];
	for (size_t i = 0; i < count; ++i) {
		arr64[i] = (uint64_t)i;
	}
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
	ptr64.addChunk(arr64, 8);
	ptr64.addChunk(arr64 + 16, 16);
	ptr64.addChunk(arr64 + 40, 24);
	ptr64 += 47;

	auto extraTest = [&ptr64](int shift, int value) {
		EXPECT_EQ(value, *(ptr64 - shift));
		EXPECT_EQ(63, *ptr64);
	};
	extraTest(1, 62);
	extraTest(2, 61);
	extraTest(3, 60);
	extraTest(7, 56);
	extraTest(8, 55);
	extraTest(15, 48);
	extraTest(23, 40);
	extraTest(24, 31);
	extraTest(39, 16);
	extraTest(40, 7);
	extraTest(47, 0);
#endif*/
}


/*
*
*
* Get by index: ptr[idx], ptr[idx] = x;
*
*
*/

TEST(GetByIndex, outOfRangeException) {
	VirtualPointer<size_t> ptr = VirtualPointer<size_t>();
	//EXPECT_THROW(ptr[0], NullPointerException);
	size_t arr1[1] = { 0 };
	size_t arr2[2] = { 1, 2 };
	ptr.addChunk(arr1, 1);
	EXPECT_EQ(0, ptr[0]);
	//ASSERT_THROW(ptr[1], std::out_of_range);
	ptr.addChunk(arr2, 2);
	EXPECT_EQ(1, ptr[1]);
	EXPECT_EQ(2, ptr[2]);
	//ASSERT_THROW(ptr[3], std::out_of_range);
}

TEST(GetByIndex, getFromDiscontinuousMemory) {
	const size_t count = 64;
	uint8_t arr8[count];
	uint16_t arr16[count];
	uint32_t arr32[count];
	for (size_t i = 0; i < count; ++i) {
		arr8[i] = static_cast<uint8_t>(i);
		arr16[i] = static_cast<uint16_t>(i);
		arr32[i] = static_cast<uint32_t>(i);
	}
	VirtualPointer<uint8_t> ptr8{};
	VirtualPointer<uint16_t> ptr16{};
	VirtualPointer<uint32_t> ptr32{};

	ptr8.addChunk(arr8, 8);
	ptr8.addChunk(arr8 + 16, 16);
	ptr8.addChunk(arr8 + 40, 24);

	ptr16.addChunk(arr16, 8);
	ptr16.addChunk(arr16 + 16, 16);
	ptr16.addChunk(arr16 + 40, 24);

	ptr32.addChunk(arr32, 8);
	ptr32.addChunk(arr32 + 16, 16);
	ptr32.addChunk(arr32 + 40, 24);
	auto test = [&ptr8, &ptr16, &ptr32](const size_t idx, const size_t value) mutable -> void {
		EXPECT_EQ(value, ptr8[idx]);
		EXPECT_EQ(value, ptr16[idx]);
		EXPECT_EQ(value, ptr32[idx]);
	};
	{
		test(0, 0);
		test(1, 1);
		test(2, 2);
		test(3, 3);
		test(7, 7);
		test(8, 16);
		test(9, 17);
		test(23, 31);
		test(24, 40);
		test(47, 63);
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[count];
	for (size_t i = 0; i < count; ++i) {
		arr64[i] = (uint64_t)i;
	}
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
	ptr64.addChunk(arr64, 8);
	ptr64.addChunk(arr64 + 16, 16);
	ptr64.addChunk(arr64 + 40, 24);

	EXPECT_EQ(0, ptr64[0]);
	EXPECT_EQ(1, ptr64[1]);
	EXPECT_EQ(2, ptr64[2]);
	EXPECT_EQ(3, ptr64[3]);
	EXPECT_EQ(7, ptr64[7]);
	EXPECT_EQ(16, ptr64[8]);
	EXPECT_EQ(17, ptr64[9]);
	EXPECT_EQ(31, ptr64[23]);
	EXPECT_EQ(40, ptr64[24]);
	EXPECT_EQ(63, ptr64[47]);
#endif
}

TEST(GetByIndex, setIntoDiscontinuousMemory) {
	const size_t count = 64;
	uint8_t arr8[count];
	uint16_t arr16[count];
	uint32_t arr32[count];
	for (size_t i = 0; i < count; ++i) {
		arr8[i] = static_cast<uint8_t>(i);
		arr16[i] = static_cast<uint16_t>(i);
		arr32[i] = static_cast<uint32_t>(i);
	}
	VirtualPointer<uint8_t> ptr8{};
	VirtualPointer<uint16_t> ptr16{};
	VirtualPointer<uint32_t> ptr32{};

	ptr8.addChunk(arr8, 8);
	ptr8.addChunk(arr8 + 16, 16);
	ptr8.addChunk(arr8 + 40, 24);

	ptr16.addChunk(arr16, 8);
	ptr16.addChunk(arr16 + 16, 16);
	ptr16.addChunk(arr16 + 40, 24);

	ptr32.addChunk(arr32, 8);
	ptr32.addChunk(arr32 + 16, 16);
	ptr32.addChunk(arr32 + 40, 24);
	auto test = [&ptr8, &ptr16, &ptr32](const size_t idx, const size_t value) mutable -> void {
		EXPECT_EQ(value, ptr8[idx]);
		EXPECT_EQ(value, ptr16[idx]);
		EXPECT_EQ(value, ptr32[idx]);
	};
	auto set = [&ptr8, &ptr16, &ptr32](const size_t idx, const size_t value) mutable -> void {
		EXPECT_NO_THROW(ptr8[idx] = static_cast<uint8_t>(value));
		EXPECT_NO_THROW(ptr16[idx] = static_cast<uint16_t>(value));
		EXPECT_NO_THROW(ptr32[idx] = static_cast<uint32_t>(value));
	};
	{
		for (size_t i = 0; i < 48; ++i) {
			set(i, 128 + i);
		}
		for (size_t i = 0; i < 48; ++i) {
			test(i, 128 + i);
		}
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[count];
	for (size_t i = 0; i < count; ++i) {
		arr64[i] = (uint64_t)i;
	}
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
	ptr64.addChunk(arr64, 8);
	ptr64.addChunk(arr64 + 16, 16);
	ptr64.addChunk(arr64 + 40, 24);


	for (size_t i = 0; i < 48; ++i) {
		ptr64[i] = (uint64_t)(128 + i);
	}
	for (size_t i = 0; i < 48; ++i) {
		EXPECT_EQ(128 + i, ptr64[i]);
	}
#endif
}


/*
*
*
* Add chunk from another virtual pinter
*
*
*/

TEST(AddChunkFromVirtualPointer, addContinuousBlocks) {
	const size_t count = 64;
	uint8_t arr8[count];
	uint16_t arr16[count];
	uint32_t arr32[count];
	for (size_t i = 0; i < count; ++i) {
		arr8[i] = (uint8_t)i;
		arr16[i] = (uint16_t)i;
		arr32[i] = (uint32_t)i;
	}
	VirtualPointer<uint8_t> ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();

	VirtualPointer<uint8_t> src_ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> src_ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> src_ptr32 = VirtualPointer<uint32_t>();

	src_ptr8.addChunk(arr8, 8);
	src_ptr8.addChunk(arr8 + 16, 16);
	src_ptr8.addChunk(arr8 + 40, 24);

	src_ptr16.addChunk(arr16, 8);
	src_ptr16.addChunk(arr16 + 16, 16);
	src_ptr16.addChunk(arr16 + 40, 24);

	src_ptr32.addChunk(arr32, 8);
	src_ptr32.addChunk(arr32 + 16, 16);
	src_ptr32.addChunk(arr32 + 40, 24);

	auto add = [&ptr8, &ptr16, &ptr32, &src_ptr8, &src_ptr16, &src_ptr32](const size_t offset, const size_t size) mutable -> void {
		EXPECT_NO_THROW(ptr8.addChunk(src_ptr8 + offset, size));
		EXPECT_NO_THROW(ptr16.addChunk(src_ptr16 + offset, size));
		EXPECT_NO_THROW(ptr32.addChunk(src_ptr32 + offset, size));
	};
	auto test = [&ptr8, &ptr16, &ptr32](const size_t idx, const size_t value) mutable -> void {
		EXPECT_EQ(value, ptr8[idx]);
		EXPECT_EQ(value, ptr16[idx]);
		EXPECT_EQ(value, ptr32[idx]);
	};
	{
		add(0, 4);
		add(4, 4);
		add(8, 4);

		ptr8 += 46;
		ptr16 += 46;
		ptr32 += 46;

		add(12, 12);
		add(24, 1);
		add(25, 23);

		ptr8 -= 46;
		ptr16 -= 46;
		ptr32 -= 46;

		for (size_t i = 0; i < 8; ++i) {
			test(i, i);
		}
		for (size_t i = 8; i < 24; ++i) {
			test(i, i + 8);
		}
		for (size_t i = 24; i < 48; ++i) {
			test(i, i + 16);
		}
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[count];
	for (size_t i = 0; i < count; ++i) {
		arr64[i] = (uint64_t)i;
	}
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
	VirtualPointer<uint64_t> src_ptr64 = VirtualPointer<uint64_t>();
	src_ptr64.addChunk(arr64, 8);
	src_ptr64.addChunk(arr64 + 16, 16);
	src_ptr64.addChunk(arr64 + 40, 24);

	EXPECT_NO_THROW(ptr64.addChunk(src_ptr64, 4));
	EXPECT_NO_THROW(ptr64.addChunk(src_ptr64 + 4, 4));
	EXPECT_NO_THROW(ptr64.addChunk(src_ptr64 + 8, 4));

	ptr64 += 46;

	EXPECT_NO_THROW(ptr64.addChunk(src_ptr64 + 12, 12));
	EXPECT_NO_THROW(ptr64.addChunk(src_ptr64 + 24, 1));
	EXPECT_NO_THROW(ptr64.addChunk(src_ptr64 + 25, 23));

	ptr64 -= 46;

	for (size_t i = 0; i < 8; ++i) {
		EXPECT_EQ(i, ptr64[i]);
	}
	for (size_t i = 8; i < 24; ++i) {
		EXPECT_EQ(i + 8, ptr64[i]);
	}
	for (size_t i = 24; i < 48; ++i) {
		EXPECT_EQ(i + 16, ptr64[i]);
	}
#endif
}

TEST(AddChunkFromVirtualPointer, addDiscontinuousBlocks) {
	const size_t count = 64;
	uint8_t arr8[count];
	uint16_t arr16[count];
	uint32_t arr32[count];
	for (size_t i = 0; i < count; ++i) {
		arr8[i] = (uint8_t)i;
		arr16[i] = (uint16_t)i;
		arr32[i] = (uint32_t)i;
	}
	VirtualPointer<uint8_t> ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();

	VirtualPointer<uint8_t> src_ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> src_ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> src_ptr32 = VirtualPointer<uint32_t>();

	src_ptr8.addChunk(arr8, 8);
	src_ptr8.addChunk(arr8 + 16, 16);
	src_ptr8.addChunk(arr8 + 40, 24);

	src_ptr16.addChunk(arr16, 8);
	src_ptr16.addChunk(arr16 + 16, 16);
	src_ptr16.addChunk(arr16 + 40, 24);

	src_ptr32.addChunk(arr32, 8);
	src_ptr32.addChunk(arr32 + 16, 16);
	src_ptr32.addChunk(arr32 + 40, 24);

	auto add = [&ptr8, &ptr16, &ptr32, &src_ptr8, &src_ptr16, &src_ptr32](const size_t offset, const size_t size) mutable -> void {
		EXPECT_NO_THROW(ptr8.addChunk(src_ptr8 + offset, size));
		EXPECT_NO_THROW(ptr16.addChunk(src_ptr16 + offset, size));
		EXPECT_NO_THROW(ptr32.addChunk(src_ptr32 + offset, size));
	};
	auto test = [&ptr8, &ptr16, &ptr32](const size_t idx, const size_t value) mutable -> void {
		EXPECT_EQ(value, ptr8[idx]);
		EXPECT_EQ(value, ptr16[idx]);
		EXPECT_EQ(value, ptr32[idx]);
	};
	{
		add(0, 4);
		add(4, 8);
		add(12, 4);
		add(16, 16);
		add(32, 16);

		for (size_t i = 0; i < 8; ++i) {
			test(i, i);
		}
		for (size_t i = 8; i < 24; ++i) {
			test(i, i + 8);
		}
		for (size_t i = 24; i < 48; ++i) {
			test(i, i + 16);
		}
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[count];
	for (size_t i = 0; i < count; ++i) {
		arr64[i] = (uint64_t)i;
	}
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
	VirtualPointer<uint64_t> src_ptr64 = VirtualPointer<uint64_t>();
	src_ptr64.addChunk(arr64, 8);
	src_ptr64.addChunk(arr64 + 16, 16);
	src_ptr64.addChunk(arr64 + 40, 24);

	EXPECT_NO_THROW(ptr64.addChunk(src_ptr64, 4));
	EXPECT_NO_THROW(ptr64.addChunk(src_ptr64 + 4, 8));
	EXPECT_NO_THROW(ptr64.addChunk(src_ptr64 + 12, 4));
	EXPECT_NO_THROW(ptr64.addChunk(src_ptr64 + 16, 16));
	EXPECT_NO_THROW(ptr64.addChunk(src_ptr64 + 32, 16));

	for (size_t i = 0; i < 8; ++i) {
		EXPECT_EQ(i, ptr64[i]);
	}
	for (size_t i = 8; i < 24; ++i) {
		EXPECT_EQ(i + 8, ptr64[i]);
	}
	for (size_t i = 24; i < 48; ++i) {
		EXPECT_EQ(i + 16, ptr64[i]);
	}
#endif
}


/*
*
*
* memset(VirtualPointer dest, T value, size_t count)
*
*
*/

TEST(Memset, usualWork) {
	const size_t count = 128;
	uint8_t arr8[count];
	uint16_t arr16[count];
	uint32_t arr32[count];

	auto arraysSet = [&count, &arr8, &arr16, &arr32]() mutable -> void {
		for (size_t i = 0; i < count; ++i) {
			arr8[i] = (uint8_t)i;
			arr16[i] = (uint16_t)i;
			arr32[i] = (uint32_t)i;
		}
	};

	VirtualPointer<uint8_t> ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();

	auto fillSrc = [&arr8, &arr16, &arr32, &ptr8, &ptr16, &ptr32](const size_t offset, const size_t size) mutable -> void {
		ptr8.addChunk(arr8 + offset, size);
		ptr16.addChunk(arr16 + offset, size);
		ptr32.addChunk(arr32 + offset, size);

	};
	auto set = [&ptr8, &ptr16, &ptr32](const size_t offset, const size_t value, const size_t size) mutable -> void {
		auto sptr8 = ptr8 + offset;
		auto sptr16 = ptr16 + offset;
		auto sptr32 = ptr32 + offset;
		memset(sptr8, value, size);
		memset(sptr16, value, size);
		memset(sptr32, value, size);
	};

	fillSrc(0, 8);
	fillSrc(8, 16);
	fillSrc(32, 32);
	fillSrc(80, 48);

	size_t ptrSize = 104;
	auto test = [&ptrSize, &ptr8, &ptr16, &ptr32](const size_t value) mutable -> void {
		for (size_t i = 0; i < ptrSize; ++i) {
			EXPECT_EQ(value, ptr8[i]);
			EXPECT_EQ(value, ptr16[i]);
			EXPECT_EQ(value, ptr32[i]);
		}
	};

	{
		for (size_t i = 0; i < ptrSize; ++i) {
			arraysSet();
			set(0, i, ptrSize - i);
			set(ptrSize - i, i, i);
			test(i);
		}
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[count];
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();

	auto arraysSet64 = [&count, &arr64]() mutable -> void {
		for (size_t i = 0; i < count; ++i) {
			arr64[i] = (uint64_t)i;
		}
	};
	auto fillSrc64 = [&arr64, &ptr64](const size_t offset, const size_t size) mutable -> void {
		ptr64.addChunk(arr64 + offset, size);

	};
	auto set64 = [&ptr64](const size_t offset, const size_t value, const size_t size) mutable -> void {
		memset(ptr64 + offset, value, size);
	};
	auto test64 = [&ptrSize, &ptr64](const size_t value) mutable -> void {
		for (size_t i = 0; i < ptrSize; ++i) {
			EXPECT_EQ(value, ptr64[i]);
		}
	};


	fillSrc64(0, 8);
	fillSrc64(8, 16);
	fillSrc64(32, 32);
	fillSrc64(80, 48);

	{
		for (size_t i = 0; i < ptrSize; ++i) {
			arraysSet64();
			set64(0, i, ptrSize - i);
			set64(ptrSize - i, i, i);
			test64(i);
		}
	}
#endif
}

TEST(Memset, exceptions) {
	const size_t count = 128;
	size_t arr[count];

	for (size_t i = 0; i < count; ++i) {
		arr[i] = i;
	}

	VirtualPointer<size_t> ptr = VirtualPointer<size_t>();

	EXPECT_NO_THROW(memset(ptr, 1, 0));
	EXPECT_THROW(memset(ptr, 1, 1), NullPointerException);
	ptr.addChunk(arr, count / 4);
	EXPECT_NO_THROW(memset(ptr, 1, 1));
	ptr--;
	EXPECT_THROW(memset(ptr, 1, 1), std::out_of_range);
	ptr += 2 + count;
	EXPECT_THROW(memset(ptr, 1, 1), std::out_of_range);
	ptr -= count + 1;
	EXPECT_THROW(memset(ptr, 1, count / 4 + 1), std::out_of_range);
	ptr.addChunk(arr + count / 4, count / 4);
	ptr.addChunk(arr + count / 2, count / 4);
	ptr.addChunk(arr + 3 * count / 4, count / 4);
	EXPECT_THROW(memset(ptr, 1, count + 1), std::out_of_range);
	EXPECT_NO_THROW(memset(ptr, 1, count));
	for (size_t i = 0; i < count; ++i) {
		EXPECT_EQ(1, arr[i]);
	}
}


/*
*
*
* memcpy(VirtualPointer dest, VirtualPointer src, size_t count)
*
*
*/

TEST(MemcpyTwoVirtual, usualWork) {
	const size_t count = 128;
	uint8_t arr8[count];
	uint16_t arr16[count];
	uint32_t arr32[count];
	uint8_t src_arr8[count];
	uint16_t src_arr16[count];
	uint32_t src_arr32[count];

	auto arraysSet = [&]() mutable -> void {
		for (size_t i = 0; i < count; ++i) {
			arr8[i] = 0;
			arr16[i] = 0;
			arr32[i] = 0;
			src_arr8[i] = (uint8_t)i;
			src_arr16[i] = (uint16_t)i;
			src_arr32[i] = (uint32_t)i;
		}
	};

	VirtualPointer<uint8_t> ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();

	VirtualPointer<uint8_t> src_ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> src_ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> src_ptr32 = VirtualPointer<uint32_t>();

	auto setChunks = [&](const size_t offset, const size_t size) mutable -> void {
		ptr8.addChunk(arr8 + offset, size);
		ptr16.addChunk(arr16 + offset, size);
		ptr32.addChunk(arr32 + offset, size);

	};
	auto setSrcChunks = [&](const size_t offset, const size_t size) mutable -> void {
		src_ptr8.addChunk(src_arr8 + offset, size);
		src_ptr16.addChunk(src_arr16 + offset, size);
		src_ptr32.addChunk(src_arr32 + offset, size);

	};
	auto cpy = [&](const size_t offset, const size_t srcOffset, const size_t size) mutable -> void {
		ptr8 += offset;
		ptr16 += offset;
		ptr32 += offset;
		memcpy(ptr8, src_ptr8 + srcOffset, size);
		memcpy(ptr16, src_ptr16 + srcOffset, size);
		memcpy(ptr32, src_ptr32 + srcOffset, size);
		ptr8 -= offset;
		ptr16 -= offset;
		ptr32 -= offset;
	};
	auto test = [&](const size_t from, const size_t srcFrom, const size_t size) mutable -> void {
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(arr8[i + from], src_arr8[i + srcFrom]);
			EXPECT_EQ(arr16[i + from], src_arr16[i + srcFrom]);
			EXPECT_EQ(arr32[i + from], src_arr32[i + srcFrom]);
		}
	};

	/*
					   slip               skip
	|8 bytes|16 bytes|   8   |32 bytes|    16    |48 bytes|
	*/

	setChunks(0, 8);
	setSrcChunks(0, 8);
	setChunks(8, 16);
	setSrcChunks(8, 16);
	setChunks(32, 32);
	setSrcChunks(32, 32);
	setChunks(80, 48);
	setSrcChunks(80, 48);

	{
		/*
			|8 bytes|16 bytes|   8   |32 bytes|    16    |48 bytes|
			|8 bytes|16 bytes|   8   |32 bytes|    16    |48 bytes|
		*/
		arraysSet();
		cpy(0, 0, 8 + 16 + 32 + 48);
		test(0, 0, 24);
		test(32, 32, 32);
		test(80, 80, 48);

		/*
				|8 bytes|16 bytes|   8   |32 bytes|    16    |48 bytes|
			|8 bytes|16 bytes|   8   |32 bytes|    16    |48 bytes|
		*/
		arraysSet();
		cpy(0, 4, 4 + 16 + 32 + 48);
		test(0, 4, 20);
		test(20, 32, 4);
		test(32, 36, 28);
		test(60, 80, 4);
		test(80, 84, 44);

		/*
			|8 bytes|16 bytes|   8   |32 bytes|    16    |48 bytes|
			 3 |8 bytes|16 bytes|   8   |32 bytes|    16    |48 bytes|
		*/
		arraysSet();
		cpy(3, 0, 5 + 16 + 32 + 48);
		test(3, 0, 21);
		test(32, 21, 3);
		test(35, 32, 29);
		test(80, 61, 3);
		test(83, 80, 45);
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[count];
	uint64_t src_arr64[count];
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();
	VirtualPointer<uint64_t> src_ptr64 = VirtualPointer<uint64_t>();

	auto arraysSet64 = [&]() mutable -> void {
		for (size_t i = 0; i < count; ++i) {
			arr64[i] = 0;
			src_arr64[i] = (uint64_t)i;
		}
	};

	auto setChunks64 = [&](const size_t offset, const size_t size) mutable -> void {
		ptr64.addChunk(arr64 + offset, size);

	};
	auto setSrcChunks64 = [&](const size_t offset, const size_t size) mutable -> void {
		src_ptr64.addChunk(src_arr64 + offset, size);

	};
	auto cpy64 = [&](const size_t offset, const size_t srcOffset, const size_t size) mutable -> void {
		ptr64 += offset;
		memcpy(ptr64, src_ptr64 + srcOffset, size);
		ptr64 -= offset;
	};
	auto test64 = [&](const size_t from, const size_t srcFrom, const size_t size) mutable -> void {
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(arr64[i + from], src_arr64[i + srcFrom]);
		}
	};


	setChunks64(0, 8);
	setSrcChunks64(0, 8);
	setChunks64(8, 16);
	setSrcChunks64(8, 16);
	setChunks64(32, 32);
	setSrcChunks64(32, 32);
	setChunks64(80, 48);
	setSrcChunks64(80, 48);

	{
		arraysSet64();
		cpy64(0, 0, 8 + 16 + 32 + 48);
		test64(0, 0, 24);
		test64(32, 32, 32);
		test64(80, 80, 48);

		arraysSet64();
		cpy64(0, 4, 4 + 16 + 32 + 48);
		test64(0, 4, 20);
		test64(20, 32, 4);
		test64(32, 36, 28);
		test64(60, 80, 4);
		test64(80, 84, 44);

		arraysSet64();
		cpy64(3, 0, 5 + 16 + 32 + 48);
		test64(3, 0, 21);
		test64(32, 21, 3);
		test64(35, 32, 29);
		test64(80, 61, 3);
		test64(83, 80, 45);
	}
#endif
}

TEST(MemcpyTwoVirtual, exceptions) {
	const size_t count = 128;
	size_t arr[count];
	size_t src_arr[count];

	for (size_t i = 0; i < count; ++i) {
		arr[i] = i;
		src_arr[i] = i;
	}

	VirtualPointer<size_t> ptr = VirtualPointer<size_t>();
	VirtualPointer<size_t> src_ptr = VirtualPointer<size_t>();

	EXPECT_NO_THROW(memcpy(ptr, src_ptr, 0));
	EXPECT_THROW(memcpy(ptr, src_ptr, 1), NullPointerException);
	ptr.addChunk(arr, count / 4);
	EXPECT_THROW(memcpy(ptr, src_ptr, 1), NullPointerException);
	src_ptr.addChunk(arr, count / 4);
	EXPECT_NO_THROW(memcpy(ptr, src_ptr, 1));
	EXPECT_THROW(memcpy(ptr - 1, src_ptr, 1), std::out_of_range);
	EXPECT_THROW(memcpy(ptr, src_ptr - 1, 1), std::out_of_range);
	EXPECT_THROW(memcpy(ptr + count + 1, src_ptr, 1), std::out_of_range);

	ptr.addChunk(arr + count / 4, count / 4);
	EXPECT_THROW(memcpy(ptr, src_ptr, count / 2 + 1), std::out_of_range);
	src_ptr.addChunk(arr + count / 4, count / 2);
	EXPECT_NO_THROW(memcpy(ptr, src_ptr, count / 2));
	EXPECT_THROW(memcpy(ptr, src_ptr, count / 2 + 1), std::out_of_range);

	ptr.addChunk(arr + count / 2, count / 2);
	src_ptr.addChunk(arr + 3 * count / 2, count / 4);
	EXPECT_THROW(memcpy(ptr, src_ptr, count + 1), std::out_of_range);
	EXPECT_THROW(memcpy(ptr, src_ptr + 1, count), std::out_of_range);
	EXPECT_THROW(memcpy(ptr + 1, src_ptr, count), std::out_of_range);
	EXPECT_NO_THROW(memcpy(ptr, src_ptr + 1, count - 1));
	EXPECT_NO_THROW(memcpy(ptr + 1, src_ptr, count - 1));
}


/*
*
*
* memcpy(VirtualPointer dest, void* src, size_t count)
*
*
*/

TEST(MemcpyPseudorealToVirtual, usualWork) {
	const size_t count = 128;
	uint8_t arr8[count];
	uint16_t arr16[count];
	uint32_t arr32[count];
	uint8_t src_arr8[count];
	uint16_t src_arr16[count];
	uint32_t src_arr32[count];

	auto arraysSet = [&]() mutable -> void {
		for (size_t i = 0; i < count; ++i) {
			arr8[i] = 0;
			arr16[i] = 0;
			arr32[i] = 0;
			src_arr8[i] = (uint8_t)i;
			src_arr16[i] = (uint16_t)i;
			src_arr32[i] = (uint32_t)i;
		}
	};

	VirtualPointer<uint8_t> ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();

	auto setChunks = [&](const size_t offset, const size_t size) mutable -> void {
		ptr8.addChunk(arr8 + offset, size);
		ptr16.addChunk(arr16 + offset, size);
		ptr32.addChunk(arr32 + offset, size);

	};
	auto cpy = [&](const size_t offset, const size_t srcOffset, const size_t size) mutable -> void {
		ptr8 += offset;
		ptr16 += offset;
		ptr32 += offset;
		memcpy(ptr8, src_arr8 + srcOffset, size);
		memcpy(ptr16, src_arr16 + srcOffset, size);
		memcpy(ptr32, src_arr32 + srcOffset, size);
		ptr8 -= offset;
		ptr16 -= offset;
		ptr32 -= offset;
	};
	auto test = [&](const size_t from, const size_t srcFrom, const size_t size) mutable -> void {
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(arr8[i + from], src_arr8[i + srcFrom]);
			EXPECT_EQ(arr16[i + from], src_arr16[i + srcFrom]);
			EXPECT_EQ(arr32[i + from], src_arr32[i + srcFrom]);
		}
	};

	/*
					   slip               skip
	|8 bytes|16 bytes|   8   |32 bytes|    16    |48 bytes|
	*/

	setChunks(0, 8);
	setChunks(8, 16);
	setChunks(32, 32);
	setChunks(80, 48);

	{
		/*
			|            continuous pseudoreal memory             |
			|8 bytes|16 bytes|   8   |32 bytes|    16    |48 bytes|
		*/
		arraysSet();
		cpy(0, 0, 8 + 16 + 32 + 48);
		test(0, 0, 24);
		test(32, 24, 32);
		test(80, 56, 48);

		/*
				|            continuous pseudoreal memory             |
			|8 bytes|16 bytes|   8   |32 bytes|    16    |48 bytes|
		*/
		arraysSet();
		cpy(0, 4, 8 + 16 + 32 + 48);
		test(0, 4, 24);
		test(32, 28, 32);
		test(80, 60, 48);

		/*
			|            continuous pseudoreal memory             |
			 3 |8 bytes|16 bytes|   8   |32 bytes|    16    |48 bytes|
		*/
		arraysSet();
		cpy(3, 0, 5 + 16 + 32 + 48);
		test(3, 0, 21);
		test(32, 21, 32);
		test(80, 53, 48);
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[count];
	uint64_t src_arr64[count];
	VirtualPointer<uint64_t> ptr64 = VirtualPointer<uint64_t>();

	auto arraysSet64 = [&]() mutable -> void {
		for (size_t i = 0; i < count; ++i) {
			arr64[i] = 0;
			src_arr64[i] = (uint64_t)i;
		}
	};

	auto setChunks64 = [&](const size_t offset, const size_t size) mutable -> void {
		ptr64.addChunk(arr64 + offset, size);

	};
	auto cpy64 = [&](const size_t offset, const size_t srcOffset, const size_t size) mutable -> void {
		ptr64 += offset;
		memcpy(ptr64, src_arr64 + srcOffset, size);
		ptr64 -= offset;
	};
	auto test64 = [&](const size_t from, const size_t srcFrom, const size_t size) mutable -> void {
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(arr64[i + from], src_arr64[i + srcFrom]);
		}
	};


	setChunks64(0, 8);
	setChunks64(8, 16);
	setChunks64(32, 32);
	setChunks64(80, 48);

	{
		arraysSet64();
		cpy64(0, 0, 8 + 16 + 32 + 48);
		test64(0, 0, 24);
		test64(32, 24, 32);
		test64(80, 56, 48);

		arraysSet64();
		cpy64(0, 4, 8 + 16 + 32 + 48);
		test64(0, 4, 24);
		test64(32, 28, 32);
		test64(80, 60, 48);

		arraysSet64();
		cpy64(3, 0, 5 + 16 + 32 + 48);
		test64(3, 0, 21);
		test64(32, 21, 32);
		test64(80, 53, 48);
	}
#endif
}

TEST(MemcpyPseudorealToVirtual, exceptions) {
	const size_t count = 128;
	size_t arr[count];
	size_t src_arr[count];

	for (size_t i = 0; i < count; ++i) {
		arr[i] = i;
		src_arr[i] = i;
	}

	VirtualPointer<size_t> ptr = VirtualPointer<size_t>();

	EXPECT_NO_THROW(memcpy(ptr, src_arr, 0));
	EXPECT_THROW(memcpy(ptr, src_arr, 1), NullPointerException);
	ptr.addChunk(arr, count / 4);

	EXPECT_NO_THROW(memcpy(ptr, src_arr, 1));
	EXPECT_THROW(memcpy(ptr - 1, src_arr, 1), std::out_of_range);
	EXPECT_THROW(memcpy(ptr + count + 1, src_arr, 1), std::out_of_range);

	ptr.addChunk(arr + count / 4, count / 4);
	EXPECT_THROW(memcpy(ptr, src_arr, count / 2 + 1), std::out_of_range);
	EXPECT_NO_THROW(memcpy(ptr, src_arr, count / 2));

	ptr.addChunk(arr + count / 2, count / 2);
	EXPECT_THROW(memcpy(ptr, src_arr, count + 1), std::out_of_range);
	EXPECT_THROW(memcpy(ptr + 1, src_arr, count), std::out_of_range);
	EXPECT_NO_THROW(memcpy(ptr, src_arr + 1, count - 1));
	EXPECT_NO_THROW(memcpy(ptr + 1, src_arr, count - 1));
}


/*
*
*
* memcpy(void* dest, VirtualPointer src, size_t count)
*
*
*/

TEST(MemcpyVirtualToPseudoreal, usualWork) {
	const size_t count = 128;
	uint8_t arr8[count];
	uint16_t arr16[count];
	uint32_t arr32[count];
	uint8_t src_arr8[count];
	uint16_t src_arr16[count];
	uint32_t src_arr32[count];

	auto arraysSet = [&]() mutable -> void {
		for (size_t i = 0; i < count; ++i) {
			arr8[i] = 0;
			arr16[i] = 0;
			arr32[i] = 0;
			src_arr8[i] = (uint8_t)i;
			src_arr16[i] = (uint16_t)i;
			src_arr32[i] = (uint32_t)i;
		}
	};

	VirtualPointer<uint8_t> src_ptr8 = VirtualPointer<uint8_t>();
	VirtualPointer<uint16_t> src_ptr16 = VirtualPointer<uint16_t>();
	VirtualPointer<uint32_t> src_ptr32 = VirtualPointer<uint32_t>();

	auto setSrcChunks = [&](const size_t offset, const size_t size) mutable -> void {
		src_ptr8.addChunk(src_arr8 + offset, size);
		src_ptr16.addChunk(src_arr16 + offset, size);
		src_ptr32.addChunk(src_arr32 + offset, size);

	};
	auto cpy = [&](const size_t offset, const size_t srcOffset, const size_t size) mutable -> void {
		memcpy(arr8 + offset, src_ptr8 + srcOffset, size);
		memcpy(arr16 + offset, src_ptr16 + srcOffset, size);
		memcpy(arr32 + offset, src_ptr32 + srcOffset, size);
	};
	auto test = [&](const size_t from, const size_t srcFrom, const size_t size) mutable -> void {
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(arr8[i + from], src_arr8[i + srcFrom]);
			EXPECT_EQ(arr16[i + from], src_arr16[i + srcFrom]);
			EXPECT_EQ(arr32[i + from], src_arr32[i + srcFrom]);
		}
	};

	/*
					   slip               skip
	|8 bytes|16 bytes|   8   |32 bytes|    16    |48 bytes|
	*/

	setSrcChunks(0, 8);
	setSrcChunks(8, 16);
	setSrcChunks(32, 32);
	setSrcChunks(80, 48);

	{
		/*
			|8 bytes|16 bytes|   8   |32 bytes|    16    |48 bytes|
			|            continuous pseudoreal memory             |
		*/
		arraysSet();
		cpy(0, 0, 8 + 16 + 32 + 48);
		test(0, 0, 24);
		test(24, 32, 32);
		test(56, 80, 48);

		/*
				|8 bytes|16 bytes|   8   |32 bytes|    16    |48 bytes|
			|            continuous pseudoreal memory             |
		*/
		arraysSet();
		cpy(0, 4, 4 + 16 + 32 + 48);
		test(0, 4, 20);
		test(20, 32, 32);
		test(52, 80, 48);

		/*
			|8 bytes|16 bytes|   8   |32 bytes|    16    |48 bytes|
			 3 |            continuous pseudoreal memory             |
		*/
		arraysSet();
		cpy(3, 0, 8 + 16 + 32 + 48);
		test(3, 0, 24);
		test(27, 32, 32);
		test(59, 80, 48);
	}
#if defined(_M_X64) || defined(__amd64__) 
	uint64_t arr64[count];
	uint64_t src_arr64[count];
	VirtualPointer<uint64_t> src_ptr64 = VirtualPointer<uint64_t>();

	auto arraysSet64 = [&]() mutable -> void {
		for (size_t i = 0; i < count; ++i) {
			arr64[i] = 0;
			src_arr64[i] = (uint64_t)i;
		}
	};

	auto setSrcChunks64 = [&](const size_t offset, const size_t size) mutable -> void {
		src_ptr64.addChunk(src_arr64 + offset, size);

	};
	auto cpy64 = [&](const size_t offset, const size_t srcOffset, const size_t size) mutable -> void {
		memcpy(arr64 + offset, src_ptr64 + srcOffset, size);
	};
	auto test64 = [&](const size_t from, const size_t srcFrom, const size_t size) mutable -> void {
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(arr64[i + from], src_arr64[i + srcFrom]);
		}
	};


	setSrcChunks64(0, 8);
	setSrcChunks64(8, 16);
	setSrcChunks64(32, 32);
	setSrcChunks64(80, 48);

	{
		arraysSet64();
		cpy64(0, 0, 8 + 16 + 32 + 48);
		test64(0, 0, 24);
		test64(24, 32, 32);
		test64(56, 80, 48);

		arraysSet64();
		cpy64(0, 4, 4 + 16 + 32 + 48);
		test64(0, 4, 20);
		test64(20, 32, 32);
		test64(52, 80, 48);

		arraysSet64();
		cpy64(3, 0, 8 + 16 + 32 + 48);
		test64(3, 0, 24);
		test64(27, 32, 32);
		test64(59, 80, 48);
	}
#endif
}

TEST(MemcpyVirtualToPseudoreal, exceptions) {
	const size_t count = 128;
	size_t arr[count];
	size_t src_arr[count];

	for (size_t i = 0; i < count; ++i) {
		arr[i] = i;
		src_arr[i] = i;
	}

	VirtualPointer<size_t> src_ptr = VirtualPointer<size_t>();

	EXPECT_NO_THROW(memcpy(arr, src_ptr, 0));
	EXPECT_THROW(memcpy(arr, src_ptr, 1), NullPointerException);
	src_ptr.addChunk(src_arr, count / 4);
	EXPECT_NO_THROW(memcpy(arr, src_ptr, 1));
	EXPECT_THROW(memcpy(arr, src_ptr - 1, 1), std::out_of_range);

	src_ptr.addChunk(src_arr + count / 4, count / 2);
	EXPECT_NO_THROW(memcpy(arr, src_ptr, 3 * count / 4));
	EXPECT_THROW(memcpy(arr, src_ptr, 3 * count / 2 + 1), std::out_of_range);

	src_ptr.addChunk(src_arr + 3 * count / 4, count / 4);
	EXPECT_THROW(memcpy(arr, src_ptr, count + 1), std::out_of_range);
	EXPECT_THROW(memcpy(arr, src_ptr + 1, count), std::out_of_range);
	EXPECT_NO_THROW(memcpy(arr, src_ptr + 1, count - 1));
	EXPECT_NO_THROW(memcpy(arr + 1, src_ptr, count - 1));
}


/*
*
*
* memmove(VirtualPointer dest, VirtualPointer src, size_t count)
*
*
*/

TEST(MemmoveTwoVirtual, usualWork) {
	const size_t count = 128;
	uint32_t arr32[count];

	auto arraysSet = [&]() mutable -> void {
		for (size_t i = 0; i < count; ++i) {
			arr32[i] = (uint32_t)i;
		}
	};

	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();

	VirtualPointer<uint32_t> src_ptr32 = VirtualPointer<uint32_t>();

	auto setChunks = [&](const size_t offset, const size_t size) mutable -> void {
		ptr32.addChunk(arr32 + offset, size);

	};
	auto setSrcChunks = [&](const size_t offset, const size_t size) mutable -> void {
		src_ptr32.addChunk(arr32 + offset, size);

	};
	auto move = [&](const size_t size) mutable -> void {
		memmove(ptr32, src_ptr32, size);
	};
	auto test = [&](const size_t from, const size_t valFrom, const size_t size) mutable -> void {
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(arr32[i + from], valFrom + i);
		}
	};


	{
		/*
			src : |    24 bytes    |  32 bytes  |      24      |16 bytes|32 bytes|
			dest: |8 bytes|16 bytes| 8 |  32 bytes  |    16    |    48 bytes     |
		*/

		setChunks(0, 8);
		setChunks(8, 16);
		setSrcChunks(0, 24);
		setChunks(32, 32);
		setSrcChunks(24, 32);
		setChunks(80, 48);
		setSrcChunks(80, 16);
		setSrcChunks(96, 32);


		arraysSet();
		move(8 + 16 + 32 + 48);
		test(0, 0, 24);
		test(32, 24, 32);
		test(80, 80, 48);


		/*
			dest: |8 bytes|16 bytes| 8 |  32 bytes  |    16    |    48 bytes     |
			src - the opposite order of adding
		*/
		ptr32 = VirtualPointer<uint32_t>();
		src_ptr32 = VirtualPointer<uint32_t>();

		setChunks(0, 8);
		setChunks(8, 16);
		setChunks(32, 32);
		setChunks(80, 48);

		setSrcChunks(80, 48);
		setSrcChunks(32, 32);
		setSrcChunks(8, 16);
		setSrcChunks(0, 8);


		arraysSet();
		move(8 + 16 + 32 + 48);
		test(0, 80, 24);
		test(32, 104, 24);
		test(56, 32, 8);
		test(80, 40, 24);
		test(104, 8, 16);
		test(120, 0, 8);
	}
}

TEST(MemmoveTwoVirtual, exceptions) {
	const size_t count = 128;
	size_t arr[count];
	size_t src_arr[count];

	for (size_t i = 0; i < count; ++i) {
		arr[i] = i;
		src_arr[i] = i;
	}

	VirtualPointer<size_t> ptr = VirtualPointer<size_t>();
	VirtualPointer<size_t> src_ptr = VirtualPointer<size_t>();

	EXPECT_NO_THROW(memmove(ptr, src_ptr, 0));
	EXPECT_THROW(memmove(ptr, src_ptr, 1), NullPointerException);
	ptr.addChunk(arr, count / 4);
	EXPECT_THROW(memmove(ptr, src_ptr, 1), NullPointerException);
	src_ptr.addChunk(arr, count / 4);
	EXPECT_NO_THROW(memmove(ptr, src_ptr, 1));
	EXPECT_THROW(memmove(ptr - 1, src_ptr, 1), std::out_of_range);
	EXPECT_THROW(memmove(ptr, src_ptr - 1, 1), std::out_of_range);
	EXPECT_THROW(memmove(ptr + count + 1, src_ptr, 1), std::out_of_range);

	ptr.addChunk(arr + count / 4, count / 4);
	EXPECT_THROW(memmove(ptr, src_ptr, count / 2 + 1), std::out_of_range);
	src_ptr.addChunk(arr + count / 4, count / 2);
	EXPECT_NO_THROW(memmove(ptr, src_ptr, count / 2));
	EXPECT_THROW(memmove(ptr, src_ptr, count / 2 + 1), std::out_of_range);

	ptr.addChunk(arr + count / 2, count / 2);
	src_ptr.addChunk(arr + 3 * count / 4, count / 4);
	EXPECT_THROW(memmove(ptr, src_ptr, count + 1), std::out_of_range);
	EXPECT_THROW(memmove(ptr, src_ptr + 1, count), std::out_of_range);
	EXPECT_THROW(memmove(ptr + 1, src_ptr, count), std::out_of_range);
	EXPECT_NO_THROW(memmove(ptr, src_ptr + 1, count - 1));
	EXPECT_NO_THROW(memmove(ptr + 1, src_ptr, count - 1));
}


/*
*
*
* memmove(VirtualPointer dest, void* src, size_t count)
*
*
*/

TEST(MemmovePseudorealToVirtual, usualWork) {
	const size_t count = 128;
	uint32_t arr32[count];

	auto arraysSet = [&]() mutable -> void {
		for (size_t i = 0; i < count; ++i) {
			arr32[i] = (uint32_t)i;
		}
	};

	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();

	auto setChunks = [&](const size_t offset, const size_t size) mutable -> void {
		ptr32.addChunk(arr32 + offset, size);

	};
	auto move = [&](const size_t size) mutable -> void {
		memmove(ptr32, arr32, size);
	};
	auto test = [&](const size_t from, const size_t valFrom, const size_t size) mutable -> void {
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(arr32[i + from], valFrom + i);
		}
	};


	{
		/*
			dest: |8 bytes|16 bytes| 8 |  32 bytes  |    16    |    48 bytes     |
			but the opposite order of adding
		*/

		setChunks(80, 48);
		setChunks(32, 32);
		setChunks(8, 16);
		setChunks(0, 8);

		arraysSet();
		move(8 + 16 + 32 + 48);
		test(80, 0, 48);
		test(32, 48, 32);
		test(8, 80, 16);
		test(0, 96, 8);
	}
}

TEST(MemmovePseudorealToVirtual, exceptions) {
	const size_t count = 128;
	size_t arr[count];
	size_t src_arr[count];

	for (size_t i = 0; i < count; ++i) {
		arr[i] = i;
		src_arr[i] = i;
	}

	VirtualPointer<size_t> ptr = VirtualPointer<size_t>();

	EXPECT_NO_THROW(memmove(ptr, src_arr, 0));
	EXPECT_THROW(memmove(ptr, src_arr, 1), NullPointerException);
	ptr.addChunk(arr, count / 4);

	EXPECT_NO_THROW(memmove(ptr, src_arr, 1));
	EXPECT_THROW(memmove(ptr - 1, src_arr, 1), std::out_of_range);
	EXPECT_THROW(memmove(ptr + count + 1, src_arr, 1), std::out_of_range);

	ptr.addChunk(arr + count / 4, count / 4);
	EXPECT_THROW(memmove(ptr, src_arr, count / 2 + 1), std::out_of_range);
	EXPECT_NO_THROW(memmove(ptr, src_arr, count / 2));

	ptr.addChunk(arr + count / 2, count / 2);
	EXPECT_THROW(memmove(ptr, src_arr, count + 1), std::out_of_range);
	EXPECT_THROW(memmove(ptr + 1, src_arr, count), std::out_of_range);
	EXPECT_NO_THROW(memmove(ptr, src_arr + 1, count - 1));
	EXPECT_NO_THROW(memmove(ptr + 1, src_arr, count - 1));
}


/*
*
*
* memmove(void* dest, VirtualPointer src, size_t count)
*
*
*/

TEST(MemmoveVirtualToPseudoreal, usualWork) {
	const size_t count = 128;
	uint32_t arr32[count];

	auto arraysSet = [&]() mutable -> void {
		for (size_t i = 0; i < count; ++i) {
			arr32[i] = (uint32_t)i;
		}
	};

	VirtualPointer<uint32_t> src_ptr32 = VirtualPointer<uint32_t>();

	auto setSrcChunks = [&](const size_t offset, const size_t size) mutable -> void {
		src_ptr32.addChunk(arr32 + offset, size);

	};
	auto move = [&](const size_t size) mutable -> void {
		memmove(arr32, src_ptr32, size);
	};
	auto test = [&](const size_t from, const size_t valFrom, const size_t size) mutable -> void {
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(arr32[i + from], valFrom + i);
		}
	};


	{

		/*
			src: |8 bytes|16 bytes| 8 |  32 bytes  |    16    |    48 bytes     |
			but the opposite order of adding
		*/

		setSrcChunks(80, 48);
		setSrcChunks(32, 32);
		setSrcChunks(8, 16);
		setSrcChunks(0, 8);


		arraysSet();
		move(8 + 16 + 32 + 48);
		test(0, 80, 48);
		test(48, 32, 32);
		test(80, 8, 16);
		test(96, 0, 8);
	}
}

TEST(MemmoveVirtualToPseudoreal, exceptions) {
	const size_t count = 128;
	size_t arr[count];
	size_t src_arr[count];

	for (size_t i = 0; i < count; ++i) {
		arr[i] = i;
		src_arr[i] = i;
	}

	VirtualPointer<size_t> src_ptr = VirtualPointer<size_t>();

	EXPECT_NO_THROW(memmove(arr, src_ptr, 0));
	EXPECT_THROW(memmove(arr, src_ptr, 1), NullPointerException);
	src_ptr.addChunk(src_arr, count / 4);
	EXPECT_NO_THROW(memmove(arr, src_ptr, 1));
	EXPECT_THROW(memmove(arr, src_ptr - 1, 1), std::out_of_range);

	src_ptr.addChunk(src_arr + count / 4, count / 2);
	EXPECT_NO_THROW(memmove(arr, src_ptr, 3 * count / 4));
	EXPECT_THROW(memmove(arr, src_ptr, 3 * count / 4 + 1), std::out_of_range);

	src_ptr.addChunk(src_arr + 3 * count / 4, count / 4);
	EXPECT_THROW(memmove(arr, src_ptr, count + 1), std::out_of_range);
	EXPECT_THROW(memmove(arr, src_ptr + 1, count), std::out_of_range);
	EXPECT_NO_THROW(memmove(arr, src_ptr + 1, count - 1));
	EXPECT_NO_THROW(memmove(arr + 1, src_ptr, count - 1));
}


/*
*
*
* memcmp(VirtualPointer dest, VirtualPointer src, size_t count)
*
*
*/

TEST(MemcmpTwoVirtual, usualWork) {
	const size_t count = 128;
	uint32_t larr32[count];
	uint32_t rarr32[count];

	auto arraysSet = [&]() mutable -> void {
		for (size_t i = 0; i < count; ++i) {
			rarr32[i] = larr32[i] = (uint32_t)i;
		}
	};

	VirtualPointer<uint32_t> lptr32 = VirtualPointer<uint32_t>();

	VirtualPointer<uint32_t> rptr32 = VirtualPointer<uint32_t>();

	auto setLChunks = [&](const size_t offset, const size_t size) mutable -> void {
		lptr32.addChunk(larr32 + offset, size);

	};
	auto setRChunks = [&](const size_t offset, const size_t size) mutable -> void {
		rptr32.addChunk(rarr32 + offset, size);

	};


	{
		/*
			l : |8 bytes|16 bytes|  32 bytes  |      24      |    48 bytes     |
			r : |8 bytes|16 bytes| 8 |  32 bytes  |    16    |    48 bytes     |
		*/

		setLChunks(0, 8);
		setLChunks(8, 16);
		setLChunks(24, 32);
		setLChunks(80, 48);

		setRChunks(0, 8);
		setRChunks(8, 16);
		setRChunks(32, 32);
		setRChunks(80, 48);

		arraysSet();

		EXPECT_EQ(0, memcmp(lptr32, rptr32, 24));
		EXPECT_EQ(0, memcmp(lptr32 + 56, rptr32 + 56, 48));
		EXPECT_TRUE(memcmp(lptr32, rptr32, 25) < 0);
		EXPECT_TRUE(memcmp(lptr32 + 1, rptr32, 24) > 0);
		larr32[0] = 1;
		EXPECT_TRUE(memcmp(lptr32, rptr32, 25) > 0);
		memcpy(lptr32, rptr32, 56);
		EXPECT_EQ(0, memcmp(lptr32, rptr32, 56));
		EXPECT_EQ(0, memcmp(lptr32 + 1, rptr32 + 1, 55));
		larr32[80] = 0;
		EXPECT_TRUE(memcmp(lptr32, rptr32, 57) < 0);
		memcpy(lptr32, rptr32, 104);
		EXPECT_EQ(0, memcmp(lptr32, rptr32, 104));
	}
}

TEST(MemcmpTwoVirtual, exceptions) {
	const size_t count = 128;
	size_t arr[count];

	for (size_t i = 0; i < count; ++i) {
		arr[i] = i;
	}

	VirtualPointer<size_t> lptr = VirtualPointer<size_t>();
	VirtualPointer<size_t> rptr = VirtualPointer<size_t>();

	EXPECT_NO_THROW(memcmp(lptr, rptr, 0));
	EXPECT_THROW(memcmp(lptr, rptr, 1), NullPointerException);
	lptr.addChunk(arr, count / 4);
	EXPECT_THROW(memcmp(lptr, rptr, 1), NullPointerException);
	rptr.addChunk(arr, count / 4);
	EXPECT_NO_THROW(memcmp(lptr, rptr, 1));
	EXPECT_THROW(memcmp(lptr - 1, rptr, 1), std::out_of_range);
	EXPECT_THROW(memcmp(lptr, rptr - 1, 1), std::out_of_range);

	lptr.addChunk(arr + count / 4, count / 4);
	EXPECT_THROW(memcmp(lptr, rptr, count / 2 + 1), std::out_of_range);
	rptr.addChunk(arr + count / 4, count / 2);
	EXPECT_NO_THROW(memcmp(lptr, rptr, count / 2));
	EXPECT_THROW(memcmp(lptr, rptr, count / 2 + 1), std::out_of_range);

	lptr.addChunk(arr + count / 2, count / 2);
	rptr.addChunk(arr + 3 * count / 4, count / 4);
	EXPECT_THROW(memcmp(lptr, rptr, count + 1), std::out_of_range);
}


/*
*
*
* memcmp(VirtualPointer dest, void* src, size_t count)
*
*
*/

TEST(MemcmpVirtualWithPseudoreal, usualWork) {
	const size_t count = 128;
	uint32_t larr32[count];
	uint32_t rarr32[count];

	auto arraysSet = [&]() mutable -> void {
		for (size_t i = 0; i < count; ++i) {
			rarr32[i] = larr32[i] = (uint32_t)i;
		}
	};

	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();

	auto setChunks = [&](const size_t offset, const size_t size) mutable -> void {
		ptr32.addChunk(larr32 + offset, size);

	};


	{
		/*
			ptr: |8 bytes|16 bytes| 8 |  32 bytes  |    16    |    48 bytes     |
		*/

		setChunks(0, 8);
		setChunks(8, 16);
		setChunks(32, 32);
		setChunks(80, 48);

		arraysSet();

		EXPECT_EQ(0, memcmp(ptr32, rarr32, 24));
		EXPECT_EQ(0, memcmp(ptr32 + 24, rarr32 + 32, 32));
		EXPECT_EQ(0, memcmp(ptr32 + 56, rarr32 + 80, 48));
		EXPECT_TRUE(memcmp(ptr32, rarr32, 25) > 0);
		EXPECT_TRUE(memcmp(ptr32, rarr32 + 1, 24) < 0);
		rarr32[0] = 1;
		EXPECT_TRUE(memcmp(ptr32, rarr32, 24) < 0);
		memcpy(rarr32, ptr32, 56);
		EXPECT_EQ(0, memcmp(ptr32, rarr32, 56));
		EXPECT_TRUE(memcmp(ptr32, rarr32, 57) > 0);
		memcpy(rarr32 + 56, ptr32 + 56, 48);
		EXPECT_EQ(0, memcmp(ptr32, rarr32, 104));
	}
}

TEST(MemcmpVirtualWithPseudoreal, exceptions) {
	const size_t count = 128;
	size_t arr[count];
	size_t src_arr[count];

	for (size_t i = 0; i < count; ++i) {
		arr[i] = i;
		src_arr[i] = i;
	}

	VirtualPointer<size_t> ptr = VirtualPointer<size_t>();

	EXPECT_NO_THROW(memcmp(ptr, src_arr, 0));
	EXPECT_THROW(memcmp(ptr, src_arr, 1), NullPointerException);
	ptr.addChunk(arr, count / 4);

	EXPECT_NO_THROW(memcmp(ptr, src_arr, 1));
	EXPECT_THROW(memcmp(ptr - 1, src_arr, 1), std::out_of_range);

	ptr.addChunk(arr + count / 4, count / 4);
	EXPECT_NO_THROW(memcmp(ptr, src_arr, count / 2));

	ptr.addChunk(arr + count / 2, count / 2);
	EXPECT_THROW(memcmp(ptr, src_arr, count + 1), std::out_of_range);
}


/*
*
*
* memcmp(void* dest, VirtualPointer src, size_t count)
*
*
*/

TEST(MemcmpPseudorealWithVirtual, usualWork) {
	const size_t count = 128;
	uint32_t larr32[count];
	uint32_t rarr32[count];

	auto arraysSet = [&]() mutable -> void {
		for (size_t i = 0; i < count; ++i) {
			rarr32[i] = larr32[i] = (uint32_t)i;
		}
	};

	VirtualPointer<uint32_t> ptr32 = VirtualPointer<uint32_t>();

	auto setChunks = [&](const size_t offset, const size_t size) mutable -> void {
		ptr32.addChunk(rarr32 + offset, size);

	};
	auto move = [&](const size_t size) mutable -> void {
		memmove(rarr32, ptr32, size);
	};
	auto test = [&](const size_t from, const size_t valFrom, const size_t size) mutable -> void {
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(rarr32[i + from], valFrom + i);
		}
	};


	{

		/*
			|8 bytes|16 bytes| 8 |  32 bytes  |    16    |    48 bytes     |
		*/

		setChunks(0, 8);
		setChunks(8, 16);
		setChunks(32, 32);
		setChunks(80, 48);


		arraysSet();

		EXPECT_EQ(0, memcmp(larr32, ptr32, 24));
		EXPECT_EQ(0, memcmp(larr32 + 32, ptr32 + 24, 32));
		EXPECT_EQ(0, memcmp(larr32 + 80, ptr32 + 56, 48));
		EXPECT_TRUE(memcmp(larr32, ptr32, 25) < 0);
		EXPECT_TRUE(memcmp(larr32 + 1, ptr32, 24) > 0);
		larr32[0] = 1;
		EXPECT_TRUE(memcmp(larr32, ptr32, 24) > 0);
		memcpy(larr32, ptr32, 56);
		EXPECT_EQ(0, memcmp(larr32, ptr32, 56));
		EXPECT_TRUE(memcmp(larr32, ptr32, 57) < 0);
		memcpy(larr32 + 56, ptr32 + 56, 48);
		EXPECT_EQ(0, memcmp(larr32, ptr32, 104));
	}
}

TEST(MemcmpPseudorealWithVirtual, exceptions) {
	const size_t count = 128;
	size_t arr[count];
	size_t src_arr[count];

	for (size_t i = 0; i < count; ++i) {
		arr[i] = i;
		src_arr[i] = i;
	}

	VirtualPointer<size_t> src_ptr = VirtualPointer<size_t>();

	EXPECT_NO_THROW(memcmp(arr, src_ptr, 0));
	EXPECT_THROW(memcmp(arr, src_ptr, 1), NullPointerException);
	src_ptr.addChunk(src_arr, count / 4);
	EXPECT_NO_THROW(memcmp(arr, src_ptr, 1));
	EXPECT_THROW(memcmp(arr, src_ptr - 1, 1), std::out_of_range);

	src_ptr.addChunk(src_arr + count / 4, count / 2);
	EXPECT_NO_THROW(memcmp(arr, src_ptr, 3 * count / 4));
}

TEST(AssignOperatorDontCopyChunksButCopyPtr, ChangingOfOneCopyChangesOtherCopy) {
	const size_t count = 128;
	size_t src_arr[count];

	for (size_t i = 0; i < count; ++i) {
		src_arr[i] = i;
	}

	auto srcPtr = VirtualPointer<size_t>();
	auto cpyPtr = srcPtr;
	auto extraPtr = VirtualPointer<size_t>();;

	srcPtr.addChunk(src_arr, 1);
	EXPECT_EQ(0, *srcPtr);
	EXPECT_EQ(0, *cpyPtr);
	++srcPtr;
	EXPECT_EQ(0, *cpyPtr);
	cpyPtr.addChunk(src_arr + 1, 1);
	EXPECT_EQ(1, *srcPtr);
	++cpyPtr;
	EXPECT_EQ(1, *cpyPtr);
	++cpyPtr;
	EXPECT_EQ(1, *srcPtr);
	cpyPtr.addChunk(src_arr + 8, 1);
	EXPECT_EQ(1, *(srcPtr++));
	EXPECT_EQ(8, *srcPtr);
	EXPECT_EQ(8, *cpyPtr);
	cpyPtr.addChunk(src_arr + 8, 1);
	EXPECT_EQ(8, *(srcPtr++));
	EXPECT_EQ(8, *(cpyPtr++));
	EXPECT_EQ(8, *srcPtr);
	EXPECT_EQ(8, *cpyPtr);
	extraPtr.addChunk(src_arr + 16, 4);
	srcPtr.addChunk(extraPtr, 4);
	EXPECT_EQ(16, *(++srcPtr));
	EXPECT_EQ(16, *(++cpyPtr));
	EXPECT_EQ(17, *(++srcPtr));
	EXPECT_EQ(17, *(++cpyPtr));
	EXPECT_EQ(18, *(++srcPtr));
	EXPECT_EQ(18, *(++cpyPtr));
	cpyPtr += 5;
	srcPtr.addChunk(src_arr, 1);
	srcPtr.addChunk(src_arr + 1, 2);
	srcPtr.addChunk(extraPtr, 4);
	EXPECT_EQ(16, *cpyPtr);
}

TEST(AssignOperatorDontCopyChunksButCopyPtr, exceptions) {
	const size_t count = 128;
	size_t src_arr[count];

	for (size_t i = 0; i < count; ++i) {
		src_arr[i] = i;
	}

	VirtualPointer<size_t> src_ptr = VirtualPointer<size_t>();
	VirtualPointer<size_t> cpy_ptr = src_ptr;
	VirtualPointer<size_t> extra_ptr = VirtualPointer<size_t>();;
	
	src_ptr.addChunk(src_arr, 1);
	++src_ptr;
	cpy_ptr.addChunk(src_arr + 1, 1);
	cpy_ptr += 2;
	EXPECT_EQ(1, *src_ptr);
	cpy_ptr.addChunk(src_arr + 8, 1);
	cpy_ptr.addChunk(src_arr + 8, 1);
	extra_ptr.addChunk(src_arr + 16, 4);
	src_ptr.addChunk(extra_ptr, 4);
	src_ptr += 4;
	EXPECT_NO_THROW(*src_ptr);
	src_ptr += 2;
	EXPECT_NO_THROW(*src_ptr);
	++src_ptr;
}


TEST(ComeBackFromOutOfBound, PositiveOutOfBound) {
	
	constexpr size_t count = 128;
	size_t src_arr[count];

	for (size_t i = 0; i < count; ++i) {
		src_arr[i] = i;
	}

	VirtualPointer<size_t> src_ptr = VirtualPointer<size_t>();

	for(size_t size = 1, shift = 0; size < count/2; size *= 2)
	{
		EXPECT_NO_THROW(src_ptr.addChunk(src_arr + shift, size));
		shift += size;
	}

	EXPECT_NO_THROW(src_ptr += count);
	EXPECT_NO_THROW(++src_ptr);
	EXPECT_NO_THROW(src_ptr++);
	EXPECT_NO_THROW(src_ptr += count);
	
	EXPECT_NO_THROW(src_ptr -= 2*count);
	EXPECT_NO_THROW(--src_ptr);
	EXPECT_NO_THROW(src_ptr--);

	EXPECT_EQ(0, *src_ptr);
}


TEST(ComeBackFromOutOfBound, NegativeOutOfBound) {
	
	constexpr size_t count = 128;
	size_t src_arr[count];

	for (size_t i = 0; i < count; ++i) {
		src_arr[i] = i;
	}

	VirtualPointer<size_t> src_ptr = VirtualPointer<size_t>();

	for(size_t size = 1, shift = 0; size < count/2; size *= 2)
	{
		EXPECT_NO_THROW(src_ptr.addChunk(src_arr + shift, size));
		shift += size;
	}


	EXPECT_NO_THROW(src_ptr -= count);
	EXPECT_NO_THROW(--src_ptr);
	EXPECT_NO_THROW(src_ptr--);
	EXPECT_NO_THROW(src_ptr -= count);
	
	EXPECT_NO_THROW(++src_ptr);
	EXPECT_NO_THROW(src_ptr++);
	EXPECT_NO_THROW(src_ptr += 2*count);

	EXPECT_EQ(0, *src_ptr);
}

template<typename T>
void BytesRemainingArithmeticOperations()
{
	constexpr size_t srcArrSize = 128;
	T srcArr[srcArrSize];
	auto vSrcArr = VirtualPointer<T>();

	for (size_t chunksSize = 0; chunksSize < srcArrSize; chunksSize += 16)
	{
		EXPECT_NO_THROW(vSrcArr.addChunk(srcArr + chunksSize, 16));
	}

	BytesRemainingArithmeticOperations<T>(srcArr, srcArrSize);
	BytesRemainingArithmeticOperations<T>(vSrcArr, srcArrSize);
}

template<typename T, typename P>
void BytesRemainingArithmeticOperations(P srcArr, const size_t srcArrSize)
{
	auto getBytesCount = [](const size_t value)
	{
		return value * sizeof(T);
	};
	
	auto srcPtr = VirtualPointer<T>();
	size_t chunksSize = 0;

	for (size_t size = 1; size < srcArrSize / 2; size *= 2)
	{
		EXPECT_NO_THROW(srcPtr.addChunk(srcArr + chunksSize, size));
		chunksSize += size;
	}

	auto shift = 0;

	EXPECT_EQ(getBytesCount(chunksSize - shift), srcPtr.bytesRemaining());
	++srcPtr;
	++shift;
	EXPECT_EQ(getBytesCount(chunksSize - shift), srcPtr.bytesRemaining());
	srcPtr += 1;
	shift += 1;
	EXPECT_EQ(getBytesCount(chunksSize - shift), srcPtr.bytesRemaining());
	srcPtr += 4;
	shift += 4;
	EXPECT_EQ(getBytesCount(chunksSize - shift), srcPtr.bytesRemaining());
	srcPtr += 32;
	shift += 32;
	EXPECT_EQ(getBytesCount(chunksSize - shift), srcPtr.bytesRemaining());


	--srcPtr;
	--shift;
	EXPECT_EQ(getBytesCount(chunksSize - shift), srcPtr.bytesRemaining());
	srcPtr -= 1;
	shift -= 1;
	EXPECT_EQ(getBytesCount(chunksSize - shift), srcPtr.bytesRemaining());
	srcPtr -= 4;
	shift -= 4;
	EXPECT_EQ(getBytesCount(chunksSize - shift), srcPtr.bytesRemaining());
	srcPtr -= 32;
	shift -= 32;
	EXPECT_EQ(getBytesCount(chunksSize - shift), srcPtr.bytesRemaining());
}

TEST(BytesRemaining, ArithmeticOperations)
{	
	BytesRemainingArithmeticOperations<uint8_t>();
	BytesRemainingArithmeticOperations<uint16_t>();
	BytesRemainingArithmeticOperations<uint32_t>();
	BytesRemainingArithmeticOperations<size_t>();
}


template<typename T>
void BytesRemainingDifferentCopies()
{
	constexpr size_t srcArrSize = 128;
	T srcArr[srcArrSize];
	auto originalVptr = VirtualPointer<T>();

	for (size_t chunksSize = 0; chunksSize < srcArrSize/2; chunksSize += 16)
	{
		EXPECT_NO_THROW(originalVptr.addChunk(srcArr + chunksSize, 16));
	}

	auto copyVptr = originalVptr;

	auto getBytesCount = [](const size_t value)
	{
		return value * sizeof(T);
	};


	EXPECT_EQ(getBytesCount(srcArrSize/2), originalVptr.bytesRemaining());
	EXPECT_EQ(getBytesCount(srcArrSize/2), copyVptr.bytesRemaining());

	constexpr auto srcArrSizeQuarter = srcArrSize / 4;

	originalVptr.addChunk(srcArr + srcArrSize / 2, srcArrSizeQuarter);

	EXPECT_EQ(getBytesCount(srcArrSizeQuarter * 3), originalVptr.bytesRemaining());
	EXPECT_EQ(getBytesCount(srcArrSizeQuarter * 3), copyVptr.bytesRemaining());
	
	copyVptr.addChunk(srcArr + 3*srcArrSizeQuarter, srcArrSizeQuarter);
	
	EXPECT_EQ(getBytesCount(srcArrSize), originalVptr.bytesRemaining());
	EXPECT_EQ(getBytesCount(srcArrSize), copyVptr.bytesRemaining());
}

TEST(BytesRemaining, DifferentCopies)
{
	BytesRemainingDifferentCopies<uint8_t>();
	BytesRemainingDifferentCopies<uint16_t>();
	BytesRemainingDifferentCopies<uint32_t>();
	BytesRemainingDifferentCopies<size_t>();
}