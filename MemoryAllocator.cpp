#include "stdafx.h"
#include "MemoryAllocator.h"


MemoryAllocator::MemoryAllocator()
	: _pool32(0, false), _pool64(0, false), _pool128(0, false), _pool256(0, false),
	_pool512(0, false), _pool1024(0, false), _pool2048(0, false), _pool4096(0, false)
{
}

//-------------------------------------------------------------------------------
/// @brief ��û�� size��ŭ�� ������ ������ �Ҵ��Ѵ�.
/// @param size �ʿ��� �����Ͱ��� ũ��
/// @return [void*] nullptr������ ����. �Ҵ���� ������ ������ġ�� �Ѱ��ش�.
//_______________________________________________________________________________
void* MemoryAllocator::Alloc(size_t size)
{
	// 0������ ���� ���ڷ� ������ ������ CRASH
	if (size <= 0)
	{
		CRASH("Have to alloc at least 1");
	}

	// Pooling�ϴ� ���� �̻��� ��û�ϸ� �׳� malloc�� ȣ���Ѵ�.
	if (size > MAX_SIZE)
	{
		return ::malloc(size);
	}

	// ��� ũ���� Pool�� �������� ���.
	int32 chunkSize = (size - 1) >> 4;

	if (chunkSize != 0)
	{
		chunkSize = 31 - __lzcnt(chunkSize);
	}


	switch (chunkSize)
	{
	case 0:
	{
		Chunk<32>* ptr = _pool32.Alloc();
		ptr->size = size;
		return reinterpret_cast<BYTE*>(ptr) + sizeof(size_t);
	}
	case 1:
	{
		Chunk<64>* ptr = _pool64.Alloc();
		ptr->size = size;
		return reinterpret_cast<BYTE*>(ptr) + sizeof(size_t);
	}
	case 2:	
	{
		Chunk<128>* ptr = _pool128.Alloc();
		ptr->size = size;
		return reinterpret_cast<BYTE*>(ptr) + sizeof(size_t);
	}
	case 3:
	{
		Chunk<256>* ptr = _pool256.Alloc();
		ptr->size = size;
		return reinterpret_cast<BYTE*>(ptr) + sizeof(size_t);
	}
	case 4:
	{
		Chunk<512>* ptr = _pool512.Alloc();
		ptr->size = size;
		return reinterpret_cast<BYTE*>(ptr) + sizeof(size_t);
	}
	case 5:
	{
		Chunk<1024>* ptr = _pool1024.Alloc();
		ptr->size = size;
		return reinterpret_cast<BYTE*>(ptr) + sizeof(size_t);
	}
	case 6:
	{
		Chunk<2048>* ptr = _pool2048.Alloc();
		ptr->size = size;
		return reinterpret_cast<BYTE*>(ptr) + sizeof(size_t);
	}
	case 7:
	{
		Chunk<4096>* ptr = _pool4096.Alloc();
		ptr->size = size;
		return reinterpret_cast<BYTE*>(ptr) + sizeof(size_t);
	}
	default:
		CRASH("Alloc Failure");
	}
	

}

//-------------------------------------------------------------------------------
/// @brief ptr������ �ٽ� Pool�� ��ȯ�Ѵ�.
/// @param ptr ��ȯ�� �����ּ�
//_______________________________________________________________________________
void MemoryAllocator::Free(void* ptr)
{
	size_t* chunk = reinterpret_cast<size_t*>(reinterpret_cast<BYTE*>(ptr) - sizeof(size_t));
	if (*chunk > MAX_SIZE)
	{
		::free(ptr);
		return;
	}

	int32 chunkSize = (*chunk - 1) >> 4;

	if (chunkSize != 0)
	{
		chunkSize = 31 - __lzcnt(chunkSize);
	}

	switch (chunkSize)
	{
	case 0:	
		_pool32.Free(reinterpret_cast<Chunk<32>*>(chunk));
		break;
	case 1:	
		_pool64.Free(reinterpret_cast<Chunk<64>*>(chunk));
		break;
	case 2:	
		_pool128.Free(reinterpret_cast<Chunk<128>*>(chunk));
		break;
	case 3:	
		_pool256.Free(reinterpret_cast<Chunk<256>*>(chunk));
		break;
	case 4:	
		_pool512.Free(reinterpret_cast<Chunk<512>*>(chunk));
		break;
	case 5:	
		_pool1024.Free(reinterpret_cast<Chunk<1024>*>(chunk));
		break;
	case 6:	
		_pool2048.Free(reinterpret_cast<Chunk<2048>*>(chunk));
		break;
	case 7:	
		_pool4096.Free(reinterpret_cast<Chunk<4096>*>(chunk));
		break;
	default:
		CRASH("Free Failure");
	}
}
