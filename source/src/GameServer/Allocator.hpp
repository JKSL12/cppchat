#pragma once

#include "pch.h"

class IAllocator
{
public:
	IAllocator() {}
	virtual ~IAllocator() {}

	virtual bool Reserve(int count) = 0;
	virtual void Return(void* o) = 0;
	virtual void* Get() = 0;
};

template <typename T>
class Allocator : public IAllocator
{
public:
	typedef struct _Item {
		SLIST_ENTRY entry;
		T* obj;
	} Item;

public:
	Allocator() : _usedCount(0LL), _freeCount(0LL)
	{
		_freeHead = (PSLIST_HEADER)_aligned_malloc(sizeof(SLIST_HEADER), MEMORY_ALLOCATION_ALIGNMENT);
		InitializeSListHead(_freeHead);

		_usedHead = (PSLIST_HEADER)_aligned_malloc(sizeof(SLIST_HEADER), MEMORY_ALLOCATION_ALIGNMENT);
		InitializeSListHead(_usedHead);
	}

	~Allocator()
	{
		assert(_usedCount == 0);

		if (_freeHead) { _aligned_free(_freeHead); } _freeHead = NULL;
		if (_usedHead) { _aligned_free(_usedHead); } _usedHead = NULL;
	}

	LONGLONG UsedCount() { return _usedCount; }

	bool Reserve(int count)
	{
		for (int i = 0; i < count; ++i) {
			Item* item = New();
			InterlockedPushEntrySList(_freeHead, &(item->entry));
			InterlockedIncrement64(&_freeCount);
		}

		return true;
	}

	void Remove()
	{
		for (PSLIST_ENTRY e = InterlockedPopEntrySList(_freeHead); e != NULL; e = InterlockedPopEntrySList(_freeHead)) {
			Item* i = CONTAINING_RECORD(e, Item, entry);
			Delete(i);
		}

		InterlockedFlushSList(_freeHead);
	}

	void* Get()
	{
		Item* i = NULL;
		PSLIST_ENTRY e = InterlockedPopEntrySList(_freeHead);
		if (e) {
			i = CONTAINING_RECORD(e, Item, entry);
			InterlockedDecrement64(&_freeCount);
		}
		else {
			i = New();
			e = &i->entry;
		}

		T* r = i->obj;
		i->obj = NULL;
		InterlockedPushEntrySList(_usedHead, e);

		InterlockedIncrement64(&_usedCount);

		return r;
	}

	void Return(void* o)
	{
		T* obj = reinterpret_cast<T*>(o);

		PSLIST_ENTRY e = InterlockedPopEntrySList(_usedHead);
		if (!e) {
			assert(!"ERROR: wrong");
			return;
		}

		Item* i = CONTAINING_RECORD(e, Item, entry);
		i->obj = obj;
		InterlockedPushEntrySList(_freeHead, e);

		InterlockedDecrement64(&_usedCount);
		InterlockedIncrement64(&_freeCount);
	}

private:
	Item* New()
	{
		T* o = (T*)_aligned_malloc(sizeof(T), MEMORY_ALLOCATION_ALIGNMENT);
		o = new (o) T();
		o->SetAllocator(this);

		Item* item = (Item*)_aligned_malloc(sizeof(Item), MEMORY_ALLOCATION_ALIGNMENT);
		item->obj = o;

		return item;
	}

	void Delete(Item* i)
	{
		i->obj->~T();
		_aligned_free(i->obj);
		_aligned_free(i);
	}

private:
	PSLIST_HEADER _freeHead;
	PSLIST_HEADER _usedHead;

	volatile LONGLONG _usedCount;
	volatile LONGLONG _freeCount;
};

class Object;
class IFinalizer
{
public:
	virtual bool OnFinalize(Object* obj) = 0;
};

class __declspec(align(MEMORY_ALLOCATION_ALIGNMENT)) Object
{
public:
	Object() : _alloc(NULL), _fi(NULL) {}
	virtual ~Object() {}

	void SetAllocator(IAllocator* alloc) {
		_alloc = alloc;
	}

	void SetFinalizer(IFinalizer* f) {
		_fi = f;
	}

	bool GetFinalizer()
	{
		return _fi == NULL ? false : true;
	}

	virtual void Release()
	{
		bool ret = true;
		if (_fi) {
			ret = _fi->OnFinalize(this);
		}
		if (_alloc && ret) {
			_alloc->Return(this);
		}
	}

private:
	IAllocator* _alloc;
	IFinalizer* _fi;
};