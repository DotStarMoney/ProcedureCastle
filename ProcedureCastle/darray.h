#pragma once
#include <cstdlib>

#define _DARRAY_DEFAULT_CAP 128

template <class T>
class DArray
{
public:
	DArray()
	{
		data = (T*) malloc(_DARRAY_DEFAULT_CAP*sizeof(T));
		size = 0;
		capacity = _DARRAY_DEFAULT_CAP;
	}
	~DArray()
	{
		free(data);
	}
	void reserve(unsigned int _elements)
	{
		unsigned int nsize;
		nsize = size + _elements;
		if (nsize > capacity)
		{
			do { capacity <<= 1; } while (nsize > capacity);
			data = (T*) realloc(data, capacity*sizeof(T));
		}
	}
	void acquire(unsigned int _elements)
	{
		size += _elements;
	}
	void clear()
	{
		size = 0;
	}
	unsigned int getSize() { return size; }
	T* getData() { return data; }
	T* getEndOfData() { return data + size; }
private:
	T* data;
	unsigned int size;
	unsigned int capacity;
};