#pragma once

//===============================================================================
/// @brief 객체단위로 동적할당 공간을 재사용하기 위한 Pool.\n
/// @details BUILD_SAFE를 define하면 사용에 필요한 안전장치를을 넣을 수 있다.
//_______________________________________________________________________________
template <typename T>
class ObjectPool
{
	struct BlockNode
	{
#ifdef BUILD_SAFE
		void* paddingFront; /// PADDING
#endif
		T data;
#ifdef BUILD_SAFE
		void* paddingRear;	/// PADDING
#endif
		BlockNode* next;
	};

public:
	ObjectPool(int32 initBlocks = 0, bool callConstructor = true);
	virtual ~ObjectPool();

	template <typename... Args>
	T*		Alloc(Args&&... args);
	void	Free(T* ptr);

	int32	GetCapacityCount() { return _capacity; }
	int32	GetUseCount() { return _useCount; }

private:
	void push(BlockNode* data)
	{
		data->next = _head->next;
		_head->next = data;
	}
	void pop()
	{
		_head->next = _head->next->next;
	}
	BlockNode* top()
	{
		return _head->next;
	}
private:
	const bool	_callConstructor;		///< 할당, 해제시 생성자, 소명자 호출여부

	BlockNode*	_head;					///< 스택 헤드

	int32		_capacity;				///< 현재 확보 된 블럭 개수
	int32		_useCount;				///< 현재 사용중인 블럭 개수
};

//-------------------------------------------------------------------------------
/// @param initBlocks 초기 블럭 개수
/// @param callConstructor 할당, 해제시 생성자, 소멸자 호출여부
//_______________________________________________________________________________
template <typename T>
ObjectPool<T>::ObjectPool(int32 initBlocks, bool callConstructor) : _callConstructor(callConstructor), _capacity(initBlocks), _useCount(0)
{
	// 스택 초기화.
	_head = reinterpret_cast<BlockNode*>(new BYTE[sizeof(BlockNode)]);
	_head->next = nullptr;

	if (_callConstructor == true)
	{
		for (int32 i = 0; i < initBlocks; i++)
		{
			BlockNode* ptr = reinterpret_cast<BlockNode*>(new BYTE[sizeof(BlockNode)]);
#ifdef BUILD_SAFE
			ptr->paddingFront = this;
			ptr->paddingRear = this;
#endif
			push(ptr);
		}
	}
	else
	{
		for (int32 i = 0; i < initBlocks; i++)
		{
			BlockNode* ptr = new BlockNode;
#ifdef BUILD_SAFE
			ptr->paddingFront = this;
			ptr->paddingRear = this;
#endif
			push(ptr);
		}
	}
}

//-------------------------------------------------------------------------------
/// @brief 현재 FreeList에 있는 블럭들만 대상으로 메모리 해제를 진행한다.
//_______________________________________________________________________________
template <typename T>
ObjectPool<T>::~ObjectPool()
{
	BlockNode* node = _head->next;

	while (node != nullptr)
	{
		BlockNode* next = node->next;
		if (_callConstructor == true)
		{
			::free(node);
		}
		else
		{
			delete node;
		}
		node = next;
	}

	::free(_head);
}



template <typename T>
//-------------------------------------------------------------------------------
/// @brief 메모리를 Pool에서 꺼내온다. 없을경우 할당한다.
/// @param args 생성자에 들어갈 인자목록
/// @return 할당된 포인터
//_______________________________________________________________________________
template <typename... Args>
T* ObjectPool<T>::Alloc(Args&&... args)
{
	BlockNode* ptr;
	T* ret;

	if (_capacity == _useCount)
	{
		ptr = reinterpret_cast<BlockNode*>(new BYTE[sizeof(BlockNode)]);

#ifdef BUILD_SAFE
		ptr->paddingFront = this;
		ptr->paddingRear = reinterpret_cast<BYTE*>(this) + 1;
		ptr = reinterpret_cast<BlockNode*>(reinterpret_cast<BYTE*>(ptr) + sizeof(void*));
#endif

		ret = new(ptr)T(args...); // 생성자 호출
		_capacity++;
	}
	else
	{
		ptr = top();
		pop();
#ifdef BUILD_SAFE
		ptr->paddingRear = reinterpret_cast<BYTE*>(ptr->paddingRear) + 1;

		ptr = reinterpret_cast<BlockNode*>(reinterpret_cast<BYTE*>(ptr) + sizeof(void*));
#endif
		if (_callConstructor == true)
		{
			ret = new(ptr)T(args...); // 생성자 호출
		}
		else
		{
			ret = reinterpret_cast<T*>(ptr);
		}
	}
	_useCount++;

	return ret;
}

//-------------------------------------------------------------------------------
/// @brief 메모리를 Pool에 반환한다.
/// @param ptr 해제할 메모리 시작주소
//_______________________________________________________________________________
template <typename T>
void ObjectPool<T>::Free(T* ptr)
{
	if (_callConstructor == true)
	{
		ptr->~T(); // 소멸자 호출
	}

	_useCount--;
	BlockNode* block = reinterpret_cast<BlockNode*>(ptr);
#ifdef BUILD_SAFE
	block = reinterpret_cast<BlockNode*>(reinterpret_cast<BYTE*>(block) - sizeof(void*));

	if (block->paddingFront != this || block->paddingRear != reinterpret_cast<BYTE*>(this) + 1)
	{
		CRASH("Free Failure");
	}

	block->paddingRear = reinterpret_cast<BYTE*>(block->paddingRear) - 1;
#endif
	push(block);
}

