#pragma once

//===============================================================================
/// @brief ��ü������ �����Ҵ� ������ �����ϱ� ���� Pool.\n
/// @details BUILD_SAFE�� define�ϸ� ��뿡 �ʿ��� ������ġ���� ���� �� �ִ�.
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
	const bool	_callConstructor;		///< �Ҵ�, ������ ������, �Ҹ��� ȣ�⿩��

	BlockNode*	_head;					///< ���� ���

	int32		_capacity;				///< ���� Ȯ�� �� �� ����
	int32		_useCount;				///< ���� ������� �� ����
};

//-------------------------------------------------------------------------------
/// @param initBlocks �ʱ� �� ����
/// @param callConstructor �Ҵ�, ������ ������, �Ҹ��� ȣ�⿩��
//_______________________________________________________________________________
template <typename T>
ObjectPool<T>::ObjectPool(int32 initBlocks, bool callConstructor) : _callConstructor(callConstructor), _capacity(initBlocks), _useCount(0)
{
	// ���� �ʱ�ȭ.
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
/// @brief ���� FreeList�� �ִ� ���鸸 ������� �޸� ������ �����Ѵ�.
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
/// @brief �޸𸮸� Pool���� �����´�. ������� �Ҵ��Ѵ�.
/// @param args �����ڿ� �� ���ڸ��
/// @return �Ҵ�� ������
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

		ret = new(ptr)T(args...); // ������ ȣ��
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
			ret = new(ptr)T(args...); // ������ ȣ��
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
/// @brief �޸𸮸� Pool�� ��ȯ�Ѵ�.
/// @param ptr ������ �޸� �����ּ�
//_______________________________________________________________________________
template <typename T>
void ObjectPool<T>::Free(T* ptr)
{
	if (_callConstructor == true)
	{
		ptr->~T(); // �Ҹ��� ȣ��
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

