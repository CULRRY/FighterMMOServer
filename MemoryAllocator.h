#pragma once
#include "ObjectPool.h"

//===============================================================================
/// @brief Malloc 느낌의 메모리 할당자.\n
///		   ObjectPool과 다르게 원하는 만큼의 공간을 할당받을 수 있다.
//_______________________________________________________________________________
class MemoryAllocator
{
	enum
	{
		MAX_SIZE = 4096,
	};

	/// @brief Size만큼의 공간을 가지고 있는 구조체
	template <int32 Size>
	struct Chunk
	{
		size_t	size;
		BYTE	data[Size];
	};

public:
	MemoryAllocator();

	void*	Alloc(size_t size);
	void	Free(void* ptr);

private:
	ObjectPool<Chunk<32>>		_pool32;	///< 32   Byte Pool
	ObjectPool<Chunk<64>>		_pool64;	///< 64   Byte Pool
	ObjectPool<Chunk<128>>		_pool128;	///< 128  Byte Pool
	ObjectPool<Chunk<256>>		_pool256;	///< 256  Byte Pool
	ObjectPool<Chunk<512>>		_pool512;	///< 512  Byte Pool
	ObjectPool<Chunk<1024>>		_pool1024;	///< 1024 Byte Pool
	ObjectPool<Chunk<2048>>		_pool2048;	///< 2048 Byte Pool
	ObjectPool<Chunk<4096>>		_pool4096;	///< 4096 Byte Pool

};

